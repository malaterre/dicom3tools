#include "attrmxls.h"
#include "transynu.h"
#include "mesgtext.h"
#include "bnopt.h"
#include "dcopt.h"

#include "xxxx.h"

int
main(int argc,char **argv)
{
	GetNamedOptions 	options(argc,argv);
	BinaryInputOptions 	binary_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

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
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< binary_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose]"
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

	XXXX_Conversion convertor(bin,cerr);
	convertor.convertAll(&list,&transfersyntax,imagenumber);

	if (!usualManagedAttributeListWrite(list,dout,
		dicom_output_options,log,verbose)) return 1;

	return 0;
}

