#include <fstream.h>

#include "attrmxls.h"
#include "attrtype.h"
#include "attrothr.h"
#include "attrval.h"
#include "elmdict.h"
#include "elmconst.h"
#include "sopclu.h"
#include "mesgtext.h"
#include "dcopt.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static ElementDictionary staticDictionary;

// Various functions to check and match values ...

static bool
errorIfStringValuesDontMatch(const char *firstValue,
	const char *newValue,
	const char *valueDescription,
	const char *filename,TextOutputStream &log)
{
	Assert(firstValue);
	Assert(newValue);
	Assert(valueDescription);
	Assert(filename);

	bool result;
	if (strcmp(firstValue,newValue) != 0) {
		log << filename << ": "<< "Error - Different " << valueDescription
		    << " got  <"
		    << (newValue ? newValue : "")
		    << "> previously seen as  <"
		    << (firstValue ? firstValue : "")
		    << "> - using original value"
		    << endl;
		result=false;
	}
	else {
		result=true;
	}
	return result;
}

static bool
errorIfIntegerValuesDontMatch(Uint16 firstValue,
	Uint16 newValue,
	const char *valueDescription,
	const char *filename,TextOutputStream &log)
{
	//Assert(firstValue);	// May legitimately be zero ?
	//Assert(newValue);	// May legitimately be zero ?
	Assert(valueDescription);
	Assert(filename);

	bool result;
	if (firstValue != newValue) {
		log << filename << ": "<< "Error - Different " << valueDescription
		    << " got  <"
		    << newValue
		    << "> previously seen as  <"
		    << firstValue
		    << "> - using original value"
		    << endl;
		result=false;
	}
	else {
		result=true;
	}
	return result;
}

static bool
errorIfFloatValuesDontMatch(double firstValue,
	double newValue,
	const char *valueDescription,
	const char *filename,TextOutputStream &log)
{
	//Assert(firstValue);	// May legitimately be zero ?
	//Assert(newValue);	// May legitimately be zero ?
	Assert(valueDescription);
	Assert(filename);

	bool result;
	if (firstValue != newValue) {
		log << filename << ": "<< "Error - Different " << valueDescription
		    << " got  <"
		    << newValue
		    << "> previously seen as  <"
		    << firstValue
		    << "> - using original value"
		    << endl;
		result=false;
	}
	else {
		result=true;
	}
	return result;
}

static Attribute *
isValuePresentElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Assert(label);
	Assert(filename);
	Attribute *a=list[tag];
	if (a && a->getVM())
		return a;
	else {
		log << filename << ": " 
		    << EMsgDC(MissingAttribute)
		    << " - " << label
		    << endl;
		return 0;
	}
}

static const char *
getStringValueElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *a=isValuePresentElseError(list,tag,label,filename,log);
	char *value=AttributeValue(a);	//SC 2.0.1 didn't like it as ? arg :(
	return a ? value : "";
}

static Uint16
getIntegerValueElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *a=isValuePresentElseError(list,tag,label,filename,log);
	Uint16 value=AttributeValue(a);
	return value;
}

static double
getFloatValueElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *a=isValuePresentElseError(list,tag,label,filename,log);
	double value=AttributeValue(a);
	return value;
}

static const char *
getAndCheckStringMatchElseError(bool first,const char * &requiredValue,AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	const char *value=getStringValueElseError(list,tag,label,filename,log);
	Assert(value);

	if (first) {		// First image file
		requiredValue=value;
	}
	else {
		errorIfStringValuesDontMatch(requiredValue,value,label,filename,log);
	}

	return value;
}

static Uint16
getAndCheckIntegerMatchElseError(bool first,Uint16 &requiredValue,AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Uint16 value=getIntegerValueElseError(list,tag,label,filename,log);
	//Assert(value);	// May legitimately be zero ?

	if (first) {		// First image file
		requiredValue=value;
	}
	else {
		errorIfIntegerValuesDontMatch(requiredValue,value,label,filename,log);
	}

	return value;
}

static double
getAndCheckFloatMatchElseError(bool first,double &requiredValue,AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	double value=getFloatValueElseError(list,tag,label,filename,log);
	//Assert(value);	// May legitimately be zero ?

	if (first) {		// First image file
		requiredValue=value;
	}
	else {
		errorIfFloatValuesDontMatch(requiredValue,value,label,filename,log);
	}

	return value;
}



