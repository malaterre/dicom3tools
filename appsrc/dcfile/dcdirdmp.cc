#include "attrmxls.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "dicomdir.h"

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool veryverbose=options.get("veryverbose") || options.get("vv");
	bool verbose=options.get("verbose") || options.get("v") || veryverbose;
	bool showrecordinfo=options.get("showrecordinfo");
	bool showdescription=options.get("description");
	bool showpaths=options.get("paths") || options.get("p");
	bool showabstract = !showpaths;
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
			<< " [-v|-verbose|-vv|-veryverbose]"
			<< " [-showrecordinfo]"
			<< " [-description]"
			<< " [-p|-paths]"
			<< " [-filename]"
			<< " [" << MMsgDC(InputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din(*(istream *)input_opener,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream log(cerr);
	
	if (showfilename) {
		const char *filenameused = input_opener.getFilename();
		cerr << "Filename: \"" << (filenameused && strlen(filenameused) > 0 ? filenameused : "-") << "\"" << endl;
	}

	if (veryverbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,veryverbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	const char *errors=list.errors();
	if (errors) log << errors << flush;
	if (!list.good()) {
		log << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}

	//if (veryverbose) log << "******** As read ... ********" << endl; 
	//log << list;
	//list.write(log,veryverbose);

	success=success&&parseDicomdir(list,log,verbose,veryverbose,showrecordinfo,showabstract,showpaths,showdescription);

	return success ? 0 : 1;
}

