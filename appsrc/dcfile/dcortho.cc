#include "attrmxls.h"
#include "attrtype.h"
#include "attrval.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "dcopt.h"

static inline Float32 Abs(Float32 x) { return ((x < 0) ? -x : x); }
static inline Float32 Sqrt(Float32 x) { return sqrt(x); }

static void
MakeImagePlaneModuleOrthogonalToXYZ(ManagedAttributeList &list,
		Float32 originX,Float32 originY,Float32 originZ)
{
//cerr << "originX = " << dec << originX << endl;
//cerr << "originY = " << dec << originY << endl;
//cerr << "originZ = " << dec << originZ << endl;

	Attribute *aImagePositionPatient=list[TagFromName(ImagePositionPatient)];		// DICOM patient relative
	Attribute *aImageOrientationPatient=list[TagFromName(ImageOrientationPatient)];		// DICOM patient relative

	Assert(aImagePositionPatient && aImagePositionPatient->getVM() == 3);
	Assert(aImageOrientationPatient && aImageOrientationPatient->getVM() == 6);

	Float32 position_X; (void)aImagePositionPatient->getValue(0,position_X);
	Float32 position_Y; (void)aImagePositionPatient->getValue(1,position_Y);
	Float32 position_Z; (void)aImagePositionPatient->getValue(2,position_Z);

	Float32 orientation_row_X; (void)aImageOrientationPatient->getValue(0,orientation_row_X);
	Float32 orientation_row_Y; (void)aImageOrientationPatient->getValue(1,orientation_row_Y);
	Float32 orientation_row_Z; (void)aImageOrientationPatient->getValue(2,orientation_row_Z);
	Float32 orientation_col_X; (void)aImageOrientationPatient->getValue(3,orientation_col_X);
	Float32 orientation_col_Y; (void)aImageOrientationPatient->getValue(4,orientation_col_Y);
	Float32 orientation_col_Z; (void)aImageOrientationPatient->getValue(5,orientation_col_Z);

	// Remove the attributes since they will be replaced later

	list-=aImagePositionPatient;
	list-=aImageOrientationPatient;

	delete aImagePositionPatient;
	delete aImageOrientationPatient;

//cerr << "input position_X = " << dec << position_X << endl;
//cerr << "input position_Y = " << dec << position_Y << endl;
//cerr << "input position_Z = " << dec << position_Z << endl;
//cerr << "input orientation_row_X = " << dec << orientation_row_X << endl;
//cerr << "input orientation_row_Y = " << dec << orientation_row_Y << endl;
//cerr << "input orientation_row_Z = " << dec << orientation_row_Z << endl;
//cerr << "input orientation_col_X = " << dec << orientation_col_X << endl;
//cerr << "input orientation_col_Y = " << dec << orientation_col_Y << endl;
//cerr << "input orientation_col_Z = " << dec << orientation_col_Z << endl;

	position_X-=originX;
	position_Y-=originY;
	position_Z-=originZ;

//cerr << "corrected position_X = " << dec << position_X << endl;
//cerr << "corrected position_Y = " << dec << position_Y << endl;
//cerr << "corrected position_Z = " << dec << position_Z << endl;

	// Determine major axis or row and column

	bool major_row_axis_is_X = false;
	bool major_row_axis_is_Y = false;
	bool major_row_axis_is_Z = false;

	bool major_col_axis_is_X = false;
	bool major_col_axis_is_Y = false;
	bool major_col_axis_is_Z = false;

	bool position_axis_is_X = false;
	bool position_axis_is_Y = false;
	bool position_axis_is_Z = false;

	int major_row_axis_sign;
	int major_col_axis_sign;
	int position_axis_sign;

	if (Abs(orientation_row_X) >= Abs(orientation_row_Y)) {
		if (Abs(orientation_row_X) >= Abs(orientation_row_Z)) {
			major_row_axis_is_X=true;
			major_row_axis_sign = (orientation_row_X < 0) ? -1 : 1;
		}
		else {
			major_row_axis_is_Z=true;
			major_row_axis_sign = (orientation_row_Z < 0) ? -1 : 1;
		}
	}
	else {
		if (Abs(orientation_row_Y) >= Abs(orientation_row_Z)) {
			major_row_axis_is_Y=true;
			major_row_axis_sign = (orientation_row_Y < 0) ? -1 : 1;
		}
		else {
			major_row_axis_is_Z=true;
			major_row_axis_sign = (orientation_row_Z < 0) ? -1 : 1;
		}
	}

	if (Abs(orientation_col_X) >= Abs(orientation_col_Y)) {
		if (Abs(orientation_col_X) >= Abs(orientation_col_Z)) {
			major_col_axis_is_X=true;
			major_col_axis_sign = (orientation_col_X < 0) ? -1 : 1;
		}
		else {
			major_col_axis_is_Z=true;
			major_col_axis_sign = (orientation_col_Z < 0) ? -1 : 1;
		}
	}
	else {
		if (Abs(orientation_col_Y) >= Abs(orientation_col_Z)) {
			major_col_axis_is_Y=true;
			major_col_axis_sign = (orientation_col_Y < 0) ? -1 : 1;
		}
		else {
			major_col_axis_is_Z=true;
			major_col_axis_sign = (orientation_col_Z < 0) ? -1 : 1;
		}
	}

	// position axis is the remaining one ...

	if      (major_row_axis_is_X && major_col_axis_is_Y) {
		position_axis_is_Z=true;
		position_axis_sign = (position_Z < 0) ? -1 : 1;
	}
	else if (major_row_axis_is_X && major_col_axis_is_Z) {
		position_axis_is_Y=true;
		position_axis_sign = (position_Y < 0) ? -1 : 1;
	}
	else if (major_row_axis_is_Y && major_col_axis_is_Z) {
		position_axis_is_X=true;
		position_axis_sign = (position_X < 0) ? -1 : 1;
	}
	else Assert(0);

//cerr << "major_row_axis_is_X = " << dec << major_row_axis_is_X << endl;
//cerr << "major_row_axis_is_Y = " << dec << major_row_axis_is_Y << endl;
//cerr << "major_row_axis_is_Z = " << dec << major_row_axis_is_Z << endl;
//cerr << "major_col_axis_is_X = " << dec << major_col_axis_is_X << endl;
//cerr << "major_col_axis_is_Y = " << dec << major_col_axis_is_Y << endl;
//cerr << "major_col_axis_is_Z = " << dec << major_col_axis_is_Z << endl;
//cerr << "position_axis_is_X = " << dec << position_axis_is_X << endl;
//cerr << "position_axis_is_Y = " << dec << position_axis_is_Y << endl;
//cerr << "position_axis_is_Z = " << dec << position_axis_is_Z << endl;

//cerr << "major_row_axis_sign = " << dec << major_row_axis_sign << endl;
//cerr << "major_col_axis_sign = " << dec << major_col_axis_sign << endl;
//cerr << "position_axis_sign = "  << dec << position_axis_sign  << endl;

	// goal now is to zero out all except major axis for both
	// position and orientation

	// assume all are vectors from origin of zero ...

	Float32 position_vector_from_origin = Sqrt(position_X*position_X
						  +position_Y*position_Y
						  +position_Z*position_Z) * position_axis_sign;
	Float32 row_vector_from_origin = Sqrt(orientation_row_X*orientation_row_X
					     +orientation_row_Y*orientation_row_Y
					     +orientation_row_Z*orientation_row_Z) * major_row_axis_sign;
	Float32 col_vector_from_origin = Sqrt(orientation_col_X*orientation_col_X
					     +orientation_col_Y*orientation_col_Y
					     +orientation_col_Z*orientation_col_Z) * major_col_axis_sign;

	// Are now going to replace original values (which are being lost) ...

	position_X = position_axis_is_X ? position_vector_from_origin : 0;
	position_Y = position_axis_is_Y ? position_vector_from_origin : 0;
	position_Z = position_axis_is_Z ? position_vector_from_origin : 0;

	orientation_row_X = major_row_axis_is_X ? row_vector_from_origin : 0;
	orientation_row_Y = major_row_axis_is_Y ? row_vector_from_origin : 0;
	orientation_row_Z = major_row_axis_is_Z ? row_vector_from_origin : 0;

	orientation_col_X = major_col_axis_is_X ? col_vector_from_origin : 0;
	orientation_col_Y = major_col_axis_is_Y ? col_vector_from_origin : 0;
	orientation_col_Z = major_col_axis_is_Z ? col_vector_from_origin : 0;


//cerr << "uncorrected position_X = " << dec << position_X << endl;
//cerr << "uncorrected position_Y = " << dec << position_Y << endl;
//cerr << "uncorrected position_Z = " << dec << position_Z << endl;

	position_X+=originX;
	position_Y+=originY;
	position_Z+=originZ;

//cerr << "output position_X = " << dec << position_X << endl;
//cerr << "output position_Y = " << dec << position_Y << endl;
//cerr << "output position_Z = " << dec << position_Z << endl;
//cerr << "output orientation_row_X = " << dec << orientation_row_X << endl;
//cerr << "output orientation_row_Y = " << dec << orientation_row_Y << endl;
//cerr << "output orientation_row_Z = " << dec << orientation_row_Z << endl;
//cerr << "output orientation_col_X = " << dec << orientation_col_X << endl;
//cerr << "output orientation_col_Y = " << dec << orientation_col_Y << endl;
//cerr << "output orientation_col_Z = " << dec << orientation_col_Z << endl;

	// Replace the (already removed) attributes with new ones ...

	aImagePositionPatient=new DecimalStringAttribute(TagFromName(ImagePositionPatient));
	Assert(aImagePositionPatient);
	aImagePositionPatient->addValue(position_X);
	aImagePositionPatient->addValue(position_Y);
	aImagePositionPatient->addValue(position_Z);
	list+=aImagePositionPatient;

	aImageOrientationPatient=new DecimalStringAttribute(TagFromName(ImageOrientationPatient));
	Assert(aImageOrientationPatient);
	aImageOrientationPatient->addValue(orientation_row_X);
	aImageOrientationPatient->addValue(orientation_row_Y);
	aImageOrientationPatient->addValue(orientation_row_Z);
	aImageOrientationPatient->addValue(orientation_col_X);
	aImageOrientationPatient->addValue(orientation_col_Y);
	aImageOrientationPatient->addValue(orientation_col_Z);
	list+=aImageOrientationPatient;
}


int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool success=true;

	bool verbose=options.get("verbose") || options.get("v");

	Float32 originX;
	Float32 originY;
	Float32 originZ;
	if (!options.get("x",originX)) {
		cerr << "Must specify -x origin of 1st slice" << endl;
		success=false;
	}
	if (!options.get("y",originY)) {
		cerr << "Must specify -y origin of 1st slice" << endl;
		success=false;
	}
	if (!options.get("z",originZ)) {
		cerr << "Must specify --z origin of 1st slice" << endl;
		success=false;
	}

	dicom_input_options.done();
	dicom_output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,dicom_input_options.filename,cin);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || !success) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " -x nn.n -y nn.n -z nn.n"
			<< " [-v|-verbose]"
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
	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	ManagedAttributeList list;

	success=true;
	TextOutputStream  log(cerr);
	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	if (!list.good()) {
		log << list.errors()
		    << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	else {
		// Don't write list warnings yet ...
		// done in usualManagedAttributeListWrite ...

		MakeImagePlaneModuleOrthogonalToXYZ(list,originX,originY,originZ);

		if (!usualManagedAttributeListWrite(list,dout,
			dicom_output_options,log,verbose)) success=false;
	}

	return success ? 0 : 1;
}

	
