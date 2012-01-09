#include "basetype.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "dcstream.h"
#include "attr.h"
#include "attrnew.h"
#include "attrval.h"
#include "elmconst.h"
#include "transynd.h"

static bool
isOtherByteOrWordVR(const char *vr)
{
	return vr && vr[0]=='O' && (vr[1]=='B' || vr[1]=='W' || vr[1]=='X');
}

static bool
isSequenceVR(const char *vr)
{
	return vr && vr[0]=='S' &&  vr[1]=='Q';
}

static bool
isUnknownVR(const char *vr)
{
	return vr && vr[0]=='U' &&  vr[1]=='N';
}

static TransferSyntax *
readMetaHeaderTillTransferSyntax(DicomInputStream &stream)
{
	Uint32 n=0;
	Uint32 length=0xffffffff;
	while (stream.peek() != EOF && (length == 0xffffffff || n < length)) {
		Tag tag;
		stream >> tag;
		if (stream.fail()) {
			cerr << EMsgDC(TagReadFailed) << endl;
			return NULL;
		}
		n+=4;

		char vr[3];
		if (stream.getTransferSyntaxInUse()->isExplicitVR()) {
			stream.read(vr,2);
			if (stream.fail()) {
				cerr << EMsgDC(VRReadFailed) << endl;
				return NULL;
			}
			n+=2;
			vr[2]=0;
		}
		else {
			vr[0]=0;
		}

		Uint32 vl;
		if (!stream.getTransferSyntaxInUse()->isExplicitVR()) {
			stream >> vl;
			n+=4;
		}
		else if (isOtherByteOrWordVR(vr) || isSequenceVR(vr) || isUnknownVR(vr)) {
			(void)stream.read16();	// "Reserved"
			stream >> vl;
			n+=6;
		}
		else {
			vl=stream.read16();
			n+=2;
		}
		if (stream.fail()) {
			cerr << EMsgDC(VLReadFailed) << endl;
			return NULL;
		}

		if (tag == TagFromName(FileMetaInformationGroupLength)) {
			if (vl == 4) {
				length=stream.read32();
				n+=vl;
			}
			else {
				cerr << EMsgDC(UnexpectedVL) << endl;
				return NULL;
			}
		}
		else if (tag == TagFromName(TransferSyntaxUID)) {
			if (!stream.getTransferSyntaxInUse()->isExplicitVR()) {
				vr[0]='U'; vr[1]='I'; vr[2]=0;
			}
			Attribute *a=newAttribute(vr,tag);
			if (a) {
				a->read(stream,vl);
				if (stream.fail()) {
					cerr << EMsgDC(VLReadFailed) << endl;
					return NULL;
				}
				else {
					n+=vl;
					TransferSyntax *dts=new TransferSyntax(AttributeValue(a));
					Assert(dts);
					return dts;
				}
			}
			else {
				cerr << EMsgDC(UnrecognizedVR) << endl;
				return NULL;
			}
		}
		else if (!tag.isMetaheaderGroup()) {
			cerr << EMsgDC(PrematureEndOfMetaHeader) << endl;
			return NULL;
		}
		else if (vl == 0xffffffff) {
			cerr << EMsgDC(UnexpectedVL) << endl;
			return NULL;
		}
		else {
			stream.seekg(vl,ios::cur);
			if (stream.fail()) {
				cerr << EMsgDC(SeekFailed) << endl;
				return NULL;
			}
			n+=vl;
		}
	}
	cerr << EMsgDC(PrematureEndOfMetaHeader) << endl;
	return NULL;
}

int
main(int argc,char **argv)
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool brief=options.get("brief") || options.get("b");
	bool showfilename=options.get("filename");

	dicom_input_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,dicom_input_options.filename,cin);

	cerr << dicom_input_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();

	if (!dicom_input_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< " [-b|-brief]"
			<< " [-filename]"
			<< " [" << MMsgDC(InputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		return 1;
	}

	DicomInputStream din(*(istream *)input_opener,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);

	bool success;

	if (showfilename) {
		const char *filenameused = input_opener.getFilename();
		cerr << "Filename: \"" << (filenameused && strlen(filenameused) > 0 ? filenameused : "-") << "\"" << endl;
	}

	if (din.haveMetaHeader()) {
		TransferSyntax *mts=din.getTransferSyntaxToReadMetaHeader();
		if (mts) {
			if (!brief) {
				dumpTransferSyntax(mts,"Meta: ");
			}
			TransferSyntax *dts = readMetaHeaderTillTransferSyntax(din);
			if (dts) {
				if (brief) {
					cerr << "DICOM file, with metaheader, " << dts->getDescription() << endl;	// do not worry if mts is not EVRLE
				}
				else {
					dumpTransferSyntax(dts,"Data: ");
				}
				success = true;
			}
			else {
				success=false;
			}
		}
		else
			success=false;
	}
	else {
		TransferSyntax *dts=din.getTransferSyntaxToReadDataSet();
		if (dts) {
			if (brief) {
				cerr << "DICOM file, no metaheader, " << dts->getDescription() << endl;
			}
			else {
				dumpTransferSyntax(dts,"Data: ");
			}
		}
		if (din.isSwapped32Big() && !brief) {
			cerr << "Data: Swapped32Big\tYes" << endl;
		}
		success=true;
	}

	return success ? 0 : 1;
}

