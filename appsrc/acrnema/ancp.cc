#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include <ctype.h>

#include "attrmxls.h"
#include "attrtag.h"
#include "dcstream.h"
#include "dcopt.h"
#include "ioopt.h"
#include "elmconst.h"
#include "elmdict.h"
#include "mesgtext.h"

static bool
isOtherByteOrWordVR(const char *vr)
{
	return vr && vr[0]=='O' && (vr[1]=='B' || vr[1]=='W' || vr[1]=='X');
}

static bool
isOtherByteOrWordOrFloatVR(const char *vr)
{
	return vr && vr[0]=='O' && (vr[1]=='B' || vr[1]=='F'  || vr[1]=='W' || vr[1]=='X');
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

static bool
isUnlimitedTextVR(const char *vr)
{
	return vr && vr[0]=='U' &&  vr[1]=='T';
}

static bool
isAgeStringVR(const char *vr)
{
	return vr && vr[0]=='A' &&  vr[1]=='S';
}

static bool
isDateStringVR(const char *vr)
{
	return vr && vr[0]=='D' &&  vr[1]=='A';
}

static bool
isTimeStringVR(const char *vr)
{
	return vr && vr[0]=='T' &&  vr[1]=='M';
}

static bool
isDateTimeStringVR(const char *vr)
{
	return vr && vr[0]=='D' &&  vr[1]=='T';
}

static bool
isUIStringVR(const char *vr)
{
	return vr && vr[0]=='U' &&  vr[1]=='I';
}

static bool
isNumericOrDateOrTimeOrUIStringVR(const char *vr)
{
	return vr &&
		(  strcmp(vr,"DA") == 0
		|| strcmp(vr,"DT") == 0
		|| strcmp(vr,"DS") == 0
		|| strcmp(vr,"IS") == 0
		|| strcmp(vr,"TM") == 0
		|| strcmp(vr,"UI") == 0);
}

static bool
isStringVR(const char *vr)
{
	return vr &&
		(  strcmp(vr,"AE") == 0
		|| strcmp(vr,"AS") == 0
		|| strcmp(vr,"CS") == 0
		|| strcmp(vr,"DA") == 0
		|| strcmp(vr,"DT") == 0
		|| strcmp(vr,"DS") == 0
		|| strcmp(vr,"IS") == 0
		|| strcmp(vr,"LO") == 0
		|| strcmp(vr,"LT") == 0
		|| strcmp(vr,"PN") == 0
		|| strcmp(vr,"SH") == 0
		|| strcmp(vr,"ST") == 0
		|| strcmp(vr,"TM") == 0
		|| strcmp(vr,"UI") == 0
		|| strcmp(vr,"UT") == 0);
}

static Uint16
extract16(const char *buffer,Endian endian)
{
	Assert(endian!=NoEndian);
	Uint16 u;
	if (endian == BigEndian) {
		u =  (Uint16)((unsigned char *)buffer)[0];
		u <<= 8;
		u |= (Uint16)((unsigned char *)buffer)[1];
	}
	else {
		u =  (Uint16)((unsigned char *)buffer)[1];
		u <<= 8;
		u |= (Uint16)((unsigned char *)buffer)[0];
	}
	return u;
}

static Uint32
extract32(const char *buffer,Endian endian,bool swapped32big)
{
	Assert(endian!=NoEndian);
	Uint32 u;
	if (endian == BigEndian) {
		if (swapped32big) {
			u =  (Uint32)((unsigned char *)buffer)[2];
			u <<= 8;
			u |= (Uint32)((unsigned char *)buffer)[3];
			u <<= 8;
			u |= (Uint32)((unsigned char *)buffer)[0];
			u <<= 8;
			u |= (Uint32)((unsigned char *)buffer)[1];
		}
		else {
			u =  (Uint32)((unsigned char *)buffer)[0];
			u <<= 8;
			u |= (Uint32)((unsigned char *)buffer)[1];
			u <<= 8;
			u |= (Uint32)((unsigned char *)buffer)[2];
			u <<= 8;
			u |= (Uint32)((unsigned char *)buffer)[3];
		}
	}
	else {
		u =  (Uint32)((unsigned char *)buffer)[3];
		u <<= 8;
		u |= (Uint32)((unsigned char *)buffer)[2];
		u <<= 8;
		u |= (Uint32)((unsigned char *)buffer)[1];
		u <<= 8;
		u |= (Uint32)((unsigned char *)buffer)[0];
	}
	return u;
}

static bool
copyAll(DicomInputStream& in,ostream& out,TextOutputStream& log,ElementDictionary& dict,
		AttributeList *replaceafterlist,AttributeList *replacebeforelist,AttributeList *deletelist,
		bool removeprivate,bool removeinstanceuid)
{
//cerr << "copyAll(): start" << endl;
	if (in.haveMetaHeader()) {
		char buffer[132];
		char *ptr=buffer;
		for (int i=0; i<128; ++i) {
			*ptr++=0;
		}
		buffer[128]='D';
		buffer[129]='I';
		buffer[130]='C';
		buffer[131]='M';
		out.write(buffer,132);
	}
	
	bool lastwasencapOXorItem=false;
	bool donewithmetaheader=false;
	while (in.peek() != EOF) {
		Endian endian = in.getEndian();			// update each time through in case changed after metaheader
		bool swapped32big = in.isSwapped32Big();
		char buffer[6];
		in.read(buffer,4);
		if (in.fail()) {
			cerr << EMsgDC(TagReadFailed) << endl;
			return false;
		}
		Uint16 group = extract16(buffer,endian);
		Uint16 element = extract16(buffer+2,endian);
		Tag tag(group,element);
//tag.write(log,&dict);
//cerr << endl;
		if (!donewithmetaheader && !tag.isMetaheaderGroup()) {
			if (in.getTransferSyntaxToReadDataSet()) {
				in.readingDataSet();
				donewithmetaheader=true;
				// backup and read again in case endian changed ...
				in.seekg(-4l,ios::cur);
				if (in.fail()) {
					cerr << EMsgDC(TagReadFailed) << " - " << EMsgDC(SeekFailed) << endl;
					return false;
				}
				in.read(buffer,4);
				if (in.fail()) {
					cerr << EMsgDC(TagReadFailed) << endl;
					return false;
				}
				group = extract16(buffer,endian);
				element = extract16(buffer+2,endian);
			}
			else {
				cerr << EMsgDC(NoTransferSyntaxInMetaHeader) << endl;
				return false;
			}
		}
		out.write(buffer,4);			// we always write - "deletion" is actually replacement by null
		
		char vre[3];
		const char *vr;
		Uint32 vl;

		if (tag == TagFromName(ItemDelimitationItem)
		 || tag == TagFromName(SequenceDelimitationItem)
		 || tag == TagFromName(Item)) {
			vr=0;
			in.read(buffer,4);
			if (in.fail()) {
				cerr << EMsgDC(VLReadFailed) << endl;
				return false;
			}
			out.write(buffer,4);
			vl = extract32(buffer,endian,swapped32big);
		}
		else  {
			if (in.getTransferSyntaxInUse()->isExplicitVR()) {
				in.read(vre,2);
				if (in.fail()) {
					cerr << EMsgDC(VRReadFailed) << endl;
					return false;
				}
				out.write(vre,2);
				vre[2]=0;
				vr=vre;
				if (isOtherByteOrWordOrFloatVR(vr) || isSequenceVR(vr)) {
					in.read(buffer,6);	// "Reserved" + VL
					if (in.fail()) {
						cerr << EMsgDC(VLReadFailed) << endl;
						return false;
					}
					out.write(buffer,6);
					vl = extract32(buffer+2,endian,swapped32big);
				}
				else if (isUnknownVR(vr) || isUnlimitedTextVR(vr)) {
					in.read(buffer,2);
					if (in.fail()) {
						cerr << EMsgDC(VLReadFailed) << endl;
						return false;
					}
					out.write(buffer,2);
					vl = extract16(buffer,endian);
					if (vl == 0) {		// if reserved bytes not zero, assume used for short form of length (risky) :(
						in.read(buffer,4);	// VL
						if (in.fail()) {
							cerr << EMsgDC(VLReadFailed) << endl;
							return false;
						}
						out.write(buffer,4);
						vl = extract32(buffer,endian,swapped32big);
					}
					else {
						cerr << EMsgDC(IncorrectShortVLForUTOrUN) << endl;
					}
				}
				else {
					in.read(buffer,2);
					if (in.fail()) {
						cerr << EMsgDC(VLReadFailed) << endl;
						return false;
					}
					out.write(buffer,2);
					vl = extract16(buffer,endian);
				}
			}
			else {
				vr=(tag.isPrivateGroup() && tag.isPrivateOwner())
					? "LO"	// PS3.5-7.8.1
					: dict.getValueRepresentation(tag);
				in.read(buffer,4);
				if (in.fail()) {
					cerr << EMsgDC(VLReadFailed) << endl;
					return false;
				}
				out.write(buffer,4);
				vl = extract32(buffer,endian,swapped32big);
			}
		}
//cerr << "VR = " << dec << vr << endl;
//cerr << "VL = " << dec << vl << endl;
		
		if (vl == 0xffffffff		// SQ or OX both will contain items
		 || vl == 0			// includes ItemDelimitationItem,SequenceDelimitationItem
		 || isSequenceVR(vr)		// SQ with defined length
		 || (tag == TagFromName(Item) && !lastwasencapOXorItem)	// Item with element contents
		) {
			// do nothing ... we have already written all there is to this element
		}
		else {
			bool zeroThisElement = (deletelist && (*deletelist)[tag])
					    || (replaceafterlist && (*replaceafterlist)[tag])
					    || (replacebeforelist && (*replacebeforelist)[tag])
					    || (removeprivate && tag.isPrivateGroup() && !tag.isPrivateOwner() && !tag.isLengthElement())
					    || (removeinstanceuid && isUIStringVR(vr)
					        && tag != TagFromName(TransferSyntaxUID)
					        && tag != TagFromName(SOPClassUID)
					        && tag != TagFromName(MediaStorageSOPClassUID)
					        && tag != TagFromName(ImplementationClassUID)
					       )
					    ;

			if (isStringVR(vr)) {
//cerr << "isStringVR" << endl;
				char buffer[vl+1];
				in.read(buffer,vl);
				buffer[vl]=0;
				if (in.fail() || in.gcount() != vl) {
					cerr << EMsgDC(ReadFailed) << endl;
					return false;
				}
				else {
					if (!zeroThisElement) {
						out.write(buffer,vl);
					}
					// else replacement written later after we are finished with value in buffer
					char *ptr=buffer+vl;
					while (ptr >= buffer && (*ptr == 0 || isspace(*ptr))) *ptr--=0;		// null trailing spaces
					ptr=buffer;
					while (*ptr && isspace(*ptr)) ++ptr;					// skip leading spaces
					if (*ptr) {												// only if not empty string
						if (tag == TagFromName(TransferSyntaxUID)) {
							in.setTransferSyntaxToReadDataSet(new TransferSyntax(ptr));
						}
						else if (tag.isPrivateGroup() && tag.isPrivateOwner()) {
							dict.addOwner(tag,ptr);
						}
					}
					if (zeroThisElement) {
						if (isAgeStringVR(vr) && vl == 4) {
							const char *replacementString = "000Y";
							out.write(replacementString,vl);
						}
						else if (isDateStringVR(vr) && vl == 8) {
							const char *replacementString = "19000101";
							out.write(replacementString,vl);
						}
						else if (isDateStringVR(vr) && vl == 10) {
							const char *replacementString = "1900.01.01";
							out.write(replacementString,vl);
						}
						else if (isTimeStringVR(vr) && vl <= 14) {
							const char *replacementString = "000000.000000 ";
							out.write(replacementString,vl);
						}
						else if (isTimeStringVR(vr) && vl <= 16) {
							const char *replacementString = "00:00:00.0000000 ";
							out.write(replacementString,vl);
						}
						else if (isDateTimeStringVR(vr) && vl <= 26) {
							const char *replacementString = "19000101000000.000000+0000";
							out.write(replacementString,vl);
						}
						else if (isUIStringVR(vr) && vl <= 64) {
							const char *replacementString = "1234567891234567891234567891234567891234567891234567891234567891";
							Assert(strlen(replacementString) == 64);
							out.write(replacementString,vl);
						}
						else if (tag == TagFromName(PatientSex) && vl == 2) {
							const char *replacementString = "O ";
							out.write(replacementString,vl);
						}
						else {
							long lengthToReplace = vl;
							bool numeric = isNumericOrDateOrTimeOrUIStringVR(vr) || (*buffer && isdigit(*buffer));	// e.g. for a string private attribute
							char replacementValue = numeric ? '1' : 'X';
							char subsequentReplacementValue = numeric ? ' ' : 'X';
							char *ptr=buffer;
							for (long i=0; i<lengthToReplace; ++i) {
								*ptr++=replacementValue;
								replacementValue=subsequentReplacementValue;
							}
							out.write(buffer,vl);
						}
					}
				}
			}
			else {
				if (zeroThisElement) {
					long lengthToReplace = vl;
					char replacementValue = 0;
					char buffer[1];					// blech - could be faster :(
					for (long i=0; i<lengthToReplace; ++i) {
						in.read(buffer,1);
						if (in.fail()) {
							cerr << EMsgDC(ReadFailed) << endl;
							return false;
						}
						buffer[0] = replacementValue;
						out.write(buffer,1);
					}
				}
				else {
					long lengthToCopy = vl;
					size_t bufsize=1000;
					char buffer[bufsize];
					while (lengthToCopy > 0) {
						in.read(buffer,lengthToCopy < bufsize ? lengthToCopy : bufsize);
						size_t count = in.gcount();
						if (count > 0) {
							out.write(buffer,count);
							lengthToCopy-=count;
						}
						else {
							break;
						}
					}
					if (in.fail() || lengthToCopy > 0) {
						cerr << EMsgDC(ReadFailed) << endl;
						return false;
					}
				}
			}
		}
		lastwasencapOXorItem=
			(isOtherByteOrWordVR(vr)
			 || tag == TagFromName(Item)
			 || tag == TagFromName(ItemDelimitationItem)
			) && in.getTransferSyntaxInUse()->isEncapsulated();
	}
	return true;
}

int
main(int argc, char *argv[])
{
	GetNamedOptions    	options(argc,argv);
	DicomInputOptions 	input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	input_options.done();
	dicom_output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,input_options.filename,cin);
	OutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << options.errors();
	cerr << input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< input_options.usage()
			<< dicom_output_options.usage()
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din(*(istream *)input_opener,
		input_options.transfersyntaxuid,
		input_options.usemetaheader);
	ostream out(output_opener);
	TextOutputStream log(cerr);
	ElementDictionary dictionary;

	copyAll(din,out,log,dictionary,
		dicom_output_options.replaceafterlist,dicom_output_options.replacebeforelist,dicom_output_options.deletelist,
		dicom_output_options.removeprivate,dicom_output_options.removeinstanceuid);
}