// Function to get value to sort using ImagePositionPatient ...

static double
getDistanceAlongNormalToImagePlane(bool first,AttributeList &list,
	double &reference_tlhc_X,double &reference_tlhc_Y,double &reference_tlhc_Z,
	const char *filename,TextOutputStream &log)
{
	Attribute *aImagePositionPatient=isValuePresentElseError(list,
		TagFromName(ImagePositionPatient),"Image Position Patient",filename,log);
	Attribute *aImageOrientationPatient=isValuePresentElseError(list,
		TagFromName(ImageOrientationPatient),"Image Orientation Patient",filename,log);

	if (!aImagePositionPatient || !aImageOrientationPatient) return 0;

	if (aImagePositionPatient->getVM() != 3) {
		log << filename << ": " 
		    << EMsgDC(BadAttributeValueMultiplicity)
		    << " - Image Position Patient - "
		    << MMsgDC(Expected) << " 3 "
		    << MMsgDC(Got) << " " << aImagePositionPatient->getVM()
		    << endl;
		return 0;
	}

	if (aImageOrientationPatient->getVM() != 6) {
		log << filename << ": " 
		    << EMsgDC(BadAttributeValueMultiplicity)
		    << " - Image Orientation Patient - "
		    << MMsgDC(Expected) << " 6 "
		    << MMsgDC(Got) << " " << aImageOrientationPatient->getVM()
		    << endl;
		return 0;
	}

	double tlhc_X; aImagePositionPatient->getValue(0,tlhc_X);
	double tlhc_Y; aImagePositionPatient->getValue(1,tlhc_Y);
	double tlhc_Z; aImagePositionPatient->getValue(2,tlhc_Z);

//cerr << "tlhc_X = " << tlhc_X << endl;
//cerr << "tlhc_Y = " << tlhc_Y << endl;
//cerr << "tlhc_Z = " << tlhc_Z << endl;

	double distance;

	// if first time, set reference TLHC to current TLHC and return distance of zero

	if (first) {
		reference_tlhc_X=tlhc_X;
		reference_tlhc_Y=tlhc_Y;
		reference_tlhc_Z=tlhc_Z;
	}

	{
		double row_X; aImageOrientationPatient->getValue(0,row_X);
		double row_Y; aImageOrientationPatient->getValue(1,row_Y);
		double row_Z; aImageOrientationPatient->getValue(2,row_Z);

//cerr << "row_X = " << row_X << endl;
//cerr << "row_Y = " << row_Y << endl;
//cerr << "row_Z = " << row_Z << endl;

		double col_X; aImageOrientationPatient->getValue(3,col_X);
		double col_Y; aImageOrientationPatient->getValue(4,col_Y);
		double col_Z; aImageOrientationPatient->getValue(5,col_Z);

//cerr << "col_X = " << col_X << endl;
//cerr << "col_Y = " << col_Y << endl;
//cerr << "col_Z = " << col_Z << endl;

		// compute normal to image plane - cross product of row and column vectors ...

		double normal_X = row_Y*col_Z - row_Z*col_Y;
		double normal_Y = row_Z*col_X - row_X*col_Z;
		double normal_Z = row_X*col_Y - row_Y*col_X;

//cerr << "normal_X = " << normal_X << endl;
//cerr << "normal_Y = " << normal_Y << endl;
//cerr << "normal_Z = " << normal_Z << endl;

		// length of normal is sqrt(dot product of normal with itself)
		// should be 1 since both row and col vectors unit vectors
		// but just in case ...

		double normal_length=sqrt(normal_X*normal_X+normal_Y*normal_Y+normal_Z*normal_Z);

//cerr << "normal_length = " << normal_length << endl;

		Assert(normal_length != 0);

		// calculate vector between tlhc and reference tlhc

		double difference_X = tlhc_X - reference_tlhc_X;
		double difference_Y = tlhc_Y - reference_tlhc_Y;
		double difference_Z = tlhc_Z - reference_tlhc_Z;

//cerr << "difference_X = " << normal_X << endl;
//cerr << "difference_Y = " << difference_Y << endl;
//cerr << "difference_Z = " << difference_Z << endl;

		// project difference vector onto normal to get distance
		// by computing dot product of difference vector and normal vector
		// and dividing by length of normal vector ...

		distance=(difference_X*normal_X+difference_Y*normal_Y+difference_Z*normal_Z)/normal_length;
	}
//cerr << "distance = " << distance << endl;

	return distance;
}

