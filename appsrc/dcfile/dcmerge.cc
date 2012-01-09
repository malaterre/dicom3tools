#include "attrmxls.h"
#include "mesgtext.h"
#include "dcopt.h"

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options1(options);
	DicomInputOptions 	dicom_input_options2(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	dicom_input_options1.done();
	dicom_input_options2.done();
	dicom_output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener1(
		options,dicom_input_options1.filename);
	DicomInputOpenerFromOptions input_opener2(
		options,dicom_input_options2.filename);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options1.errors();
	cerr << dicom_input_options2.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener1.errors();
	cerr << input_opener2.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options1.good()
	 || !dicom_input_options2.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener1.good()
	 || !input_opener2.good()
	 || !output_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options1.usage()
			<< dicom_input_options2.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose]"
			<< MMsgDC(InputFile) << " " << MMsgDC(InputFile)
			<< "[" << MMsgDC(OutputFile) << "]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din1(*(istream *)input_opener1,
		dicom_input_options1.transfersyntaxuid,
		dicom_input_options1.usemetaheader);
	DicomInputStream din2(*(istream *)input_opener2,
		dicom_input_options2.transfersyntaxuid,
		dicom_input_options2.usemetaheader);
	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream  log(cerr);
	if (verbose) log << "******** While reading 1st dataset ... ********" << endl; 
	list.read(din1,&log,verbose,0xffffffff,true,dicom_input_options1.uselengthtoend,dicom_input_options1.ignoreoutofordertags,dicom_input_options1.useUSVRForLUTDataIfNotExplicit);
	if (verbose) log << "******** While reading 2nd dataset ... ********" << endl; 
	list.read(din2,&log,verbose,0xffffffff,true,dicom_input_options2.uselengthtoend,dicom_input_options2.ignoreoutofordertags,dicom_input_options2.useUSVRForLUTDataIfNotExplicit);

	if (!list.good()) {
		log << list.errors()
		    << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	else {
		// Don't write list warnings yet ...
		// done in usualManagedAttributeListWrite ...

		if (!usualManagedAttributeListWrite(list,dout,
			dicom_output_options,log,verbose)) success=false;
	}

	return success ? 0 : 1;
}

	
