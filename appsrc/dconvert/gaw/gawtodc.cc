#include "attrmxls.h"
#include "transynu.h"
#include "mesgtext.h"
#include "bnopt.h"
#include "dcopt.h"

#include "gaw.h"

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	BinaryInputOptions 	binary_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	int useoptioncount=0;

	bool useawct=options.get("awct");
	if (useawct) ++useoptioncount;

	bool useawmr=options.get("awmr");
	if (useawmr) ++useoptioncount;

	int noffset;
	Uint32 offsetargs[5];
	if ((noffset=options.get("offset",offsetargs,5)) != -1 && noffset != 5) {
		cerr << EMsgDC(OptionIncomplete)
		     << " - offset"
		     << endl;
		bad=true;
	}

	bool lengthpresent=options.get("lengthpresent");

	if (useoptioncount && (lengthpresent || noffset != -1)) {
		cerr << EMsgDC(OptionsIncompatible) << endl;
		bad=true; 
	}

	// there is no default ...

	if (!useoptioncount && noffset == -1) {
		cerr << EMsgDC(NeedOption) << " -awct|-awmr|-offset" << endl;
		bad=true; 
	}

	binary_input_options.done();
	dicom_output_options.done();
	options.done();

	BinaryInputOpenerFromOptions input_opener(
		options,binary_input_options.filename,cin);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << binary_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!binary_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< binary_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose]"
			<< " -awct|-awmr"
			<< "|-offset file suite exam series image [-lengthpresent]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	BinaryInputStream bin(*(istream *)input_opener);
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

	GAW_Conversion *convertor=0;
	if (noffset == 5)
		convertor=new GAW_Conversion(bin,cerr,
			true,
			lengthpresent,
			offsetargs[0],
			offsetargs[1],
			offsetargs[2],
			offsetargs[3],
			offsetargs[4]);
	else if (useawct)
		convertor=new GAW_Conversion(bin,cerr,
			true,	// explicit
			false,	// length field
			3240,	// file header
			0,	// suite header
			116,	// exam header
			1156,	// series header
			2184);	// image header
	else if (useawmr)
		convertor=new GAW_Conversion(bin,cerr,
			true,	// explicit
			false,	// length field
			3228,	// file header
			0,	// suite header
			116,	// exam header
			1156,	// series header
			2184);	// image header
	Assert(convertor);

	convertor->convertAll(&list,&transfersyntax);

	return usualManagedAttributeListWrite(list,dout,
		dicom_output_options,log,verbose) ? 0 : 1;
}