// Function to get value to sort using ImageOrientationPatient ...

static double
getAngleOfNormalToImagePlane(bool first,AttributeList &list,
	double &reference_normal_X,double &reference_normal_Y,double &reference_normal_Z,
	double &reference_normal_length,
	const char *filename,TextOutputStream &log)
{
	Attribute *aImageOrientationPatient=isValuePresentElseError(list,
		TagFromName(ImageOrientationPatient),"Image Orientation Patient",filename,log);

	if (!aImageOrientationPatient) return 0;

	if (aImageOrientationPatient->getVM() != 6) {
		log << filename << ": " 
		    << EMsgDC(BadAttributeValueMultiplicity)
		    << " - Image Orientation Patient - "
		    << MMsgDC(Expected) << " 6 "
		    << MMsgDC(Got) << " " << aImageOrientationPatient->getVM()
		    << endl;
		return 0;
	}

	double angle;

	double row_X; aImageOrientationPatient->getValue(0,row_X);
	double row_Y; aImageOrientationPatient->getValue(1,row_Y);
	double row_Z; aImageOrientationPatient->getValue(2,row_Z);

//cerr << "row_X = " << row_X << endl;
//cerr << "row_Y = " << row_Y << endl;
//cerr << "row_Z = " << row_Z << endl;

	double col_X; aImageOrientationPatient->getValue(3,col_X);
	double col_Y; aImageOrientationPatient->getValue(4,col_Y);
	double col_Z; aImageOrientationPatient->getValue(5,col_Z);

//cerr << "col_X = " << col_X << endl;
//cerr << "col_Y = " << col_Y << endl;
//cerr << "col_Z = " << col_Z << endl;

	// compute normal to image plane - cross product of row and column vectors ...

	double normal_X = row_Y*col_Z - row_Z*col_Y;
	double normal_Y = row_Z*col_X - row_X*col_Z;
	double normal_Z = row_X*col_Y - row_Y*col_X;

//cerr << "normal_X = " << normal_X << endl;
//cerr << "normal_Y = " << normal_Y << endl;
//cerr << "normal_Z = " << normal_Z << endl;

	// length of normal is sqrt(dot product of normal with itself)
	// should be 1 since both row and col vectors unit vectors
	// but just in case ...

	double normal_length=sqrt(normal_X*normal_X+normal_Y*normal_Y+normal_Z*normal_Z);

//cerr << "normal_length = " << normal_length << endl;

	Assert(normal_length != 0);

	// if first time, set reference normal to current normal

	if (first) {
		reference_normal_X=normal_X;
		reference_normal_Y=normal_Y;
		reference_normal_Z=normal_Z;
		reference_normal_length=normal_length;
	}

	// calculate angle in degrees between normal and reference normal
	// by computing arccos of dot product of vectors divided by lengths ...

	angle=acos(
		 (normal_X*reference_normal_X+normal_Y*reference_normal_Y+normal_Z*reference_normal_Z)
		/(normal_length*reference_normal_length)
		 ) * (360.0 / (M_PI * 2.0));

//cerr << "angle = " << angle << endl;

	return angle;
}

// structure and sort compare to sort filenames by some numeric value

struct sortentry {
	double primaryvalue;
	double secondaryvalue;
	const char *filename;
};

static int
//sortentrycompareascending(const sortentry *i, const sortentry *j)
//sortentrycompareascending(sortentry *i, sortentry *j)
sortentrycompareascending(const void *ip, const void *jp)
{
	const sortentry *i=(const sortentry *)ip;
	const sortentry *j=(const sortentry *)jp;
	Assert(i);
	Assert(j);
	if (fabs(i->primaryvalue - j->primaryvalue) < 0.00001) {	// equal
		if (i->secondaryvalue > j->secondaryvalue)
			return (1);
		if (i->secondaryvalue < j->secondaryvalue)
			return (-1);
	}
	else {
		if (i->primaryvalue > j->primaryvalue)
			return (1);
		if (i->primaryvalue < j->primaryvalue)
			return (-1);
	}
	return (0);
}

