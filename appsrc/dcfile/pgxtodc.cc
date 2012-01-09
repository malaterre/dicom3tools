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
pgx_getc(istream &istr)		// 1st non-comment, non-whitespace, 0 on failure
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
//cerr << "pgx_getc: returns " << c << endl;
	return c;
}

static
int
pgx_getu(istream &istr,int pushedBackChar)		// get unsigned value, -1 on failure
{
//cerr << "pgx_getu: pushedBackChar " << pushedBackChar << " " << char(pushedBackChar) << endl;
	char c = pushedBackChar == -1
		? pgx_getc(istr)			// skips comments and whitespace
		: pushedBackChar;			// skipping already done ... start with this character
	if(!c || !isdigit(c)) return -1;
	unsigned value=0;
	while (c && isdigit(c)) {
		value*=10;
		value+=(c-'0');
		istr.get(c);
	}
	// discards single trailing white space
//cerr << "pgx_getu: returns " << value << endl;
	return value;
}

static
int
pgx_getu(istream &istr)		// get unsigned value, -1 on failure
{
	return pgx_getu(istr,-1);
}

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	InputOptions		input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

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
			<< " [-v|-verbose]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	istream in(input_opener);

/*
       PGX is a trivial monochrome file format defined just
       to enable simple testing of JPEG2000 with image data having unusual
       bit-depths. The file consists of a single text header line of the form
       "PG <byte order> [+|-]<bit-depth> <cols> <rows>", with the binary data
       appearing immediately after the newline character, packed into 1, 2 or
       4 bytes per sample, depending upon the value of <bit-depth>. The <byte
       order> field must be one of the fixed strings, "LM" (LSB's first) or
       "ML" (MSB's first), while the optional `+' (default) or `-' character
       preceding the <bit-depth> field indicates whether the data is signed
       or not. Any bit-depth from 1 to 32 is acceptable.
*/

	char magic=pgx_getc(in);
	char format;
	in.get(format);

	if (!in || magic != 'P' || format != 'G') {
		cerr << EMsgDC(UnrecognizedFormat) << endl;
		return 1;
	}

	char endian1=pgx_getc(in);
	char endian2;
	in.get(endian2);

	bool littleendian=false;
	bool bigendian=false;

	if (!in) {
		cerr << EMsgDC(UnrecognizedFormat) << endl;
		return 1;
	}
	else if (endian1 == 'L' && endian2 == 'M') {
		littleendian=true;
	}
	else if (endian1 == 'M' && endian2 == 'L') {
		bigendian=true;
	}
	else {
		cerr << EMsgDC(UnrecognizedFormat) << " - endian not LM or ML, was " << endian1 << endian2 << endl;
		return 1;
	}
	
	char sign=pgx_getc(in);
	int bits=-1;
	if (!in) {
		cerr << EMsgDC(UnrecognizedFormat) << endl;
		return 1;
	}
	else if (sign == '+' || sign == '-') {
		bits = pgx_getu(in);		// skip to and fetch the bit depth
	}
	else if (isdigit(sign)) {
		bits = pgx_getu(in,sign);	// use what we thought was the sign as the start of the bit depth
		sign='+';			// when sign is missing it is assumed to be + (i.e. always +ve, not signed)
	}
	else {
		cerr << EMsgDC(UnrecognizedFormat) << " - sign not + or - or start of bit depth, was " << sign << endl;
		return 1;
	}

	int cols = pgx_getu(in);
	int rows = pgx_getu(in);

	unsigned samples = 1;
	const char *photometricInterpretation ="MONOCHROME2";

	// Last whitespace absorbed ... at start of raw pixel data

	long offset=in.tellg();

	if (!in || cols == -1 || rows == -1 || bits == -1) {
		cerr << EMsgDC(UnrecognizedFormat) << endl;
		return 1;
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
	list+=new UnsignedShortAttribute(TagFromName(PixelRepresentation),sign == '-' ? 1u : 0u);
	list+=new CodeStringAttribute(TagFromName(PhotometricInterpretation),photometricInterpretation);

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

