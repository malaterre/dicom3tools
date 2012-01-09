#include "attrmxls.h"
#include "mesgtext.h"
#include "dcopt.h"

#include "pixposn.h"

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool showfilename=options.get("filename");

	bool bad=false;
	unsigned row,col;

	bad|=!(options.get("row",row) || options.get("y",row));
	bad|=!(options.get("col",col) || options.get("x",col));

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
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< " [-v|-verbose]"
			<< " [-filename]"
			<< " -[col|x] n -[row|y] n"
			<< " [" << MMsgDC(InputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		return 1;
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

	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	const char *errors=list.errors();
	if (errors) log << errors << flush;
	if (!list.good()) {
		log << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}

	PositionOfPixel positioner(list,log,true);
	Float64 patientX,patientY,patientZ;
	if (positioner.getPosition(row,col,patientX,patientY,patientZ)) {
		cout << dec
		     << "\tRow=" << row
		     << "\tCol=" << col
		     << setiosflags(ios::fixed|ios::showpoint)
		     << "\tX=" <<  setprecision(1) << patientX
		     << "\tY=" <<  setprecision(1) << patientY
		     << "\tZ=" <<  setprecision(1) << patientZ
		     << endl;
	}
	else {
		// errors already logged
		success=false;
	}

	return success ? 0 : 1;
}