static int
//sortentrycomparedescending(const sortentry *i, const sortentry *j)
//sortentrycomparedescending(sortentry *i, sortentry *j)
sortentrycomparedescending(const void *ip, const void *jp)
{
	const sortentry *i=(const sortentry *)ip;
	const sortentry *j=(const sortentry *)jp;
	Assert(i);
	Assert(j);
	if (fabs(i->primaryvalue - j->primaryvalue) < 0.00001) {	// equal
		if (i->secondaryvalue > j->secondaryvalue)
			return (-1);
		if (i->secondaryvalue < j->secondaryvalue)
			return (1);
	}
	else {
		if (i->primaryvalue > j->primaryvalue)
			return (-1);
		if (i->primaryvalue < j->primaryvalue)
			return (1);
	}
	return (0);
}

static void
dumpsortedfilenamestable(sortentry *table,int n,const char *name,ostream &log)
{
	int i;
	for (i=0; i<n; ++i) {
		log << name << "[" << i << "] = " << table[i].primaryvalue << " in " << table[i].filename << endl;
	}
}

static void
dumpindexfilenamestable(sortentry *table,int n,bool showsortkeyvalue,bool showinterval,ostream &log)
{
	double lastprimaryvalue = table[0].primaryvalue;
	int i;
	for (i=0; i<n; ++i) {
		double thisprimaryvalue = table[i].primaryvalue;
		double delta = thisprimaryvalue - lastprimaryvalue;
		lastprimaryvalue = thisprimaryvalue;
		if (showsortkeyvalue && showinterval) {
			log << i << "\t" << table[i].filename << "\t" << thisprimaryvalue << "\t" << delta << endl;
		}
		else if (showsortkeyvalue) {
			log << i << "\t" << table[i].filename << "\t" << thisprimaryvalue << endl;
		}
		else {
			log << i << "\t" << table[i].filename << endl;
		}
	}
}

static void
getintervalfromindexfilenamestable(sortentry *table,int n,double tolerance,ostream &log) {
	bool success=true;
	double total;
	double interval;
	double lastprimaryvalue = table[0].primaryvalue;
	int i;
	for (i=1; i<n; ++i) {
		double thisprimaryvalue = table[i].primaryvalue;
		double delta = thisprimaryvalue - lastprimaryvalue;
		lastprimaryvalue = thisprimaryvalue;
		if (i == 1) {
			interval = delta;
			total = delta;
		}
		else if (fabs(interval-delta) > tolerance) {
			log << "Error: unequal interval" << endl;
			success=false;
			break;
		}
		else {
			total+=delta;
			interval = total/i;
		}
	}
	if (success) {
		log << "Interval\t" << interval << endl;
	}
}

