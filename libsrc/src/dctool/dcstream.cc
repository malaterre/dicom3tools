#include <ctype.h>	// for isupper()

#include "dcstream.h"

static bool
checkSwapped32BigEndian(char *buffer)
{
	// DICOM PS3.5 says big-endian is msb first, lsb last
	// However have encountered files where a 32 bit value is sent
	// as big-endian lsb 16 bits, then big-endian msb 16 bits :(
	// These were from a Philips CT

	Uint32 goodvalue;
	Uint32 badvalue;

	goodvalue =  (Uint32)buffer[0];
	goodvalue <<= 8;
	goodvalue |= (Uint32)buffer[1];
	goodvalue <<= 8;
	goodvalue |= (Uint32)buffer[2];
	goodvalue <<= 8;
	goodvalue |= (Uint32)buffer[3];

	badvalue  =  (Uint32)buffer[2];
	badvalue  <<= 8;
	badvalue  |= (Uint32)buffer[3];
	badvalue  <<= 8;
	badvalue  |= (Uint32)buffer[0];
	badvalue  <<= 8;
	badvalue  |= (Uint32)buffer[1];

	// assume that the lower value is the correct one
	// (the check is applied to the first entry which
	// should be a group length (IdentifyingGroup) that
	// is VERY unlikely to be long enough to break this

	return badvalue < goodvalue;
}

void
DicomInputStream::initializeTransferSyntax(const char *uid,bool meta)
{
	TransferSyntaxToReadMetaHeader = 0;
	TransferSyntaxToReadDataSet = 0;

	// First make use of command line parameters that override guesswork ...

	if (uid) {
//cerr << "DicomInputStream::initializeTransferSyntax() initial uid=" << uid << endl;
		TransferSyntax *ts = new TransferSyntax(uid);
		if (meta) {
			TransferSyntaxToReadMetaHeader = ts;	// specified UID is transfer syntax to read metaheader
		}
		else {
			TransferSyntaxToReadDataSet = ts;	// specified UID is transfer syntax to read dataset (there is no metaheader)
		}
	}
	// else transfer syntax has to be determined by either guesswork or metaheader ...

	char b[8];
	bool setswapped32big = false;

	if (meta) {
//cerr << "DicomInputStream::initializeTransferSyntax() testing for metaheader after 128 byte preamble" << endl;
		// test for metaheader prefix after 128 byte preamble
		seekg(128,ios::beg);
		if (good() && read(b,4) && strncmp(b,"DICM",4) == 0) {
//cerr << "DicomInputStream::initializeTransferSyntax() found metaheader after 128 byte preamble" << endl;
			if (!TransferSyntaxToReadMetaHeader) TransferSyntaxToReadMetaHeader = 	// guess only if not specified on command line
				read(b,6) && isupper(b[4]) && isupper(b[5])
				? new TransferSyntax(ExplicitVRLittleEndianTransferSyntaxUID)	// standard
				: new TransferSyntax(ImplicitVRLittleEndianTransferSyntaxUID);	// old draft (e.g. used internally on GE IOS platform)

			// leaves positioned at start of metaheader
			seekg(128+4,ios::beg);
		}
		else {
//cerr << "DicomInputStream::initializeTransferSyntax() did not find metaheader after 128 byte preamble" << endl;
			clear(); seekg(0,ios::beg);		// reset stream since metaheader was sought but not found
			TransferSyntaxToReadDataSet=TransferSyntaxToReadMetaHeader;
			TransferSyntaxToReadMetaHeader=0;
		}
	}
//cerr << "DicomInputStream::initializeTransferSyntax() after metaheader checks, good()=" << good() << endl;
//cerr << "DicomInputStream::initializeTransferSyntax() after metaheader checks, TransferSyntaxToReadMetaHeader=" << (TransferSyntaxToReadMetaHeader ? TransferSyntaxToReadMetaHeader->getUID() : "") << endl;
//cerr << "DicomInputStream::initializeTransferSyntax() after metaheader checks, TransferSyntaxToReadDataSet=" << (TransferSyntaxToReadDataSet ? TransferSyntaxToReadDataSet->getUID() : "") << endl;
	
	if (!TransferSyntaxToReadDataSet && !TransferSyntaxToReadMetaHeader){	// was not specified on the command line and there is no metaheader
//cerr << "DicomInputStream::initializeTransferSyntax() no metaheader found and no transfer syntax prespecified, guessing" << endl;
		bool bigendian = false;
		bool explicitvr	= false;
		clear();
		seekg(0,ios::beg);
		if (good() && read(b,8)) {
			// examine probable group number ... assume <= 0x00ff
			if (b[0] < b[1]) bigendian=true;
			else if (b[0] == 0 && b[1] == 0) {
				// blech ... group number is zero
				// no point in looking at element number
				// as it will probably be zero too (group length)
				// try the 32 bit value length of implicit vr
				if (b[4] < b[7]) bigendian=true;
			}
			// else littleendian
			if (isupper(b[4]) && isupper(b[5])) explicitvr=true;
			else if (bigendian) setswapped32big=checkSwapped32BigEndian(b+4);
		}
		// else unrecognized ... assume default

		if (bigendian)
			if (explicitvr)
				TransferSyntaxToReadDataSet = new TransferSyntax(ExplicitVRBigEndianTransferSyntaxUID);
			else
				TransferSyntaxToReadDataSet = new TransferSyntax(ImplicitVR,BigEndian);
		else
			if (explicitvr)
				TransferSyntaxToReadDataSet = new TransferSyntax(ExplicitVRLittleEndianTransferSyntaxUID);
			else
				TransferSyntaxToReadDataSet = new TransferSyntax(ImplicitVRLittleEndianTransferSyntaxUID);

		// leaves positioned at start of dataset
		clear();
		seekg(0,ios::beg);
	}

	TransferSyntaxInUse = TransferSyntaxToReadMetaHeader ? TransferSyntaxToReadMetaHeader : TransferSyntaxToReadDataSet;
	Assert(TransferSyntaxInUse);
	setEndian(TransferSyntaxInUse->getEndian());
	if (setswapped32big) setSwapped32Big();	// This is not reflected in the TS
//cerr << "DicomInputStream::initializeTransferSyntax() at end, good()=" << good() << endl;
//cerr << "DicomInputStream::initializeTransferSyntax() at end, TransferSyntaxToReadMetaHeader=" << (TransferSyntaxToReadMetaHeader ? TransferSyntaxToReadMetaHeader->getUID() : "") << endl;
//cerr << "DicomInputStream::initializeTransferSyntax() at end, TransferSyntaxToReadDataSet=" << (TransferSyntaxToReadDataSet ? TransferSyntaxToReadDataSet->getUID() : "") << endl;
//cerr << "DicomInputStream::initializeTransferSyntax() at end, TransferSyntaxInUse=" << (TransferSyntaxInUse ? TransferSyntaxInUse->getUID() : "") << endl;
}

