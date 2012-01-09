#include "attrmxls.h"
#include "attrothr.h"
#include "mesgtext.h"
#include "ioopt.h"
#include "dcopt.h"
#include "elmconst.h"

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	OutputOptions		output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool quiet=options.get("quiet") || options.get("silent");

	dicom_input_options.done();
	output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,dicom_input_options.filename,cin);
	OutputOpenerFromOptions output_opener(
		options,output_options.filename,cout);

	cerr << dicom_input_options.errors();
	cerr << output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options.good()
	 || !output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< output_options.usage()
			<< " [-v|-verbose]"
			<< " [-quiet|-silent]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din(*(istream *)input_opener,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);
	ostream out(output_opener);
	BinaryOutputStream bout(out,NoEndian);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream log(cerr);
	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	const char *errors=list.errors();
	if (errors) log << errors << flush;
	if (!list.good()) {
		log << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}

	Attribute *apdfdata=list[TagFromName(EncapsulatedDocument)];
	if (!apdfdata) {
		log << EMsgDC(MissingAttribute) << " - \"EncapsulatedDocument\"" << endl;
		success=false;
	}
	else if (!apdfdata->isOtherByteNonPixel()) {
		log << EMsgDC(EncapsulatedDataIncorrectVR) << endl;
		success=false;
	}
	else {
		if (!quiet) {
			log << "******** Parameters ... ********" << endl; 
			ElementDictionary *dict=list.getDictionary();
			Attribute *a;
			a=list[TagFromName(MIMETypeOfEncapsulatedDocument)];
			if (a) { a->write(log,dict); log << endl; }
		}

		apdfdata->writeData(bout);

		if (!bout.good()) {
			log << EMsgDC(Writefailed) << endl;
			success=false;
		}
	}

	return success ? 0 : 1;
}

	