static bool
checkSameSeries(int numberofinputfiles,struct sortentry *sortedfilenamestable,DicomInputOptions &dicom_input_options,
	bool verbose,bool veryverbose,bool veryveryverbose,TextOutputStream &log)
{
	// make pass to check attributes required to have constant values for all image files ...

	bool success=true;

	const char *requiredPatientID;
	const char *requiredStudyInstanceUID;
	const char *requiredSeriesInstanceUID;
	const char *requiredSOPClassUID;
	const char *requiredPatientName;
	const char *requiredStudyID;
	Uint16 requiredSeriesNumber;
	const char *requiredModality;

	Uint16 requiredRows;
	Uint16 requiredColumns;
	Uint16 requiredBitsAllocated;
	Uint16 requiredBitsStored;
	Uint16 requiredHighBit;
	Uint16 requiredPixelRepresentation;
	Uint16 requiredSamplesPerPixel;

	double requiredSliceThickness;

	//Uint16 requiredEchoNumber;
	double requiredEchoTime;
	double requiredRepetitionTime;

	double requiredRescaleSlope;
	double requiredRescaleIntercept;

	// iterate through image files to extract and check attributes values ...

	int i;
	for (i=0; i < numberofinputfiles; ++i) {

		// open and read each image file ...

		const char *filename=sortedfilenamestable[i].filename;
		Assert(filename);
		if (veryverbose) log << "Reading \"" << filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
		ifstream *fstr=new ifstream(filename,ios::in|ios::binary);
#else
		ifstream *fstr=new ifstream(filename);
#endif
		if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileReadOpenFailed);
			if (filename) cerr <<" - \"" << filename << "\"";
			success=false;
			break;
		}

		DicomInputStream din(*(istream *)fstr);

		ManagedAttributeList list;

		if (veryverbose) log << "******** While reading ... " << filename << " ... ********" << endl; 
		list.read(din,&log,veryveryverbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

		if (!list.good()) {
			log << list.errors()
			    << EMsgDC(DatasetReadFailed) << endl;
			success=false;
			break;
		}

		// extract and check that attributes values are consistent for all image files ...

		const char *patientID=getAndCheckStringMatchElseError(i == 0,requiredPatientID,list,
			TagFromName(PatientID),"Patient ID",filename,log);

		const char *patientName=getAndCheckStringMatchElseError(i == 0,requiredPatientName,list,
			TagFromName(PatientName),"Patient Name",filename,log);

		const char *studyInstanceUID=getAndCheckStringMatchElseError(i == 0,requiredStudyInstanceUID,list,
			TagFromName(StudyInstanceUID),"Study Instance UID",filename,log);

		const char *studyID=getAndCheckStringMatchElseError(i == 0,requiredStudyID,list,
			TagFromName(StudyID),"Study ID",filename,log);

		const char *seriesInstanceUID=getAndCheckStringMatchElseError(i == 0,requiredSeriesInstanceUID,list,
			TagFromName(SeriesInstanceUID),"Series Instance UID",filename,log);

		const char *SOPClassUID=getAndCheckStringMatchElseError(i == 0,requiredSOPClassUID,list,
			TagFromName(SOPClassUID),"SOP Class UID",filename,log);

		Uint16 seriesNumber=getAndCheckIntegerMatchElseError(i == 0,requiredSeriesNumber,list,
			TagFromName(SeriesNumber),"Series Number",filename,log);

		const char *modality=getAndCheckStringMatchElseError(i == 0,requiredModality,list,
			TagFromName(Modality),"Modality",filename,log);

		Uint16 rows=getAndCheckIntegerMatchElseError(i == 0,requiredRows,list,
			TagFromName(Rows),"Rows",filename,log);

		Uint16 columns=getAndCheckIntegerMatchElseError(i == 0,requiredColumns,list,
			TagFromName(Columns),"Columns",filename,log);

		Uint16 bitsAllocated=getAndCheckIntegerMatchElseError(i == 0,requiredBitsAllocated,list,
			TagFromName(BitsAllocated),"Bits Allocated",filename,log);

		Uint16 bitsStored=getAndCheckIntegerMatchElseError(i == 0,requiredBitsStored,list,
			TagFromName(BitsStored),"Bits Stored",filename,log);

		Uint16 highBit=getAndCheckIntegerMatchElseError(i == 0,requiredHighBit,list,
			TagFromName(HighBit),"High Bit",filename,log);

		Uint16 pixelRepresentation=getAndCheckIntegerMatchElseError(i == 0,requiredPixelRepresentation,list,
			TagFromName(PixelRepresentation),"Pixel Representation",filename,log);

		Uint16 samplesPerPixel=getAndCheckIntegerMatchElseError(i == 0,requiredSamplesPerPixel,list,
			TagFromName(SamplesPerPixel),"Samples Per Pixel",filename,log);

		if (strcmp(requiredSOPClassUID,MRImageStorageSOPClassUID) == 0
		 || strcmp(requiredSOPClassUID,CTImageStorageSOPClassUID) == 0) {
			double sliceThickness=getAndCheckFloatMatchElseError(i == 0,requiredSliceThickness,list,
				TagFromName(SliceThickness),"Slice Thickness",filename,log);
		}

		if (strcmp(requiredSOPClassUID,MRImageStorageSOPClassUID) == 0) {
			//Uint16 echoNumber=getAndCheckIntegerMatchElseError(i == 0,requiredEchoNumber,list,
			//	TagFromName(EchoNumber),"Echo Number",filename,log);
			double echoTime=getAndCheckFloatMatchElseError(i == 0,requiredEchoTime,list,
				TagFromName(EchoTime),"Echo Time",filename,log);
			double repetitionTime=getAndCheckFloatMatchElseError(i == 0,requiredRepetitionTime,list,
				TagFromName(RepetitionTime),"Repetition Time",filename,log);
		}

		if (strcmp(requiredSOPClassUID,CTImageStorageSOPClassUID) == 0) {
			double rescaleSlope=getAndCheckFloatMatchElseError(i == 0,requiredRescaleSlope,list,
				TagFromName(RescaleSlope),"Rescale Slope",filename,log);
			double rescaleIntercept=getAndCheckFloatMatchElseError(i == 0,requiredRescaleIntercept,list,
				TagFromName(RescaleIntercept),"Rescale Intercept",filename,log);
		}

		// should also check PixelSpacing, ImageOrientationPatient ...

		// clean up ... the DICOM input stream goes out of scope with each iteration of the loop

		if (fstr) delete fstr;
	}

	return success;
}