DicomInputStream::DicomInputStream(streambuf *buf,const char *uid,bool meta)
	: BinaryInputStream(buf)
{
	initializeTransferSyntax(uid,meta);
}

DicomInputStream::DicomInputStream(istream& istr,const char *uid,bool meta)
	: BinaryInputStream(istr)
{
	initializeTransferSyntax(uid,meta);
}

DicomInputStream::~DicomInputStream(void)
{
#ifdef TRACE_DESTRUCTORS
cerr << "DicomInputStream::~DicomInputStream" << endl;
#endif
	if (TransferSyntaxToReadDataSet)
		delete TransferSyntaxToReadDataSet;
	if (TransferSyntaxToReadMetaHeader)
		delete TransferSyntaxToReadMetaHeader;
}

void
DicomInputStream::setTransferSyntaxToReadDataSet(TransferSyntax *ts)
{
//cerr << "DicomInputStream::setTransferSyntaxToReadDataSet(): was " << (TransferSyntaxToReadDataSet ? TransferSyntaxToReadDataSet->getUID() : "") << endl;
	if (TransferSyntaxToReadDataSet)
		delete TransferSyntaxToReadDataSet;
	TransferSyntaxToReadDataSet=ts;
//cerr << "DicomInputStream::setTransferSyntaxToReadDataSet(): now " << (TransferSyntaxToReadDataSet ? TransferSyntaxToReadDataSet->getUID() : "") << endl;
}

