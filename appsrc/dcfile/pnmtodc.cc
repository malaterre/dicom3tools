#include <ctype.h>	// for isdigit()

#include "attrtype.h"
#include "attrothr.h"
#include "attrmxls.h"
#include "transynu.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "ioopt.h"
#include "dcopt.h"
#include "rawsrc.h"

static
char
pnm_getc(istream &istr)		// 1st non-comment, non-whitespace, 0 on failure
{
	char c;
	while (istr.get(c) && (c == ' ' || c == '\t' || c == '\n'));
	if (!istr) return 0;
	if (c == '#') {		// skip comments
		while (istr.get(c) && c != '\n');
		if (!istr) return 0;
		while (istr.get(c) && (c == ' ' || c == '\t' || c == '\n'));
		if (!istr) return 0;
	}
	return c;
}

static
int
pnm_getu(istream &istr)		// get unsigned value, -1 on failure
{
	char c=pnm_getc(istr);	// skips comments and whitespace
	if(!c || !isdigit(c)) return -1;
	unsigned value=0;
	while (c && isdigit(c)) {
		value*=10;
		value+=(c-'0');
		istr.get(c);
	}
	// discards single trailing white space
	return value;
}

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	InputOptions		input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	bool littleendian=options.get("little") || options.get("l");
	bool bigendian=options.get("big") || options.get("b");

	bool imagepixelmoduleonly=options.get("imagepixelmoduleonly");

	input_options.done();
	dicom_output_options.done();
	options.done();

	InputOpenerFromOptions input_opener(
		options,input_options.filename,cin);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< input_options.usage()
			<< dicom_output_options.usage()
			<< " [-little|-l|-big|-b]"
			<< " [-imagepixelmoduleonly]"
			<< " [-v|-verbose]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	istream in(input_opener);

	char magic=pnm_getc(in);
	char format;
	in.get(format);

	if (!in || magic != 'P') {
		cerr << EMsgDC(UnrecognizedFormat) << endl;
		return 1;
	}

	unsigned samples;
	const char *photometricInterpretation;
	unsigned planarConfiguration;

	switch (format) {
		case '1':
		case '2':
		case '3':
		case '4':
			cerr << EMsgDC(Unsupported) << " - P" << format << endl;
			return 1;
		case '5':
			samples=1;
			photometricInterpretation="MONOCHROME2";
			break;
		case '6':
			samples=3;
			photometricInterpretation="RGB";
			planarConfiguration=0;	// color by pixel (RGB order)
			break;
		default:
			cerr << EMsgDC(UnrecognizedFormat) << endl;
			return 1;
	}

	int cols = pnm_getu(in);
	int rows = pnm_getu(in);
	int maxval = pnm_getu(in);

	// Last whitespace absorbed ... at start of raw pixel data

	long offset=in.tellg();

	if (!in || cols == -1 || rows == -1 || maxval == -1) {
		cerr << EMsgDC(UnrecognizedFormat) << endl;
		return 1;
	}

	unsigned bits;
	unsigned test;
	for (test=maxval,bits=0; test; test=test>>1,++bits);
	if (bits < 1 || bits > 16) {
		cerr << EMsgDC(Unsupported) << " - bits = " << dec << bits << endl;
		return 1;
	}

	if (bits > 8 && !(littleendian || bigendian)) {
		cerr << EMsgDC(OptionsIncompatible) << " - bits > 8 && !(little|big)" << endl;
		return 1;
	}
	else if (bits <= 8 && (littleendian || bigendian)) {
		cerr << EMsgDC(OptionsIncompatible) << " - bits <= 8 & (little|big)" << endl;
		return 1;
	}
	else if (bits <= 8) {
		littleendian = true;
		// bigendian already false
	}

	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	ManagedAttributeList list;

	TransferSyntax transfersyntax(
		dicom_output_options.transfersyntaxuid
		? dicom_output_options.transfersyntaxuid
		: DefaultTransferSyntaxUID);

	TextOutputStream log (cerr);
	if (verbose) log << "Converting ..." << endl;

	Uint16 bitsAllocated=((bits-1u)/8u+1u)*8u;
	Uint16 bitsStored=bits;
	Uint16 highBit=bits-1;
	Uint16 frames=1;

	list+=new UnsignedShortAttribute(TagFromName(BitsAllocated),bitsAllocated);
	list+=new UnsignedShortAttribute(TagFromName(BitsStored),bitsStored);
	list+=new UnsignedShortAttribute(TagFromName(HighBit),highBit);
	list+=new UnsignedShortAttribute(TagFromName(Rows),rows);
	list+=new UnsignedShortAttribute(TagFromName(Columns),cols);
	list+=new IntegerStringAttribute(TagFromName(NumberOfFrames),frames);
	list+=new UnsignedShortAttribute(TagFromName(SamplesPerPixel),samples);
	list+=new UnsignedShortAttribute(TagFromName(PixelRepresentation),0u);
	list+=new CodeStringAttribute(TagFromName(PhotometricInterpretation),photometricInterpretation);
	if (samples > 1) list+=new UnsignedShortAttribute(TagFromName(PlanarConfiguration),planarConfiguration);

	if (!imagepixelmoduleonly) {
		// various Type 1 and Type 2 attributes for mandatory SC modules ...

		list+=new PersonNameAttribute(TagFromName(PatientName),"^^^^");
		list+=new LongStringAttribute(TagFromName(PatientID));
		list+=new DateStringAttribute(TagFromName(PatientBirthDate));
		list+=new CodeStringAttribute(TagFromName(PatientSex));
		list+=new ShortStringAttribute(TagFromName(StudyID));
		list+=new DateStringAttribute(TagFromName(StudyDate));
		list+=new TimeStringAttribute(TagFromName(StudyTime));
		list+=new PersonNameAttribute(TagFromName(ReferringPhysicianName),"^^^^");
		list+=new ShortStringAttribute(TagFromName(AccessionNumber));
		list+=new IntegerStringAttribute(TagFromName(SeriesNumber));
		list+=new IntegerStringAttribute(TagFromName(InstanceNumber));

		list+=new CodeStringAttribute(TagFromName(Modality),"OT");
		list+=new CodeStringAttribute(TagFromName(ConversionType),"WSD");

		list+=new LongStringAttribute(TagFromName(Manufacturer));
		list+=new CodeStringAttribute(TagFromName(PatientOrientation));
	}
	
	Raw_PixelDataSourceBase *pixeldatasrc=0;
	if (bits <= 8) {
		pixeldatasrc=new Raw_PixelDataSource_Byte(in,offset,rows,cols,frames,samples);
	}
	else if (littleendian) {
		pixeldatasrc=new Raw_PixelDataSource_LittleEndian(in,offset,rows,cols,frames,samples);
	}
	else if (bigendian) {
		pixeldatasrc=new Raw_PixelDataSource_BigEndian(in,offset,rows,cols,frames,samples);
	}
	Assert(pixeldatasrc);

	list+=new OtherUnspecifiedLargeAttribute(
		TagFromName(PixelData),
		pixeldatasrc,
		rows,
		cols,
		frames,
		samples,
		&transfersyntax,
		0,bitsAllocated,bitsStored,highBit);

	if (!usualManagedAttributeListWrite(list,dout,
		dicom_output_options,log,verbose)) return 1;

	return 0;
}