static bool
checkSameFrameOfReferenceUID(int numberofinputfiles,struct sortentry *sortedfilenamestable,DicomInputOptions &dicom_input_options,
        bool verbose,bool veryverbose,bool veryveryverbose,TextOutputStream &log)
{
        // make pass to check attributes required to have constant values for all image files ...

        bool success=true;

        const char *requiredFrameOfReferenceUID;

        // iterate through image files to extract and check attributes values ...

        int i;
        for (i=0; i < numberofinputfiles; ++i) {

                // open and read each image file ...

                const char *filename=sortedfilenamestable[i].filename;
                Assert(filename);
                if (veryverbose) log << "Reading \"" << filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
                ifstream *fstr=new ifstream(filename,ios::in|ios::binary);
#else
                ifstream *fstr=new ifstream(filename);
#endif
                if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
                        cerr << AMsgDC(FileReadOpenFailed);
                        if (filename) cerr <<" - \"" << filename << "\"";
                        success=false;
                        break;
                }

                DicomInputStream din(*(istream *)fstr,
			dicom_input_options.transfersyntaxuid,
			dicom_input_options.usemetaheader);

                ManagedAttributeList list;

                if (veryverbose) log << "******** While reading ... " << filename << " ... ********" << endl;
                list.read(din,&log,veryveryverbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

                if (!list.good()) {
                        log << list.errors()
                            << EMsgDC(DatasetReadFailed) << endl;
                        success=false;
                        break;
                }

                // extract and check that attributes values are consistent for all image files ...

                const char *frameOfReferenceUID=getAndCheckStringMatchElseError(i == 0,requiredFrameOfReferenceUID,list,
                        TagFromName(FrameOfReferenceUID),"Frame Of Reference UID",filename,log);

                // clean up ... the DICOM input stream goes out of scope with each iteration of the loop

                if (fstr) delete fstr;
        }

        return success;
}

static bool
sortBy(int numberofinputfiles,struct sortentry *&sortedfilenamestable,const char *sortbyname,Tag sortbytag,bool descending,
	DicomInputOptions &dicom_input_options,bool showindex,bool showsortkeyvalue,bool showinterval,double tolerance,
	bool verbose,bool veryverbose,bool veryveryverbose,TextOutputStream &log)
{
		// Make pass through images files to get sorttag

	bool success=true;

	double reference_tlhc_X;	// these are used for ImagePositionPatient only
	double reference_tlhc_Y;
	double reference_tlhc_Z;
	double reference_normal_X;	// these are used for ImageOrientationPatient only
	double reference_normal_Y;
	double reference_normal_Z;
	double reference_normal_length;

	int i;
	for (i=0; i < numberofinputfiles; ++i) {

		// open and read each image file ...

		const char *filename=sortedfilenamestable[i].filename;
		Assert(filename);
		if (veryverbose) log << "Reading \"" << filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
		ifstream *fstr=new ifstream(filename,ios::in|ios::binary);
#else
		ifstream *fstr=new ifstream(filename);
#endif
		if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileReadOpenFailed);
			if (filename) cerr <<" - \"" << filename << "\"";
			success=false;
			break;
		}

		DicomInputStream din(*(istream *)fstr,
			dicom_input_options.transfersyntaxuid,
			dicom_input_options.usemetaheader);

		ManagedAttributeList list;

		if (veryverbose) log << "******** While reading ... " << filename << " ... ********" << endl; 
		list.read(din,&log,veryveryverbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

		if (!list.good()) {
			log << list.errors()
			    << EMsgDC(DatasetReadFailed) << endl;
			success=false;
			break;
		}

		// extract sort key attributes value ...

		double value;
		if (sortbytag == TagFromName(ImagePositionPatient)) {
			value=getDistanceAlongNormalToImagePlane(i == 0,list,
				reference_tlhc_X,reference_tlhc_Y,reference_tlhc_Z,
				filename,log);
		}
		else if (sortbytag == TagFromName(ImageOrientationPatient)) {
			value=getAngleOfNormalToImagePlane(i == 0,list,
				reference_normal_X,reference_normal_Y,reference_normal_Z,
				reference_normal_length,filename,log);
		}
		else {
			value=getFloatValueElseError(list,sortbytag,sortbyname,filename,log);
		}

		sortedfilenamestable[i].primaryvalue = value;
		sortedfilenamestable[i].secondaryvalue = getFloatValueElseError(list,TagFromName(SeriesNumber),"Series Number",filename,log);

		// clean up ... the DICOM input stream goes out of scope with each iteration of the loop

		if (fstr) delete fstr;
	}

	if (veryverbose) {
		log << "Before sorting" << endl;
		dumpsortedfilenamestable(sortedfilenamestable,numberofinputfiles,sortbyname,log);
	}
	qsort((char *)sortedfilenamestable,numberofinputfiles,sizeof(sortentry),
		descending ? sortentrycomparedescending : sortentrycompareascending);

	if (verbose) {
		log << "After sorting" << endl;
		dumpsortedfilenamestable(sortedfilenamestable,numberofinputfiles,sortbyname,log);
	}

	if (showindex) {
		dumpindexfilenamestable(sortedfilenamestable,numberofinputfiles,showsortkeyvalue,showinterval,log);
	}

	if (showinterval) {
		getintervalfromindexfilenamestable(sortedfilenamestable,numberofinputfiles,tolerance,log);
	}

	return success;
}


