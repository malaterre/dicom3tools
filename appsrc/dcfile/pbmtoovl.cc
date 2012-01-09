#include <ctype.h>	// for isdigit()

#include "attrtype.h"
#include "attrothr.h"
#include "attrmxls.h"
#include "transynu.h"
#include "elmconst.h"
#include "sopclu.h"
#include "mesgtext.h"
#include "ioopt.h"
#include "dcopt.h"

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
	bool standalone=options.get("standalone");

	int overlaygroup = 0;
	(void)options.get("overlaygroup",overlaygroup);

	Assert(overlaygroup <= 0x1e && overlaygroup%2 == 0);
			
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
			<< " [-standalone]"
			<< " [-overlaygroup 0x6000+n]"
			<< " [-v|-verbose]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	istream in(input_opener);
	BinaryInputStream bin(in,LittleEndian);

	char magic=pnm_getc(bin);
	char format;
	bin.get(format);

	if (!bin || magic != 'P') {
		cerr << EMsgDC(UnrecognizedFormat) << endl;
		return 1;
	}

	switch (format) {
		case '1':
		case '2':
		case '3':
		case '5':
		case '6':
			cerr << EMsgDC(Unsupported) << " - P" << format << endl;
			return 1;
		case '4':		// only raw bitmap (PBM) supported
			break;
		default:
			cerr << EMsgDC(UnrecognizedFormat) << endl;
			return 1;
	}

	int cols = pnm_getu(bin);
	int rows = pnm_getu(bin);

	// Last whitespace absorbed ... at start of raw data

	long offset=bin.tellg();

	if (!bin || cols == -1 || rows == -1) {
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

	list+=new UnsignedShortAttribute(Tag(0x6000+overlaygroup,OverlayBitsAllocated_ELEMENT),1);
	list+=new UnsignedShortAttribute(Tag(0x6000+overlaygroup,OverlayBitPosition_ELEMENT),0);
	list+=new UnsignedShortAttribute(Tag(0x6000+overlaygroup,OverlayRows_ELEMENT),rows);
	list+=new UnsignedShortAttribute(Tag(0x6000+overlaygroup,OverlayColumns_ELEMENT),cols);
	{
		SignedShortAttribute *a=new SignedShortAttribute(Tag(0x6000+overlaygroup,OverlayOrigin_ELEMENT));
		Assert(a); a->addValue(Int16(1)); a->addValue(Int16(1));
		list+=a;
	}
	list+=new CodeStringAttribute(Tag(0x6000+overlaygroup,OverlayType_ELEMENT),"G");

	if (standalone) {
		list+=new IntegerStringAttribute(TagFromName(OverlayNumber));

		list+=new UIStringAttribute(TagFromName(SOPClassUID),StandaloneOverlayStorageSOPClassUID);

		// What is necessary to make a valid instance ...
		// (this also triggers the default make DICOM procedures in attmxls.cc
		// to recognize as Composite, and file in UIDs etc.)

		list+=new PersonNameAttribute(TagFromName(PatientName));
		list+=new LongStringAttribute(TagFromName(PatientID));
		list+=new DateStringAttribute(TagFromName(PatientBirthDate));
		list+=new CodeStringAttribute(TagFromName(PatientSex));
		list+=new DateStringAttribute(TagFromName(StudyDate));
		list+=new TimeStringAttribute(TagFromName(StudyTime));
		list+=new PersonNameAttribute(TagFromName(ReferringPhysicianName));
		list+=new ShortStringAttribute(TagFromName(StudyID));
		list+=new ShortStringAttribute(TagFromName(AccessionNumber));
		list+=new CodeStringAttribute(TagFromName(Modality),"OT");
		list+=new IntegerStringAttribute(TagFromName(SeriesNumber));
		list+=new LongStringAttribute(TagFromName(Manufacturer));
	}

	OtherWordLargeNonPixelAttribute *overlayData=
		new OtherWordLargeNonPixelAttribute(Tag(0x6000+overlaygroup,OverlayData_ELEMENT),bin,offset);
	Assert(overlayData);
	overlayData->read(bin,((cols*rows-1)/16+1)*2);		// Actually just sets VL and skips
	list+=overlayData;

	if (!usualManagedAttributeListWrite(list,dout,
		dicom_output_options,log,verbose)) return 1;

	return 0;
}