void
DicomInputStream::readingDataSet(void)
{
//cerr << "DicomInputStream::readingDataSet()" << endl;
	Assert(TransferSyntaxToReadDataSet);
	TransferSyntaxInUse=TransferSyntaxToReadDataSet;
	setEndian(TransferSyntaxInUse->getEndian());
	//Assert(TransferSyntaxInUse->isImplicitVR() != TransferSyntaxInUse->isExplicitVR());
}

void
DicomInputStream::readingMetaHeader(void)
{
//cerr << "DicomInputStream::readingMetaHeader()" << endl;
	Assert(TransferSyntaxToReadMetaHeader);
	TransferSyntaxInUse=TransferSyntaxToReadMetaHeader;
	setEndian(TransferSyntaxInUse->getEndian());
	//Assert(TransferSyntaxInUse->isImplicitVR() != TransferSyntaxInUse->isExplicitVR());
}

void
DicomOutputStream::initializeTransferSyntax(const char *uid,bool meta,bool implicitmeta,bool addtiff)
{
	if (!uid) uid=DefaultTransferSyntaxUID;
	TransferSyntaxToWriteDataSet = new TransferSyntax(uid);
	preambledone=false;
	tiffinpreamble=false;
	if (meta) {
		if (addtiff) tiffinpreamble=true;
		TransferSyntaxToWriteMetaHeader =
		    (implicitmeta ? new TransferSyntax(ImplicitVRLittleEndianTransferSyntaxUID)
				  : new TransferSyntax(MetaInformationTransferSyntaxUID));
		//writingMetaHeader();	// Musn't be done now else preamble written before TIFF IFD offset is known
	}
	else {
		TransferSyntaxToWriteMetaHeader = 0;
		//writingDataSet();
	}
}

void
DicomOutputStream::writeMetaHeaderPreamble(void)
{
	char zeroes[128];
	memset(zeroes,0,128);
	if (tiffinpreamble) {
		Assert(TransferSyntaxToWriteDataSet);
		setEndian(TransferSyntaxToWriteDataSet->getEndian());
		write(isBigEndian() ? "MM":"II",2);	// Motorola or Intel byte order flag
		write16(0x002A);			// TIFF Version number
		Assert(offsetofIFD);
		write32(offsetofIFD);
		write(zeroes,120);
		Assert(TransferSyntaxToWriteMetaHeader);
		setEndian(TransferSyntaxToWriteMetaHeader->getEndian());
	}
	else {
		write(zeroes,128);
	}
	write("DICM",4);
	preambledone=true;
}

DicomOutputStream::DicomOutputStream(streambuf *buf,
		  const char *uid,
		  bool meta,bool implicitmeta,
		  bool addtiff)
	: BinaryOutputStream(buf)
{
	initializeTransferSyntax(uid,meta,implicitmeta,addtiff);
}

DicomOutputStream::DicomOutputStream(ostream& ostr,
		  const char *uid,
		  bool meta,bool implicitmeta,
		  bool addtiff)
	: BinaryOutputStream(ostr)
{
	initializeTransferSyntax(uid,meta,implicitmeta,addtiff);
}

DicomOutputStream::~DicomOutputStream(void)
{
#ifdef TRACE_DESTRUCTORS
cerr << "DicomOutputStream::~DicomOutputStream" << endl;
#endif
	if (TransferSyntaxToWriteDataSet)
		delete TransferSyntaxToWriteDataSet;
	if (TransferSyntaxToWriteMetaHeader)
		delete TransferSyntaxToWriteMetaHeader;
}

void
DicomOutputStream::writingDataSet(void)
{
//cerr << "DicomOutputStream::writingDataSet" << endl;
	TransferSyntaxInUse=TransferSyntaxToWriteDataSet;
	Assert(TransferSyntaxInUse);
	setEndian(TransferSyntaxInUse->getEndian());
}

void DicomOutputStream::writingMetaHeader(void)
{
//cerr << "DicomOutputStream::writingMetaHeader" << endl;
	TransferSyntaxInUse=TransferSyntaxToWriteMetaHeader;
	Assert(TransferSyntaxInUse);
	setEndian(TransferSyntaxInUse->getEndian());
	if (!preambledone) writeMetaHeaderPreamble();
}