int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool veryverbose=options.get("veryverbose") || options.get("vv");
	bool veryveryverbose=options.get("veryveryverbose") || options.get("vvv");
	if (veryveryverbose) veryverbose=true;
	if (veryverbose) verbose=true;

	bool checksameseries=options.get("check");
	bool checksameFoRUID=options.get("checkFoR");

	bool showindex=options.get("index");
	bool showsortkeyvalue=options.get("show");
	bool showinterval=options.get("interval");
	double tolerance = 0.01;
	options.get("tolerance",tolerance);

	const char *sortbyname;
	bool sortby=options.get("sortby",sortbyname) || options.get("k",sortbyname);
	Tag sortbytag;

	if (sortby && !staticDictionary.getTag(sortbyname,sortbytag)) {
		cerr << "Sort tag <" << sortbyname << "> is not a valid data element" << endl;
		bad=true;
	}

	bool descending=options.get("descending");	// should check present only if sortby

	dicom_input_options.done();
	Assert(!dicom_input_options.filename);

	int numberofinputfiles=!options;

	struct sortentry *sortedfilenamestable = new sortentry [numberofinputfiles];
	Assert(sortedfilenamestable);
	struct sortentry *ptr = sortedfilenamestable;

	const char *filename;

	while(!options && (filename=options())) {
		++options;
		ptr->filename=filename;
		ptr->primaryvalue=0;			// leave sort value unused
		ptr->secondaryvalue=0;			// leave sort value unused
		++ptr;
	}

	options.done();

	cerr << dicom_input_options.errors();
	cerr << options.errors();

	if (!dicom_input_options.good()
	 || !options.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< " [-v|-verbose|-vv|-veryverbose|-vvv|-veryveryverbose]"
			<< " [-index]"
			<< " [-check]"
			<< " [-checkFoR]"
			<< " [-show]"
			<< " [-interval]"
			<< " [-tolerance mm]"
			<< " [-descending]"
			<< " [-sortby|k elementname]"
			<< " " << MMsgDC(InputFile) << " ["<< MMsgDC(InputFile) << " ...]"
			<< endl;
		exit(1);
	}

	bool success=true;
	TextOutputStream log(cerr);

	if (checksameseries) {
		success=checkSameSeries(numberofinputfiles,sortedfilenamestable,
			dicom_input_options,verbose,veryverbose,veryveryverbose,log);
	}

	if (checksameFoRUID) {
		success=checkSameFrameOfReferenceUID(numberofinputfiles,sortedfilenamestable,
			dicom_input_options,verbose,veryverbose,veryveryverbose,log);
	}

	if (!success) exit(1);

	if (sortby) {
		success=sortBy(numberofinputfiles,sortedfilenamestable,sortbyname,sortbytag,descending,
			dicom_input_options,showindex,showsortkeyvalue,showinterval,tolerance,
			verbose,veryverbose,veryveryverbose,log);
	}

	if (!success) exit(1);

	if (numberofinputfiles && sortedfilenamestable) delete[] sortedfilenamestable;

	return success ? 0 : 1;
}

	
