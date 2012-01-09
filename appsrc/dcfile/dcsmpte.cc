#include "attrtype.h"
#include "attrothr.h"
#include "attrmxls.h"
#include "transynu.h"
#include "elmconst.h"
#include "mesgtext.h"
//#include "ioopt.h"
#include "dcopt.h"

#include "smptesrc.h"

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	unsigned rows=512;
	(void)(options.get("rows",rows) || options.get("x",rows));

	unsigned cols=512;
	(void)(options.get("cols",cols) || options.get("columns",cols) || options.get("y",cols));

	unsigned bits=8;
	(void)(options.get("bits",bits) || options.get("depth",bits) || options.get("d",bits));

	if (bits < 1 || bits > 16) {
		cerr << EMsgDC(OptionUnsupported) << " - bits = " << dec << bits << endl;
		bad=true;
	}

	int minval=0;
	(void)options.get("minval",minval);

	int maxval=(int)(((long)(0x0001l)<<bits)-1);
	(void)options.get("maxval",maxval);

	bool wantsigned=options.get("signed");

	bool wantinverted=options.get("invert") || options.get("inverted") || options.get("monochrome1");

	dicom_output_options.done();
	options.done();

	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << output_opener.errors();

	if (!dicom_output_options.good()
	 || !options.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_output_options.usage()
			<< " [-rows|-y rows]"
			<< " [-cols|-columns|-x columns]"
			<< " [-bits|-depth|-d bits]"
			<< " [-minval i]"
			<< " [-mavxal i]"
			<< " [-signed]"
			<< " [-inverted|-invert|-monochrome1]"
			<< " [-v|-verbose]"
			<< " [" << MMsgDC(OutputFile) << "]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
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

	TextOutputStream log(cerr);

	Uint16 bitsAllocated=((bits-1)/8+1)*8;
	Uint16 bitsStored=bits;
	Uint16 highBit=bits-1;

	list+=new UnsignedShortAttribute(TagFromName(BitsAllocated),bitsAllocated);
	list+=new UnsignedShortAttribute(TagFromName(BitsStored),bitsStored);
	list+=new UnsignedShortAttribute(TagFromName(HighBit),highBit);
	list+=new UnsignedShortAttribute(TagFromName(Rows),rows);
	list+=new UnsignedShortAttribute(TagFromName(Columns),cols);
	list+=new UnsignedShortAttribute(TagFromName(SamplesPerPixel),1u);
	list+=new UnsignedShortAttribute(TagFromName(PixelRepresentation),wantsigned?1u:0u);
	list+=new CodeStringAttribute(TagFromName(PhotometricInterpretation),wantinverted ? "MONOCHROME1":"MONOCHROME2");

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

	SMPTE_PixelDataSource *pixeldatasrc=new SMPTE_PixelDataSource(rows,cols,wantsigned,wantinverted,(Uint16)minval,(Uint16)maxval);
	Assert(pixeldatasrc);

	list+=new OtherUnspecifiedLargeAttribute(
		TagFromName(PixelData),
		pixeldatasrc,
		rows,
		cols,
		1,
		1,
		&transfersyntax,
		0,bitsAllocated,bitsStored,highBit);

	if (!usualManagedAttributeListWrite(list,dout,
		dicom_output_options,log,verbose)) return 1;

	return 0;
}

