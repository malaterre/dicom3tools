#include <fstream.h>
#include <ctype.h>

#include "attrmxls.h"
#include "attrtype.h"
#include "attrseq.h"
#include "attrothr.h"
#include "attrval.h"
#include "elmdict.h"
#include "elmconst.h"
#include "sopclu.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "uidgen.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const double defaultWindowCenter = 127.5;
static const double defaultWindowWidth = 256;

static ElementDictionary staticDictionary;

// Define a funky derived OX class to be able to writeBase() and not activateSource() ...

class OtherUnspecifiedLargeAttributeDummy : public OtherUnspecifiedLargeAttributeBase {
public:
	OtherUnspecifiedLargeAttributeDummy(Tag t,
			Uint16 r,Uint16 c,Uint16 f,Uint16 sperp,
			TransferSyntax *transfersyntax,
			Uint16 bytesinword,
			Uint16 bitsallocated,
			Uint16 bitsstored,
			Uint16 highbit,
			Uint32 length=0xffffffff)
		: OtherUnspecifiedLargeAttributeBase(t,r,c,f,sperp)
		{
			srcpixeldata=0;

			srcbitsallocated=bitsallocated;
			srcbitsstored=bitsstored;
			srchighbit=highbit;

			Assert(transfersyntax);
			setOutputEncoding(
				transfersyntax,
				bytesinword,bitsallocated,
				bitsstored,highbit,length);
		}

	virtual ~OtherUnspecifiedLargeAttributeDummy()
		{
		}

	bool activateSource(void)
		{
			Assert(0);	// only here to make concrete ... pixel data is actually written manually
			return true;
		}

	bool activateSourceWithoutUnpacking(void)
		{
			Assert(0);	// only here to make concrete ... pixel data is actually written manually
			return true;
		}

	DicomOutputStream& writeBase(DicomOutputStream& dout)
		{
			return OtherUnspecifiedLargeAttributeBase::writeBase(dout);	//protected method
		}

	TextOutputStream& writeBase(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false)
		{
			return OtherUnspecifiedLargeAttributeBase::writeBase(stream,dict,verbose);	//protected method
		}
};

// Various utility functions ...

// getStringFromTime() should really be added as a static function to generic/datetype.h

static char *getStringFromTime(Time time) {
//cerr << "getStringFromTime: isgood()=" << time.isgood() << " hh=" << time.getHour() << " mm=" << time.getMinute() << " ss=" << time.getSecond() << " ms=" << time.getMilliSecond() << endl;
	ostrstream ost;
	ost << dec << setfill('0');
	if (time.isgood()) {
		ost << setw(2) << time.getHour()
		    << setw(2) << time.getMinute()
		    << setw(2) << time.getSecond();
		if (time.getMilliSecond())
			ost << '.' << setw(3) << time.getMilliSecond();
	}
	ost << ends;
	return ost.str();
}

// removeLeadingSpaces() is copied from libsrc/src/attrtyps.cc and probably should be factored out somewhere

static const char*
removeLeadingSpaces(const char *from)
{
	if (from) while (*from && isspace(*from)) ++from;
	return from;
}

static bool arrayOfStringValuesContains(char **a,int n,const char *v) {
//cerr << "arrayOfStringValuesContains:" << endl;
	if (a && v) {
		int i;
		for (i=0; i<n; ++i) {
//cerr << "arrayOfStringValuesContains: i=" << i << endl;
			if (a[i] && strcmp(a[i],v) == 0) {
//cerr << "arrayOfStringValuesContains: true" << endl;
				return true;
			}
		}
	}
//cerr << "arrayOfStringValuesContains: false" << endl;
	return false;
}					

static char *makeDateTimeString(const char *d,const char *t) {
//cerr << "makeDateTimeString: d=" << d << " t=" << t << endl;
	int l= (d ? strlen(d) : 0) + (t ? strlen(t) : 0) + 1;
	char *dt=new char[l];
	*dt=0;
	if (d) strcat(dt,d);
	if (t) strcat(dt,t);
	return dt;
}

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
	const char *value=AttributeValue(a);	//SC 2.0.1 didn't like it as ? arg :(
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


static const char *fixRescaleTypeForCTIfNecessary(const char *use_type,bool addMultiFrameCTStuff,bool imageIsDerived) {
	if (use_type == NULL) {
		use_type = addMultiFrameCTStuff && !imageIsDerived ? "HU" : "US";
	}
	else if (strncmp(use_type,"Houn",4) == 0) {	// e.g., "Hounsfield Units", or incorrectly spelled "Houndsfield Units"
		use_type="HU";				// use standard abbreviation instead
	}
	// else leave it alone
	return use_type;
}

// Functions to get value to sort using ImagePositionPatient ...

static double getDistanceWithinImagePlane(double tlhc_X,double tlhc_Y,double tlhc_Z,
		double reference_tlhc_X,double reference_tlhc_Y,double reference_tlhc_Z,
		double row_X,double row_Y,double row_Z,double col_X,double col_Y,double col_Z) {

	// length of row (and column) is sqrt(dot product of vector with itself)
	// should be 1 since both row and col vectors unit vectors
	// but just in case ...

	double row_length=sqrt(row_X*row_X+row_Y*row_Y+row_Z*row_Z);
	double col_length=sqrt(col_X*col_X+col_Y*col_Y+col_Z*col_Z);

//cerr << "getDistanceWithinImagePlane: row_length = " << row_length << endl;
//cerr << "getDistanceWithinImagePlane: col_length = " << col_length << endl;

	Assert(row_length != 0);
	Assert(col_length != 0);

	// calculate vector between tlhc and reference tlhc

	double difference_X = tlhc_X - reference_tlhc_X;
	double difference_Y = tlhc_Y - reference_tlhc_Y;
	double difference_Z = tlhc_Z - reference_tlhc_Z;

//cerr << "getDistanceWithinImagePlane: difference_X = " << difference_X << endl;
//cerr << "getDistanceWithinImagePlane: difference_Y = " << difference_Y << endl;
//cerr << "getDistanceWithinImagePlane: difference_Z = " << difference_Z << endl;

	// project difference vector onto row (and column) axes to get distance
	// by computing dot product of difference vector and row (and column) vector
	// and dividing by length of row (or column) vector ...

	double row_distance = (difference_X*row_X+difference_Y*row_Y+difference_Z*row_Z)/row_length;
	double col_distance = (difference_X*col_X+difference_Y*col_Y+difference_Z*col_Z)/col_length;
	
	// distance we want is hypotenuse ...
	
	double distance = sqrt(row_distance*row_distance + col_distance*col_distance);
	
//cerr << "getDistanceWithinImagePlane: distance = " << distance << endl;
	
	return distance;
}

static double getDistanceAlongNormalToImagePlane(double tlhc_X,double tlhc_Y,double tlhc_Z,
		double reference_tlhc_X,double reference_tlhc_Y,double reference_tlhc_Z,
		double row_X,double row_Y,double row_Z,double col_X,double col_Y,double col_Z) {

	// compute normal to image plane - cross product of row and column vectors ...

	double normal_X = row_Y*col_Z - row_Z*col_Y;
	double normal_Y = row_Z*col_X - row_X*col_Z;
	double normal_Z = row_X*col_Y - row_Y*col_X;

//cerr << "getDistanceAlongNormalToImagePlane:normal_X = " << normal_X << endl;
//cerr << "getDistanceAlongNormalToImagePlane:normal_Y = " << normal_Y << endl;
//cerr << "getDistanceAlongNormalToImagePlane:normal_Z = " << normal_Z << endl;

	// length of normal is sqrt(dot product of normal with itself)
	// should be 1 since both row and col vectors unit vectors
	// but just in case ...

	double normal_length=sqrt(normal_X*normal_X+normal_Y*normal_Y+normal_Z*normal_Z);

//cerr << "getDistanceAlongNormalToImagePlane:normal_length = " << normal_length << endl;

	Assert(normal_length != 0);

	// calculate vector between tlhc and reference tlhc

	double difference_X = tlhc_X - reference_tlhc_X;
	double difference_Y = tlhc_Y - reference_tlhc_Y;
	double difference_Z = tlhc_Z - reference_tlhc_Z;

//cerr << "getDistanceAlongNormalToImagePlane:difference_X = " << difference_X << endl;
//cerr << "getDistanceAlongNormalToImagePlane:difference_Y = " << difference_Y << endl;
//cerr << "getDistanceAlongNormalToImagePlane:difference_Z = " << difference_Z << endl;

	// project difference vector onto normal to get distance
	// by computing dot product of difference vector and normal vector
	// and dividing by length of normal vector ...

	double distance = (difference_X*normal_X+difference_Y*normal_Y+difference_Z*normal_Z)/normal_length;
	
//cerr << "getDistanceAlongNormalToImagePlane: distance = " << distance << endl;
	
	return distance;
}

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

		distance = getDistanceAlongNormalToImagePlane(tlhc_X,tlhc_Y,tlhc_Z,
			reference_tlhc_X,reference_tlhc_Y,reference_tlhc_Z,
			row_X,row_Y,row_Z,col_X,col_Y,col_Z);
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
	double value;
	const char *filename;
};

static int
//sortentrycompareascending(const sortentry *i, const sortentry *j)
//sortentrycompareascending(sortentry *i, sortentry *j)
sortentrycompareascending(const void *ip, const void *jp)
{
	const sortentry *i=(const sortentry *)ip;
	const sortentry *j=(const sortentry *)jp;
	if (i->value > j->value)
		return (1);
	if (i->value < j->value)
		return (-1);
	return (0);
}

static int
//sortentrycomparedescending(const sortentry *i, const sortentry *j)
//sortentrycomparedescending(sortentry *i, sortentry *j)
sortentrycomparedescending(const void *ip, const void *jp)
{
	const sortentry *i=(const sortentry *)ip;
	const sortentry *j=(const sortentry *)jp;
	if (i->value > j->value)
		return (-1);
	if (i->value < j->value)
		return (1);
	return (0);
}

static void
dumpsortedfilenamestable(sortentry *table,int n,const char *name,ostream &log)
{
	int i;
	for (i=0; i<n; ++i) {
		log << name << "[" << i << "] = " << table[i].value << " in " << table[i].filename << endl;
	}
}

static int
doublecompareascending(const void *ip, const void *jp)
{
	const double *i=(const double *)ip;
	const double *j=(const double *)jp;
	if (*i > *j)
		return (1);
	if (*i < *j)
		return (-1);
	return (0);
}

static int
doublecomparedescending(const void *ip, const void *jp)
{
	const double *i=(const double *)ip;
	const double *j=(const double *)jp;
	if (*i > *j)
		return (-1);
	if (*i < *j)
		return (1);
	return (0);
}

static void allocatePerFrameArrayAndCopySharedSingleStringValue(char *&shared,char **&perFrame,char *v,int numberofframes,int currentframe)
{
//cerr <<"allocatePerFrameArrayAndCopySharedSingleStringValue:" << endl;
        perFrame = new char *[numberofframes];
        Assert(perFrame);
        int k;
        for (k=0; k<currentframe; ++k) perFrame[k]=shared;
        shared=0;
        perFrame[currentframe]=v;
}


static void allocatePerFrameArrayAndCopySharedMultipleStringValues(int n,char **&shared,char ***&perFrame,char **v,int numberofframes,int currentframe)
{
        perFrame = new char **[numberofframes];
        Assert(perFrame);
        int k;
        for (k=0; k<currentframe; ++k) {
                perFrame[k]=shared;		// same value array is re-used multiple times
        }
        shared=NULL;
        perFrame[currentframe]=v;		// supplied value array is used, not a copy
}

static void allocatePerFrameArrayAndCopySharedSingleUint32Value(Uint32 &shared,Uint32 *&perFrame,Uint32 v,int numberofframes,int currentframe)
{
        perFrame = new Uint32[numberofframes];
        Assert(perFrame);
        int k;
        for (k=0; k<currentframe; ++k) perFrame[k]=shared;
        shared=0;
        perFrame[currentframe]=v;
}


static void allocatePerFrameArrayAndCopySharedMultipleUint32Values(int n,Uint32 *&shared,Uint32 **&perFrame,Uint32 *v,int numberofframes,int currentframe)
{
        perFrame = new Uint32 *[numberofframes];
        Assert(perFrame);
        int k;
        for (k=0; k<currentframe; ++k) {
                perFrame[k]=shared;		// same value array is re-used multiple times
        }
        shared=NULL;
        perFrame[currentframe]=v;		// supplied value array is used, not a copy
}

static void allocatePerFrameArrayAndCopySharedSingleDoubleValue(double &shared,double *&perFrame,double v,int numberofframes,int currentframe)
{
        perFrame = new double[numberofframes];
        Assert(perFrame);
        int k;
        for (k=0; k<currentframe; ++k) perFrame[k]=shared;
        shared=0;
        perFrame[currentframe]=v;
}

static void allocatePerFrameArrayAndCopySharedMultipleDoubleValues(int n,double *&shared,double **&perFrame,double *v,int numberofframes,int currentframe)
{
        perFrame = new double *[numberofframes];
        Assert(perFrame);
        int k;
        for (k=0; k<currentframe; ++k) {
                perFrame[k]=shared;		// same value array is re-used multiple times
        }
        shared=NULL;
        perFrame[currentframe]=v;		// supplied value array is used, not a copy
}

static bool multipleStringValuesAreEqual(int n,char **a,char **b)
{
//cerr << "multipleStringValuesAreEqual: n = " << n << endl;
	Assert(a);
	Assert(b);
        bool same = true;
        int i;
        for (i=0; i<n && same; ++i) {
//cerr << "multipleStringValuesAreEqual: a[" << i << "] = <" << (a[i] ? a[i] : "null") << ">" << endl;
//cerr << "multipleStringValuesAreEqual: b[" << i << "] = <" << (b[i] ? b[i] : "null") << ">" << endl;
                if ((a[i] && !b[i]) || (!a[i] && b[i]) || (a[i] && b[i] && strcmp(a[i],b[i]) != 0)) same=false;
                //if (strcmp(a[i],b[i]) != 0) same=false;
//cerr << "multipleStringValuesAreEqual: same = " << (same ? "T" : "F") << endl;
        }
        return same;
}

static char **createMultipleStringValues(int n,char *fill=NULL)
{
//cerr << "createMultipleStringValues: n = " << n << endl;
        char **v=new char *[n];
        Assert(v);
        int m;
        for (m=0; m<n; ++m) v[m]=fill;
        return v;
}

static char **getCopyOfMultipleStringValues(int nMax,int &nActual,Attribute *a)
{
//cerr << "getCopyOfMultipleStringValues: nMax = " << nMax << endl;
        Assert(a);
        char **v=createMultipleStringValues(nMax);
        int vm=a->getVM();
//cerr << "getCopyOfMultipleStringValues: vm = " << vm << endl;
	nActual = vm > nMax ? nMax : vm;
//cerr << "getCopyOfMultipleStringValues: nActual = " << nActual << endl;
        int m;
        for (m=0; m<nActual; ++m) a->getValue(m,v[m]);
        for (m=nActual; m<nMax; ++m) v[m]="";		// fill missing values with zero length string rather than null; saves checks later
        return v;
}


static bool multipleUint32ValuesAreEqual(int n,Uint32 *a,Uint32 *b)
{
        bool same = true;
        int i;
        for (i=0; i<n && same; ++i) {
                if (a[i] != b[i]) same=false;
        }
        return same;
}

static Uint32 *createMultipleUint32Values(int n,Uint32 fill=0)
{
        Uint32 *v=new Uint32[n];
        Assert(v);
        int m;
        for (m=0; m<n; ++m) v[m]=fill;
        return v;
}

static Uint32 *getCopyOfMultipleUint32ValuesElseError(int n,Attribute *a,const char *label,const char *filename,TextOutputStream &log)
{
        Assert(a);
        Uint32 *v=createMultipleUint32Values(n);
        int vm=a->getVM();
        if (vm < n) {
                log << filename << ": " 
                    << EMsgDC(BadValueMultiplicity) << ", expected " << n << " but got " << vm
                    << " - " << label
                    << endl;
        }
        int m;
        for (m=0; m<vm; ++m) a->getValue(m,v[m]);
       
        return v;
}

static bool multipleDoubleValuesAreEqual(int n,double *a,double *b)
{
        bool same = true;
        int i;
        for (i=0; i<n && same; ++i) {
                if (fabs(a[i]-b[i]) > .000001) same=false;
        }
        return same;
}

static double *createMultipleDoubleValues(int n,double fill=0)
{
        double *v=new double[n];
        Assert(v);
        int m;
        for (m=0; m<n; ++m) v[m]=fill;
        return v;
}

static double *getCopyOfMultipleDoubleValuesElseError(int n,Attribute *a,const char *label,const char *filename,TextOutputStream &log)
{
        Assert(a);
        double *v=createMultipleDoubleValues(n);
        int vm=a->getVM();
        if (vm < n) {
                log << filename << ": " 
                    << EMsgDC(BadValueMultiplicity) << ", expected " << n << " but got " << vm
                    << " - " << label
                    << endl;
        }
        int m;
        for (m=0; m<vm; ++m) a->getValue(m,v[m]);
       
        return v;
}

static void trackSingleValuedStringAttributePerFrame(AttributeList &list,Tag tag,const char *label,bool &encountered,char *&shared,char **&perFrame,int numberofframes,int currentframe,const char *filename,TextOutputStream &log)
{
//cerr << "trackSingleValuedStringAttributePerFrame: frame " << currentframe << " " << label << endl;
        Attribute *a = list[tag];
        if (a) {
                char *v=AttributeValue(a);
                if (encountered) {
                        Assert(currentframe>0);
                        if (perFrame) {
                                perFrame[currentframe]=v;
                        }
                        else {
                                if ((shared && !v) || (!shared && v) || (shared && v && strcmp(shared,v) != 0)) {
                                        allocatePerFrameArrayAndCopySharedSingleStringValue(shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                else {
                        encountered=true;
                        if (currentframe == 0) {
                                shared=v;
                        }
                        else {
                                log << filename << ": " 
                                        << EMsgDC(MissingAttribute) << " in prior frames"
                                        << " - " << label
                                        << endl;
                                Assert(perFrame == NULL);
                                shared=NULL;
                                allocatePerFrameArrayAndCopySharedSingleStringValue(shared,perFrame,v,numberofframes,currentframe);
                        }
                }
        }
        else {	// a == null
                if (encountered) {
                        log << filename << ": " 
                                << EMsgDC(MissingAttribute) << " in current frame"
                                << " - " << label
                                << endl;
                        char *v=NULL;		// best we can do
                        if (perFrame) {
                                perFrame[currentframe]=v;
                        }
                        else {
                                if ((shared && !v) || (!shared && v) || (shared && v && strcmp(shared,v) != 0)) {
                                        allocatePerFrameArrayAndCopySharedSingleStringValue(shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                // else do nothing ... not here now and haven't seen it yet
        }
}


static void trackMultiValuedStringAttributePerFrame(AttributeList &list,Tag tag,const char *label,int nMax,int &nActual,bool &encountered,char **&shared,char ***&perFrame,int numberofframes,int currentframe,const char *filename,TextOutputStream &log)
{
//cerr << "trackMultiValuedStringAttributePerFrame: frame " << currentframe << " " << label << endl;
        Attribute *a = list[tag];
        if (a) {
		int previousNActual=nActual;
                char **v=getCopyOfMultipleStringValues(nMax,nActual,a);
                if (encountered) {
                        Assert(currentframe>0);
			//Assert(previousNActual == nActual);	// must be the same number of values for every frame 
			if (previousNActual > nActual) {
				nActual=previousNActual;	// hmm :(
			} 
                        if (perFrame) {
                                perFrame[currentframe]=v;	// NB. does not create a new copy
                        }
                        else {
                                if (!multipleStringValuesAreEqual(nMax,shared,v)) {
                                        allocatePerFrameArrayAndCopySharedMultipleStringValues(nMax,shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                else {
                        encountered=true;
                        if (currentframe == 0) {
                                shared=v;	// NB. does not create a new copy
                        }
                        else {
                                log << filename << ": " 
                                        << EMsgDC(MissingAttribute) << " in prior frames"
                                        << " - " << label
                                        << endl;
                                Assert(perFrame == NULL);
                                shared=createMultipleStringValues(nMax);
                                allocatePerFrameArrayAndCopySharedMultipleStringValues(nMax,shared,perFrame,v,numberofframes,currentframe);
                        }
                }
        }
        else {	// a == null
                if (encountered) {
                        log << filename << ": " 
                                << EMsgDC(MissingAttribute) << " in current frame"
                                << " - " << label
                                << endl;
                        char **v=createMultipleStringValues(nMax);
                        if (perFrame) {
                                perFrame[currentframe]=v;	// NB. does not create a new copy
                        }
                        else {
                                if (!multipleStringValuesAreEqual(nMax,shared,v)) {
                                        allocatePerFrameArrayAndCopySharedMultipleStringValues(nMax,shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                // else do nothing ... not here now and haven't seen it yet
        }
}

static void trackSingleValuedUint32AttributePerFrame(AttributeList &list,Tag tag,const char *label,bool &encountered,Uint32 &shared,Uint32 *&perFrame,int numberofframes,int currentframe,const char *filename,TextOutputStream &log)
{
//cerr << "trackSingleValuedIntegerAttributePerFrame: frame " << currentframe << " " << label << endl;
        Attribute *a = list[tag];
        if (a) {
                Uint32 v=AttributeValue(a);
                if (encountered) {
                        Assert(currentframe>0);
                        if (perFrame) {
                                perFrame[currentframe]=v;
                        }
                        else {
                                if (shared != v) {
                                        allocatePerFrameArrayAndCopySharedSingleUint32Value(shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                else {
                        encountered=true;
                        if (currentframe == 0) {
                                shared=v;
                        }
                        else {
                                log << filename << ": " 
                                        << EMsgDC(MissingAttribute) << " in prior frames"
                                        << " - " << label
                                        << endl;
                                Assert(perFrame == NULL);
                                shared=0;
                                allocatePerFrameArrayAndCopySharedSingleUint32Value(shared,perFrame,v,numberofframes,currentframe);
                        }
                }
        }
        else {	// a == null
                if (encountered) {
                        log << filename << ": " 
                                << EMsgDC(MissingAttribute) << " in current frame"
                                << " - " << label
                                << endl;
                        Uint32 v=0;		// best we can do
                        if (perFrame) {
                                perFrame[currentframe]=v;
                        }
                        else {
                                if (shared != v) {
                                        allocatePerFrameArrayAndCopySharedSingleUint32Value(shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                // else do nothing ... not here now and haven't seen it yet
        }
}

static void trackMultiValuedUint32AttributePerFrame(AttributeList &list,Tag tag,const char *label,int n,bool &encountered,Uint32 *&shared,Uint32 **&perFrame,int numberofframes,int currentframe,const char *filename,TextOutputStream &log)
{
//cerr << "trackMultiValuedUint32AttributePerFrame: frame " << currentframe << " " << label << endl;
        Attribute *a = list[tag];
        if (a) {
                Uint32 *v=getCopyOfMultipleUint32ValuesElseError(n,a,label,filename,log);
                if (encountered) {
                        Assert(currentframe>0);
                        if (perFrame) {
                                perFrame[currentframe]=v;	// NB. does not create a new copy
                        }
                        else {
                                if (!multipleUint32ValuesAreEqual(n,shared,v)) {
                                        allocatePerFrameArrayAndCopySharedMultipleUint32Values(n,shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                else {
                        encountered=true;
                        if (currentframe == 0) {
                                shared=v;	// NB. does not create a new copy
                        }
                        else {
                                log << filename << ": " 
                                        << EMsgDC(MissingAttribute) << " in prior frames"
                                        << " - " << label
                                        << endl;
                                Assert(perFrame == NULL);
                                shared=createMultipleUint32Values(n);
                                allocatePerFrameArrayAndCopySharedMultipleUint32Values(n,shared,perFrame,v,numberofframes,currentframe);
                        }
                }
        }
        else {	// a == null
                if (encountered) {
                        log << filename << ": " 
                                << EMsgDC(MissingAttribute) << " in current frame"
                                << " - " << label
                                << endl;
                        Uint32 *v=createMultipleUint32Values(n);
                        if (perFrame) {
                                perFrame[currentframe]=v;	// NB. does not create a new copy
                        }
                        else {
                                if (!multipleUint32ValuesAreEqual(n,shared,v)) {
                                        allocatePerFrameArrayAndCopySharedMultipleUint32Values(n,shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                // else do nothing ... not here now and haven't seen it yet
        }
}

static void trackSingleValuedDoubleAttributePerFrame(AttributeList &list,Tag tag,const char *label,bool &encountered,double &shared,double *&perFrame,int numberofframes,int currentframe,const char *filename,TextOutputStream &log)
{
//cerr << "trackSingleValuedDoubleAttributePerFrame: frame " << currentframe << " " << label << endl;
        Attribute *a = list[tag];
        if (a) {
                double v=AttributeValue(a);
                if (encountered) {
                        Assert(currentframe>0);
                        if (perFrame) {
                                perFrame[currentframe]=v;
                        }
                        else {
                                if (shared != v) {
                                        allocatePerFrameArrayAndCopySharedSingleDoubleValue(shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                else {
                        encountered=true;
                        if (currentframe == 0) {
                                shared=v;
                        }
                        else {
                                log << filename << ": " 
                                        << EMsgDC(MissingAttribute) << " in prior frames"
                                        << " - " << label
                                        << endl;
                                Assert(perFrame == NULL);
                                shared=0;
                                allocatePerFrameArrayAndCopySharedSingleDoubleValue(shared,perFrame,v,numberofframes,currentframe);
                        }
                }
        }
        else {	// a == null
                if (encountered) {
                        log << filename << ": " 
                                << EMsgDC(MissingAttribute) << " in current frame"
                                << " - " << label
                                << endl;
                        double v=0;		// best we can do
                        if (perFrame) {
                                perFrame[currentframe]=v;
                        }
                        else {
                                if (shared != v) {
                                        allocatePerFrameArrayAndCopySharedSingleDoubleValue(shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                // else do nothing ... not here now and haven't seen it yet
        }
}

static void trackMultiValuedDoubleAttributePerFrame(AttributeList &list,Tag tag,const char *label,int n,bool &encountered,double *&shared,double **&perFrame,int numberofframes,int currentframe,const char *filename,TextOutputStream &log)
{
//cerr << "trackMultiValuedDoubleAttributePerFrame: frame " << currentframe << " " << label << endl;
        Attribute *a = list[tag];
        if (a) {
                double *v=getCopyOfMultipleDoubleValuesElseError(n,a,label,filename,log);
                if (encountered) {
                        Assert(currentframe>0);
                        if (perFrame) {
                                perFrame[currentframe]=v;	// NB. does not create a new copy
                        }
                        else {
                                if (!multipleDoubleValuesAreEqual(n,shared,v)) {
                                        allocatePerFrameArrayAndCopySharedMultipleDoubleValues(n,shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                else {
                        encountered=true;
                        if (currentframe == 0) {
                                shared=v;	// NB. does not create a new copy
                        }
                        else {
                                log << filename << ": " 
                                        << EMsgDC(MissingAttribute) << " in prior frames"
                                        << " - " << label
                                        << endl;
                                Assert(perFrame == NULL);
                                shared=createMultipleDoubleValues(n);
                                allocatePerFrameArrayAndCopySharedMultipleDoubleValues(n,shared,perFrame,v,numberofframes,currentframe);
                        }
                }
        }
        else {	// a == null
                if (encountered) {
                        log << filename << ": " 
                                << EMsgDC(MissingAttribute) << " in current frame"
                                << " - " << label
                                << endl;
                        double *v=createMultipleDoubleValues(n);
                        if (perFrame) {
                                perFrame[currentframe]=v;	// NB. does not create a new copy
                        }
                        else {
                                if (!multipleDoubleValuesAreEqual(n,shared,v)) {
                                        allocatePerFrameArrayAndCopySharedMultipleDoubleValues(n,shared,perFrame,v,numberofframes,currentframe);
                                }
                                // else same as existing shared ... done
                        }
                }
                // else do nothing ... not here now and haven't seen it yet
        }
}

static bool findInStringRegardlessOfCase(const char *big, const char *little) {
	if (!big || !*big || !little || !*little) return false;
	int nb = strlen(big);
	int nl = strlen(little);
	if (nl > nb) return false;			// big string has to be bigger
	const char *btry = big;
	const char *blast = btry + (nb-nl+1);
	while (btry<blast) {
		const char *lp=little;
		const char *bp=btry;
		while (toupper(*lp) == toupper(*bp)) {
			if (!*++lp) return true;	// success when got to end of (null-terminated) little string
			++bp;
		}
		++btry;
	}
	return false;
}

static const char *returnDifferentValueOnlyIfActuallyDifferent(char ***perFrame_ImageType,int nFrames,int whichValue,const char *defaultValue,const char *differentValue) {
	const char *imageType=defaultValue;
	if (perFrame_ImageType && nFrames && perFrame_ImageType[0] && perFrame_ImageType[0][whichValue]) {
		imageType = perFrame_ImageType[0][whichValue];
		int i;
		for (i=1; i < nFrames; ++i) {
			if (!perFrame_ImageType[i] || !perFrame_ImageType[i][whichValue] || strcmp(imageType,perFrame_ImageType[i][whichValue]) != 0) {
				imageType=differentValue;
				break;
			}
		}
	}
	return imageType;
}

static int countDifferentSingleValuedUint32Values(bool encountered,Uint32 shared,Uint32 *perFrame,int n) {
//cerr << "countDifferentSingleValuedUint32Values:" << endl;
	int count=0;
	if (encountered) {
		if (perFrame) {
			Uint32 *observed = new Uint32[n];
			{ int j; for (j=0; j<n; ++j) observed[j]=Uint32(-1); }	// unlikely value
			int i;
			for (i=0; i<n; ++i) {
				Uint32 v = perFrame[i];
//cerr << "countDifferentSingleValuedUint32Values: [" << dec << i << "] = " << v << endl;
				int j;
				bool found=false;
				for (j=0; j<count && !found; ++j) if (observed[j] == v) found=true;
				if (!found) {
//cerr << "countDifferentSingleValuedUint32Values: not found, adding" << endl;
					observed[count] = v;
					++count;
				}
			}
		}
		else {
			count=1;
		}
	}
//cerr << "countDifferentSingleValuedUint32Values: count = " << dec << count << endl;
	return count;
}
					
static const char *reMapOldMRToNewMRImageTypeValue3(const char *o) {
	const char *n;
	if      (strcmp(o,"VELOCITY MAP") == 0) n="VELOCITY";
	else if (strcmp(o,"T1 MAP") == 0) n="T1";
	else if (strcmp(o,"T2 MAP") == 0) n="T2";
	else if (strcmp(o,"DIFFUSION MAP") == 0) n="DIFFUSION";
	else n=o;
	return n;
}

static const char *extractNewMRImageTypeValue4FromOldValueType3(const char *o) {
	const char *n;
	if      (strcmp(o,"VELOCITY MAP") == 0) n="VELOCITY";
	else if (strcmp(o,"T1 MAP") == 0) n="T1_MAP";
	else if (strcmp(o,"T2 MAP") == 0) n="T2_MAP";
	else if (strcmp(o,"DIFFUSION MAP") == 0) n="DIFFUSION";
	else if (strcmp(o,"DENSITY MAP") == 0) n="RHO";
	else if (strcmp(o,"PHASE SUBTRACT") == 0) n="SUBTRACTION";
	else if (strcmp(o,"MODULUS SUBTRACT") == 0) n="SUBTRACTION";
	else if (strcmp(o,"IMAGE ADDITION") == 0) n="ADDITION";
	//else n=o;
	else n="NONE";
	return n;
}

// The following builds both the CommonCTMRImageDescriptionMacro and the MRImageDescriptionMacro (if MR) ...

static bool makeCTMRImageDescriptionMacroAttributesAndAppendToList(bool isMR,AttributeList &list,bool frameLevel,int frame,int nFrames,
		bool encountered_ImageType,char **shared_ImageType,char ***perFrame_ImageType,int nValues_ImageType,
		bool encountered_PixelPresentation,char *shared_PixelPresentation,char **perFrame_PixelPresentation,
		bool encountered_VolumetricProperties,char *shared_VolumetricProperties,char **perFrame_VolumetricProperties,
		bool encountered_VolumeBasedCalculationTechnique,char *shared_VolumeBasedCalculationTechnique,char **perFrame_VolumeBasedCalculationTechnique,
		bool encountered_ComplexImageComponent,char *shared_ComplexImageComponent,char **perFrame_ComplexImageComponent,
		bool encountered_AcquisitionContrast,char *shared_AcquisitionContrast,char **perFrame_AcquisitionContrast,
		TextOutputStream &log) {
//cerr << "makeCTMRImageDescriptionMacroAttributesAndAppendToList: start frameLevel=" << frameLevel << endl;		
//cerr << "encountered_ImageType = " << encountered_ImageType << endl;
//cerr << "nValues_ImageType = " << nValues_ImageType << endl;
//cerr << "shared_ImageType [0] = " << (shared_ImageType ? shared_ImageType [0] : "null") << endl;
//cerr << "perFrame_ImageType [frame][0] = " << (perFrame_ImageType ? (frameLevel ? (perFrame_ImageType[frame] ? perFrame_ImageType[frame][0] : "[frame][0] null") : "not frame so no index") : "all null") << endl;
//cerr << "shared_ImageType [1] = " << (shared_ImageType ? shared_ImageType [1] : "null") << endl;
//cerr << "perFrame_ImageType [frame][1] = " << (perFrame_ImageType ? (frameLevel ? (perFrame_ImageType[frame] ? perFrame_ImageType[frame][1] : "[frame][1] null") : "not frame so no index") : "all null") << endl;
//cerr << "shared_ImageType [2] = " << (shared_ImageType ? shared_ImageType [2] : "null") << endl;
//cerr << "perFrame_ImageType [frame][2] = " << (perFrame_ImageType ? (frameLevel ? (perFrame_ImageType[frame] ? perFrame_ImageType[frame][2] : "[frame][2] null") : "not frame so no index") : "all null") << endl;
//cerr << "shared_ImageType [3] = " << (shared_ImageType ? shared_ImageType [3] : "null") << endl;
//cerr << "perFrame_ImageType [frame][3] = " << (perFrame_ImageType ? (frameLevel ? (perFrame_ImageType[frame] ? perFrame_ImageType[frame][3] : "[frame][3] null") : "not frame so no index") : "all null") << endl;

	bool isDerived=false;
	
	if (frameLevel) {
		Attribute *aFrameType = new CodeStringAttribute(TagFromName(FrameType));
		Assert(aFrameType);
		list+=aFrameType;
		
		const char *value1 = encountered_ImageType && nValues_ImageType > 0
			? (perFrame_ImageType && perFrame_ImageType[frame] ? perFrame_ImageType[frame][0] : (shared_ImageType ? shared_ImageType [0] : "ORIGINAL")) : "ORIGINAL";
		const char *value3 = encountered_ImageType && nValues_ImageType > 2
			? (perFrame_ImageType && perFrame_ImageType[frame] ? perFrame_ImageType[frame][2] : (shared_ImageType ? shared_ImageType [2] : "")) : "";
		const char *value4 = strcmp(value1,"ORIGINAL") == 0 ? "NONE" : (
			encountered_ImageType && nValues_ImageType > 3
			? (perFrame_ImageType && perFrame_ImageType[frame] ? perFrame_ImageType[frame][3] : (shared_ImageType ? shared_ImageType [3] : 0)) : 0);

		if (!value4 && isMR) value4=extractNewMRImageTypeValue4FromOldValueType3(value3);
		
		if (isMR) value3 = reMapOldMRToNewMRImageTypeValue3(value3);

//cerr << "value1 = " << value1 << endl;
//cerr << "value3 = " << value3 << endl;
//cerr << "value4 = " << value4 << endl;

		aFrameType->addValue(value1);
		aFrameType->addValue("PRIMARY");
		aFrameType->addValue(value3);
		aFrameType->addValue(value4);
		
		if (value1 && strcmp(value1,"DERIVED") == 0) isDerived=true;
	}
	else {
		list-=TagFromName(ImageType);
		Attribute *aImageType = new CodeStringAttribute(TagFromName(ImageType));
		Assert(aImageType);
		list+=aImageType;
		
		// Should probably actually trawl through all perFrame_ImageType value 0 to see if same, because could be a different value that is different
		const char *value1 = encountered_ImageType && nValues_ImageType > 0
			? (perFrame_ImageType ? returnDifferentValueOnlyIfActuallyDifferent(perFrame_ImageType,nFrames,0,"ORIGINAL","MIXED")
			: (shared_ImageType ? shared_ImageType [0] : "ORIGINAL")) : "ORIGINAL";
		// not allowed to be Type 2 at image level, OTHER is not a standard  defined term but nothing else is suitable ...
		const char *value3 = encountered_ImageType && nValues_ImageType > 2
			? (perFrame_ImageType
				? returnDifferentValueOnlyIfActuallyDifferent(perFrame_ImageType,nFrames,2,"OTHER","OTHER")
				: (shared_ImageType ? shared_ImageType [2] : "OTHER"))
			: "OTHER";
		const char *value4 = strcmp(value1,"ORIGINAL") == 0 ? "NONE" : (
			encountered_ImageType && nValues_ImageType > 3
			? (perFrame_ImageType
				? returnDifferentValueOnlyIfActuallyDifferent(perFrame_ImageType,nFrames,3,0,"MIXED")
				: (shared_ImageType ? shared_ImageType [3] : 0))
			: 0
			);
		if (!value4 && isMR) value4=extractNewMRImageTypeValue4FromOldValueType3(value3);
		
		if (isMR) value3 = reMapOldMRToNewMRImageTypeValue3(value3);

		aImageType->addValue(value1);
		aImageType->addValue("PRIMARY");
		aImageType->addValue(value3);
		aImageType->addValue(value4);
		
		if (value1 && strcmp(value1,"DERIVED") == 0) isDerived=true;
	}
	
	list+=new CodeStringAttribute(TagFromName(PixelPresentation),
		encountered_PixelPresentation ?
			(perFrame_PixelPresentation ? (frameLevel ? (perFrame_PixelPresentation[frame] ? perFrame_PixelPresentation[frame] : "MONOCHROME") : "MIXED")
			: shared_PixelPresentation) : "MONOCHROME");
	list+=new CodeStringAttribute(TagFromName(VolumetricProperties),encountered_VolumetricProperties ? (perFrame_VolumetricProperties ?
			(frameLevel ? (perFrame_VolumetricProperties[frame] ? perFrame_VolumetricProperties[frame] : "VOLUME") : "MIXED")
			: shared_VolumetricProperties) : "VOLUME");
	list+=new CodeStringAttribute(TagFromName(VolumeBasedCalculationTechnique),
		encountered_VolumeBasedCalculationTechnique ? (perFrame_VolumeBasedCalculationTechnique ?
			(frameLevel ? (perFrame_VolumeBasedCalculationTechnique[frame] ? perFrame_VolumeBasedCalculationTechnique[frame] : "NONE") : "MIXED")
			: shared_VolumeBasedCalculationTechnique) : "NONE");
	if (isMR) {
		// do the MR Image Description Macro stuff as well ...
		list+=new CodeStringAttribute(TagFromName(ComplexImageComponent),
			encountered_ComplexImageComponent ? (perFrame_ComplexImageComponent ?
				(frameLevel ? (perFrame_ComplexImageComponent[frame] ? perFrame_ComplexImageComponent[frame] : "MAGNITUDE") : "MIXED")
				: shared_ComplexImageComponent) : "MAGNITUDE");
		list+=new CodeStringAttribute(TagFromName(AcquisitionContrast),
			encountered_AcquisitionContrast ? (perFrame_AcquisitionContrast ?
				(frameLevel ? (perFrame_AcquisitionContrast[frame] ? perFrame_AcquisitionContrast[frame] : "UNKNOWN") : "MIXED")
				: shared_AcquisitionContrast) : "UNKNOWN");
	}
//cerr << "makeCTMRImageDescriptionMacroAttributesAndAppendToList: done isDerived = " << (isDerived ? "T" : "F") << endl;
	return isDerived;
}

struct dateTimeSortEntry {
	long du;
	long tu;
	char *ds;
	char *ts;
};

static int
dateTimeCompare(const void *ip, const void *jp)
{
//cerr << "dateTimeCompare: " << dec << ((struct dateTimeSortEntry *)ip)->ds << ((struct dateTimeSortEntry *)ip)->ts
//			    << " " << ((struct dateTimeSortEntry *)jp)->ds << ((struct dateTimeSortEntry *)jp)->ts << endl;
//cerr << "dateTimeCompare: " << dec << ((struct dateTimeSortEntry *)ip)->du << ((struct dateTimeSortEntry *)ip)->tu
//			    << " " << ((struct dateTimeSortEntry *)jp)->du << ((struct dateTimeSortEntry *)jp)->tu << endl;
	long rv;
	long d = ((struct dateTimeSortEntry *)ip)->du - ((struct dateTimeSortEntry *)jp)->du;
	if (d == 0) {
//cerr << "dateTimeCompare: same date, compare times" << endl;
		rv=((struct dateTimeSortEntry *)ip)->tu - ((struct dateTimeSortEntry *)jp)->tu;
	}
	else {
		rv=d;
	}
//cerr << "dateTimeCompare: rv = " << dec << rv << endl;
	// Need to be careful of limited precision of int vs. long and sign extension, hence this cautious approach ...
	int irv = rv < 0 ? -1 : (rv > 0 ? 1 : 0);
//cerr << "dateTimeCompare: irv = " << dec << irv << endl;
	return irv;
}

static void addEarliestContentDateAndTime(AttributeList &list,int numberofinputfiles,
	bool encountered_ContentDate,char *shared_ContentDate,char **perFrame_ContentDate,
	bool encountered_ContentTime,char *shared_ContentTime,char **perFrame_ContentTime,
	TextOutputStream &log) {
//cerr << "addEarliestContentDateAndTime: start" << endl;
	
	char *badDate = "29990101";
	char *badTime = "000000";
	
	char *useDate = NULL;
	char *useTime = NULL;
	
	if (encountered_ContentDate && !perFrame_ContentDate) {
		useDate = shared_ContentDate;					// all the same date
		if (encountered_ContentTime && !perFrame_ContentTime) {
			useTime = shared_ContentTime;				// all the same time on the same date
		}
	}
	
	struct dateTimeSortEntry *perFrame_ContentDateTime = NULL;
	if (!useDate || !useTime) {
		// Scan for the earliest data and time
		perFrame_ContentDateTime = new dateTimeSortEntry[numberofinputfiles];
		Assert(perFrame_ContentDateTime);
		int i;
		for (i=0; i<numberofinputfiles; ++i) {
			char *ds=encountered_ContentDate ? (perFrame_ContentDate ? perFrame_ContentDate[i] : shared_ContentDate) : (char *)NULL;
			char *ts=encountered_ContentTime ? (perFrame_ContentTime ? perFrame_ContentTime[i] : shared_ContentTime) : (char *)NULL;
			if (!ds || !strlen(ds)) ds=badDate;	// deliberately large so will be the last value chosen
			if (!ts || !strlen(ts)) ts=badTime;
//cerr << "addEarliestContentDateAndTime: ds = " << ds << " and ts = " << ts << endl;
			perFrame_ContentDateTime[i].ds=ds;
			perFrame_ContentDateTime[i].ts=ts;			
			perFrame_ContentDateTime[i].du = Date(ds,DateOrderMonthMiddleYearFirst);
			perFrame_ContentDateTime[i].tu = Time(ts);
//cerr << "addEarliestContentDateAndTime: perFrame_ContentDateTime[" << i << "].du and tu = " << perFrame_ContentDateTime[i].du << " " << perFrame_ContentDateTime[i].tu << endl;
		}
		qsort((void *)perFrame_ContentDateTime,numberofinputfiles,sizeof(dateTimeSortEntry),dateTimeCompare);
		useDate=perFrame_ContentDateTime[0].ds;
		useTime=perFrame_ContentDateTime[0].ts;
//cerr << "addEarliestContentDateAndTime: after sort using " << dec << useDate << useTime << endl;
	}
	if (useDate && strcmp(useDate,badDate) != 0) {
		list+=new DateStringAttribute(TagFromName(ContentDate),useDate ? useDate : badDate);	// don't add date unless it was good
	}
	if (useTime && (!useDate || strcmp(useDate,badDate) != 0)) {
		list+=new TimeStringAttribute(TagFromName(ContentTime),getStringFromTime(Time(useTime ? useTime : badTime)));	// don't add time unless it was good, but add it even if date absent or bad
	}
	if (perFrame_ContentDateTime) delete[] perFrame_ContentDateTime;
//cerr << "addEarliestContentDateAndTime: done" << endl;
}
					
static void addEarliestAcquisitionDateAndTime(AttributeList &list,int numberofinputfiles,
	bool encountered_AcquisitionDate,char *shared_AcquisitionDate,char **perFrame_AcquisitionDate,
	bool encountered_AcquisitionTime,char *shared_AcquisitionTime,char **perFrame_AcquisitionTime,
	TextOutputStream &log) {
//cerr << "addEarliestAcquisitionDateAndTime: start" << endl;
	char *badDate = "29990101";
	char *badTime = "000000";

	char *useDate = NULL;
	char *useTime = NULL;
	
	if (encountered_AcquisitionDate && !perFrame_AcquisitionDate) {
		useDate = shared_AcquisitionDate;					// all the same date
		if (encountered_AcquisitionTime && !perFrame_AcquisitionTime) {
			useTime = shared_AcquisitionTime;				// all the same time on the same date
//cerr << "addEarliestAcquisitionDateAndTime: all the same time on the same date using " << dec << useDate << useTime << endl;
		}
	}
	
	struct dateTimeSortEntry *perFrame_AcquisitionDateTime = NULL;
	if (!useDate || !useTime) {
		// Scan for the earliest data and time
		perFrame_AcquisitionDateTime = new dateTimeSortEntry[numberofinputfiles];
		Assert(perFrame_AcquisitionDateTime);
		int i;
		for (i=0; i<numberofinputfiles; ++i) {
			char *ds=encountered_AcquisitionDate ? (perFrame_AcquisitionDate ? perFrame_AcquisitionDate[i] : shared_AcquisitionDate) : (char *)NULL;
			char *ts=encountered_AcquisitionTime ? (perFrame_AcquisitionTime ? perFrame_AcquisitionTime[i] : shared_AcquisitionTime) : (char *)NULL;
			if (!ds || !strlen(ds)) ds=badDate;	// deliberately large so will be the last value chosen
			if (!ts || !strlen(ts)) ts=badTime;
//cerr << "addEarliestAcquisitionDateAndTime: ds = " << ds << " and ts = " << ts << endl;
			perFrame_AcquisitionDateTime[i].ds=ds;
			perFrame_AcquisitionDateTime[i].ts=ts;			
			perFrame_AcquisitionDateTime[i].du = Date(ds,DateOrderMonthMiddleYearFirst);
			perFrame_AcquisitionDateTime[i].tu = Time(ts);
//cerr << "addEarliestAcquisitionDateAndTime: perFrame_AcquisitionDateTime[" << i << "].du and tu = " << perFrame_AcquisitionDateTime[i].du << " " << perFrame_AcquisitionDateTime[i].tu << endl;
		}
		qsort((void *)perFrame_AcquisitionDateTime,numberofinputfiles,sizeof(dateTimeSortEntry),dateTimeCompare);
		useDate=perFrame_AcquisitionDateTime[0].ds;
		useTime=perFrame_AcquisitionDateTime[0].ts;
//cerr << "addEarliestAcquisitionDateAndTime: after sort using " << dec << useDate << useTime << endl;
	}
	
	list+=new DateTimeStringAttribute(TagFromName(AcquisitionDateTime),makeDateTimeString(useDate ? useDate : badDate,getStringFromTime(Time(useTime ? useTime : badTime))));
	
	// use Acquisition values instead of Content values if the latter are absent ...
	// (assumes addEarliestContentDateAndTime() has already been executed)
	
	if (!list[TagFromName(ContentDate)]) list+=new DateStringAttribute(TagFromName(ContentDate),useDate ? useDate : badDate);
	if (!list[TagFromName(ContentTime)]) list+=new TimeStringAttribute(TagFromName(ContentTime),getStringFromTime(Time(useTime ? useTime : badTime)));
	
	if (perFrame_AcquisitionDateTime) delete[] perFrame_AcquisitionDateTime;
//cerr << "addEarliestAcquisitionDateAndTime: done" << endl;
}

static void copyImagedNucleusIntoResonantNucleusIfNotAlreadyPresent(AttributeList &dst,AttributeList &src) {
//cerr << "copyImagedNucleusIntoResonantNucleusIfNotAlreadyPresent: start" << endl;
	const char *vImagedNucleus = AttributeValue(src[TagFromName(ImagedNucleus)]);
	if (vImagedNucleus) {
		if      (strcmp(vImagedNucleus,"H")      == 0) vImagedNucleus="1H";
		else if (strcmp(vImagedNucleus,"H1")     == 0) vImagedNucleus="1H";
		else if (strcmp(vImagedNucleus,"HE3")    == 0) vImagedNucleus="3HE";
		else if (strcmp(vImagedNucleus,"LI7")    == 0) vImagedNucleus="17L";
		else if (strcmp(vImagedNucleus,"C13")    == 0) vImagedNucleus="13C";
		else if (strcmp(vImagedNucleus,"F19")    == 0) vImagedNucleus="19F";
		else if (strcmp(vImagedNucleus,"NA23")   == 0) vImagedNucleus="23NA";
		else if (strcmp(vImagedNucleus,"P31")    == 0) vImagedNucleus="31P";
		else if (strcmp(vImagedNucleus,"XE129")  == 0) vImagedNucleus="129XE";
		// else leave it alone, probably in correct form, if not then OK as non-standard defined term
		// (could consider converting to upper case) :(
	}
	else {
		vImagedNucleus="1H";
	}
	dst+=new CodeStringAttribute(TagFromName(ResonantNucleus),vImagedNucleus);
//cerr << "copyImagedNucleusIntoResonantNucleusIfNotAlreadyPresent: done" << endl;
}

static void copyAttributeIntoNewAttributeIfNotAlreadyPresent(Tag dstTag,Tag srcTag,AttributeList &dst,AttributeList &src) {
	// assumes VRs the same
	if (!dst[dstTag]) {
		Attribute *a = src[srcTag];
		if (a) {
			src-=srcTag;
			a->setTag(dstTag);
			dst+=a;
		}
	}
}

static void copyAttributeIfNotAlreadyPresent(Tag tag,AttributeList &dst,AttributeList &src) {
	if (!dst[tag]) {
		Attribute *a = src[tag];
		if (a) {
			src-=tag;
			dst+=a;
		}
	}
}

static void copyPatientModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientBirthDate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientSex),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ReferencedPatientSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientBirthTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(OtherPatientIDs),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(OtherPatientNames),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(EthnicGroup),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientComments),dst,src);
}

static void copyGeneralStudyModule(AttributeList &dst,AttributeList &src){
	copyAttributeIfNotAlreadyPresent(TagFromName(StudyInstanceUID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(StudyDate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(StudyTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ReferringPhysicianName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(StudyID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AccessionNumber),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(StudyDescription),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PhysiciansOfRecord),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(NameOfPhysiciansReadingStudy),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ReferencedStudySequence),dst,src);
}

static void copyPatientStudyModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(AdmittingDiagnosesDescription),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AdmittingDiagnosesCodeSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientAge),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientSize),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientWeight),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(Occupation),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AdditionalPatientHistory),dst,src);
}

static void copyGeneralSeriesModule(AttributeList &dst,AttributeList &src,bool addMultiFrameCTOrMRStuff) {
	copyAttributeIfNotAlreadyPresent(TagFromName(Modality),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SeriesInstanceUID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SeriesNumber),dst,src);
	if (!addMultiFrameCTOrMRStuff) copyAttributeIfNotAlreadyPresent(TagFromName(Laterality),dst,src);	// No longer used in MR as of CP 365
	copyAttributeIfNotAlreadyPresent(TagFromName(SeriesDate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SeriesTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PerformingPhysicianName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ProtocolName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SeriesDescription),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(OperatorsName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ReferencedPerformedProcedureStepSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(BodyPartExamined),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientPosition),dst,src);
	//copyAttributeIfNotAlreadyPresent(TagFromName(SmallestPixelValueInSeries),dst,src);	// probably invalidated even if present
	//copyAttributeIfNotAlreadyPresent(TagFromName(LargestPixelValueInSeries),dst,src);	// probably invalidated even if present
	copyAttributeIfNotAlreadyPresent(TagFromName(RequestAttributesSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PerformedProcedureStepID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PerformedProcedureStepStartDate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PerformedProcedureStepStartTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PerformedProcedureStepDescription),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PerformedProtocolCodeSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(CommentsOnThePerformedProcedureStep),dst,src);
}

static void copyFrameOfReferenceModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameOfReferenceUID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PositionReferenceIndicator),dst,src);
}

static void copySynchronizationFrameOfReferenceModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(SynchronizationFrameOfReferenceUID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SynchronizationTrigger),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(TriggerSourceOrType),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SynchronizationChannel),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AcquisitionTimeSynchronized),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(TimeSource),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(TimeDistributionProtocol),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(NTPSourceAddress),dst,src);
}

static void copyGeneralEquipmentModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(Manufacturer),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(InstitutionName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(InstitutionAddress),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(StationName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(InstitutionalDepartmentName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ManufacturerModelName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(DeviceSerialNumber),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SoftwareVersions),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SpatialResolution),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(DateOfLastCalibration),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(TimeOfLastCalibration),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PixelPaddingValue),dst,src);
}

static void copyImagePixelModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(SamplesPerPixel),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PhotometricInterpretation),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(Rows),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(Columns),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(BitsAllocated),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(BitsStored),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(HighBit),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PixelRepresentation),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PixelData),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PlanarConfiguration),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PixelAspectRatio),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SmallestImagePixelValue),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(LargestImagePixelValue),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(RedPaletteColorLookupTableDescriptor),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(GreenPaletteColorLookupTableDescriptor),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(BluePaletteColorLookupTableDescriptor),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(RedPaletteColorLookupTableData),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(GreenPaletteColorLookupTableData),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(BluePaletteColorLookupTableData),dst,src);
}

static void copyContrastBolusModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusAgent),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusRoute),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusVolume),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusStartTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusStopTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusTotalDose),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusAgentSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusAdministrationRouteSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastFlowRate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastFlowDuration),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusIngredient),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContrastBolusIngredientConcentration),dst,src);
}

static void copySoftcopyPresentationLUTModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(PresentationLUTSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PresentationLUTShape),dst,src);
}

static void copyAcquisitionContextModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(AcquisitionContextSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AcquisitionContextDescription),dst,src);
}

static void copySOPCommonModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(SOPClassUID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SOPInstanceUID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SpecificCharacterSet),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(InstanceCreationDate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(InstanceCreationTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(InstanceCreatorUID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(TimezoneOffsetFromUTC),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(InstanceNumber),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SOPInstanceStatus),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SOPAuthorizationDateTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SOPAuthorizationComment),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AuthorizationEquipmentCertificationNumber),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(MACParametersSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(DigitalSignaturesSequence),dst,src);
}

static void copySCEquipmentModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(ConversionType),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(Modality),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SecondaryCaptureDeviceID),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SecondaryCaptureDeviceManufacturer),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SecondaryCaptureDeviceManufacturerModelName),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SecondaryCaptureDeviceSoftwareVersions),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(VideoImageFormatAcquired),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(DigitalImageFormatAcquired),dst,src);
}

static void copyGeneralImageModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(InstanceNumber),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PatientOrientation),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContentDate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ContentTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ImageType),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AcquisitionNumber),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AcquisitionDate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AcquisitionTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(AcquisitionDateTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ReferencedImageSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(DerivationDescription),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(DerivationCodeSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SourceImageSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ImagesInAcquisition),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ImageComments),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(QualityControlImage),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(BurnedInAnnotation),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(LossyImageCompression),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(LossyImageCompressionRatio),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(IconImageSequence),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PresentationLUTShape),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(IrradiationEventUID),dst,src);
}

static void copyCineModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameTime),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameTimeVector),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(StartTrim),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(StopTrim),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(RecommendedDisplayFrameRate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(CineRate),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameDelay),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(EffectiveDuration),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ActualFrameDuration),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PreferredPlaybackSequencing),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ImageTriggerDelay),dst,src);
}

static void copyMultiFrameModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(NumberOfFrames),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameIncrementPointer),dst,src);
}

static void copyFramePointersModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(RepresentativeFrameNumber),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameNumbersOfInterest),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameOfInterestDescription),dst,src);
}

static void copySCImageModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(DateOfSecondaryCapture),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(TimeOfSecondaryCapture),dst,src);
}

static void copySCMultiFrameImageModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(BurnedInAnnotation),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PresentationLUTShape),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(Illumination),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(ReflectedAmbientLight),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(RescaleIntercept),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(RescaleSlope),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(RescaleType),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameIncrementPointer),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(NominalScannedPixelSpacing),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(DigitizingDeviceTransportDirection),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(RotationOfScannedFilm),dst,src);
}

static void copySCMultiFrameVectorModule(AttributeList &dst,AttributeList &src) {
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameTimeVector),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(PageNumberVector),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameLabelVector),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(FramePrimaryAngleVector),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(FrameSecondaryAngleVector),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(SliceLocationVector),dst,src);
	copyAttributeIfNotAlreadyPresent(TagFromName(DisplayWindowLabelVector),dst,src);
}

static SequenceAttribute *makeNewSequenceAttributeWithoutItem(AttributeList *list,Tag tag) {
	SequenceAttribute *a=new SequenceAttribute(tag);
	Assert(a);
	Assert(list);
	(*list)+=a;
	return a;
}

static AttributeList *makeNewSequenceAttributeWithItem(AttributeList *list,Tag tag) {
	SequenceAttribute *a=makeNewSequenceAttributeWithoutItem(list,tag);
	Assert(a);
	AttributeList *itemlist = new AttributeList();
	Assert(itemlist);
	(*a)+=itemlist;
	return itemlist;
}

static const char *makeCoilTypeFromName(const char *tryValue) {
//cerr << "makeCoilTypeFromName:" << endl;
	const char *useValue=0;
	if (tryValue) {
//cerr << "makeCoilTypeFromName: try " << tryValue << endl;
		if      (findInStringRegardlessOfCase(tryValue,"BODY")) useValue="BODY";
		else if (findInStringRegardlessOfCase(tryValue,"HEAD")) useValue="VOLUME";
		else if (findInStringRegardlessOfCase(tryValue,"NECK")) useValue="VOLUME";
		else if (findInStringRegardlessOfCase(tryValue,"KNEE")) useValue="VOLUME";
		else if (findInStringRegardlessOfCase(tryValue,"SHOULDER")) useValue="SURFACE";
		else if (findInStringRegardlessOfCase(tryValue,"TORSO")) useValue="VOLUME";		// GE
		else if (findInStringRegardlessOfCase(tryValue,"SURFACE")) useValue="SURFACE";
		else if (findInStringRegardlessOfCase(tryValue,"MULTICOIL")) useValue="MULTICOIL";
		else if (findInStringRegardlessOfCase(tryValue,"ARRAY")) useValue="MULTICOIL";
		else if (findInStringRegardlessOfCase(tryValue,"HE")) useValue="VOLUME";		// Siemens head coil
		else if (findInStringRegardlessOfCase(tryValue,"H")) useValue="VOLUME";			// Philips head coil
		else if (findInStringRegardlessOfCase(tryValue,"B")) useValue="BODY";			// Philips body coil
		else if (findInStringRegardlessOfCase(tryValue,"S")) useValue="SURFACE";		// Philips surface coil
		else useValue="UNKNOWN";	// not a standard defined term
	}
	else {
		useValue="UNKNOWN";
	}
//cerr << "makeCoilTypeFromName: return " << useValue << endl;
	return useValue;	
}

static const char *
lookUpAndFillInCodeMeaning(
	const char *csd,
	const char *cv,
	const char *cmDefault,
	const char *csvn)
{
	const char *cm = cmDefault;
	if (csd && cv) {
		if (strcmp(csd,"SNM3") == 0 || strcmp(csd,"SRT") == 0) {
			if      (strcmp(cv,"T-A0100") == 0) { cm="Brain"; }
			else if (strcmp(cv,"T-D1100") == 0) { cm="Head"; }
			else if (strcmp(cv,"T-11100") == 0) { cm="Skull"; }
			else if (strcmp(cv,"T-D1217") == 0) { cm="Maxilla and mandible"; }
			else if (strcmp(cv,"T-45010") == 0) { cm="Carotid artery"; }
			else if (strcmp(cv,"T-D1600") == 0) { cm="Neck"; }
			else if (strcmp(cv,"T-11500") == 0) { cm="Spine"; }
			else if (strcmp(cv,"T-11501") == 0) { cm="Cervical spine"; }
			else if (strcmp(cv,"T-11502") == 0) { cm="Thoracic spine"; }
			else if (strcmp(cv,"T-11503") == 0) { cm="Lumbar spine"; }
			else if (strcmp(cv,"T-11AD0") == 0) { cm="Sacrum"; }
			else if (strcmp(cv,"T-11BF0") == 0) { cm="Coccyx"; }
			else if (strcmp(cv,"T-32000") == 0) { cm="Heart"; }
			else if (strcmp(cv,"T-D3000") == 0) { cm="Chest"; }
			else if (strcmp(cv,"T-D4000") == 0) { cm="Abdomen"; }
			else if (strcmp(cv,"T-62000") == 0) { cm="Liver"; }
			else if (strcmp(cv,"T-D6000") == 0) { cm="Pelvis"; }
			else if (strcmp(cv,"T-D0300") == 0) { cm="Extremity"; }
			else if (strcmp(cv,"T-D8200") == 0) { cm="Arm"; }
			else if (strcmp(cv,"T-D8700") == 0) { cm="Hand"; }
			else if (strcmp(cv,"T-D8600") == 0) { cm="Wrist joint"; }
			else if (strcmp(cv,"T-D8300") == 0) { cm="Elbow"; }
			else if (strcmp(cv,"T-D2220") == 0) { cm="Shoulder"; }
			else if (strcmp(cv,"T-12310") == 0) { cm="Clavicle"; }
			else if (strcmp(cv,"T-15710") == 0) { cm="Hip joint"; }
			else if (strcmp(cv,"T-D9200") == 0) { cm="Knee"; }
			else if (strcmp(cv,"T-D9400") == 0) { cm="Leg"; }
			else if (strcmp(cv,"T-D9100") == 0) { cm="Thigh"; }
			else if (strcmp(cv,"T-D9440") == 0) { cm="Calf of leg"; }
			else if (strcmp(cv,"T-D9700") == 0) { cm="Foot"; }
			else if (strcmp(cv,"T-15750") == 0) { cm="Ankle joint"; }
			else if (strcmp(cv,"T-04000") == 0) { cm="Breast"; }
			else if (strcmp(cv,"T-AB959") == 0) { cm="Internal auditory canal"; }
			else if (strcmp(cv,"T-D1000") == 0) { cm="Head and Neck"; }
			else if (strcmp(cv,"R-FAB52") == 0) { cm="Neck and Chest"; }
			else if (strcmp(cv,"R-FAB53") == 0) { cm="Neck, Chest and Abdomen"; }
			else if (strcmp(cv,"R-FAB54") == 0) { cm="Neck, Chest, Abdomen and Pelvis"; }
			else if (strcmp(cv,"R-FAB55") == 0) { cm="Chest and Abdomen"; }
			else if (strcmp(cv,"R-FAB56") == 0) { cm="Chest, Abdomen and Pelvis"; }
			else if (strcmp(cv,"R-FAB57") == 0) { cm="Abdomen and Pelvis"; }

			else if (strcmp(cv,"F-6175A") == 0) { cm="N-acetylaspartate"; }
			else if (strcmp(cv,"F-61620") == 0) { cm="Choline"; }
			else if (strcmp(cv,"F-61380") == 0) { cm="Creatine"; }
		}
		else if (strcmp(csd,"DCM") == 0) {
			if      (strcmp(cv,"113040") == 0)	{ cm="Lossy Compression"; }
			else if (strcmp(cv,"113041") == 0)	{ cm="Apparent Diffusion Coefficient"; }
			else if (strcmp(cv,"113042") == 0)	{ cm="Pixel by pixel addition"; }
			else if (strcmp(cv,"113043") == 0)	{ cm="Diffusion weighted"; }
			else if (strcmp(cv,"113044") == 0)	{ cm="Diffusion Anisotropy"; }
			else if (strcmp(cv,"113045") == 0)	{ cm="Diffusion Attenuated"; }
			else if (strcmp(cv,"113046") == 0)	{ cm="Pixel by pixel division"; }
			else if (strcmp(cv,"113047") == 0)	{ cm="Pixel by pixel mask"; }
			else if (strcmp(cv,"113048") == 0)	{ cm="Pixel by pixel Maximum"; }
			else if (strcmp(cv,"113049") == 0)	{ cm="Pixel by pixel mean"; }
			else if (strcmp(cv,"113050") == 0)	{ cm="Metabolite Maps from spectroscopy data"; }
			else if (strcmp(cv,"113051") == 0)	{ cm="Pixel by pixel Minimum"; }
			else if (strcmp(cv,"113052") == 0)	{ cm="Mean Transit Time"; }
			else if (strcmp(cv,"113053") == 0)	{ cm="Pixel by pixel multiplication"; }
			else if (strcmp(cv,"113054") == 0)	{ cm="Negative Enhancement Integral"; }
			else if (strcmp(cv,"113055") == 0)	{ cm="Regional Cerebral Blood Flow"; }
			else if (strcmp(cv,"113056") == 0)	{ cm="Regional Cerebral Blood Volume"; }
			else if (strcmp(cv,"113057") == 0)	{ cm="R-Coefficient Map"; }
			else if (strcmp(cv,"113058") == 0)	{ cm="Proton Density map"; }
			else if (strcmp(cv,"113059") == 0)	{ cm="Signal Change Map"; }
			else if (strcmp(cv,"113060") == 0)	{ cm="Signal to Noise Map"; }
			else if (strcmp(cv,"113061") == 0)	{ cm="Standard Deviation"; }
			else if (strcmp(cv,"113062") == 0)	{ cm="Pixel by pixel subtraction"; }
			else if (strcmp(cv,"113063") == 0)	{ cm="T1 Map"; }
			else if (strcmp(cv,"113064") == 0)	{ cm="T2* Map"; }
			else if (strcmp(cv,"113065") == 0)	{ cm="T2 Map"; }
			else if (strcmp(cv,"113066") == 0)	{ cm="Time Course of Signal"; }
			else if (strcmp(cv,"113067") == 0)	{ cm="Temperature encoded"; }
			else if (strcmp(cv,"113068") == 0)	{ cm="StudentÕs T-Test"; }
			else if (strcmp(cv,"113069") == 0)	{ cm="Time To Peak map"; }
			else if (strcmp(cv,"113070") == 0)	{ cm="Velocity encoded"; }
			else if (strcmp(cv,"113071") == 0)	{ cm="Z-Score Map"; }
			else if (strcmp(cv,"113072") == 0)	{ cm="Multiplanar reformat"; }
			else if (strcmp(cv,"113073") == 0)	{ cm="Curved multiplanar reformat"; }
			else if (strcmp(cv,"113074") == 0)	{ cm="Volume rendering"; }
			else if (strcmp(cv,"113075") == 0)	{ cm="Surface rendering"; }
			else if (strcmp(cv,"113076") == 0)	{ cm="Segmentation"; }
			else if (strcmp(cv,"113077") == 0)	{ cm="Volume editing"; }
			else if (strcmp(cv,"113078") == 0)	{ cm="Maximum intensity projection"; }
			else if (strcmp(cv,"113079") == 0)	{ cm="Minimum intensity projection"; }
			else if (strcmp(cv,"113085") == 0)	{ cm="Spatial resampling"; }
			else if (strcmp(cv,"113086") == 0)	{ cm="Edge enhancement"; }
			else if (strcmp(cv,"113087") == 0)	{ cm="Smooth"; }
			else if (strcmp(cv,"113088") == 0)	{ cm="Gaussian blur"; }
			else if (strcmp(cv,"113089") == 0)	{ cm="Unsharp mask"; }
			else if (strcmp(cv,"113090") == 0)	{ cm="Image stitching"; }
			else if (strcmp(cv,"121311") == 0)	{ cm="Localizer"; }
			else if (strcmp(cv,"121322") == 0)	{ cm="Source image for image processing operation"; }
		}
		else if (strcmp(csd,"99PMPMRMF") == 0) {
			if      (strcmp(cv,"000001") == 0)	{ cm="Multiplanar reformat"; }
			else if (strcmp(cv,"000002") == 0)	{ cm="Curved multiplanar reformat"; }
			else if (strcmp(cv,"000003") == 0)	{ cm="Volume rendering"; }
			else if (strcmp(cv,"000004") == 0)	{ cm="Surface rendering"; }
			else if (strcmp(cv,"000005") == 0)	{ cm="Segmentation"; }
			else if (strcmp(cv,"000006") == 0)	{ cm="Volume editing"; }
			else if (strcmp(cv,"000007") == 0)	{ cm="Maximum intensity projection"; }
			else if (strcmp(cv,"000008") == 0)	{ cm="Minimum intensity projection"; }
			else if (strcmp(cv,"000009") == 0)	{ cm="Spatial resampling"; }
			else if (strcmp(cv,"000010") == 0)	{ cm="Edge enhancement"; }
			else if (strcmp(cv,"000011") == 0)	{ cm="Smooth"; }
			else if (strcmp(cv,"000012") == 0)	{ cm="Gaussian blur"; }
			else if (strcmp(cv,"000013") == 0)	{ cm="Unsharp mask"; }
		}
	}
	return cm;
}

static void addCodeSequenceItemToItemList(
	AttributeList *i,
	const char *csd,
	const char *cv,
	const char *cm,
	const char *csvn)
{
	Assert(i);
	cm = lookUpAndFillInCodeMeaning(csd,cv,cm,csvn);
	if (csd  && strlen(csd))  (*i)+=new ShortStringAttribute(TagFromName(CodingSchemeDesignator),csd);
	if (cv   && strlen(cv))   (*i)+=new ShortStringAttribute(TagFromName(CodeValue),cv);
	if (cm   && strlen(cm))   (*i)+=new  LongStringAttribute(TagFromName(CodeMeaning),cm);
	if (csvn && strlen(csvn)) (*i)+=new ShortStringAttribute(TagFromName(CodingSchemeVersion),csvn);
}

static void addCodeSequenceItemToAttribute(
	SequenceAttribute *a,
	const char *csd,
	const char *cv,
	const char *cm,
	const char *csvn)
{
	Assert(a);                                                
	AttributeList *i = new AttributeList();
	Assert(i);
	(*a)+=i;
	addCodeSequenceItemToItemList(i,csd,cv,cm,csvn);
}

static const char *searchStringAttributeForPotentialBodyParts(const char *tryValue) {
//cerr << "searchStringAttributeForPotentialBodyParts:" << endl;
	const char *useValue=0;
	if (tryValue) {
//cerr << "searchStringAttributeForPotentialBodyParts: try " << tryValue << endl;
		if      (findInStringRegardlessOfCase(tryValue,"NECK") && findInStringRegardlessOfCase(tryValue,"CHEST") && findInStringRegardlessOfCase(tryValue,"ABD") && findInStringRegardlessOfCase(tryValue,"PELV")) useValue="NECKCHESTABDPELV";
		else if (findInStringRegardlessOfCase(tryValue,"NECK") && findInStringRegardlessOfCase(tryValue,"CHEST") && findInStringRegardlessOfCase(tryValue,"ABD")) useValue="NECKCHESTABDOMEN";
		else if (findInStringRegardlessOfCase(tryValue,"NECK") && findInStringRegardlessOfCase(tryValue,"CHEST")) useValue="NECKCHEST";
		else if (findInStringRegardlessOfCase(tryValue,"CHEST") && findInStringRegardlessOfCase(tryValue,"ABD") && findInStringRegardlessOfCase(tryValue,"PELV")) useValue="CHESTABDPELVIS";
		else if (findInStringRegardlessOfCase(tryValue,"CHEST") && findInStringRegardlessOfCase(tryValue,"ABD")) useValue="CHESTABDOMEN";
		else if (findInStringRegardlessOfCase(tryValue,"ABD") && findInStringRegardlessOfCase(tryValue,"PELVIS")) useValue="ABDOMENPELVIS";
		else if (findInStringRegardlessOfCase(tryValue,"HEAD") && findInStringRegardlessOfCase(tryValue,"NECK")) useValue="HEADNECK";
		else if (findInStringRegardlessOfCase(tryValue,"BRAIN")) useValue="BRAIN";
		else if (findInStringRegardlessOfCase(tryValue,"HEAD")) useValue="HEAD";
		else if (findInStringRegardlessOfCase(tryValue,"SKULL")) useValue="SKULL";
		else if (findInStringRegardlessOfCase(tryValue,"JAW")) useValue="JAW";
		else if (findInStringRegardlessOfCase(tryValue,"CAROTID")) useValue="CAROTID";
		else if (findInStringRegardlessOfCase(tryValue,"NECK")) useValue="NECK";
		else if (findInStringRegardlessOfCase(tryValue,"SPINE")) useValue="SPINE";
		else if (findInStringRegardlessOfCase(tryValue,"CSPINE")) useValue="CSPINE";
		else if (findInStringRegardlessOfCase(tryValue,"TSPINE")) useValue="TSPINE";
		else if (findInStringRegardlessOfCase(tryValue,"LSPINE")) useValue="LSPINE";
		else if (findInStringRegardlessOfCase(tryValue,"SSPINE")) useValue="SSPINE";
		else if (findInStringRegardlessOfCase(tryValue,"COCCYX")) useValue="COCCYX";
		else if (findInStringRegardlessOfCase(tryValue,"HEART")) useValue="HEART";
		else if (findInStringRegardlessOfCase(tryValue,"CARDIAC")) useValue="HEART";
		else if (findInStringRegardlessOfCase(tryValue,"CHEST")) useValue="CHEST";
		else if (findInStringRegardlessOfCase(tryValue,"ABDOMEN")) useValue="ABDOMEN";
		else if (findInStringRegardlessOfCase(tryValue,"LIVER")) useValue="LIVER";
		else if (findInStringRegardlessOfCase(tryValue,"PELVIS")) useValue="PELVIS";
		else if (findInStringRegardlessOfCase(tryValue,"EXTREMITY")) useValue="EXTREMITY";
		else if (findInStringRegardlessOfCase(tryValue,"ARM")) useValue="ARM";
		else if (findInStringRegardlessOfCase(tryValue,"HAND")) useValue="HAND";
		else if (findInStringRegardlessOfCase(tryValue,"WRIST")) useValue="WRIST";
		else if (findInStringRegardlessOfCase(tryValue,"ELBOW")) useValue="ELBOW";
		else if (findInStringRegardlessOfCase(tryValue,"SHOULDER")) useValue="SHOULDER";
		else if (findInStringRegardlessOfCase(tryValue,"CLAVICLE")) useValue="CLAVICLE";
		else if (findInStringRegardlessOfCase(tryValue,"HIP")) useValue="HIP";
		else if (findInStringRegardlessOfCase(tryValue,"KNEE")) useValue="KNEE";
		else if (findInStringRegardlessOfCase(tryValue,"LEG")) useValue="LEG";
		else if (findInStringRegardlessOfCase(tryValue,"THIGH")) useValue="LEG";
		else if (findInStringRegardlessOfCase(tryValue,"CALF")) useValue="CALF";
		else if (findInStringRegardlessOfCase(tryValue,"FOOT")) useValue="FOOT";
		else if (findInStringRegardlessOfCase(tryValue,"ANKLE")) useValue="ANKLE";
		else if (findInStringRegardlessOfCase(tryValue,"BREAST")) useValue="BREAST";
		else if (findInStringRegardlessOfCase(tryValue,"IAC")) useValue="IAC";
		else if (findInStringRegardlessOfCase(tryValue,"CAP")) useValue="CHESTABDPELVIS";
		else if (findInStringRegardlessOfCase(tryValue,"C/A/P")) useValue="CHESTABDPELVIS";
	}
//cerr << "searchStringAttributeForPotentialBodyParts: return " << useValue << endl;
	return useValue;
}

static SequenceAttribute *makeAnatomicRegionSequenceFromBodyPart(const char *bodyPart) {
	SequenceAttribute *aAnatomicRegionSequence=new SequenceAttribute(TagFromName(AnatomicRegionSequence));
	Assert(aAnatomicRegionSequence);                                                
	
	const char *csd = 0;
	const char *cv = 0;
	
	if (bodyPart) {
		if      (strcmp(bodyPart,"BRAIN") == 0)		{ csd="SNM3"; cv="T-A0100"; }
		else if (strcmp(bodyPart,"HEAD") == 0)		{ csd="SNM3"; cv="T-D1100"; }
		else if (strcmp(bodyPart,"SKULL") == 0)		{ csd="SNM3"; cv="T-11100"; }
		else if (strcmp(bodyPart,"JAW") == 0)		{ csd="SNM3"; cv="T-D1217"; }
		else if (strcmp(bodyPart,"CAROTID") == 0)	{ csd="SNM3"; cv="T-45010"; }
		else if (strcmp(bodyPart,"NECK") == 0)		{ csd="SNM3"; cv="T-D1600"; }
		else if (strcmp(bodyPart,"SPINE") == 0)		{ csd="SNM3"; cv="T-11500"; }
		else if (strcmp(bodyPart,"CSPINE") == 0)	{ csd="SNM3"; cv="T-11501"; }
		else if (strcmp(bodyPart,"TSPINE") == 0)	{ csd="SNM3"; cv="T-11502"; }
		else if (strcmp(bodyPart,"LSPINE") == 0)	{ csd="SNM3"; cv="T-11503"; }
		else if (strcmp(bodyPart,"SSPINE") == 0)	{ csd="SNM3"; cv="T-11AD0"; }
		else if (strcmp(bodyPart,"COCCYX") == 0)	{ csd="SNM3"; cv="T-11BF0"; }
		else if (strcmp(bodyPart,"HEART") == 0)		{ csd="SNM3"; cv="T-32000"; }
		else if (strcmp(bodyPart,"CHEST") == 0)		{ csd="SNM3"; cv="T-D3000"; }
		else if (strcmp(bodyPart,"ABDOMEN") == 0)	{ csd="SNM3"; cv="T-D4000"; }
		else if (strcmp(bodyPart,"LIVER") == 0)		{ csd="SNM3"; cv="T-62000"; }
		else if (strcmp(bodyPart,"PELVIS") == 0)	{ csd="SNM3"; cv="T-D6000"; }
		else if (strcmp(bodyPart,"EXTREMITY") == 0)	{ csd="SNM3"; cv="T-D0300"; }
		else if (strcmp(bodyPart,"ARM") == 0)		{ csd="SNM3"; cv="T-D8200"; }
		else if (strcmp(bodyPart,"HAND") == 0)		{ csd="SNM3"; cv="T-D8700"; }
		else if (strcmp(bodyPart,"WRIST") == 0)		{ csd="SNM3"; cv="T-D8600"; }
		else if (strcmp(bodyPart,"ELBOW") == 0)		{ csd="SNM3"; cv="T-D8300"; }
		else if (strcmp(bodyPart,"SHOULDER") == 0)	{ csd="SNM3"; cv="T-D2220"; }
		else if (strcmp(bodyPart,"CLAVICLE") == 0)	{ csd="SNM3"; cv="T-12310"; }
		else if (strcmp(bodyPart,"HIP") == 0)		{ csd="SNM3"; cv="T-15710"; }
		else if (strcmp(bodyPart,"KNEE") == 0)		{ csd="SNM3"; cv="T-D9200"; }
		else if (strcmp(bodyPart,"LEG") == 0)		{ csd="SNM3"; cv="T-D9400"; }
		else if (strcmp(bodyPart,"THIGH") == 0)		{ csd="SNM3"; cv="T-D9100"; }
		else if (strcmp(bodyPart,"CALF") == 0)		{ csd="SNM3"; cv="T-D9440"; }
		else if (strcmp(bodyPart,"FOOT") == 0)		{ csd="SNM3"; cv="T-D9700"; }
		else if (strcmp(bodyPart,"ANKLE") == 0)		{ csd="SNM3"; cv="T-15750"; }
		else if (strcmp(bodyPart,"BREAST") == 0)	{ csd="SNM3"; cv="T-04000"; }
		else if (strcmp(bodyPart,"IAC") == 0)		{ csd="SNM3"; cv="T-AB959"; }
		else if (strcmp(bodyPart,"HEADNECK") == 0)				{ csd="SRT"; cv="T-D1000"; }
		else if (strcmp(bodyPart,"NECKCHEST") == 0)				{ csd="SRT"; cv="R-FAB52"; } 
		else if (strcmp(bodyPart,"NECKCHESTABDOMEN") == 0)		{ csd="SRT"; cv="R-FAB53"; }
		else if (strcmp(bodyPart,"NECKCHESTABDPELV") == 0)		{ csd="SRT"; cv="R-FAB54"; }
		else if (strcmp(bodyPart,"CHESTABDOMEN") == 0)			{ csd="SRT"; cv="R-FAB55"; }
		else if (strcmp(bodyPart,"CHESTABDPELVIS") == 0)		{ csd="SRT"; cv="R-FAB56"; }
		else if (strcmp(bodyPart,"ABDOMENPELVIS") == 0)			{ csd="SRT"; cv="R-FAB57"; }
	}
	
	addCodeSequenceItemToAttribute(aAnatomicRegionSequence,csd,cv,0,0);

	return aAnatomicRegionSequence;
}

static SequenceAttribute *makeMeasurementUnitsCodeSequence(const char *value,const char *meaning) {
	SequenceAttribute *aMeasurementUnitsCodeSequence=new SequenceAttribute(TagFromName(MeasurementUnitsCodeSequence));
	Assert(aMeasurementUnitsCodeSequence);                                                

	addCodeSequenceItemToAttribute(aMeasurementUnitsCodeSequence,"UCUM",value,meaning,"1.4");

	return aMeasurementUnitsCodeSequence;
}

static SequenceAttribute *makeMeasurementUnitsCodeSequence(const char *value) {
	const char *meaning = value;
	if (value) {
		int length = strlen(value);
		if (strcmp(value,"1") == 0) {
			meaning = "no units";
		}
		else if (length > 2 && value[0] == '{' && value[length-1] == '}') {	// remove braces from meaning
			char *newMeaning = new char[length-1];
			strncpy(newMeaning,value+1,length-2);
			newMeaning[length-2]=0;
			meaning=newMeaning;
		}
	}
	return makeMeasurementUnitsCodeSequence(value,meaning);
}

static void addMetaboliteMapCodeSequence(AttributeList *iMRMetaboliteMapSequence,const char *description) {
//cerr << "addMetaboliteMapCodeSequence:" << endl;
	const char *csd = 0;
	const char *cv = 0;
	
	if (description) {
		if      (strcmp(description,"NAA") == 0)		{ csd="SRT"; cv="F-6175A"; }
		else if (strcmp(description,"Choline") == 0)		{ csd="SRT"; cv="F-61620"; }
		else if (strcmp(description,"Creatine") == 0)		{ csd="SRT"; cv="F-61380"; }
	}
	
	if (csd && cv) {
//cerr << "addMetaboliteMapCodeSequence: adding " << cv << endl;
		SequenceAttribute *aMetaboliteMapCodeSequence=new SequenceAttribute(TagFromName(MetaboliteMapCodeSequence));
		Assert(aMetaboliteMapCodeSequence);
		(*iMRMetaboliteMapSequence)+=aMetaboliteMapCodeSequence;                                                
		addCodeSequenceItemToAttribute(aMetaboliteMapCodeSequence,csd,cv,0,0);
	}
}

static void
makeSupplementalPaletteColorLUT(
		AttributeList *list,
		bool minimalAttributesOnly,
		int addSupplementalPaletteColorLUT_numberOfEntries,
		int addSupplementalPaletteColorLUT_firstValueMapped,
		int addSupplementalPaletteColorLUT_firstRedValue,
		int addSupplementalPaletteColorLUT_incrementRedValue,
		int addSupplementalPaletteColorLUT_entryToStartIncrementingRedValue,
		int addSupplementalPaletteColorLUT_entryToStartDecrementingRedValue,
		int addSupplementalPaletteColorLUT_entryToStopChangingRedValue,
		int addSupplementalPaletteColorLUT_firstGreenValue,
		int addSupplementalPaletteColorLUT_incrementGreenValue,
		int addSupplementalPaletteColorLUT_entryToStartIncrementingGreenValue,
		int addSupplementalPaletteColorLUT_entryToStartDecrementingGreenValue,
		int addSupplementalPaletteColorLUT_entryToStopChangingGreenValue,
		int addSupplementalPaletteColorLUT_firstBlueValue,
		int addSupplementalPaletteColorLUT_incrementBlueValue,
		int addSupplementalPaletteColorLUT_entryToStartIncrementingBlueValue,
		int addSupplementalPaletteColorLUT_entryToStartDecrementingBlueValue,
		int addSupplementalPaletteColorLUT_entryToStopChangingBlueValue) {
//cerr << "makeSupplementalPaletteColorLUT:" << endl;
	Assert(list);
	
	//if (!minimalAttributesOnly) (*list)+=new UnsignedShortAttribute(TagFromName(LargestMonochromePixelValue),
	//	Uint16(addSupplementalPaletteColorLUT_firstValueMapped-1));
			
	(*list)+=new UnsignedShortAttribute(TagFromName(RedPaletteColorLookupTableDescriptor),
			Uint16(addSupplementalPaletteColorLUT_numberOfEntries),
			Uint16(addSupplementalPaletteColorLUT_firstValueMapped),
			Uint16(16));
	(*list)+=new UnsignedShortAttribute(TagFromName(GreenPaletteColorLookupTableDescriptor),
			Uint16(addSupplementalPaletteColorLUT_numberOfEntries),
			Uint16(addSupplementalPaletteColorLUT_firstValueMapped),
			Uint16(16));
	(*list)+=new UnsignedShortAttribute(TagFromName(BluePaletteColorLookupTableDescriptor),
			Uint16(addSupplementalPaletteColorLUT_numberOfEntries),
			Uint16(addSupplementalPaletteColorLUT_firstValueMapped),
			Uint16(16));

	OtherWordSmallNonPixelAttributeBase *aRedPaletteColorLookupTableData = new OtherWordSmallNonPixelAttributeBase(TagFromName(RedPaletteColorLookupTableData));
	Assert(aRedPaletteColorLookupTableData);
	(*list)+=aRedPaletteColorLookupTableData;
	OtherWordSmallNonPixelAttributeBase *aGreenPaletteColorLookupTableData = new OtherWordSmallNonPixelAttributeBase(TagFromName(GreenPaletteColorLookupTableData));
	Assert(aGreenPaletteColorLookupTableData);
	(*list)+=aGreenPaletteColorLookupTableData;
	OtherWordSmallNonPixelAttributeBase *aBluePaletteColorLookupTableData = new OtherWordSmallNonPixelAttributeBase(TagFromName(BluePaletteColorLookupTableData));
	Assert(aBluePaletteColorLookupTableData);
	(*list)+=aBluePaletteColorLookupTableData;

	Uint16 *redValues = new Uint16[addSupplementalPaletteColorLUT_numberOfEntries];
	Assert(redValues);
	Uint16 *greenValues = new Uint16[addSupplementalPaletteColorLUT_numberOfEntries];
	Assert(greenValues);
	Uint16 *blueValues = new Uint16[addSupplementalPaletteColorLUT_numberOfEntries];
	Assert(blueValues);
	Uint16 r = addSupplementalPaletteColorLUT_firstRedValue;
	Uint16 g = addSupplementalPaletteColorLUT_firstGreenValue;
	Uint16 b = addSupplementalPaletteColorLUT_firstBlueValue;
	int i;
	for (i=0; i<addSupplementalPaletteColorLUT_numberOfEntries; ++i) {
//cerr << "r=0x" << hex << r << " g=0x" << g << " b=0x" << b << dec << endl;
		redValues[i]=r;
		if (i <= addSupplementalPaletteColorLUT_entryToStopChangingRedValue) {
			if (i >= addSupplementalPaletteColorLUT_entryToStartDecrementingRedValue) {
				r-=addSupplementalPaletteColorLUT_incrementRedValue;
			}
			else if (i >= addSupplementalPaletteColorLUT_entryToStartIncrementingRedValue) {
				r+=addSupplementalPaletteColorLUT_incrementRedValue;
			}
		}
		greenValues[i]=g;
		if (i <= addSupplementalPaletteColorLUT_entryToStopChangingGreenValue) {
			if (i >= addSupplementalPaletteColorLUT_entryToStartDecrementingGreenValue) {
				g-=addSupplementalPaletteColorLUT_incrementGreenValue;
			}
			else if (i >= addSupplementalPaletteColorLUT_entryToStartIncrementingGreenValue) {
				g+=addSupplementalPaletteColorLUT_incrementGreenValue;
			}
		}
		blueValues[i]=b;
		if (i <= addSupplementalPaletteColorLUT_entryToStopChangingBlueValue) {
			if (i >= addSupplementalPaletteColorLUT_entryToStartDecrementingBlueValue) {
				b-=addSupplementalPaletteColorLUT_incrementBlueValue;
			}
			else if (i >= addSupplementalPaletteColorLUT_entryToStartIncrementingBlueValue) {
				b+=addSupplementalPaletteColorLUT_incrementBlueValue;
			}
		}
	}
	aRedPaletteColorLookupTableData->setValue(redValues,addSupplementalPaletteColorLUT_numberOfEntries);
	aGreenPaletteColorLookupTableData->setValue(greenValues,addSupplementalPaletteColorLUT_numberOfEntries);
	aBluePaletteColorLookupTableData->setValue(blueValues,addSupplementalPaletteColorLUT_numberOfEntries);
	if (redValues) delete[] redValues;
	if (greenValues) delete[] greenValues;
	if (blueValues) delete[] blueValues;
}

static void
makeSequenceUsingSOPInstanceReferenceMacro(
		AttributeList *list,
		bool minimalAttributesOnly,
		Tag sequenceTag,
		const char *studyInstanceUID,
		const char *seriesInstanceUID,
		const char *retrieveAETitle,
		const char *storageMediaFileSetID,
		const char *storageMediaFileSetUID,
		const char *referencedSOPClassUID,
		const char *referencedSOPInstanceUID) {
//cerr << "makeSequenceUsingSOPInstanceReferenceMacro:" << endl;
	Assert(list);
	AttributeList *iStudySequence = makeNewSequenceAttributeWithItem(list,sequenceTag);
	Assert(iStudySequence);
	if (!minimalAttributesOnly) {
		if (studyInstanceUID && strlen(studyInstanceUID))
			(*iStudySequence)+= new UIStringAttribute(TagFromName(StudyInstanceUID),studyInstanceUID);
		AttributeList *iReferencedSeriesSequence  = makeNewSequenceAttributeWithItem(iStudySequence,TagFromName(ReferencedSeriesSequence));
		Assert(iReferencedSeriesSequence);
		if (seriesInstanceUID && strlen(seriesInstanceUID))
			(*iReferencedSeriesSequence)+= new UIStringAttribute(TagFromName(SeriesInstanceUID),seriesInstanceUID);
		if (retrieveAETitle && strlen(retrieveAETitle))
			(*iReferencedSeriesSequence)+= new UIStringAttribute(TagFromName(RetrieveAETitle),retrieveAETitle);
		if (storageMediaFileSetID && strlen(storageMediaFileSetID))
			(*iReferencedSeriesSequence)+= new UIStringAttribute(TagFromName(StorageMediaFileSetID),storageMediaFileSetID);
		if (storageMediaFileSetUID && strlen(storageMediaFileSetUID))
			(*iReferencedSeriesSequence)+= new UIStringAttribute(TagFromName(StorageMediaFileSetUID),storageMediaFileSetUID);
		AttributeList *iReferencedSOPSequence  = makeNewSequenceAttributeWithItem(iReferencedSeriesSequence,TagFromName(ReferencedSOPSequence));
		Assert(iReferencedSOPSequence);
		if (referencedSOPClassUID && strlen(referencedSOPClassUID))
			(*iReferencedSOPSequence)+= new UIStringAttribute(TagFromName(ReferencedSOPClassUID),referencedSOPClassUID);
		if (referencedSOPInstanceUID && strlen(referencedSOPInstanceUID))
			(*iReferencedSOPSequence)+= new UIStringAttribute(TagFromName(ReferencedSOPInstanceUID),referencedSOPInstanceUID);
	}
}		

static void
makeReferencedImageEvidenceSequence(
		AttributeList *list,
		bool minimalAttributesOnly,
		const char *studyInstanceUID,
		const char *seriesInstanceUID,
		const char *referencedSOPClassUID,
		const char *referencedSOPInstanceUID) {
//cerr << "makeReferencedImageEvidenceSequence:" << endl;
	makeSequenceUsingSOPInstanceReferenceMacro(list,minimalAttributesOnly,TagFromName(ReferencedImageEvidenceSequence),
		studyInstanceUID,seriesInstanceUID,NULL,NULL,NULL,referencedSOPClassUID,referencedSOPInstanceUID);
}

static void
makeSourceImageEvidenceSequence(
		AttributeList *list,
		bool minimalAttributesOnly,
		const char *studyInstanceUID,
		const char *seriesInstanceUID,
		const char *referencedSOPClassUID,
		const char *referencedSOPInstanceUID) {
//cerr << "makeSourceImageEvidenceSequence:" << endl;
	makeSequenceUsingSOPInstanceReferenceMacro(list,minimalAttributesOnly,TagFromName(SourceImageEvidenceSequence),
		studyInstanceUID,seriesInstanceUID,NULL,NULL,NULL,referencedSOPClassUID,referencedSOPInstanceUID);
}

static void
makeReferencedImageSequence(
		AttributeList *functionalGroupsSequence,
		bool minimalAttributesOnly,
		const char *referencedSOPClassUID,
		const char *referencedSOPInstanceUID,
		const char *referencedFrameNumbers,
		const char *purposeOfReferenceCodeSequenceCodingSchemeDesignator,
		const char *purposeOfReferenceCodeSequenceCodeValue,
		const char *purposeOfReferenceCodeSequenceCodeMeaning,
		const char *purposeOfReferenceCodeSequenceCodingSchemeVersion) {
//cerr << "makeReferencedImageSequence:" << endl;
	if (minimalAttributesOnly) {
		(void)makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(ReferencedImageSequence));
	}
	else {
		Assert(functionalGroupsSequence);
		AttributeList *iReferencedImageSequence  = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(ReferencedImageSequence));
		if (referencedSOPClassUID && strlen(referencedSOPClassUID))
			(*iReferencedImageSequence)+= new UIStringAttribute(TagFromName(ReferencedSOPClassUID),referencedSOPClassUID);
		if (referencedSOPInstanceUID && strlen(referencedSOPInstanceUID))
			(*iReferencedImageSequence)+= new UIStringAttribute(TagFromName(ReferencedSOPInstanceUID),referencedSOPInstanceUID);
		if (referencedFrameNumbers && strlen(referencedFrameNumbers))
			(*iReferencedImageSequence)+= new IntegerStringAttribute(TagFromName(ReferencedFrameNumber),referencedFrameNumbers);
			// NB. Tag is (0x0008,0x1160) IS Referenced Frame Number; not (0x0040,0xa136) US Referenced Frame Numbers
	
		AttributeList *iPurposeOfReferenceCodeSequence  = makeNewSequenceAttributeWithItem(iReferencedImageSequence,TagFromName(PurposeOfReferenceCodeSequence));
		addCodeSequenceItemToItemList(iPurposeOfReferenceCodeSequence,
			purposeOfReferenceCodeSequenceCodingSchemeDesignator,
			purposeOfReferenceCodeSequenceCodeValue,
			purposeOfReferenceCodeSequenceCodeMeaning,
			purposeOfReferenceCodeSequenceCodingSchemeVersion);
	}
}

static void
makeDerivationImageSequence(
		AttributeList *functionalGroupsSequence,
		bool minimalAttributesOnly,
		const char *derivationCodeSequenceCodingSchemeDesignator,
		const char *derivationCodeSequenceCodeValue,
		const char *derivationCodeSequenceCodeMeaning,
		const char *derivationCodeSequenceCodingSchemeVersion,
		const char *referencedSOPClassUID,
		const char *referencedSOPInstanceUID,
		const char *referencedFrameNumbers,
		const char *purposeOfReferenceCodeSequenceCodingSchemeDesignator,
		const char *purposeOfReferenceCodeSequenceCodeValue,
		const char *purposeOfReferenceCodeSequenceCodeMeaning,
		const char *purposeOfReferenceCodeSequenceCodingSchemeVersion) {
//cerr << "makeDerivationImageSequence:" << endl;
	if (minimalAttributesOnly) {
		(void)makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(DerivationImageSequence));
	}
	else {
		Assert(functionalGroupsSequence);
		AttributeList *iDerivationImageSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(DerivationImageSequence));
		AttributeList *iDerivationCodeSequence  = makeNewSequenceAttributeWithItem(iDerivationImageSequence,TagFromName(DerivationCodeSequence));
		addCodeSequenceItemToItemList(iDerivationCodeSequence,
			derivationCodeSequenceCodingSchemeDesignator,
			derivationCodeSequenceCodeValue,
			derivationCodeSequenceCodeMeaning,
			derivationCodeSequenceCodingSchemeVersion);
		AttributeList *iSourceImageSequence  = makeNewSequenceAttributeWithItem(iDerivationImageSequence,TagFromName(SourceImageSequence));
		if (referencedSOPClassUID && strlen(referencedSOPClassUID))
			(*iSourceImageSequence)+= new UIStringAttribute(TagFromName(ReferencedSOPClassUID),referencedSOPClassUID);
		if (referencedSOPInstanceUID && strlen(referencedSOPInstanceUID))
			(*iSourceImageSequence)+= new UIStringAttribute(TagFromName(ReferencedSOPInstanceUID),referencedSOPInstanceUID);
		if (referencedFrameNumbers && strlen(referencedFrameNumbers))
			(*iSourceImageSequence)+= new IntegerStringAttribute(TagFromName(ReferencedFrameNumber),referencedFrameNumbers);
			// NB. Tag is (0x0008,0x1160) IS Referenced Frame Number; not (0x0040,0xa136) US Referenced Frame Numbers
	
		AttributeList *iPurposeOfReferenceCodeSequence  = makeNewSequenceAttributeWithItem(iSourceImageSequence,TagFromName(PurposeOfReferenceCodeSequence));
		addCodeSequenceItemToItemList(iPurposeOfReferenceCodeSequence,
			purposeOfReferenceCodeSequenceCodingSchemeDesignator,
			purposeOfReferenceCodeSequenceCodeValue,
			purposeOfReferenceCodeSequenceCodeMeaning,
			purposeOfReferenceCodeSequenceCodingSchemeVersion);
	}
}

static void
makeMRSpatialSaturationSequence(
		AttributeList *functionalGroupsSequence,
		bool minimalAttributesOnly,
		int addSatSlab_numberOfSlabs,
		double *addSatSlab_thickness,	
		double *addSatSlab_orientX,double *addSatSlab_orientY,double *addSatSlab_orientZ,	
		double *addSatSlab_midpointX,double *addSatSlab_midpointY,double *addSatSlab_midpointZ) {
//cerr << "makeMRSpatialSaturationSequence:" << endl;
	Assert(addSatSlab_thickness);
	Assert(addSatSlab_orientX);
	Assert(addSatSlab_orientY);
	Assert(addSatSlab_orientZ);
	Assert(addSatSlab_midpointX);
	Assert(addSatSlab_midpointY);
	Assert(addSatSlab_midpointZ);
	Assert(functionalGroupsSequence);
	SequenceAttribute *aMRSpatialSaturationSequence=new SequenceAttribute(TagFromName(MRSpatialSaturationSequence));
	Assert(aMRSpatialSaturationSequence);
	Assert(functionalGroupsSequence);
	(*functionalGroupsSequence)+=aMRSpatialSaturationSequence;
	if (!minimalAttributesOnly) {
		int i;
		for (i=0; i<addSatSlab_numberOfSlabs; ++i) {
			AttributeList *itemlist = new AttributeList();
			Assert(itemlist);
			(*itemlist)+=new FloatDoubleAttribute(TagFromName(SlabThickness),addSatSlab_thickness[i]);
			(*itemlist)+=new FloatDoubleAttribute(TagFromName(SlabOrientation),addSatSlab_orientX[i],addSatSlab_orientY[i],addSatSlab_orientZ[i]);
			(*itemlist)+=new FloatDoubleAttribute(TagFromName(MidSlabPosition),addSatSlab_midpointX[i],addSatSlab_midpointY[i],addSatSlab_midpointZ[i]);
			(*aMRSpatialSaturationSequence)+=itemlist;
		}
	}
}

static bool makeFrameAnatomySequence(
	AttributeList *functionalGroupsSequence,
	bool frameLevel,int frame,
	bool encountered_BodyPartExamined,char *shared_BodyPartExamined,char **perFrame_BodyPartExamined,
	bool encountered_StudyDescription,char *shared_StudyDescription,char **perFrame_StudyDescription,
	bool encountered_SeriesDescription,char *shared_SeriesDescription,char **perFrame_SeriesDescription,
	bool encountered_ImageComments,char *shared_ImageComments,char **perFrame_ImageComments,
	bool encountered_PatientName,char *shared_PatientName,char **perFrame_PatientName,
	bool encountered_Laterality,char *shared_Laterality,char **perFrame_Laterality) {
	
//cerr << "makeFrameAnatomySequence: framelevel " << frameLevel << " frame " << dec << frame << endl;
//cerr << "makeFrameAnatomySequence: shared_BodyPartExamined " << shared_BodyPartExamined << endl;
//cerr << "makeFrameAnatomySequence: shared_StudyDescription " << shared_StudyDescription << endl;
//cerr << "makeFrameAnatomySequence: shared_SeriesDescription " << shared_SeriesDescription << endl;
//cerr << "makeFrameAnatomySequence: shared_ImageComments " << shared_ImageComments << endl;
//cerr << "makeFrameAnatomySequence: shared_Laterality " << shared_Laterality << endl;
	Assert(functionalGroupsSequence);

	// When we want the values for the frame level, use either the per-frame value, or shared value if the former absent
	// When we want the values for the shared level, they all must be from the shared values

	const char *useBodyPart = 0;
	if (encountered_BodyPartExamined) {
		if (frameLevel) {
			useBodyPart=perFrame_BodyPartExamined ? perFrame_BodyPartExamined[frame] : shared_BodyPartExamined;
		}
		else {
			if (!perFrame_BodyPartExamined) {
				useBodyPart=shared_BodyPartExamined;
			}
		}
	}
	if (useBodyPart && strlen(useBodyPart) == 0) useBodyPart=0;
	if (!useBodyPart && encountered_StudyDescription) {
		if (frameLevel) {
			useBodyPart=searchStringAttributeForPotentialBodyParts(perFrame_StudyDescription ? perFrame_StudyDescription[frame] : shared_StudyDescription);
		}
		else {
			if (!perFrame_StudyDescription) {
				useBodyPart=searchStringAttributeForPotentialBodyParts(shared_StudyDescription);
			}
		}
	}
	if (useBodyPart && strlen(useBodyPart) == 0) useBodyPart=0;
	if (!useBodyPart && encountered_SeriesDescription) {
		if (frameLevel) {
			useBodyPart=searchStringAttributeForPotentialBodyParts(perFrame_SeriesDescription ? perFrame_SeriesDescription[frame] : shared_SeriesDescription);
		}
		else {
			if (!perFrame_SeriesDescription) {
				useBodyPart=searchStringAttributeForPotentialBodyParts(shared_SeriesDescription);
			}
		}
	}
	if (useBodyPart && strlen(useBodyPart) == 0) useBodyPart=0;
	if (!useBodyPart && encountered_ImageComments) {
		if (frameLevel) {
			useBodyPart=searchStringAttributeForPotentialBodyParts(perFrame_ImageComments ? perFrame_ImageComments[frame] : shared_ImageComments);
		}
		else {
			if (!perFrame_ImageComments) {
				useBodyPart=searchStringAttributeForPotentialBodyParts(shared_ImageComments);
			}
		}
	}
	if (useBodyPart && strlen(useBodyPart) == 0) useBodyPart=0;
	if (!useBodyPart && encountered_PatientName) {
		if (frameLevel) {
			useBodyPart=searchStringAttributeForPotentialBodyParts(perFrame_PatientName ? perFrame_PatientName[frame] : shared_PatientName);
		}
		else {
			if (!perFrame_PatientName) {
				useBodyPart=searchStringAttributeForPotentialBodyParts(shared_PatientName);
			}
		}
	}
	if (useBodyPart && strlen(useBodyPart) == 0) useBodyPart=0;
	
//cerr << "makeFrameAnatomySequence: useBodyPart " << useBodyPart << endl;
	
	const char *useLaterality;
	if (encountered_Laterality) {
		if (frameLevel) {
			useLaterality=perFrame_Laterality ? perFrame_Laterality[frame] : shared_Laterality;
			useLaterality=useLaterality && strlen(useLaterality) ? useLaterality : "U";		// no further recourse, so assume unpaired
		}
		else {
			if (perFrame_Laterality) {
				useLaterality=0;								// leave it for the frame level
			}
			else {
				useLaterality=shared_Laterality;
				useLaterality=useLaterality && strlen(useLaterality) ? useLaterality : "U";	// shared and zero length, make it unpaired, no better frame level info available
			}
		}
	}
	else {
		useLaterality="U";										// regardless of frame or shared level, it wasn't there
	}
	
//cerr << "makeFrameAnatomySequence: useLaterality " << useLaterality << endl;

	if (useBodyPart && useLaterality) {
		AttributeList *iFrameAnatomySequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(FrameAnatomySequence));
		(*iFrameAnatomySequence)+=makeAnatomicRegionSequenceFromBodyPart(useBodyPart);
		(*iFrameAnatomySequence)+=new CodeStringAttribute(TagFromName(FrameLaterality),useLaterality);
		return true;
	}
	else {
		return false;
	}
}

static void makeRealWorldValueMappingSequence(AttributeList *functionalGroupsSequence,int frame,
		int pixelRepresentation,
		bool encountered_RealWorldValueSlope,double shared_RealWorldValueSlope,double *perFrame_RealWorldValueSlope,
		bool encountered_RealWorldValueIntercept,double shared_RealWorldValueIntercept,double *perFrame_RealWorldValueIntercept,
		bool encountered_Units,char *shared_Units,char **perFrame_Units,
		bool encountered_RealWorldValueFirstValueMapped,Uint32 shared_RealWorldValueFirstValueMapped,Uint32 *perFrame_RealWorldValueFirstValueMapped,
		bool encountered_RealWorldValueLastValueMapped,Uint32 shared_RealWorldValueLastValueMapped,Uint32 *perFrame_RealWorldValueLastValueMapped,
		bool encountered_LUTExplanation,char *shared_LUTExplanation,char **perFrame_LUTExplanation,
		bool encountered_LUTLabel,char *shared_LUTLabel,char **perFrame_LUTLabel) {
//cerr << "makeRealWorldValueMappingSequence" << endl;
	double useRealWorldValueSlope = perFrame_RealWorldValueSlope
		? perFrame_RealWorldValueSlope[frame]
		: (encountered_RealWorldValueSlope ? shared_RealWorldValueSlope : 1.0);
	double useRealWorldValueIntercept = perFrame_RealWorldValueIntercept
		? perFrame_RealWorldValueIntercept[frame]
		: (encountered_RealWorldValueIntercept ? shared_RealWorldValueIntercept : 0.0);
	const char *useUnits = perFrame_Units
		? perFrame_Units[frame]
		: (encountered_Units ? shared_Units : NULL);
	Uint32 useRealWorldValueFirstValueMapped = perFrame_RealWorldValueFirstValueMapped
		? perFrame_RealWorldValueFirstValueMapped[frame]
		: (encountered_RealWorldValueFirstValueMapped ? shared_RealWorldValueFirstValueMapped : 0);
	Uint32 useRealWorldValueLastValueMapped = perFrame_RealWorldValueLastValueMapped
		? perFrame_RealWorldValueLastValueMapped[frame]
		: (encountered_RealWorldValueLastValueMapped ? shared_RealWorldValueLastValueMapped : 0);
	const char *useLUTExplanation = perFrame_LUTExplanation
		? perFrame_LUTExplanation[frame]
		: (encountered_LUTExplanation ? shared_LUTExplanation : "");
	const char *useLUTLabel = perFrame_LUTLabel
		? perFrame_LUTLabel[frame]
		: (encountered_LUTLabel ? shared_LUTLabel : "");
	
	if (useUnits != NULL) {		// use the absence of Units as a flag that THIS FRAME has no real world value mapping
		Assert(functionalGroupsSequence);
		AttributeList *iRealWorldValueMappingSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(RealWorldValueMappingSequence));
		
		(*iRealWorldValueMappingSequence)+=new FloatDoubleAttribute(TagFromName(RealWorldValueIntercept),useRealWorldValueIntercept);
		(*iRealWorldValueMappingSequence)+=new FloatDoubleAttribute(TagFromName(RealWorldValueSlope),useRealWorldValueSlope);
		(*iRealWorldValueMappingSequence)+=makeMeasurementUnitsCodeSequence(useUnits);
		(*iRealWorldValueMappingSequence)+=(pixelRepresentation == 0)
			? (Attribute *)(new UnsignedShortAttribute(TagFromName(RealWorldValueFirstValueMapped),(Uint16)useRealWorldValueFirstValueMapped))
			: (Attribute *)(new SignedShortAttribute(TagFromName(RealWorldValueFirstValueMapped),(Int16)useRealWorldValueFirstValueMapped));
		(*iRealWorldValueMappingSequence)+=(pixelRepresentation == 0)
			? (Attribute *)(new UnsignedShortAttribute(TagFromName(RealWorldValueLastValueMapped),(Uint16)useRealWorldValueLastValueMapped))
			: (Attribute *)(new SignedShortAttribute(TagFromName(RealWorldValueLastValueMapped),(Int16)useRealWorldValueLastValueMapped));
		(*iRealWorldValueMappingSequence)+=new LongStringAttribute(TagFromName(LUTExplanation),useLUTExplanation); 
		(*iRealWorldValueMappingSequence)+=new ShortStringAttribute(TagFromName(LUTLabel),useLUTLabel);
	}
}	

static double
computeAcquisitionDurationInMilliSecondsFromTiming(
		bool encountered_RepetitionTime,double shared_RepetitionTime,double *perFrame_RepetitionTime,
		bool encountered_NumberOfAverages,double shared_NumberOfAverages,double *perFrame_NumberOfAverages,
		bool encountered_AcquisitionMatrix,Uint32 *shared_AcquisitionMatrix,Uint32 **perFrame_AcquisitionMatrix,
		bool encountered_EchoTrainLength,Uint32 shared_EchoTrainLength,Uint32 *perFrame_EchoTrainLength,
		bool encountered_ScanningSequence,char **shared_ScanningSequence,char ***perFrame_ScanningSequence,int nValues_ScanningSequence,
		int frame) {
	double use_RepetitionTime = encountered_RepetitionTime ? (perFrame_RepetitionTime ? perFrame_RepetitionTime[frame] : shared_RepetitionTime) : 0;
	double use_NumberOfAverages = encountered_NumberOfAverages ? (perFrame_NumberOfAverages ? perFrame_NumberOfAverages[frame] : shared_NumberOfAverages) : 0;
	// Acquisition Matrix contains freq rows\freq cols\phase rows\phase cols
	Uint32 *use_AcquisitionMatrix = encountered_AcquisitionMatrix ? (perFrame_AcquisitionMatrix ? perFrame_AcquisitionMatrix[frame] : shared_AcquisitionMatrix) : NULL;
	Uint32 use_frequencyEncodingSteps = use_AcquisitionMatrix ? use_AcquisitionMatrix[0] + use_AcquisitionMatrix[1] : 0;
	Uint32 use_EchoTrainLength = encountered_EchoTrainLength ? (perFrame_EchoTrainLength ? perFrame_EchoTrainLength[frame] : shared_EchoTrainLength) : 1;
	char **use_ScanningSequence = encountered_ScanningSequence ? (perFrame_ScanningSequence ? perFrame_ScanningSequence[0] : shared_ScanningSequence) : NULL;
	bool isEchoPlanar = arrayOfStringValuesContains(use_ScanningSequence,nValues_ScanningSequence,"EP");

//cerr << "computeAcquisitionDurationInMilliSecondsFromTiming: use_frequencyEncodingSteps " << use_frequencyEncodingSteps << endl;
//cerr << "computeAcquisitionDurationInMilliSecondsFromTiming: use_EchoTrainLength " << use_EchoTrainLength << endl;
//cerr << "computeAcquisitionDurationInMilliSecondsFromTiming: use_NumberOfAverages " << use_NumberOfAverages << endl;
//cerr << "computeAcquisitionDurationInMilliSecondsFromTiming: use_RepetitionTime " << use_RepetitionTime << endl;

	return use_RepetitionTime*use_NumberOfAverages*(isEchoPlanar ? 1 : use_frequencyEncodingSteps)/(use_EchoTrainLength == 0 ? 1 : use_EchoTrainLength);
}

static void makeMRFOVGeometrySequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,const char *requiredMRAcquisitionType,
		bool encountered_MRAcquisitionPhaseEncodingStepsOutOfPlane,Uint32 shared_MRAcquisitionPhaseEncodingStepsOutOfPlane,Uint32 *perFrame_MRAcquisitionPhaseEncodingStepsOutOfPlane,
		bool encountered_InPlanePhaseEncodingDirection,char *shared_InPlanePhaseEncodingDirection,char **perFrame_InPlanePhaseEncodingDirection,
		bool encountered_NumberOfPhaseEncodingSteps,Uint32 shared_NumberOfPhaseEncodingSteps,Uint32 *perFrame_NumberOfPhaseEncodingSteps,
		bool encountered_PercentSampling,double shared_PercentSampling,double *perFrame_PercentSampling,
		bool encountered_PercentPhaseFieldOfView,double shared_PercentPhaseFieldOfView,double *perFrame_PercentPhaseFieldOfView,
		bool encountered_AcquisitionMatrix,Uint32 *shared_AcquisitionMatrix,Uint32 **perFrame_AcquisitionMatrix) {
//cerr << "makeMRFOVGeometrySequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iMRFOVGeometrySequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(MRFOVGeometrySequence));
	
	if (treatAsNotOriginalFrame) return;
	
	Uint32 matrix_MRAcquisitionPhaseEncodingStepsInPlane = 0;
	Uint32 matrix_MRAcquisitionFrequencyEncodingSteps = 0;
	const char *matrix_InPlanePhaseEncodingDirection = "";
	// Acquisition Matrix contains freq rows\freq cols\phase rows\phase cols
	Uint32 *use_AcquisitionMatrix = encountered_AcquisitionMatrix ?
		(perFrame_AcquisitionMatrix ? perFrame_AcquisitionMatrix[frame] : shared_AcquisitionMatrix) : NULL;
	if (use_AcquisitionMatrix) {
//cerr << "use_AcquisitionMatrix " << dec << use_AcquisitionMatrix[0] << "," << use_AcquisitionMatrix[1] << "," << use_AcquisitionMatrix[2] << "," << use_AcquisitionMatrix[3] << endl;
		matrix_MRAcquisitionPhaseEncodingStepsInPlane = use_AcquisitionMatrix[2] + use_AcquisitionMatrix[3];
		matrix_MRAcquisitionFrequencyEncodingSteps = use_AcquisitionMatrix[0] + use_AcquisitionMatrix[1];
		if      (use_AcquisitionMatrix[2] > 0 && use_AcquisitionMatrix[3] == 0) matrix_InPlanePhaseEncodingDirection = "ROW";
		else if (use_AcquisitionMatrix[2] == 0 && use_AcquisitionMatrix[3] > 0) matrix_InPlanePhaseEncodingDirection = "COLUMN";
	}

	const char *use_InPlanePhaseEncodingDirection = encountered_InPlanePhaseEncodingDirection ?
		(perFrame_InPlanePhaseEncodingDirection ? perFrame_InPlanePhaseEncodingDirection[frame] : shared_InPlanePhaseEncodingDirection) : "";
//cerr << "use_InPlanePhaseEncodingDirection " << use_InPlanePhaseEncodingDirection << endl;
	// New MR object uses COLUMN rather than COL in old MR object
	if (use_InPlanePhaseEncodingDirection && strcmp(use_InPlanePhaseEncodingDirection,"COL") == 0) use_InPlanePhaseEncodingDirection="COLUMN";
//cerr << "use_InPlanePhaseEncodingDirection " << use_InPlanePhaseEncodingDirection << endl;
	if (!use_InPlanePhaseEncodingDirection || !strlen(use_InPlanePhaseEncodingDirection)) use_InPlanePhaseEncodingDirection = matrix_InPlanePhaseEncodingDirection;
//cerr << "use_InPlanePhaseEncodingDirection " << use_InPlanePhaseEncodingDirection << endl;
	
	Uint32 use_MRAcquisitionPhaseEncodingStepsInPlane = encountered_NumberOfPhaseEncodingSteps ?
		(perFrame_NumberOfPhaseEncodingSteps ? perFrame_NumberOfPhaseEncodingSteps[frame] : shared_NumberOfPhaseEncodingSteps) : Uint32(0);
//cerr << "use_MRAcquisitionPhaseEncodingStepsInPlane " << dec << use_MRAcquisitionPhaseEncodingStepsInPlane << endl;
	if (use_MRAcquisitionPhaseEncodingStepsInPlane == 0) use_MRAcquisitionPhaseEncodingStepsInPlane = matrix_MRAcquisitionPhaseEncodingStepsInPlane;
//cerr << "use_MRAcquisitionPhaseEncodingStepsInPlane " << dec << use_MRAcquisitionPhaseEncodingStepsInPlane << endl;
	
	Uint32 use_MRAcquisitionPhaseEncodingStepsOutOfPlane = encountered_MRAcquisitionPhaseEncodingStepsOutOfPlane ?
		(perFrame_MRAcquisitionPhaseEncodingStepsOutOfPlane ? perFrame_MRAcquisitionPhaseEncodingStepsOutOfPlane[frame] : shared_MRAcquisitionPhaseEncodingStepsOutOfPlane) : Uint32(0);

	Uint32 use_MRAcquisitionFrequencyEncodingSteps = matrix_MRAcquisitionFrequencyEncodingSteps;		// Nowhere else to get it from :(
	
	double use_PercentSampling = encountered_PercentSampling ?
		(perFrame_PercentSampling ? perFrame_PercentSampling[frame] : shared_PercentSampling) : double(100);
	double use_PercentPhaseFieldOfView = encountered_PercentPhaseFieldOfView ?
		(perFrame_PercentPhaseFieldOfView ? perFrame_PercentPhaseFieldOfView[frame] : shared_PercentPhaseFieldOfView) : double(100);

	(*iMRFOVGeometrySequence)+=new CodeStringAttribute(TagFromName(InPlanePhaseEncodingDirection),use_InPlanePhaseEncodingDirection);
	(*iMRFOVGeometrySequence)+=new UnsignedShortAttribute(TagFromName(MRAcquisitionFrequencyEncodingSteps),use_MRAcquisitionFrequencyEncodingSteps);
	(*iMRFOVGeometrySequence)+=new UnsignedShortAttribute(TagFromName(MRAcquisitionPhaseEncodingStepsInPlane),use_MRAcquisitionPhaseEncodingStepsInPlane);
	if (requiredMRAcquisitionType && strlen(requiredMRAcquisitionType) > 0 && strcmp(requiredMRAcquisitionType,"3D") == 0) {
		(*iMRFOVGeometrySequence)+=new UnsignedShortAttribute(TagFromName(MRAcquisitionPhaseEncodingStepsOutOfPlane),use_MRAcquisitionPhaseEncodingStepsOutOfPlane);
	}
	(*iMRFOVGeometrySequence)+=new DecimalStringAttribute(TagFromName(PercentSampling),use_PercentSampling);
	(*iMRFOVGeometrySequence)+=new DecimalStringAttribute(TagFromName(PercentPhaseFieldOfView),use_PercentPhaseFieldOfView);
}
								
static void makeMRTimingAndRelatedParametersSequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,const char *vEchoPulseSequence,
		bool encountered_RepetitionTime,double shared_RepetitionTime,double *perFrame_RepetitionTime,
		bool encountered_FlipAngle,double shared_FlipAngle,double *perFrame_FlipAngle,
		bool encountered_EchoTrainLength,Uint32 shared_EchoTrainLength,Uint32 *perFrame_EchoTrainLength,
		bool encountered_RFEchoTrainLength,Uint32 shared_RFEchoTrainLength,Uint32 *perFrame_RFEchoTrainLength,
		bool encountered_GradientEchoTrainLength,Uint32 shared_GradientEchoTrainLength,Uint32 *perFrame_GradientEchoTrainLength,
		bool encountered_SAR,double shared_SAR,double *perFrame_SAR,
		bool encountered_GradientOutputType,char *shared_GradientOutputType,char **perFrame_GradientOutputType,
		bool encountered_GradientOutput,double shared_GradientOutput,double *perFrame_GradientOutput
		) {
//cerr << "makeMRTimingAndRelatedParametersSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iMRTimingAndRelatedParametersSequence = makeNewSequenceAttributeWithItem(
		functionalGroupsSequence,TagFromName(MRTimingAndRelatedParametersSequence));
	
	if (treatAsNotOriginalFrame) return;
	
	(*iMRTimingAndRelatedParametersSequence)+=new DecimalStringAttribute(TagFromName(RepetitionTime),
		perFrame_RepetitionTime ? perFrame_RepetitionTime[frame] : (encountered_RepetitionTime ? shared_RepetitionTime : double(0)));
	(*iMRTimingAndRelatedParametersSequence)+=new DecimalStringAttribute(TagFromName(FlipAngle),
		perFrame_FlipAngle ? perFrame_FlipAngle[frame] : (encountered_FlipAngle ? shared_FlipAngle : double(90)));
		
	Uint32 use_EchoTrainLength = perFrame_EchoTrainLength ? perFrame_EchoTrainLength[frame] : (encountered_EchoTrainLength ? shared_EchoTrainLength : Uint32(1));

	Uint16 use_RFEchoTrainLength = 0;
	if (encountered_RFEchoTrainLength) {
		use_RFEchoTrainLength = perFrame_RFEchoTrainLength ? perFrame_RFEchoTrainLength[frame] : shared_RFEchoTrainLength;
	}
	else if (encountered_EchoTrainLength && vEchoPulseSequence && strcmp(vEchoPulseSequence,"SPIN") == 0) {
		use_RFEchoTrainLength = use_EchoTrainLength;
	}
	
	Uint16 use_GradientEchoTrainLength = 0;
	if (encountered_GradientEchoTrainLength) {
		use_GradientEchoTrainLength = perFrame_GradientEchoTrainLength ? perFrame_GradientEchoTrainLength[frame] : shared_GradientEchoTrainLength;
	}
	else if (encountered_EchoTrainLength && vEchoPulseSequence && strcmp(vEchoPulseSequence,"GRADIENT") == 0) {
		use_GradientEchoTrainLength = use_EchoTrainLength;
	}

	(*iMRTimingAndRelatedParametersSequence)+=new IntegerStringAttribute(TagFromName(EchoTrainLength),use_EchoTrainLength);
	(*iMRTimingAndRelatedParametersSequence)+=new UnsignedShortAttribute(TagFromName(RFEchoTrainLength),use_RFEchoTrainLength);
	(*iMRTimingAndRelatedParametersSequence)+=new UnsignedShortAttribute(TagFromName(GradientEchoTrainLength),use_GradientEchoTrainLength);
								
	AttributeList *iSpecificAbsorptionRateSequence = makeNewSequenceAttributeWithItem(
		iMRTimingAndRelatedParametersSequence,TagFromName(SpecificAbsorptionRateSequence));
	(*iSpecificAbsorptionRateSequence)+=new CodeStringAttribute(TagFromName(SpecificAbsorptionRateDefinition),"IEC_WHOLE_BODY"); // just a guess :(
	(*iSpecificAbsorptionRateSequence)+=new FloatDoubleAttribute(TagFromName(SpecificAbsorptionRateValue),
		perFrame_SAR ? perFrame_SAR[frame] : (encountered_SAR ? shared_SAR : double(0)));
	
	if (encountered_GradientOutputType || encountered_GradientOutput) {
		(*iMRTimingAndRelatedParametersSequence)+=new CodeStringAttribute(TagFromName(GradientOutputType),
			perFrame_GradientOutputType ? perFrame_GradientOutputType[frame] : (encountered_GradientOutputType ? shared_GradientOutputType : ""));
		(*iMRTimingAndRelatedParametersSequence)+=new FloatDoubleAttribute(TagFromName(GradientOutput),
			perFrame_GradientOutput ? perFrame_GradientOutput[frame] : (encountered_GradientOutput ? shared_GradientOutput : double(0)));
	}
	
	SequenceAttribute *aOperatingModeSequence=new SequenceAttribute(TagFromName(OperatingModeSequence));
	Assert(aOperatingModeSequence);
	(*iMRTimingAndRelatedParametersSequence)+=aOperatingModeSequence;
	{
		AttributeList *item = new AttributeList();
		Assert(item);
		(*aOperatingModeSequence)+=item;
		(*item)+=new CodeStringAttribute(TagFromName(OperatingModeType),"STATIC FIELD");
		(*item)+=new CodeStringAttribute(TagFromName(OperatingMode),"IEC_NORMAL");
	}
	{
		AttributeList *item = new AttributeList();
		Assert(item);
		(*aOperatingModeSequence)+=item;
		(*item)+=new CodeStringAttribute(TagFromName(OperatingModeType),"RF");
		(*item)+=new CodeStringAttribute(TagFromName(OperatingMode),"IEC_NORMAL");
	}
	{
		AttributeList *item = new AttributeList();
		Assert(item);
		(*aOperatingModeSequence)+=item;
		(*item)+=new CodeStringAttribute(TagFromName(OperatingModeType),"GRADIENT");
		(*item)+=new CodeStringAttribute(TagFromName(OperatingMode),"IEC_NORMAL");
	}
}

static void makeMRModifierSequence(
		AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,
		bool encountered_InversionTime,double shared_InversionTime,double *perFrame_InversionTime,
		bool     isGradientRecalled,
		bool    isInversionRecovery,
		bool     isFlowCompensation,
		bool             isSpoiling,
		bool isSpatialPresaturation,
		bool   isPartialFourierFreq,
		bool  isPartialFourierPhase) {
//cerr << "makeMRModifierSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iMRModifierSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(MRModifierSequence));

	if (treatAsNotOriginalFrame) return;
	
	(*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(InversionRecovery),isInversionRecovery ? "YES" : "NO");
	if (isInversionRecovery && encountered_InversionTime) {
		(*iMRModifierSequence)+=new FloatDoubleAttribute(TagFromName(InversionTimes),perFrame_InversionTime ? perFrame_InversionTime[frame] : shared_InversionTime);
	}
	(*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(FlowCompensation),isFlowCompensation ? "OTHER" : "NONE");			// unknown ACCELERATION, VELOCITY
	if (isFlowCompensation) (*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(FlowCompensationDirection),"OTHER");		// direction unknown
	if (isGradientRecalled) (*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(Spoiling),isSpoiling ? "GRADIENT" : "NONE");	// unknown RF or RF_AND_GRADIENT
	(*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(T2Preparation),"NO");							// unknown :(
	(*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(SpectrallySelectedExcitation),"NONE");					// unknown :(
	(*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(SpatialPresaturation),isSpatialPresaturation ? "SLAB" : "NONE");
	{
		const char *partialFourier;
		if (isPartialFourierFreq || isPartialFourierPhase) {
			partialFourier="YES";
			(*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(PartialFourierDirection),
				isPartialFourierFreq ? (isPartialFourierPhase ? "COMBINATION" : "FREQUENCY") : "PHASE");
		}
		else {
			partialFourier="NO";
		}
		(*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(PartialFourier),partialFourier);
	}
	(*iMRModifierSequence)+=new CodeStringAttribute(TagFromName(ParallelAcquisition),"NO");					// unknown :(
}

static void makeMRImagingModifierSequence(
		AttributeList *functionalGroupsSequence,
		bool treatAsNotOriginalFrame,
		bool isMagnetizationTransfer,
		double imagingFrequency,
		double pixelBandwidth) {
//cerr << "makeMRImagingModifierSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iMRImagingModifierSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(MRImagingModifierSequence));
	
	if (treatAsNotOriginalFrame) return;
	
	(*iMRImagingModifierSequence)+=new CodeStringAttribute(TagFromName(MagnetizationTransfer),isMagnetizationTransfer ? "OFF_RESONANCE" : "NONE");	// ON_RESONANCE ?
	(*iMRImagingModifierSequence)+=new CodeStringAttribute(TagFromName(BloodSignalNulling),"NO");
	(*iMRImagingModifierSequence)+=new CodeStringAttribute(TagFromName(Tagging),"NONE");
	(*iMRImagingModifierSequence)+=new FloatDoubleAttribute(TagFromName(TransmitterFrequency),imagingFrequency);
	(*iMRImagingModifierSequence)+=new DecimalStringAttribute(TagFromName(PixelBandwidth),pixelBandwidth);
}

static void makeMRDiffusionSequence(
		AttributeList *functionalGroupsSequence,
		bool treatAsNotOriginalFrame,
		double use_DiffusionBValue,
		const char *use_DiffusionDirectionality) {
//cerr << "makeMRDiffusionSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iMRDiffusionSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(MRDiffusionSequence));
	
	if (treatAsNotOriginalFrame) return;
	
	(*iMRDiffusionSequence)+=new FloatDoubleAttribute(TagFromName(DiffusionBValue),use_DiffusionBValue);
	(*iMRDiffusionSequence)+=new CodeStringAttribute(TagFromName(DiffusionDirectionality),use_DiffusionDirectionality);
}

static void makeMRVelocityEncodingSequence(
										   AttributeList *functionalGroupsSequence,
										   bool treatAsNotOriginalFrame,
										   double   *useVelocityEncodingDirection,
										   double useVelocityEncodingMinimumValue,
										   double useVelocityEncodingMaximumValue) {
	//cerr << "makeMRVelocityEncodingSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iMRVelocityEncodingSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(MRVelocityEncodingSequence));
	
	if (treatAsNotOriginalFrame) return;
	
	if (useVelocityEncodingDirection) {
		(*iMRVelocityEncodingSequence)+=new FloatDoubleAttribute(TagFromName(VelocityEncodingDirection),useVelocityEncodingDirection[0],useVelocityEncodingDirection[1],useVelocityEncodingDirection[2]);
	}
	(*iMRVelocityEncodingSequence)+=new FloatDoubleAttribute(TagFromName(VelocityEncodingMinimumValue),useVelocityEncodingMinimumValue);
	(*iMRVelocityEncodingSequence)+=new FloatDoubleAttribute(TagFromName(VelocityEncodingMaximumValue),useVelocityEncodingMaximumValue);
}

static void makeVelocityEncodingAcquisitionSequence(AttributeList *list,double *useVelocityEncodingDirection) {
	//cerr << "makeMRVelocityAcquisitionEncodingSequence" << endl;
	AttributeList *iVelocityEncodingAcquisitionSequence = makeNewSequenceAttributeWithItem(list,TagFromName(VelocityEncodingAcquisitionSequence));
	
	if (useVelocityEncodingDirection) {
		(*iVelocityEncodingAcquisitionSequence)+=new FloatDoubleAttribute(TagFromName(VelocityEncodingDirection),useVelocityEncodingDirection[0],useVelocityEncodingDirection[1],useVelocityEncodingDirection[2]);
	}
}


static void makeMRPulseSequenceModule(
		AttributeList *list,int numberofinputfiles,
		const char *requiredSequenceName,const char *requiredProtocolName,const char *requiredMRAcquisitionType,
		bool encountered_ScanningSequence,char **shared_ScanningSequence,char ***perFrame_ScanningSequence,int nValues_ScanningSequence,
		bool encountered_SequenceVariant,char **shared_SequenceVariant,char ***perFrame_SequenceVariant,int nValues_SequenceVariant,
		bool encountered_ScanOptions,char **shared_ScanOptions,char ***perFrame_ScanOptions,int nValues_ScanOptions,
		bool encountered_EchoNumbers,Uint32 shared_EchoNumbers,Uint32 *perFrame_EchoNumbers,
		bool encountered_VelocityEncodingDirection,double *shared_VelocityEncodingDirection,double **perFrame_VelocityEncodingDirection,
		bool phaseContrast) {
//cerr << "makeMRPulseSequenceModule" << endl;
	Assert(list);
	(*list)+=new ShortStringAttribute(TagFromName(PulseSequenceName),
			requiredSequenceName && strlen(requiredSequenceName)
			? requiredSequenceName
			: (requiredProtocolName && strlen(requiredProtocolName) ? requiredProtocolName : "UNNAMED"));
							
	const char *useMRAcquisitionType = requiredMRAcquisitionType && strlen(requiredMRAcquisitionType) ? requiredMRAcquisitionType : "2D";
	(*list)+=new CodeStringAttribute(TagFromName(MRAcquisitionType),useMRAcquisitionType);
	if (strcmp(useMRAcquisitionType,"3D") == 0) 
		(*list)+=new CodeStringAttribute(TagFromName(CoverageOfKSpace),"FULL");		// nop way to determine ELLIPTICAL, WEIGHTED
					
	// choose based on shared Scanning Sequence if possible, otherwise give up and just use first ...
						
	char **useScanningSequence = encountered_ScanningSequence ? 
		(perFrame_ScanningSequence ? perFrame_ScanningSequence[0] : shared_ScanningSequence) : NULL;
	char **useSequenceVariant = encountered_SequenceVariant ? 
		(perFrame_SequenceVariant ? perFrame_SequenceVariant[0] : shared_SequenceVariant) : NULL;
	char **useScanOptions = encountered_ScanOptions ? 
		(perFrame_ScanOptions ? perFrame_ScanOptions[0] : shared_ScanOptions) : NULL;
						
	bool     isSpinEcho       = arrayOfStringValuesContains(useScanningSequence,nValues_ScanningSequence,"SE")
				 || arrayOfStringValuesContains(useScanningSequence,nValues_ScanningSequence,"IR");	// ?? but some images have IR alone :(
	bool isGradientEcho       = arrayOfStringValuesContains(useScanningSequence,nValues_ScanningSequence,"GR");
	bool   isEchoPlanar       = arrayOfStringValuesContains(useScanningSequence,nValues_ScanningSequence,"EP");
	bool isSaturationRecovery = arrayOfStringValuesContains(useScanningSequence,nValues_ScanningSequence,"SR");	// not a standard enumerated value
						
	bool               isTRSS = arrayOfStringValuesContains(useSequenceVariant,nValues_SequenceVariant,"TRSS");
	bool                 isSS = arrayOfStringValuesContains(useSequenceVariant,nValues_SequenceVariant,"SS");
	bool    isSegmentedKSpace = arrayOfStringValuesContains(useSequenceVariant,nValues_SequenceVariant,"SK");
						
	bool                 isFS = arrayOfStringValuesContains(useScanOptions,nValues_ScanOptions,"FS");
						
	(*list)+=new CodeStringAttribute(TagFromName(EchoPulseSequence),
		isSpinEcho ? (isGradientEcho ? "BOTH" : "SPIN") : (isGradientEcho ? "GRADIENT" : "SPIN"));	// spin by default since no other or none :(
							
	(*list)+=new CodeStringAttribute(TagFromName(SteadyStatePulseSequence),
		isTRSS ? "TIME_REVERSED" : (isSS ? "FREE_PRECESSION" : "NONE"));	// can't distinguish FREE_PRECESSION, TRANSVERSE, LONGITUDINAL 

	(*list)+=new CodeStringAttribute(TagFromName(EchoPlanarPulseSequence),isEchoPlanar ? "YES" : "NO");
	(*list)+=new CodeStringAttribute(TagFromName(SaturationRecovery),isSaturationRecovery ? "YES" : "NO");
	(*list)+=new CodeStringAttribute(TagFromName(SpectrallySelectedSuppression),isFS ? "FAT" : "NONE");		// can't determine WATER, SILICON_GEL
						
	(*list)+=new CodeStringAttribute(TagFromName(SegmentedKSpaceTraversal),isEchoPlanar ? "FULL" : "SINGLE");	// can't determine PARTIAL
						
	if (isSpinEcho) {
		(*list)+=new CodeStringAttribute(TagFromName(MultipleSpinEcho),
			countDifferentSingleValuedUint32Values(encountered_EchoNumbers,shared_EchoNumbers,perFrame_EchoNumbers,numberofinputfiles) > 1
			? "YES" : "NO");
							
	}
	(*list)+=new CodeStringAttribute(TagFromName(RectilinearPhaseEncodeReordering),isSegmentedKSpace ? "SEGMENTED" : "LINEAR");

	// Fixed values ... no way to automatically derive these ... :(
					
	(*list)+=new CodeStringAttribute(TagFromName(MultiPlanarExcitation),"NO");
	(*list)+=new CodeStringAttribute(TagFromName(PhaseContrast),phaseContrast ? "YES" : "NO");
	(*list)+=new CodeStringAttribute(TagFromName(TimeOfFlightContrast),"NO");
	(*list)+=new CodeStringAttribute(TagFromName(GeometryOfKSpaceTraversal),"RECTILINEAR");
	(*list)+=new CodeStringAttribute(TagFromName(OversamplingPhase),"NONE");
	(*list)+=new UnsignedShortAttribute(TagFromName(NumberOfKSpaceTrajectories),Uint32(1));
	
	if (phaseContrast && encountered_VelocityEncodingDirection) {
		double   *useVelocityEncodingDirection = perFrame_VelocityEncodingDirection == NULL ? shared_VelocityEncodingDirection : perFrame_VelocityEncodingDirection[0];
		if (useVelocityEncodingDirection) {
			makeVelocityEncodingAcquisitionSequence(list,useVelocityEncodingDirection);
		}
	}
	
}

static const char* getAcquisitionTypeToUse(int frame,bool encountered_ImageType,char **shared_ImageType,char ***perFrame_ImageType,int nValues_ImageType,
		bool encountered_AcquisitionType,char *shared_AcquisitionType,char **perFrame_AcquisitionType) {
	const char *imageTypeValue3 = encountered_ImageType && nValues_ImageType > 2
		? (perFrame_ImageType && perFrame_ImageType[frame] ? perFrame_ImageType[frame][2] : (shared_ImageType ? shared_ImageType [2] : "")) : "";
	const char *defaultAcquisitionType = strcmp(imageTypeValue3,"LOCALIZER") == 0 ? "CONSTANT_ANGLE" : "SEQUENCED";
	const char *useAcquisitionType = perFrame_AcquisitionType ? perFrame_AcquisitionType[frame] : (encountered_AcquisitionType ? shared_AcquisitionType : defaultAcquisitionType);
	return useAcquisitionType;
}

static void makeCTAcquisitionTypeSequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,
		bool encountered_ImageType,char **shared_ImageType,char ***perFrame_ImageType,int nValues_ImageType,
		bool encountered_AcquisitionType,char *shared_AcquisitionType,char **perFrame_AcquisitionType,
		bool encountered_TubeAngle,double shared_TubeAngle,double *perFrame_TubeAngle,
		bool encountered_ConstantVolumeFlag,char *shared_ConstantVolumeFlag,char **perFrame_ConstantVolumeFlag,
		bool encountered_FluoroscopyFlag,char *shared_FluoroscopyFlag,char **perFrame_FluoroscopyFlag) {
//cerr << "makeCTAcquisitionTypeSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iCTAcquisitionTypeSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(CTAcquisitionTypeSequence));
	if (treatAsNotOriginalFrame) return;

	const char *useAcquisitionType = getAcquisitionTypeToUse(frame,encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
			encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType);
	(*iCTAcquisitionTypeSequence)+=new CodeStringAttribute(TagFromName(AcquisitionType),useAcquisitionType);
	if (strcmp(useAcquisitionType,"CONSTANT_ANGLE") == 0) {
		double useTubeAngle = perFrame_AcquisitionType ? perFrame_TubeAngle[frame] : (encountered_TubeAngle ? shared_TubeAngle : double(0.0));
		(*iCTAcquisitionTypeSequence)+=new FloatDoubleAttribute(TagFromName(TubeAngle),useTubeAngle);
	}
	(*iCTAcquisitionTypeSequence)+=new CodeStringAttribute(TagFromName(ConstantVolumeFlag),
			perFrame_ConstantVolumeFlag ? perFrame_ConstantVolumeFlag[frame] : (encountered_ConstantVolumeFlag ? shared_ConstantVolumeFlag : "NO"));
	(*iCTAcquisitionTypeSequence)+=new CodeStringAttribute(TagFromName(FluoroscopyFlag),
			perFrame_FluoroscopyFlag ? perFrame_FluoroscopyFlag[frame] : (encountered_FluoroscopyFlag ? shared_FluoroscopyFlag : "NO"));
}

static void makeCTAcquisitionDetailsSequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,
		bool encountered_ImageType,char **shared_ImageType,char ***perFrame_ImageType,int nValues_ImageType,
		bool encountered_AcquisitionType,char *shared_AcquisitionType,char **perFrame_AcquisitionType,
		bool encountered_RotationDirection,char *shared_RotationDirection,char **perFrame_RotationDirection,
		bool encountered_RevolutionTime,double shared_RevolutionTime,double *perFrame_RevolutionTime,
		bool encountered_SingleCollimationWidth,double shared_SingleCollimationWidth,double *perFrame_SingleCollimationWidth,
		bool encountered_TotalCollimationWidth,double shared_TotalCollimationWidth,double *perFrame_TotalCollimationWidth,
		bool encountered_TableHeight,double shared_TableHeight,double *perFrame_TableHeight,
		bool encountered_GantryDetectorTilt,double shared_GantryDetectorTilt,double *perFrame_GantryDetectorTilt,
		bool encountered_DataCollectionDiameter,double shared_DataCollectionDiameter,double *perFrame_DataCollectionDiameter) {
//cerr << "makeCTAcquisitionDetailsSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iCTAcquisitionDetailsSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(CTAcquisitionDetailsSequence));
	if (treatAsNotOriginalFrame) return;

	const char *useAcquisitionType = getAcquisitionTypeToUse(frame,encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
			encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType);

	if (strcmp(useAcquisitionType,"CONSTANT_ANGLE") != 0) {
		const char *useRotationDirection = perFrame_RotationDirection ? perFrame_RotationDirection[frame] : (encountered_RotationDirection ? shared_RotationDirection : "CW");
		(*iCTAcquisitionDetailsSequence)+=new CodeStringAttribute(TagFromName(RotationDirection),useRotationDirection);
		double useRevolutionTime = perFrame_RevolutionTime ? perFrame_RevolutionTime[frame] : (encountered_RevolutionTime ? shared_RevolutionTime : double(0.0));
		(*iCTAcquisitionDetailsSequence)+=new FloatDoubleAttribute(TagFromName(RevolutionTime),useRevolutionTime);
	}
	double useSingleCollimationWidth = perFrame_SingleCollimationWidth ? perFrame_SingleCollimationWidth[frame] : (encountered_SingleCollimationWidth ? shared_SingleCollimationWidth : double(0.0));
	(*iCTAcquisitionDetailsSequence)+=new FloatDoubleAttribute(TagFromName(SingleCollimationWidth),useSingleCollimationWidth);
	double useTotalCollimationWidth = perFrame_TotalCollimationWidth ? perFrame_TotalCollimationWidth[frame] : (encountered_TotalCollimationWidth ? shared_TotalCollimationWidth : double(0.0));
	(*iCTAcquisitionDetailsSequence)+=new FloatDoubleAttribute(TagFromName(TotalCollimationWidth),useTotalCollimationWidth);
	double useTableHeight = perFrame_TableHeight ? perFrame_TableHeight[frame] : (encountered_TableHeight ? shared_TableHeight : double(0.0));
	(*iCTAcquisitionDetailsSequence)+=new DecimalStringAttribute(TagFromName(TableHeight),useTableHeight);
	double useGantryDetectorTilt = perFrame_GantryDetectorTilt ? perFrame_GantryDetectorTilt[frame] : (encountered_GantryDetectorTilt ? shared_GantryDetectorTilt : double(0.0));
	(*iCTAcquisitionDetailsSequence)+=new DecimalStringAttribute(TagFromName(GantryDetectorTilt),useGantryDetectorTilt);
	double useDataCollectionDiameter = perFrame_DataCollectionDiameter ? perFrame_DataCollectionDiameter[frame] : (encountered_DataCollectionDiameter ? shared_DataCollectionDiameter : double(0.0));
	(*iCTAcquisitionDetailsSequence)+=new DecimalStringAttribute(TagFromName(DataCollectionDiameter),useDataCollectionDiameter);
}

static void makeCTTableDynamicsSequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,
		bool encountered_ImageType,char **shared_ImageType,char ***perFrame_ImageType,int nValues_ImageType,
		bool encountered_AcquisitionType,char *shared_AcquisitionType,char **perFrame_AcquisitionType,
		bool encountered_TableSpeed,double shared_TableSpeed,double *perFrame_TableSpeed,
		bool encountered_TableFeedPerRotation,double shared_TableFeedPerRotation,double *perFrame_TableFeedPerRotation,
		bool encountered_SpiralPitchFactor,double shared_SpiralPitchFactor,double *perFrame_SpiralPitchFactor) {
//cerr << "makeCTTableDynamicsSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iCTTableDynamicsSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(CTTableDynamicsSequence));
	if (treatAsNotOriginalFrame) return;

	const char *useAcquisitionType = getAcquisitionTypeToUse(frame,encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
			encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType);

	if (strcmp(useAcquisitionType,"SPIRAL") == 0 || strcmp(useAcquisitionType,"CONSTANT_ANGLE") == 0) {
		double useTableSpeed = perFrame_TableSpeed ? perFrame_TableSpeed[frame] : (encountered_TableSpeed ? shared_TableSpeed : double(0.0));
		(*iCTTableDynamicsSequence)+=new FloatDoubleAttribute(TagFromName(TableSpeed),useTableSpeed);
	}
	if (strcmp(useAcquisitionType,"SPIRAL") == 0) {
		double useTableFeedPerRotation = perFrame_TableFeedPerRotation ? perFrame_TableFeedPerRotation[frame] : (encountered_TableFeedPerRotation ? shared_TableFeedPerRotation : double(0.0));
		(*iCTTableDynamicsSequence)+=new FloatDoubleAttribute(TagFromName(TableFeedPerRotation),useTableFeedPerRotation);
		double useSpiralPitchFactor = perFrame_SpiralPitchFactor ? perFrame_SpiralPitchFactor[frame] : (encountered_SpiralPitchFactor ? shared_SpiralPitchFactor : double(0.0));
		(*iCTTableDynamicsSequence)+=new FloatDoubleAttribute(TagFromName(SpiralPitchFactor),useSpiralPitchFactor);
	}
}

static void makeCTPositionSequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,
		Uint16 rows,Uint16 columns,
		double *reference_ImagePositionPatient,	// null if none or shared
		bool encountered_PatientPosition,char *shared_PatientPosition,char **perFrame_PatientPosition,
		bool encountered_PixelSpacing,double *shared_PixelSpacing,double **perFrame_PixelSpacing,
		bool encountered_SliceLocation,double shared_SliceLocation,double *perFrame_SliceLocation,
		bool encountered_TablePosition,double shared_TablePosition,double *perFrame_TablePosition,
		bool encountered_ImagePositionPatient,double *shared_ImagePositionPatient,double **perFrame_ImagePositionPatient,
		bool encountered_ImageOrientationPatient,double *shared_ImageOrientationPatient,double **perFrame_ImageOrientationPatient,
		bool encountered_DataCollectionCenterPatient,double *shared_DataCollectionCenterPatient,double **perFrame_DataCollectionCenterPatient,
		bool encountered_ReconstructionTargetCenterPatient,double *shared_ReconstructionTargetCenterPatient,double **perFrame_ReconstructionTargetCenterPatient) {
//cerr << "makeCTPositionSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iCTPositionSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(CTPositionSequence));
	if (treatAsNotOriginalFrame) return;
	
	double *usePixelSpacing = perFrame_PixelSpacing ? perFrame_PixelSpacing[frame] : (encountered_PixelSpacing ? shared_PixelSpacing : NULL);
	double *useImagePositionPatient = perFrame_ImagePositionPatient ? perFrame_ImagePositionPatient[frame] : (encountered_ImagePositionPatient ? shared_ImagePositionPatient : NULL);
	double *useImageOrientationPatient = perFrame_ImageOrientationPatient ? perFrame_ImageOrientationPatient[frame] : (encountered_ImageOrientationPatient ? shared_ImageOrientationPatient : NULL);

	double distanceAlongNormal=0.0;
	if (reference_ImagePositionPatient && useImagePositionPatient && useImageOrientationPatient) {
		distanceAlongNormal = getDistanceAlongNormalToImagePlane(
				useImagePositionPatient[0],useImagePositionPatient[1],useImagePositionPatient[2],
				reference_ImagePositionPatient[0],reference_ImagePositionPatient[1],reference_ImagePositionPatient[2],
				useImageOrientationPatient[0],useImageOrientationPatient[1],useImageOrientationPatient[2],
				useImageOrientationPatient[3],useImageOrientationPatient[4],useImageOrientationPatient[5]);
//cerr << "makeCTPositionSequence: distanceAlongNormal=" << distanceAlongNormal << endl;
		// In the CT Position Macro, Table Position is defined to be gantry relative,
		// "Positions as the table moves into the gantry viewed from the front are more negative",
		// so should correct based on patient vs. gantry position (feet first or head first)
		char *usePatientPosition = perFrame_PatientPosition ? perFrame_PatientPosition[frame] : (encountered_PatientPosition ? shared_PatientPosition : NULL);
		if (usePatientPosition && strlen(usePatientPosition) >= 2 && strncmp(usePatientPosition,"FF",2) == 0) {		// first two are always either FF or HF
			distanceAlongNormal=-distanceAlongNormal;	// change sign if feet first
//cerr << "makeCTPositionSequence: changed since of distanceAlongNormal since feet first position" << endl;
		}
	}
	
	double useSliceLocation = perFrame_SliceLocation ? perFrame_SliceLocation[frame] : (encountered_SliceLocation ? shared_SliceLocation : distanceAlongNormal);
//cerr << "makeCTPositionSequence: useSliceLocation=" << useSliceLocation << endl;
	double useTablePosition = perFrame_TablePosition ? perFrame_TablePosition[frame] : (encountered_TablePosition ? shared_TablePosition : useSliceLocation);
//cerr << "makeCTPositionSequence: useTablePosition=" << useTablePosition << endl;

	(*iCTPositionSequence)+=new FloatDoubleAttribute(TagFromName(TablePosition),useTablePosition);
	
	double *imageCenter = NULL;
	if (useImagePositionPatient && useImageOrientationPatient && usePixelSpacing) {
		imageCenter = new double[3];
		double centerColumn =  double(columns-1)/2*usePixelSpacing[1];
		double centerRow =  double(rows-1)/2*usePixelSpacing[0];
		imageCenter[0] = useImagePositionPatient[0] + useImageOrientationPatient[0]*centerColumn + useImageOrientationPatient[3]*centerRow;
		imageCenter[1] = useImagePositionPatient[1] + useImageOrientationPatient[1]*centerColumn + useImageOrientationPatient[4]*centerRow;
		imageCenter[2] = useImagePositionPatient[2] + useImageOrientationPatient[2]*centerColumn + useImageOrientationPatient[5]*centerRow;
//cerr << "makeCTPositionSequence: usePixelSpacing[0]=" << usePixelSpacing[0] << endl;
//cerr << "makeCTPositionSequence: usePixelSpacing[1]=" << usePixelSpacing[1] << endl;
//cerr << "makeCTPositionSequence: useImagePositionPatient[0]=" << useImagePositionPatient[0] << endl;
//cerr << "makeCTPositionSequence: useImagePositionPatient[1]=" << useImagePositionPatient[1] << endl;
//cerr << "makeCTPositionSequence: useImagePositionPatient[2]=" << useImagePositionPatient[2] << endl;
//cerr << "makeCTPositionSequence: useImageOrientationPatient[0]=" << useImageOrientationPatient[0] << endl;
//cerr << "makeCTPositionSequence: useImageOrientationPatient[1]=" << useImageOrientationPatient[1] << endl;
//cerr << "makeCTPositionSequence: useImageOrientationPatient[2]=" << useImageOrientationPatient[2] << endl;
//cerr << "makeCTPositionSequence: useImageOrientationPatient[3]=" << useImageOrientationPatient[3] << endl;
//cerr << "makeCTPositionSequence: useImageOrientationPatient[4]=" << useImageOrientationPatient[4] << endl;
//cerr << "makeCTPositionSequence: useImageOrientationPatient[5]=" << useImageOrientationPatient[5] << endl;
//cerr << "makeCTPositionSequence: imageCenter[0]=" << imageCenter[0] << endl;
//cerr << "makeCTPositionSequence: imageCenter[1]=" << imageCenter[1] << endl;
//cerr << "makeCTPositionSequence: imageCenter[2]=" << imageCenter[2] << endl;
	}
	
	double *useReconstructionTargetCenterPatient = perFrame_ReconstructionTargetCenterPatient
		? perFrame_ReconstructionTargetCenterPatient[frame]
		: (encountered_ReconstructionTargetCenterPatient ? shared_ReconstructionTargetCenterPatient : imageCenter);
	if (useReconstructionTargetCenterPatient) {
		FloatDoubleAttribute *aReconstructionTargetCenterPatient =  new FloatDoubleAttribute(TagFromName(ReconstructionTargetCenterPatient));
		Assert(aReconstructionTargetCenterPatient);
		(*iCTPositionSequence)+=aReconstructionTargetCenterPatient;
		aReconstructionTargetCenterPatient->addValue(useReconstructionTargetCenterPatient[0]);
		aReconstructionTargetCenterPatient->addValue(useReconstructionTargetCenterPatient[1]);
		aReconstructionTargetCenterPatient->addValue(useReconstructionTargetCenterPatient[2]);
//cerr << "makeCTPositionSequence: useReconstructionTargetCenterPatient[0]=" << useReconstructionTargetCenterPatient[0] << endl;
//cerr << "makeCTPositionSequence: useReconstructionTargetCenterPatient[1]=" << useReconstructionTargetCenterPatient[1] << endl;
//cerr << "makeCTPositionSequence: useReconstructionTargetCenterPatient[2]=" << useReconstructionTargetCenterPatient[2] << endl;
	}

	double *useDataCollectionCenterPatient = perFrame_DataCollectionCenterPatient
		? perFrame_DataCollectionCenterPatient[frame]
		: (encountered_DataCollectionCenterPatient ? shared_DataCollectionCenterPatient : useReconstructionTargetCenterPatient);
	if (useDataCollectionCenterPatient) {
		FloatDoubleAttribute *aDataCollectionCenterPatient =  new FloatDoubleAttribute(TagFromName(DataCollectionCenterPatient));
		Assert(aDataCollectionCenterPatient);
		(*iCTPositionSequence)+=aDataCollectionCenterPatient;
		aDataCollectionCenterPatient->addValue(useDataCollectionCenterPatient[0]);
		aDataCollectionCenterPatient->addValue(useDataCollectionCenterPatient[1]);
		aDataCollectionCenterPatient->addValue(useDataCollectionCenterPatient[2]);
//cerr << "makeCTPositionSequence: useDataCollectionCenterPatient[0]=" << useDataCollectionCenterPatient[0] << endl;
//cerr << "makeCTPositionSequence: useDataCollectionCenterPatient[1]=" << useDataCollectionCenterPatient[1] << endl;
//cerr << "makeCTPositionSequence: useDataCollectionCenterPatient[2]=" << useDataCollectionCenterPatient[2] << endl;
	}
}		
		
static void makeCTGeometrySequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,
		bool encountered_DistanceSourceToDetector,double shared_DistanceSourceToDetector,double *perFrame_DistanceSourceToDetector,
		bool encountered_DistanceSourceToDataCollectionCenter,double shared_DistanceSourceToDataCollectionCenter,double *perFrame_DistanceSourceToDataCollectionCenter,
		bool encountered_DistanceSourceToPatient,double shared_DistanceSourceToPatient,double *perFrame_DistanceSourceToPatient) {
//cerr << "makeCTGeometrySequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iCTGeometrySequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(CTGeometrySequence));
	if (treatAsNotOriginalFrame) return;
	
	double useDistanceSourceToDetector = perFrame_DistanceSourceToDetector ? perFrame_DistanceSourceToDetector[frame] : (encountered_DistanceSourceToDetector ? shared_DistanceSourceToDetector : double(0.0));
	(*iCTGeometrySequence)+=new DecimalStringAttribute(TagFromName(DistanceSourceToDetector),useDistanceSourceToDetector);
	double useDistanceSourceToPatient = perFrame_DistanceSourceToPatient ? perFrame_DistanceSourceToPatient[frame] : (encountered_DistanceSourceToPatient ? shared_DistanceSourceToPatient : (useDistanceSourceToDetector/2));
	double useDistanceSourceToDataCollectionCenter = perFrame_DistanceSourceToDataCollectionCenter ? perFrame_DistanceSourceToDataCollectionCenter[frame] : (encountered_DistanceSourceToDataCollectionCenter ? shared_DistanceSourceToDataCollectionCenter : useDistanceSourceToPatient);
	(*iCTGeometrySequence)+=new FloatDoubleAttribute(TagFromName(DistanceSourceToDataCollectionCenter),useDistanceSourceToDataCollectionCenter);
}

static const char *extractConvolutionKernelGroupFromConvolutionKernel(const char *o) {
	const char *n;
	if      (strcmp(o,"STANDARD") == 0) n="SOFT_TISSUE";	// GE
	else if (strcmp(o,"STND") == 0)     n="SOFT_TISSUE";	// GE
	else if (strcmp(o,"STD+") == 0)     n="SOFT_TISSUE";	// GE
	else if (strcmp(o,"SOFT") == 0)     n="SOFT_TISSUE";	// GE
	else if (strcmp(o,"DETAIL") == 0)   n="SOFT_TISSUE";	// GE
	else if (strcmp(o,"BONE") == 0)     n="BONE";		// GE
	else if (strcmp(o,"EDGE") == 0)     n="BONE";		// GE
	else if (strcmp(o,"LUNG") == 0)     n="LUNG";		// GE
	else if (strlen(o) == 4 && isupper(o[0]) && isdigit(o[1]) && isdigit(o[2]) && islower(o[3])) {	// Siemens e.g., "B35s"
		if      (o[0] == 'B') n="BONE";
		else if (o[0] == 'T') n="CONSTANT_ANGLE";
		else if (o[0] == 'S') n="SOFT_TISSUE";
		else if (o[0] == 'H') n="BRAIN";
	}
	else if (strcmp(o,"CONSTANT_ANGLE") == 0)  n="CONSTANT_ANGLE";		// Ours
	else if (strcmp(o,"LOCALIZER") == 0)       n="CONSTANT_ANGLE";		// Ours
	else n=o;
	return n;
}

static void makeCTReconstructionSequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,
		Uint16 rows,Uint16 columns,
		bool encountered_ImageType,char **shared_ImageType,char ***perFrame_ImageType,int nValues_ImageType,
		bool encountered_AcquisitionType,char *shared_AcquisitionType,char **perFrame_AcquisitionType,
		bool encountered_PixelSpacing,double *shared_PixelSpacing,double **perFrame_PixelSpacing,
		bool encountered_ReconstructionAlgorithm,char *shared_ReconstructionAlgorithm,char **perFrame_ReconstructionAlgorithm,
		bool encountered_ConvolutionKernel,char *shared_ConvolutionKernel,char **perFrame_ConvolutionKernel,
		bool encountered_ConvolutionKernelGroup,char *shared_ConvolutionKernelGroup,char **perFrame_ConvolutionKernelGroup,
		bool encountered_ReconstructionDiameter,double shared_ReconstructionDiameter,double *perFrame_ReconstructionDiameter,
		bool encountered_ReconstructionFieldOfView,double *shared_ReconstructionFieldOfView,double **perFrame_ReconstructionFieldOfView,
		bool encountered_ReconstructionPixelSpacing,double *shared_ReconstructionPixelSpacing,double **perFrame_ReconstructionPixelSpacing,
		bool encountered_ReconstructionAngle,double shared_ReconstructionAngle,double *perFrame_ReconstructionAngle,
		bool encountered_ImageFilter,char *shared_ImageFilter,char **perFrame_ImageFilter) {
//cerr << "makeCTReconstructionSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iCTReconstructionSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(CTReconstructionSequence));
	if (treatAsNotOriginalFrame) return;
	
	const char *useAcquisitionType = getAcquisitionTypeToUse(frame,encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
			encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType);

	const char *useReconstructionAlgorithm = perFrame_ReconstructionAlgorithm ? perFrame_ReconstructionAlgorithm[frame] : (encountered_ReconstructionAlgorithm ? shared_ReconstructionAlgorithm : "ITERATIVE");
	(*iCTReconstructionSequence)+=new CodeStringAttribute(TagFromName(ReconstructionAlgorithm),useReconstructionAlgorithm);

	const char *defaultConvolutionKernel = strcmp(useAcquisitionType,"CONSTANT_ANGLE") == 0 ? "CONSTANT_ANGLE" : "";
	const char *useConvolutionKernel = perFrame_ConvolutionKernel ? perFrame_ConvolutionKernel[frame] : (encountered_ConvolutionKernel ? shared_ConvolutionKernel : defaultConvolutionKernel);
	(*iCTReconstructionSequence)+=new ShortStringAttribute(TagFromName(ConvolutionKernel),useConvolutionKernel);
	const char *useConvolutionKernelGroup = perFrame_ConvolutionKernelGroup ? perFrame_ConvolutionKernelGroup[frame] : (encountered_ConvolutionKernelGroup ? shared_ConvolutionKernelGroup : 
		extractConvolutionKernelGroupFromConvolutionKernel(useConvolutionKernel));
	(*iCTReconstructionSequence)+=new CodeStringAttribute(TagFromName(ConvolutionKernelGroup),useConvolutionKernelGroup);

	double defaultReconstructionAngle = strcmp(useAcquisitionType,"CONSTANT_ANGLE") == 0 ? 0.0 : 360.0;
	double useReconstructionAngle = perFrame_ReconstructionAngle ? perFrame_ReconstructionAngle[frame] : (encountered_ReconstructionAngle ? shared_ReconstructionAngle : defaultReconstructionAngle);
	(*iCTReconstructionSequence)+=new FloatDoubleAttribute(TagFromName(ReconstructionAngle),useReconstructionAngle);
	
	const char *useImageFilter = perFrame_ImageFilter ? perFrame_ImageFilter[frame] : (encountered_ImageFilter ? shared_ImageFilter : "None");
	(*iCTReconstructionSequence)+=new ShortStringAttribute(TagFromName(ImageFilter),useImageFilter);

	double useReconstructionDiameter = 0;
	double useReconstructionFieldOfViewXDimension = 0;
	double useReconstructionFieldOfViewYDimension = 0;
	double useReconstructionPixelSpacingXValue = 0;
	double useReconstructionPixelSpacingYValue = 0;
	
	if (encountered_ReconstructionDiameter) {
		useReconstructionDiameter = perFrame_ReconstructionDiameter ? perFrame_ReconstructionDiameter[frame] : shared_ReconstructionDiameter;
	}
	if (encountered_ReconstructionFieldOfView) {
		double *useReconstructionFieldOfView = perFrame_ReconstructionFieldOfView ? perFrame_ReconstructionFieldOfView[frame] : shared_ReconstructionFieldOfView;
		useReconstructionFieldOfViewXDimension = useReconstructionFieldOfView[0];
		useReconstructionFieldOfViewYDimension = useReconstructionFieldOfView[1];
	}
	if (encountered_ReconstructionPixelSpacing) {
		double *useReconstructionPixelSpacing = perFrame_ReconstructionPixelSpacing ? perFrame_ReconstructionPixelSpacing[frame] : shared_ReconstructionPixelSpacing;
		useReconstructionPixelSpacingYValue = useReconstructionPixelSpacing[0];
		useReconstructionPixelSpacingXValue = useReconstructionPixelSpacing[1];
	}
	if (encountered_PixelSpacing) {
		double *usePixelSpacing = perFrame_PixelSpacing ? perFrame_PixelSpacing[frame] : shared_PixelSpacing;
		if (useReconstructionPixelSpacingXValue <= 0 && useReconstructionPixelSpacingYValue <= 0) {
			useReconstructionPixelSpacingYValue = usePixelSpacing[0];
			useReconstructionPixelSpacingXValue = usePixelSpacing[1];
		}
		if (useReconstructionFieldOfViewXDimension <= 0 && useReconstructionFieldOfViewYDimension <= 0 && useReconstructionDiameter <= 0) {
			double columnHeightInMM = rows * usePixelSpacing[0];
			double rowWidthInMM = columns * usePixelSpacing[1];
			if (columnHeightInMM == rowWidthInMM) {
				useReconstructionDiameter=columnHeightInMM;
			}
			else {
				useReconstructionFieldOfViewXDimension = rowWidthInMM;
				useReconstructionFieldOfViewYDimension = columnHeightInMM;
			}
		}
	}

	if (useReconstructionFieldOfViewXDimension > 0 && useReconstructionFieldOfViewYDimension > 0) {
		FloatDoubleAttribute *aReconstructionFieldOfView =  new FloatDoubleAttribute(TagFromName(ReconstructionFieldOfView));
		Assert(aReconstructionFieldOfView);
		(*iCTReconstructionSequence)+=aReconstructionFieldOfView;
		aReconstructionFieldOfView->addValue(useReconstructionFieldOfViewXDimension);
		aReconstructionFieldOfView->addValue(useReconstructionFieldOfViewYDimension);
	}
	else {
		(*iCTReconstructionSequence)+=new DecimalStringAttribute(TagFromName(ReconstructionDiameter),useReconstructionDiameter);
	}
	{
		FloatDoubleAttribute *aReconstructionPixelSpacing =  new FloatDoubleAttribute(TagFromName(ReconstructionPixelSpacing));
		Assert(aReconstructionPixelSpacing);
		(*iCTReconstructionSequence)+=aReconstructionPixelSpacing;
		aReconstructionPixelSpacing->addValue(useReconstructionPixelSpacingXValue);
		aReconstructionPixelSpacing->addValue(useReconstructionPixelSpacingYValue);
	}
}

static void makeCTExposureSequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,
		bool encountered_ExposureTime,Uint32 shared_ExposureTime,Uint32 *perFrame_ExposureTime,
		bool encountered_XRayTubeCurrent,Uint32 shared_XRayTubeCurrent,Uint32 *perFrame_XRayTubeCurrent,
		bool encountered_Exposure,Uint32 shared_Exposure,Uint32 *perFrame_Exposure,
		bool encountered_ExposureInuAs,Uint32 shared_ExposureInuAs,Uint32 *perFrame_ExposureInuAs,
		bool encountered_ExposureTimeInms,double shared_ExposureTimeInms,double *perFrame_ExposureTimeInms,
		bool encountered_XRayTubeCurrentInmA,double shared_XRayTubeCurrentInmA,double *perFrame_XRayTubeCurrentInmA,
		bool encountered_ExposureInmAs,double shared_ExposureInmAs,double *perFrame_ExposureInmAs,
		bool encountered_ExposureModulationType,char *shared_ExposureModulationType,char **perFrame_ExposureModulationType,
		bool encountered_EstimatedDoseSaving,double shared_EstimatedDoseSaving,double *perFrame_EstimatedDoseSaving,
		bool encountered_CTDIvol,double shared_CTDIvol,double *perFrame_CTDIvol) {
//cerr << "makeCTExposureSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iCTExposureSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(CTExposureSequence));
	if (treatAsNotOriginalFrame) return;

	double useExposureTime = (double)(perFrame_ExposureTime ? perFrame_ExposureTime[frame] : (encountered_ExposureTime ? shared_ExposureTime : Uint32(0)));
	double useXRayTubeCurrent = (double)(perFrame_XRayTubeCurrent ? perFrame_XRayTubeCurrent[frame] : (encountered_XRayTubeCurrent ? shared_XRayTubeCurrent : Uint32(0)));
	Uint32 useExposure = perFrame_Exposure ? perFrame_Exposure[frame] : (encountered_Exposure ? shared_Exposure : Uint32(0));
	double useExposureInuAs = (double)(perFrame_ExposureInuAs ? perFrame_ExposureInuAs[frame] : (encountered_ExposureInuAs ? shared_ExposureInuAs : (useExposure*1000)));

	double useExposureTimeInms = perFrame_ExposureTimeInms ? perFrame_ExposureTimeInms[frame] : (encountered_ExposureTimeInms ? shared_ExposureTimeInms : useExposureTime);
	double useXRayTubeCurrentInmA = perFrame_XRayTubeCurrentInmA ? perFrame_XRayTubeCurrentInmA[frame] : (encountered_XRayTubeCurrentInmA ? shared_XRayTubeCurrentInmA : useXRayTubeCurrent);
	double useExposureInmAs = perFrame_ExposureInmAs ? perFrame_ExposureInmAs[frame] : (encountered_ExposureInmAs ? shared_ExposureInmAs : (useExposureInuAs/1000.0));
	
	(*iCTExposureSequence)+=new FloatDoubleAttribute(TagFromName(ExposureTimeInms),useExposureTimeInms);
	(*iCTExposureSequence)+=new FloatDoubleAttribute(TagFromName(XRayTubeCurrentInmA),useXRayTubeCurrentInmA);
	(*iCTExposureSequence)+=new FloatDoubleAttribute(TagFromName(ExposureInmAs),useExposureInmAs);

	const char *useExposureModulationType = perFrame_ExposureModulationType ? perFrame_ExposureModulationType[frame] : (encountered_ExposureModulationType ? shared_ExposureModulationType : "NONE");
	(*iCTExposureSequence)+=new CodeStringAttribute(TagFromName(ExposureModulationType),useExposureModulationType);
	
	if (strcmp(useExposureModulationType,"NONE") != 0) {	// is conditional and may not be present otherwise
		if (encountered_EstimatedDoseSaving) {
			double useEstimatedDoseSaving = perFrame_EstimatedDoseSaving ? perFrame_EstimatedDoseSaving[frame] : shared_EstimatedDoseSaving;
			(*iCTExposureSequence)+=new FloatDoubleAttribute(TagFromName(EstimatedDoseSaving),useEstimatedDoseSaving);
		}
		else {
			(*iCTExposureSequence)+=new FloatDoubleAttribute(TagFromName(EstimatedDoseSaving));	// is Type 2
		}
	}
	if (encountered_CTDIvol) {
		double useCTDIvol = perFrame_CTDIvol ? perFrame_CTDIvol[frame] : shared_CTDIvol;
		(*iCTExposureSequence)+=new FloatDoubleAttribute(TagFromName(CTDIvol),useCTDIvol);
	}
	else {
		(*iCTExposureSequence)+=new FloatDoubleAttribute(TagFromName(CTDIvol));				// is Type 2
	}
}

static void makeCTXRayDetailsSequence(AttributeList *functionalGroupsSequence,int frame,bool treatAsNotOriginalFrame,
		bool encountered_KVP,double shared_KVP,double *perFrame_KVP,
		bool encountered_FocalSpots,double shared_FocalSpots,double *perFrame_FocalSpots,
		bool encountered_FilterType,char *shared_FilterType,char **perFrame_FilterType,
		bool encountered_FilterMaterial,char *shared_FilterMaterial,char **perFrame_FilterMaterial) {
//cerr << "makeCTXRayDetailsSequence" << endl;
	Assert(functionalGroupsSequence);
	AttributeList *iCTXRayDetailsSequence = makeNewSequenceAttributeWithItem(functionalGroupsSequence,TagFromName(CTXRayDetailsSequence));
	if (treatAsNotOriginalFrame) return;
	
	double useKVP = perFrame_KVP ? perFrame_KVP[frame] : (encountered_KVP ? shared_KVP : double(0.0));
	(*iCTXRayDetailsSequence)+=new DecimalStringAttribute(TagFromName(KVP),useKVP);
	double useFocalSpots = perFrame_FocalSpots ? perFrame_FocalSpots[frame] : (encountered_FocalSpots ? shared_FocalSpots : double(0.0));
	(*iCTXRayDetailsSequence)+=new DecimalStringAttribute(TagFromName(FocalSpots),useFocalSpots);
	const char *useFilterType = perFrame_FilterType ? perFrame_FilterType[frame] : (encountered_FilterType ? shared_FilterType : "NONE");
	(*iCTXRayDetailsSequence)+=new ShortStringAttribute(TagFromName(FilterType),useFilterType);
	const char *useFilterMaterial = perFrame_FilterMaterial ? perFrame_FilterMaterial[frame] : (encountered_FilterMaterial ? shared_FilterMaterial : "NONE");
	(*iCTXRayDetailsSequence)+=new CodeStringAttribute(TagFromName(FilterMaterial),useFilterMaterial);
}

static void makeContrastBolusUsageSequence(AttributeList *functionalGroupsSequence,int frame,
	bool encountered_ContrastBolusAgent,char **shared_ContrastBolusAgent,char ***perFrame_ContrastBolusAgent,int nValues_ContrastBolusAgent,
	bool encountered_ContrastBolusRoute,char **shared_ContrastBolusRoute,char ***perFrame_ContrastBolusRoute,int nValues_ContrastBolusRoute,
	bool encountered_ContrastBolusVolume,char **shared_ContrastBolusVolume,char ***perFrame_ContrastBolusVolume,int nValues_ContrastBolusVolume,
	bool encountered_ContrastBolusIngredientConcentration,char **shared_ContrastBolusIngredientConcentration,
		char ***perFrame_ContrastBolusIngredientConcentration,int nValues_ContrastBolusIngredientConcentration,
	bool encountered_ContrastBolusAgentAdministered,char **shared_ContrastBolusAgentAdministered,
		char ***perFrame_ContrastBolusAgentAdministered,int nValues_ContrastBolusAgentAdministered,
	bool encountered_ContrastBolusAgentDetected,char **shared_ContrastBolusAgentDetected,char ***perFrame_ContrastBolusAgentDetected,int nValues_ContrastBolusAgentDetected,
	bool encountered_ContrastBolusAgentPhase,char **shared_ContrastBolusAgentPhase,char ***perFrame_ContrastBolusAgentPhase,int nValues_ContrastBolusAgentPhase) {
//cerr << "makeContrastBolusUsageSequence" << endl;
	if (nValues_ContrastBolusAgent) {
		Assert(functionalGroupsSequence);
		SequenceAttribute *aContrastBolusUsageSequence=makeNewSequenceAttributeWithoutItem(functionalGroupsSequence,TagFromName(ContrastBolusUsageSequence));
		Assert(aContrastBolusUsageSequence);
		
		for (int agent=0; agent<nValues_ContrastBolusAgent; ++agent) {
			AttributeList *itemlist = new AttributeList();
			Assert(itemlist);
			(*aContrastBolusUsageSequence)+=itemlist;
			
			(*itemlist)+=new UnsignedShortAttribute(TagFromName(ContrastBolusAgentNumber),agent+1);		// DICOM numbers from 1 not 0
			
			const char *useContrastBolusRoute = encountered_ContrastBolusRoute && nValues_ContrastBolusRoute > agent
				? (perFrame_ContrastBolusRoute && perFrame_ContrastBolusRoute[frame]
					? perFrame_ContrastBolusRoute[frame][agent]
					: (shared_ContrastBolusRoute
						? shared_ContrastBolusRoute[agent]
						: "IV"))
				: "IV";
//cerr << "makeContrastBolusUsageSequence: agent=" << agent << " useContrastBolusRoute " << useContrastBolusRoute << endl;

			const char *useContrastBolusAgentAdministered = encountered_ContrastBolusAgentAdministered && nValues_ContrastBolusAgentAdministered > agent
				? (perFrame_ContrastBolusAgentAdministered && perFrame_ContrastBolusAgentAdministered[frame]
					? perFrame_ContrastBolusAgentAdministered[frame][agent]
					: (shared_ContrastBolusAgentAdministered
						? shared_ContrastBolusAgentAdministered[agent]
						: "YES"))
				: "YES";
//cerr << "makeContrastBolusUsageSequence: agent=" << agent << " useContrastBolusAgentAdministered " << useContrastBolusAgentAdministered << endl;
			(*itemlist)+=new CodeStringAttribute(TagFromName(ContrastBolusAgentAdministered),useContrastBolusAgentAdministered);

			const char *useContrastBolusAgentDetected = encountered_ContrastBolusAgentDetected && nValues_ContrastBolusAgentDetected > agent
				? (perFrame_ContrastBolusAgentDetected && perFrame_ContrastBolusAgentDetected[frame]
					? perFrame_ContrastBolusAgentDetected[frame][agent]
					: (shared_ContrastBolusAgentDetected
						? shared_ContrastBolusAgentDetected[agent]
						: ""))
				: "";
//cerr << "makeContrastBolusUsageSequence: agent=" << agent << " useContrastBolusAgentDetected " << useContrastBolusAgentDetected << endl;
			(*itemlist)+=new CodeStringAttribute(TagFromName(ContrastBolusAgentDetected),useContrastBolusAgentDetected);

			const char *useContrastBolusAgentPhase = encountered_ContrastBolusAgentPhase && nValues_ContrastBolusAgentPhase > agent
				? (perFrame_ContrastBolusAgentPhase && perFrame_ContrastBolusAgentPhase[frame]
					? perFrame_ContrastBolusAgentPhase[frame][agent]
					: (shared_ContrastBolusAgentPhase
						? shared_ContrastBolusAgentPhase[agent]
						: ""))
				: "";
//cerr << "makeContrastBolusUsageSequence: agent=" << agent << " useContrastBolusAgentPhase " << useContrastBolusAgentPhase << endl;

			if (strcmp(useContrastBolusRoute,"IV") == 0) {
				(*itemlist)+=new CodeStringAttribute(TagFromName(ContrastBolusAgentPhase),useContrastBolusAgentPhase);
			}
		}
	}
	// else do not add at all if no contrast
}

static const char *makeContrastBolusAgentCodeSequenceFromContrastBolusAgentAndReturnIngredient(AttributeList *itemlist,const char *contrastBolusAgent) {
	const char *csd = 0;
	const char *cv = 0;
	const char *cm = 0;
	const char *ingredient = 0;
	
	if (contrastBolusAgent) {
		if      (findInStringRegardlessOfCase(contrastBolusAgent,"AIR"))		{ ingredient="AIR";        csd="SNM3"; cv="A-80230"; cm="Air"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"O2"))			{ ingredient="OXYGEN";     csd="SNM3"; cv="C-10110"; cm="Oxygen"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"OXYGEN"))		{ ingredient="OXYGEN";     csd="SNM3"; cv="C-10110"; cm="Oxygen"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"WATER"))		{ ingredient="WATER";      csd="SNM3"; cv="C-10120"; cm="Water"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"CO2"))		{ ingredient="CO2";        csd="SNM3"; cv="C-10520"; cm="Carbon dioxide"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"CARBON")
		      && findInStringRegardlessOfCase(contrastBolusAgent,"DIOXIDE"))		{ ingredient="CO2";        csd="SNM3"; cv="C-10520"; cm="Carbon dioxide"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"BARIUM"))		{ ingredient="BARIUM";     csd="SNM3"; cv="C-12217"; cm="Barium Sulfate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Gadopentetate"))	{ ingredient="GADOLINIUM"; csd="SRT";  cv="C-B03AA"; cm="Dimeglumine gadopentetate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Magnevist"))		{ ingredient="GADOLINIUM"; csd="SRT";  cv="C-B03AA"; cm="Dimeglumine gadopentetate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Gadodiamide"))	{ ingredient="GADOLINIUM"; csd="SRT";  cv="C-B03C3"; cm="Gadodiamide"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Omniscan"))		{ ingredient="GADOLINIUM"; csd="SRT";  cv="C-B03C3"; cm="Gadodiamide"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"GAD"))		{ ingredient="GADOLINIUM"; csd="SNM3"; cv="C-17800"; cm="Gadolinium"; }
		
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Diatrizoate"))	{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Angiovist"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Cardiografin"))	{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Cystografin"))	{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Gastrografin"))	{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Gastrovist"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Hypaque"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Renografin"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Renovist"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Urovist"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0317"; cm="Diatrizoate"; }
		
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Iodipamide"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0318"; cm="Iodipamide"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Cholographin"))	{ ingredient="IODINE";     csd="SNM3"; cv="C-B0318"; cm="Iodipamide"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Sinografin"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0318"; cm="Iodipamide"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Iodamide"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0326"; cm="Iodamide meglumine"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Renovue"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0326"; cm="Iodamide meglumine"; }
		
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Iopanoic"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0328"; cm="Iopanoic acid"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Telepaque"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0328"; cm="Iopanoic acid"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Iophendylate"))	{ ingredient="IODINE";     csd="SNM3"; cv="C-B0331"; cm="Iophendylate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Pantopaque"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0331"; cm="Iophendylate"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Ipodate"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0335"; cm="Ipodate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Bilivist"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0335"; cm="Ipodate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Oragrafin"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0335"; cm="Ipodate"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Propyliodone"))	{ ingredient="IODINE";     csd="SNM3"; cv="C-B0337"; cm="Propyliodone"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Dionosil"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0337"; cm="Propyliodone"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Acetrizoate"))	{ ingredient="IODINE";     csd="SNM3"; cv="C-B0338"; cm="Sodium acetrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Salpix"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0338"; cm="Sodium acetrizoate"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Metrizamide"))	{ ingredient="IODINE";     csd="SNM3"; cv="C-B0348"; cm="Metrizamide"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Amipaque"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0348"; cm="Metrizamide"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Iohexol"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B0322"; cm="Iohexol"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Omnipaque"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B0322"; cm="Iohexol"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Iodixanol"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B03BC"; cm="Iodixanol"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Visipaque"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B03BC"; cm="Iodixanol"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Iothalamate"))	{ ingredient="IODINE";     csd="SRT";  cv="C-B038B"; cm="Iothalamate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Conray"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B038B"; cm="Iothalamate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Vascoray"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B038B"; cm="Iothalamate"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Ioxaglate"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B0339"; cm="Ioxaglate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Hexbrix"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B0339"; cm="Ioxaglate"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Metrizoate"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B03C9"; cm="Metrizoate"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Isopaque"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B03C9"; cm="Metrizoate"; }

		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Iopamidol"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B0329"; cm="Iopamidol"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Isovue"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B0329"; cm="Iopamidol"; }
		
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Optiray"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B0332"; cm="Ioversol"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"Ioversol"))		{ ingredient="IODINE";     csd="SRT";  cv="C-B0332"; cm="Ioversol"; }
		
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"non")
		      && findInStringRegardlessOfCase(contrastBolusAgent,"ionic"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0302"; cm="Non-ionic iodinated contrast agent"; }
		else if (findInStringRegardlessOfCase(contrastBolusAgent,"ionic"))		{ ingredient="IODINE";     csd="SNM3"; cv="C-B0301"; cm="Ionic iodinated contrast agent"; }

		else										{ ingredient="";           csd="SNM3"; cv="C-B0300"; cm="Contrast agent"; }
	}
	
	addCodeSequenceItemToItemList(itemlist,csd,cv,cm,NULL);
	return ingredient;
}

static SequenceAttribute *makeContrastBolusAdministrationRouteSequenceFromContrastBolusRoute(const char *contrastBolusRoute) {
	SequenceAttribute *aContrastBolusAdministrationRouteSequence=new SequenceAttribute(TagFromName(ContrastBolusAdministrationRouteSequence));
	Assert(aContrastBolusAdministrationRouteSequence);                                                
	
	const char *csd = 0;
	const char *cv = 0;
	const char *cm = 0;
	
	if (contrastBolusRoute) {
		if      (findInStringRegardlessOfCase(contrastBolusRoute,"IV"))			{ csd="SNM3"; cv="G-D101"; cm="Intravenous route"; }
		else if (findInStringRegardlessOfCase(contrastBolusRoute,"ORAL"))		{ csd="SNM3"; cv="G-D140"; cm="Oral route"; }
		else if (findInStringRegardlessOfCase(contrastBolusRoute,"VAGINAL"))		{ csd="SNM3"; cv="G-D164"; cm="Vaginal route"; }
		else if (findInStringRegardlessOfCase(contrastBolusRoute,"RECTAL"))		{ csd="SNM3"; cv="G-D160"; cm="Per rectum"; }
		else if (findInStringRegardlessOfCase(contrastBolusRoute,"INTRATHECAL"))	{ csd="SNM3"; cv="G-D108"; cm="Intrathecal route"; }
	}
	
	addCodeSequenceItemToAttribute(aContrastBolusAdministrationRouteSequence,csd,cv,cm,0);

	return aContrastBolusAdministrationRouteSequence;
}

static SequenceAttribute *makeContrastBolusIngredientCodeSequenceFromIngredient(const char *ingredient) {
	SequenceAttribute *aContrastBolusIngredientCodeSequence=new SequenceAttribute(TagFromName(ContrastBolusIngredientCodeSequence));
	Assert(aContrastBolusIngredientCodeSequence);                                                
	
	const char *csd = 0;
	const char *cv = 0;
	const char *cm = 0;
	
	if (ingredient) {
		if      (findInStringRegardlessOfCase(ingredient,"IODINE"))	{ csd="SRT"; cv="C-11400"; cm="Iodine"; }
		else if (findInStringRegardlessOfCase(ingredient,"GADOLINIUM"))	{ csd="SRT"; cv="C-17800"; cm="Gadolinium"; }
		else if (findInStringRegardlessOfCase(ingredient,"OXYGEN"))	{ csd="SRT"; cv="C-10110"; cm="Oxygen"; }
		else if (findInStringRegardlessOfCase(ingredient,"CO2"))	{ csd="SRT"; cv="C-10520"; cm="Carbon Dioxide"; }
		else if (findInStringRegardlessOfCase(ingredient,"BARIUM"))	{ csd="SRT"; cv="C-12200"; cm="Barium"; }
		else if (findInStringRegardlessOfCase(ingredient,"XENON"))	{ csd="SRT"; cv="C-17200"; cm="Xenon"; }
		else if (findInStringRegardlessOfCase(ingredient,"WATER"))	{ csd="SRT"; cv="C-10120"; cm="Water"; }
		else if (findInStringRegardlessOfCase(ingredient,"AIR"))	{ csd="SRT"; cv="A-80230"; cm="Air"; }
		else if (findInStringRegardlessOfCase(ingredient,"IRON"))	{ csd="SRT"; cv="C-130F9"; cm="Iron"; }
	}

	addCodeSequenceItemToAttribute(aContrastBolusIngredientCodeSequence,csd,cv,cm,0);

	return aContrastBolusIngredientCodeSequence;
}

static void makeEnhancedContrastBolusModule(AttributeList *list,
	bool encountered_ContrastBolusAgent,char **shared_ContrastBolusAgent,char ***perFrame_ContrastBolusAgent,int nValues_ContrastBolusAgent,
	bool encountered_ContrastBolusRoute,char **shared_ContrastBolusRoute,char ***perFrame_ContrastBolusRoute,int nValues_ContrastBolusRoute,
	bool encountered_ContrastBolusVolume,char **shared_ContrastBolusVolume,char ***perFrame_ContrastBolusVolume,int nValues_ContrastBolusVolume,
	bool encountered_ContrastBolusIngredientConcentration,char **shared_ContrastBolusIngredientConcentration,
		char ***perFrame_ContrastBolusIngredientConcentration,int nValues_ContrastBolusIngredientConcentration) {
//cerr << "makeEnhancedContrastBolusModule" << endl;
	if (nValues_ContrastBolusAgent) {
		Assert(list);
		SequenceAttribute *aContrastBolusAgentSequence=makeNewSequenceAttributeWithoutItem(list,TagFromName(ContrastBolusAgentSequence));
		Assert(aContrastBolusAgentSequence);
		for (int agent=0; agent<nValues_ContrastBolusAgent; ++agent) {
			AttributeList *itemlist = new AttributeList();
			Assert(itemlist);
			(*aContrastBolusAgentSequence)+=itemlist;

			const char *useContrastBolusAgent = encountered_ContrastBolusAgent && nValues_ContrastBolusAgent > agent
				? (perFrame_ContrastBolusAgent && perFrame_ContrastBolusAgent[0]
					? perFrame_ContrastBolusAgent[0][agent]
					: (shared_ContrastBolusAgent
						? shared_ContrastBolusAgent[agent]
						: ""))
				: "";
//cerr << "makeEnhancedContrastBolusModule: agent=" << agent << " useContrastBolusAgent " << useContrastBolusAgent << endl;
			const char *ingredient = makeContrastBolusAgentCodeSequenceFromContrastBolusAgentAndReturnIngredient(itemlist,useContrastBolusAgent);
			
			(*itemlist)+=new UnsignedShortAttribute(TagFromName(ContrastBolusAgentNumber),agent+1);		// DICOM numbers from 1 not 0

			const char *useContrastBolusRoute = encountered_ContrastBolusRoute && nValues_ContrastBolusRoute > agent
				? (perFrame_ContrastBolusRoute && perFrame_ContrastBolusRoute[0]
					? perFrame_ContrastBolusRoute[0][agent]
					: (shared_ContrastBolusRoute
						? shared_ContrastBolusRoute[agent]
						: "IV"))
				: "IV";
//cerr << "makeEnhancedContrastBolusModule: agent=" << agent << " useContrastBolusRoute " << useContrastBolusRoute << endl;
			(*itemlist)+=makeContrastBolusAdministrationRouteSequenceFromContrastBolusRoute(useContrastBolusRoute);

			(*itemlist)+=makeContrastBolusIngredientCodeSequenceFromIngredient(ingredient);
			
			const char *useContrastBolusVolume = encountered_ContrastBolusVolume && nValues_ContrastBolusVolume > agent
				? (perFrame_ContrastBolusVolume && perFrame_ContrastBolusVolume[0]
					? perFrame_ContrastBolusVolume[0][agent]
					: (shared_ContrastBolusVolume
						? shared_ContrastBolusVolume[agent]
						: ""))
				: "";
//cerr << "makeEnhancedContrastBolusModule: agent=" << agent << " useContrastBolusVolume " << useContrastBolusVolume << endl;
			(*itemlist)+=new DecimalStringAttribute(TagFromName(ContrastBolusVolume),useContrastBolusVolume);
			
			const char *useContrastBolusIngredientConcentration = encountered_ContrastBolusIngredientConcentration && nValues_ContrastBolusIngredientConcentration > agent
				? (perFrame_ContrastBolusIngredientConcentration && perFrame_ContrastBolusIngredientConcentration[0]
					? perFrame_ContrastBolusIngredientConcentration[0][agent]
					: (shared_ContrastBolusIngredientConcentration
						? shared_ContrastBolusIngredientConcentration[agent]
						: ""))
				: "";
//cerr << "makeEnhancedContrastBolusModule: agent=" << agent << " useContrastBolusIngredientConcentration " << useContrastBolusIngredientConcentration << endl;
			(*itemlist)+=new DecimalStringAttribute(TagFromName(ContrastBolusIngredientConcentration),useContrastBolusIngredientConcentration);
			//ContrastAdministrationProfileSequence
		}
	}
	// else do not add at all if no contrast
}

struct StackDescriptor {
	double *reference_ImagePositionPatient;
	double *reference_ImageOrientationPatient;
	double reference_HorizontalFOV;
	double reference_VerticalFOV;
	double reference_SliceThickness;
	double *distancesAlongNormal;
	int nDistancesAlongNormal;
};

static bool floatEquals(double a,double b) {
	return (fabs(a-b)< .001) ? true : false;
}

static void findFrameInStackDescriptorsOrInsert(int i,int numberofinputfiles,StackDescriptor *tableOfStackDescriptors,int &numberOfStackDescriptors,
		Uint32 *perFrame_Rows,Uint32 shared_Rows,
		Uint32 *perFrame_Columns,Uint32 shared_Columns,
		double **perFrame_ImagePositionPatient,double *shared_ImagePositionPatient,
		double **perFrame_ImageOrientationPatient,double *shared_ImageOrientationPatient,
		double *perFrame_SliceThickness,double shared_SliceThickness,
		double **perFrame_PixelSpacing,double *shared_PixelSpacing,
		int &useStack,int &useDistance) {

	Uint32 use_Rows = perFrame_Rows ? perFrame_Rows[i] : shared_Rows;
	Uint32 use_Columns = perFrame_Columns ? perFrame_Columns[i] : shared_Columns;
	double *use_ImagePositionPatient = perFrame_ImagePositionPatient ? perFrame_ImagePositionPatient[i] : shared_ImagePositionPatient;
	double *use_ImageOrientationPatient = perFrame_ImageOrientationPatient ? perFrame_ImageOrientationPatient[i] : shared_ImageOrientationPatient;
	double use_SliceThickness = perFrame_SliceThickness ? perFrame_SliceThickness[i] : shared_SliceThickness;
	double *use_PixelSpacing = perFrame_PixelSpacing ? perFrame_PixelSpacing[i] : shared_PixelSpacing;
			
	Assert(use_ImagePositionPatient);
	Assert(use_ImageOrientationPatient);
	Assert(use_PixelSpacing);
//cerr << "findFrameInStackDescriptorsOrInsert: use_ImagePositionPatient[0]=" << use_ImagePositionPatient[0] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_ImagePositionPatient[1]=" << use_ImagePositionPatient[1] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_ImagePositionPatient[2]=" << use_ImagePositionPatient[2] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_ImageOrientationPatient[0]=" << use_ImageOrientationPatient[0] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_ImageOrientationPatient[1]=" << use_ImageOrientationPatient[1] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_ImageOrientationPatient[2]=" << use_ImageOrientationPatient[2] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_ImageOrientationPatient[3]=" << use_ImageOrientationPatient[3] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_ImageOrientationPatient[4]=" << use_ImageOrientationPatient[4] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_ImageOrientationPatient[5]=" << use_ImageOrientationPatient[5] << endl;
	double use_VerticalFOV = use_Rows * use_PixelSpacing[0];
	double use_HorizontalFOV = use_Columns * use_PixelSpacing[1];
//cerr << "findFrameInStackDescriptorsOrInsert: use_VerticalFOV=" << use_VerticalFOV << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_HorizontalFOV=" << use_HorizontalFOV << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: use_SliceThickness=" << use_SliceThickness << endl;
	double toleranceWithinPlane = 5.0/100.0 * use_VerticalFOV;	// allow up to 5% jitter (or progressive displacement) in stack
									// this is sufficient to distinguish side-by-side para-coronal peripheral angio
									// yet match contiguous lumbar spine para-axials
	useStack=-1;
	useDistance;
	int j;
	for (j=0; j < numberOfStackDescriptors; ++j) {
		StackDescriptor *d = tableOfStackDescriptors+j;
		Assert(d);
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_ImagePositionPatient[0]=" << d->reference_ImagePositionPatient[0] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_ImagePositionPatient[1]=" << d->reference_ImagePositionPatient[1] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_ImagePositionPatient[2]=" << d->reference_ImagePositionPatient[2] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_ImageOrientationPatient[0]=" << d->reference_ImageOrientationPatient[0] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_ImageOrientationPatient[1]=" << d->reference_ImageOrientationPatient[1] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_ImageOrientationPatient[2]=" << d->reference_ImageOrientationPatient[2] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_ImageOrientationPatient[3]=" << d->reference_ImageOrientationPatient[3] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_ImageOrientationPatient[4]=" << d->reference_ImageOrientationPatient[4] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_ImageOrientationPatient[5]=" << d->reference_ImageOrientationPatient[5] << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_VerticalFOV=" << d->reference_VerticalFOV << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_HorizontalFOV=" << d->reference_HorizontalFOV << endl;
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] d->reference_SliceThickness=" << d->reference_SliceThickness << endl;
		if (floatEquals(d->reference_ImageOrientationPatient[0],use_ImageOrientationPatient[0])
		 && floatEquals(d->reference_ImageOrientationPatient[1],use_ImageOrientationPatient[1])
		 && floatEquals(d->reference_ImageOrientationPatient[2],use_ImageOrientationPatient[2])
		 && floatEquals(d->reference_ImageOrientationPatient[3],use_ImageOrientationPatient[3])
		 && floatEquals(d->reference_ImageOrientationPatient[4],use_ImageOrientationPatient[4])
		 && floatEquals(d->reference_ImageOrientationPatient[5],use_ImageOrientationPatient[5])
		 && floatEquals(d->reference_VerticalFOV,use_VerticalFOV)
		 && floatEquals(d->reference_HorizontalFOV,use_HorizontalFOV)
		 && floatEquals(d->reference_SliceThickness,use_SliceThickness)
		 && getDistanceWithinImagePlane(use_ImagePositionPatient[0],use_ImagePositionPatient[1],use_ImagePositionPatient[2],
				d->reference_ImagePositionPatient[0],d->reference_ImagePositionPatient[1],d->reference_ImagePositionPatient[2],
				use_ImageOrientationPatient[0],use_ImageOrientationPatient[1],use_ImageOrientationPatient[2],
				use_ImageOrientationPatient[3],use_ImageOrientationPatient[4],use_ImageOrientationPatient[5]) < toleranceWithinPlane
		) {
//cerr << "findFrameInStackDescriptorsOrInsert: [" << j << "] MATCH" << endl;
			useStack=j;
			double distanceAlongNormal = getDistanceAlongNormalToImagePlane(
				use_ImagePositionPatient[0],use_ImagePositionPatient[1],use_ImagePositionPatient[2],
				d->reference_ImagePositionPatient[0],d->reference_ImagePositionPatient[1],d->reference_ImagePositionPatient[2],
				use_ImageOrientationPatient[0],use_ImageOrientationPatient[1],use_ImageOrientationPatient[2],
				use_ImageOrientationPatient[3],use_ImageOrientationPatient[4],use_ImageOrientationPatient[5]);
			Assert(d->distancesAlongNormal);
			Assert(d->nDistancesAlongNormal);
			useDistance=-1;
			int k;
			for (k=0; k < d->nDistancesAlongNormal; ++k) {
				if (d->distancesAlongNormal[k] == distanceAlongNormal) {
					useDistance=k;
					break;
				}
			}
			if (useDistance == -1) {
				useDistance=d->nDistancesAlongNormal++;
				d->distancesAlongNormal[useDistance]=distanceAlongNormal;
			}
			break;
			}
	}
	if (useStack == -1) {
cerr << "findFrameInStackDescriptorsOrInsert: NO MATCH" << endl;
		// there was no matching stack, so make a new one ...
		useStack=numberOfStackDescriptors++;
		StackDescriptor *d = tableOfStackDescriptors+useStack;
		// and copy in the values ...
		d->reference_ImagePositionPatient = new double[3];
		Assert(d->reference_ImagePositionPatient);
		d->reference_ImagePositionPatient[0] = use_ImagePositionPatient[0];
		d->reference_ImagePositionPatient[1] = use_ImagePositionPatient[1];
		d->reference_ImagePositionPatient[2] = use_ImagePositionPatient[2];
		d->reference_ImageOrientationPatient = new double[6];
		Assert(d->reference_ImageOrientationPatient);
		d->reference_ImageOrientationPatient[0] = use_ImageOrientationPatient[0];
		d->reference_ImageOrientationPatient[1] = use_ImageOrientationPatient[1];
		d->reference_ImageOrientationPatient[2] = use_ImageOrientationPatient[2];
		d->reference_ImageOrientationPatient[3] = use_ImageOrientationPatient[3];
		d->reference_ImageOrientationPatient[4] = use_ImageOrientationPatient[4];
		d->reference_ImageOrientationPatient[5] = use_ImageOrientationPatient[5];
		d->reference_VerticalFOV = use_Rows * use_PixelSpacing[0];
		d->reference_HorizontalFOV = use_Columns * use_PixelSpacing[1];
		d->reference_SliceThickness = use_SliceThickness;
		// and create the first in-stack position entry based on distance along the normal to the plane ...
		d->distancesAlongNormal=new double[numberofinputfiles];		// probably nothing like numberofinputfiles in-stack positions, but upper bound
		Assert(d->distancesAlongNormal);
		useDistance=0;
		d->nDistancesAlongNormal=1;
		d->distancesAlongNormal[useDistance]=getDistanceAlongNormalToImagePlane(
			use_ImagePositionPatient[0],use_ImagePositionPatient[1],use_ImagePositionPatient[2],
			d->reference_ImagePositionPatient[0],d->reference_ImagePositionPatient[1],d->reference_ImagePositionPatient[2],
			use_ImageOrientationPatient[0],use_ImageOrientationPatient[1],use_ImageOrientationPatient[2],
			use_ImageOrientationPatient[3],use_ImageOrientationPatient[4],use_ImageOrientationPatient[5]);
	}
//cerr << "Frame " << dec << i << " stack " << useStack << " in-stack position " << useDistance << endl;
	// values are returned in useStack and useDistance arguments
}

static int findFrameInTableOfTimesOrInsert(int i,int numberofinputfiles,double *tableOfTimes,int &numberOfTimes,
		double *perFrame_TriggerTime,double shared_TriggerTime,
		char **perFrame_AcquisitionTime,char *shared_AcquisitionTime,
		char **perFrame_AcquisitionDate,char *shared_AcquisitionDate,
		char **perFrame_ContentTime,char *shared_ContentTime,
		char **perFrame_ContentDate,char *shared_ContentDate,
		bool doItWithTriggerTime,bool doItWithAcquisitionTime,bool doItWithAcquisitionTimeAndDate,
		bool doItWithContentTime,bool doItWithContentTimeAndDate) {

	
	double useTime=0;
	if (doItWithTriggerTime) {
		useTime=perFrame_TriggerTime ? perFrame_TriggerTime[i] : shared_TriggerTime;
	}
	else if (doItWithAcquisitionTime) {
		char *use_AcquisitionTime = perFrame_AcquisitionTime ? perFrame_AcquisitionTime[i] : shared_AcquisitionTime;
		//if (use_AcquisitionTime) useTime=Uint32(Time(use_AcquisitionTime));
		if (use_AcquisitionTime) {
			Time *t = new Time(use_AcquisitionTime);	// there seems to be something funky about allowing Time's to go out of scope :(
			useTime=*t;
			if (t) delete t;
		}
	}
	else if (doItWithAcquisitionTimeAndDate) {
		char *use_AcquisitionTime = perFrame_AcquisitionTime ? perFrame_AcquisitionTime[i] : shared_AcquisitionTime;
		char *use_AcquisitionDate = perFrame_AcquisitionDate ? perFrame_AcquisitionDate[i] : shared_AcquisitionDate;
		// date not implemented yet :(
		//if (use_AcquisitionTime) useTime=Uint32(Time(use_AcquisitionTime));
		if (use_AcquisitionTime) {
			Time *t = new Time(use_AcquisitionTime);	// there seems to be something funky about allowing Time's to to go out of scope :(
			useTime=*t;
			if (t) delete t;
		}
	}
	else if (doItWithContentTime) {
		char *use_ContentTime = perFrame_ContentTime ? perFrame_ContentTime[i] : shared_ContentTime;
		//if (use_ContentTime) useTime=Uint32(Time(use_ContentTime));
		if (use_ContentTime) {
			Time *t = new Time(use_ContentTime);	// there seems to be something funky about allowing Time's to go out of scope :(
			useTime=*t;
			if (t) delete t;
		}
	}
	else if (doItWithContentTimeAndDate) {
		char *use_ContentTime = perFrame_ContentTime ? perFrame_ContentTime[i] : shared_ContentTime;
		char *use_ContentDate = perFrame_ContentDate ? perFrame_ContentDate[i] : shared_ContentDate;
		// date not implemented yet :(
		//if (use_ContentTime) useTime=Uint32(Time(use_ContentTime));
		if (use_ContentTime) {
			Time *t = new Time(use_ContentTime);	// there seems to be something funky about allowing Time's to to go out of scope :(
			useTime=*t;
			if (t) delete t;
		}
	}
	else {
		Assert(0);
	}
			
	int useTemporalPosition=-1;
	int j;
	Assert(tableOfTimes);
	for (j=0; j < numberOfTimes; ++j) {
		if (tableOfTimes[j] == useTime) {
			useTemporalPosition=j;
			break;
			}
	}
	if (useTemporalPosition == -1) {
		// there was no matching temporal position, so make a new one ...
		useTemporalPosition=numberOfTimes++;
		tableOfTimes[useTemporalPosition]=useTime;
	}
	return useTemporalPosition;
}


static int findDoubleInTableOfDoublesOrInsert(double value,double *table,int &tableSize) {

	int found=-1;
	int j;
	Assert(table);
	for (j=0; j < tableSize; ++j) {
		if (table[j] == value) {
			found=j;
			break;
			}
	}
	if (found == -1) {
		// there was no matching value, so make a new one ...
		found=tableSize++;
		table[found]=value;
	}
	return found;
}

static void partitionFramesByValuesForDimensions(int nFrames,int d,int nDimensions,double **dimensionValues,int **dimensionIndices,int partitionedSetSize,int *partitionedSet) {
	double *differentDimensionValues = new double[nFrames];	// upper bound
	Assert(differentDimensionValues);
	int nDifferentDimensionValues=0;
//cerr << "partitionFramesByValuesForDimensions: dimension " << d << " partitionedSetSize " << partitionedSetSize << endl;
	int i;
	for (i=0; i<partitionedSetSize; i++) {
		int j=partitionedSet[i];
		double v=dimensionValues[d][j];
		findDoubleInTableOfDoublesOrInsert(v,differentDimensionValues,nDifferentDimensionValues);
	}
	if (nDifferentDimensionValues) {
//cerr << "partitionFramesByValuesForDimensions: nDifferentDimensionValues " << nDifferentDimensionValues << endl;
		qsort((char *)(differentDimensionValues),nDifferentDimensionValues,sizeof(double),doublecompareascending);
		// build new partitions, one for each different value ...
		int nextd = d+1;
		int l;
		for (l=0; l<nDifferentDimensionValues; ++l) {
			// count how many of this value ...
			int newPartitionedSetSize=0;
			for (i=0; i<partitionedSetSize; i++) {
				double v = dimensionValues[d][partitionedSet[i]];
				if (l == findDoubleInTableOfDoublesOrInsert(v,differentDimensionValues,nDifferentDimensionValues))
					++newPartitionedSetSize;
			}
//cerr << "partitionFramesByValuesForDimensions:  index " << l << " newPartitionedSetSize " << newPartitionedSetSize << endl;
			int *newPartitionedSet=new int[newPartitionedSetSize];
			Assert(newPartitionedSet);
			// now copy the frame indices into the new set ...
			int next=0;
			for (i=0; i<partitionedSetSize; i++) {
				double v = dimensionValues[d][partitionedSet[i]];
				if (l == findDoubleInTableOfDoublesOrInsert(v,differentDimensionValues,nDifferentDimensionValues)) {
					newPartitionedSet[next++]=partitionedSet[i];
					dimensionIndices[d][partitionedSet[i]]=l;
//cerr << "partitionFramesByValuesForDimensions: dimension " << d << " value " << v << " index " << l << endl;
				}
			}
			Assert(next == newPartitionedSetSize);
			if (nextd < nDimensions) partitionFramesByValuesForDimensions(nFrames,nextd,nDimensions,dimensionValues,dimensionIndices,newPartitionedSetSize,newPartitionedSet);
			if (newPartitionedSet) delete[] newPartitionedSet;
		}
		if (differentDimensionValues) delete[] differentDimensionValues;
	}
}

static double
getNumericValueFromNonNumericAttributeForDimensionSorting(Attribute *a) {
	double v=0;
	const char *s = AttributeValue(a);
	if (a->getTag() == TagFromName(FrameType)) {
		if (strcmp(s,"ORIGINAL") == 0) v=1;
		else if (strcmp(s,"DERIVED") == 0) v=2;
//cerr << "getNumericValueFromNonNumericAttributeForDimensionSorting: FrameType " << s << " = " << v << endl;
	}
	else if (a->getTag() == TagFromName(ContrastBolusAgentPhase)) {
		if (strcmp(s,"PRE_CONTRAST") == 0) v=1;
		else if (strcmp(s,"IMMEDIATE") == 0) v=2;
		else if (strcmp(s,"DYNAMIC") == 0) v=3;
		else if (strcmp(s,"ARTERIAL") == 0) v=4;
		else if (strcmp(s,"CAPILLARY") == 0) v=5;
		else if (strcmp(s,"PORTAL_VENOUS") == 0) v=6;
		else if (strcmp(s,"VENOUS") == 0) v=7;
		else if (strcmp(s,"STEADY_STATE") == 0) v=8;
		else if (strcmp(s,"POST_CONTRAST") == 0) v=9;
		else if (strcmp(s,"DELAYED") == 0) v=10;
//cerr << "getNumericValueFromNonNumericAttributeForDimensionSorting: ContrastBolusAgentPhase " << s << " = " << v << endl;
	}
	else {
		const char *p = s;
		bool allDigits = true;
		while (*p != 0 && allDigits) if (!isdigit(*p++)) allDigits=false;
		if (allDigits) {
			v = (s ? strtol(s,NULL,0) : 0);		// e.g. for IDs which are SH VR but usually contain numbers
//cerr << "getNumericValueFromNonNumericAttributeForDimensionSorting: " << a->getTag() << " all digits value = " << v << endl;
		}
		else {
			// make number based on the numeric ascii values of all the characters
			p=s;
			while (*p != 0) {
				v*=128;
				v+=toascii(*p++)&0x7f;
			}
//cerr << "getNumericValueFromNonNumericAttributeForDimensionSorting: " << a->getTag() << " non digits value = " << v << endl;
		}
	}
	return v;
}
						
int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool veryverbose=options.get("veryverbose") || options.get("vv");
	bool veryveryverbose=options.get("veryveryverbose") || options.get("vvv");
	if (veryveryverbose) veryverbose=true;
	if (veryverbose) verbose=true;

	const char *sortbyname;
	bool sortby=options.get("sortby",sortbyname);
	Tag sortbytag;

	if (sortby && !staticDictionary.getTag(sortbyname,sortbytag)) {
		cerr << "Sort tag <" << sortbyname << "> is not a valid data element" << endl;
		bad=true;
	}

	bool noMRMultiFrame=options.get("nomrmf");

	bool noCTMultiFrame=options.get("noctmf");

	bool makeStack=options.get("makestack");

	bool descending=options.get("descending");	// should check present only if sortby
	
	bool stackdescending=options.get("stackdescending");	// should check present only if  makestack
	
	bool addTemporalPosition=options.get("temporalposition");

	bool copyAll=options.get("copyall");

	bool accumulateDuration=options.get("accumulateduration");

	bool deriveDurationFromTiming=options.get("derivedurationfromtiming");
	
	bool addNonDerivedStuffAnyway=options.get("addnonderivedstuffanyway");

	bool addPositionStuffAnyway=options.get("addpositionstuffanyway");

	bool addTimingStuffAnyway=options.get("addtimingstuffanyway");

	bool phaseContrast=options.get("phasecontrast");

	bool minimalAttributesOnly=options.get("minimalattributes");

	bool nowindow=options.get("nowindow");
	
	bool addEnhancedContrastBolusModuleAndMacro=options.get("enhancedcontrast");
	
	bool addSynchronizationModule=options.get("sync");

	bool addSupplementalPaletteColorLUT = false;
	int addSupplementalPaletteColorLUT_numberOfEntries = 0;
	int addSupplementalPaletteColorLUT_firstValueMapped = 0;
	int addSupplementalPaletteColorLUT_firstRedValue = 0;
	int addSupplementalPaletteColorLUT_incrementRedValue = 0;
	int addSupplementalPaletteColorLUT_entryToStartIncrementingRedValue = 0;
	int addSupplementalPaletteColorLUT_entryToStartDecrementingRedValue = 0;
	int addSupplementalPaletteColorLUT_entryToStopChangingRedValue = 0;
	int addSupplementalPaletteColorLUT_firstGreenValue = 0;
	int addSupplementalPaletteColorLUT_incrementGreenValue = 0;
	int addSupplementalPaletteColorLUT_entryToStartIncrementingGreenValue = 0;
	int addSupplementalPaletteColorLUT_entryToStartDecrementingGreenValue = 0;
	int addSupplementalPaletteColorLUT_entryToStopChangingGreenValue = 0;
	int addSupplementalPaletteColorLUT_firstBlueValue = 0;
	int addSupplementalPaletteColorLUT_incrementBlueValue = 0;
	int addSupplementalPaletteColorLUT_entryToStartIncrementingBlueValue = 0;
	int addSupplementalPaletteColorLUT_entryToStartDecrementingBlueValue = 0;
	int addSupplementalPaletteColorLUT_entryToStopChangingBlueValue = 0;
	{
		int addSupplementalPaletteColorLUTArgs[17];
		int n = options.get("addcolorlut",addSupplementalPaletteColorLUTArgs,17);
//cerr << "addSupplementalPaletteColorLUT returns " << dec << n << endl;
		if (n > 0) {
			if (n == 17) {
				addSupplementalPaletteColorLUT=true;
				addSupplementalPaletteColorLUT_numberOfEntries = addSupplementalPaletteColorLUTArgs[0];
				addSupplementalPaletteColorLUT_firstValueMapped = addSupplementalPaletteColorLUTArgs[1];
				
				addSupplementalPaletteColorLUT_firstRedValue = addSupplementalPaletteColorLUTArgs[2];
				addSupplementalPaletteColorLUT_incrementRedValue = addSupplementalPaletteColorLUTArgs[3];
				addSupplementalPaletteColorLUT_entryToStartIncrementingRedValue = addSupplementalPaletteColorLUTArgs[4];
				addSupplementalPaletteColorLUT_entryToStartDecrementingRedValue = addSupplementalPaletteColorLUTArgs[5];
				addSupplementalPaletteColorLUT_entryToStopChangingRedValue = addSupplementalPaletteColorLUTArgs[6];
				
				addSupplementalPaletteColorLUT_firstGreenValue = addSupplementalPaletteColorLUTArgs[7];
				addSupplementalPaletteColorLUT_incrementGreenValue = addSupplementalPaletteColorLUTArgs[8];
				addSupplementalPaletteColorLUT_entryToStartIncrementingGreenValue = addSupplementalPaletteColorLUTArgs[9];
				addSupplementalPaletteColorLUT_entryToStartDecrementingGreenValue = addSupplementalPaletteColorLUTArgs[10];
				addSupplementalPaletteColorLUT_entryToStopChangingGreenValue = addSupplementalPaletteColorLUTArgs[11];

				addSupplementalPaletteColorLUT_firstBlueValue = addSupplementalPaletteColorLUTArgs[12];
				addSupplementalPaletteColorLUT_incrementBlueValue = addSupplementalPaletteColorLUTArgs[13];
				addSupplementalPaletteColorLUT_entryToStartIncrementingBlueValue = addSupplementalPaletteColorLUTArgs[14];
				addSupplementalPaletteColorLUT_entryToStartDecrementingBlueValue = addSupplementalPaletteColorLUTArgs[15];
				addSupplementalPaletteColorLUT_entryToStopChangingBlueValue = addSupplementalPaletteColorLUTArgs[16];
			}
			else {
				cerr << "addcolorlut needs 17 arguments, not " << dec << n << endl;
				bad=true;
			}
		}
	}
			
	bool forceReferencedPerFrame=options.get("perframereference");

	bool addReferenced = false;
	const char *addReferenced_studyInstanceUID;
	const char *addReferenced_seriesInstanceUID;
	const char *addReferenced_referencedSOPClassUID;
	const char *addReferenced_referencedSOPInstanceUID;
	const char *addReferenced_referencedFrameNumbers;
	const char *addReferenced_purposeOfReferenceCodeSequenceCodingSchemeDesignator;
	const char *addReferenced_purposeOfReferenceCodeSequenceCodeValue;
	const char *addReferenced_purposeOfReferenceCodeSequenceCodeMeaning;
	const char *addReferenced_purposeOfReferenceCodeSequenceCodingSchemeVersion;
	{
		const char *addReferencedArgs[9];
		int n = options.get("addreferenced",addReferencedArgs,9);
//cerr << "addReferenced returns " << dec << n << endl;
		if (n > 0) {
			if (n == 9) {
//cerr << "option: addReferenced: addReferencedArgs[0] " << addReferencedArgs[0] << endl;
//cerr << "option: addReferenced: addReferencedArgs[1] " << addReferencedArgs[1] << endl;
//cerr << "option: addReferenced: addReferencedArgs[2] " << addReferencedArgs[2] << endl;
//cerr << "option: addReferenced: addReferencedArgs[3] " << addReferencedArgs[3] << endl;
//cerr << "option: addReferenced: addReferencedArgs[4] " << addReferencedArgs[4] << endl;
//cerr << "option: addReferenced: addReferencedArgs[5] " << addReferencedArgs[5] << endl;
//cerr << "option: addReferenced: addReferencedArgs[6] " << addReferencedArgs[6] << endl;
//cerr << "option: addReferenced: addReferencedArgs[7] " << addReferencedArgs[7] << endl;
//cerr << "option: addReferenced: addReferencedArgs[8] " << addReferencedArgs[8] << endl;
				addReferenced=true;
				                                    addReferenced_studyInstanceUID = removeLeadingSpaces(addReferencedArgs[0]);
				                                   addReferenced_seriesInstanceUID = removeLeadingSpaces(addReferencedArgs[1]);
				                               addReferenced_referencedSOPClassUID = removeLeadingSpaces(addReferencedArgs[2]);
				                            addReferenced_referencedSOPInstanceUID = removeLeadingSpaces(addReferencedArgs[3]);
				                              addReferenced_referencedFrameNumbers = removeLeadingSpaces(addReferencedArgs[4]);
				addReferenced_purposeOfReferenceCodeSequenceCodingSchemeDesignator = removeLeadingSpaces(addReferencedArgs[5]);
				             addReferenced_purposeOfReferenceCodeSequenceCodeValue = removeLeadingSpaces(addReferencedArgs[6]);
				           addReferenced_purposeOfReferenceCodeSequenceCodeMeaning = removeLeadingSpaces(addReferencedArgs[7]);
				   addReferenced_purposeOfReferenceCodeSequenceCodingSchemeVersion = removeLeadingSpaces(addReferencedArgs[8]);
			}
			else {
				cerr << "addreferenced needs 9 arguments, not " << dec << n << endl;
				bad=true;
			}
		}
	}

	bool forceDerivationPerFrame=options.get("perframederivation");

	bool addDerivation = false;
	const char *addDerivation_studyInstanceUID;
	const char *addDerivation_seriesInstanceUID;
	const char *addDerivation_referencedSOPClassUID;
	const char *addDerivation_referencedSOPInstanceUID;
	const char *addDerivation_referencedFrameNumbers;
	const char *addDerivation_derivationCodeSequenceCodingSchemeDesignator;
	const char *addDerivation_derivationCodeSequenceCodeValue;
	const char *addDerivation_derivationCodeSequenceCodeMeaning;
	const char *addDerivation_derivationCodeSequenceCodingSchemeVersion;
	const char *addDerivation_purposeOfReferenceCodeSequenceCodingSchemeDesignator;
	const char *addDerivation_purposeOfReferenceCodeSequenceCodeValue;
	const char *addDerivation_purposeOfReferenceCodeSequenceCodeMeaning;
	const char *addDerivation_purposeOfReferenceCodeSequenceCodingSchemeVersion;
	{
		const char *addDerivationArgs[13];
		int n = options.get("addderivation",addDerivationArgs,13);
//cerr << "addderivation returns " << dec << n << endl;
		if (n > 0) {
			if (n == 13) {
//cerr << "option: addderivation: addDerivationArgs[0] " << addDerivationArgs[0] << endl;
//cerr << "option: addderivation: addDerivationArgs[1] " << addDerivationArgs[1] << endl;
//cerr << "option: addderivation: addDerivationArgs[2] " << addDerivationArgs[2] << endl;
//cerr << "option: addderivation: addDerivationArgs[3] " << addDerivationArgs[3] << endl;
//cerr << "option: addderivation: addDerivationArgs[4] " << addDerivationArgs[4] << endl;
//cerr << "option: addderivation: addDerivationArgs[5] " << addDerivationArgs[5] << endl;
//cerr << "option: addderivation: addDerivationArgs[6] " << addDerivationArgs[6] << endl;
//cerr << "option: addderivation: addDerivationArgs[7] " << addDerivationArgs[7] << endl;
//cerr << "option: addderivation: addDerivationArgs[8] " << addDerivationArgs[8] << endl;
//cerr << "option: addderivation: addDerivationArgs[9] " << addDerivationArgs[9] << endl;
//cerr << "option: addderivation: addDerivationArgs[10] " << addDerivationArgs[10] << endl;
//cerr << "option: addderivation: addDerivationArgs[11] " << addDerivationArgs[11] << endl;
//cerr << "option: addderivation: addDerivationArgs[12] " << addDerivationArgs[12] << endl;
				addDerivation=true;
				                                    addDerivation_studyInstanceUID = removeLeadingSpaces(addDerivationArgs[0]);
				                                   addDerivation_seriesInstanceUID = removeLeadingSpaces(addDerivationArgs[1]);
				                               addDerivation_referencedSOPClassUID = removeLeadingSpaces(addDerivationArgs[2]);
				                            addDerivation_referencedSOPInstanceUID = removeLeadingSpaces(addDerivationArgs[3]);
				                              addDerivation_referencedFrameNumbers = removeLeadingSpaces(addDerivationArgs[4]);
				        addDerivation_derivationCodeSequenceCodingSchemeDesignator = removeLeadingSpaces(addDerivationArgs[5]);
						     addDerivation_derivationCodeSequenceCodeValue = removeLeadingSpaces(addDerivationArgs[6]);
				                   addDerivation_derivationCodeSequenceCodeMeaning = removeLeadingSpaces(addDerivationArgs[7]);
				           addDerivation_derivationCodeSequenceCodingSchemeVersion = removeLeadingSpaces(addDerivationArgs[8]);
				addDerivation_purposeOfReferenceCodeSequenceCodingSchemeDesignator = removeLeadingSpaces(addDerivationArgs[9]);
				             addDerivation_purposeOfReferenceCodeSequenceCodeValue = removeLeadingSpaces(addDerivationArgs[10]);
				           addDerivation_purposeOfReferenceCodeSequenceCodeMeaning = removeLeadingSpaces(addDerivationArgs[11]);
				   addDerivation_purposeOfReferenceCodeSequenceCodingSchemeVersion = removeLeadingSpaces(addDerivationArgs[12]);
			}
			else {
				cerr << "addderivation needs 13 arguments, not " << dec << n << endl;
				bad=true;
			}
		}
	}

	static const int addSatSlab_maximumNumberOfSlabs = 6;		// i.e. two on each possible side
	int addSatSlab_numberOfSlabs = 0;
	double addSatSlab_thickness[addSatSlab_maximumNumberOfSlabs];	
	double addSatSlab_orientX[addSatSlab_maximumNumberOfSlabs];	
	double addSatSlab_orientY[addSatSlab_maximumNumberOfSlabs];	
	double addSatSlab_orientZ[addSatSlab_maximumNumberOfSlabs];	
	double addSatSlab_midpointX[addSatSlab_maximumNumberOfSlabs];	
	double addSatSlab_midpointY[addSatSlab_maximumNumberOfSlabs];	
	double addSatSlab_midpointZ[addSatSlab_maximumNumberOfSlabs];	
	while (true) {
		double addSatSlab_Args[7];
		int n = options.get("addsatslab",addSatSlab_Args,7);
		if (n < 0) {
			break;
		}
		else {
			if (n == 7) {
				Assert(addSatSlab_numberOfSlabs < addSatSlab_maximumNumberOfSlabs);
				addSatSlab_thickness[addSatSlab_numberOfSlabs] = addSatSlab_Args[0];	
				  addSatSlab_orientX[addSatSlab_numberOfSlabs] = addSatSlab_Args[1];	
				  addSatSlab_orientY[addSatSlab_numberOfSlabs] = addSatSlab_Args[2];	
				  addSatSlab_orientZ[addSatSlab_numberOfSlabs] = addSatSlab_Args[3];	
				addSatSlab_midpointX[addSatSlab_numberOfSlabs] = addSatSlab_Args[4];	
				addSatSlab_midpointY[addSatSlab_numberOfSlabs] = addSatSlab_Args[5];	
				addSatSlab_midpointZ[addSatSlab_numberOfSlabs] = addSatSlab_Args[6];
				++addSatSlab_numberOfSlabs;
			}
			else {
				cerr << "addsatslab needs 7 arguments, not " << dec << n << endl;
				bad=true;
			}
		}
	}

	const int MAX_DIMENSION_ORGANIZATIONS = 10;
	const int MAX_DIMENSIONS = 20;
	
	const char ***dimensionIndexNames = NULL;
	Tag **dimensionIndexPointers = NULL;
	Tag **dimensionFunctionalGroupPointers = NULL;
	
	const char **dimensionOrganizationLabels = NULL;
	const char **dimensionOrganizationUIDs = NULL;
	
	int *nDimensions = NULL;
	int nDimensionOrganizations = 0;

	{
		const char *argumentNames[2];
		
		int argCount;
		while ((argCount=options.get("dimension",argumentNames,2)) == 2 || (argCount=options.get("multidimension",argumentNames,4)) == 4) {
//cerr << "option: dimension or multidimension: argCount " << argCount << endl;
			const char *indexPointerName = argumentNames[0];
			const char *functionalGroupPointerName = argumentNames[1];
//cerr << "option: dimension or multidimension: indexPointerName " << indexPointerName << endl;
//cerr << "option: dimension or multidimension: functionalGroupPointerName " << functionalGroupPointerName << endl;
			const char *organizationLabel = argCount == 4 ? argumentNames[2] : NULL;
			const char *organizationUID =  argCount == 4 ? argumentNames[3] : NULL;
			
			if (organizationLabel) {
				const char *p = organizationLabel;
				while (*p && isspace(*p)) ++p;
				if (!*p) organizationLabel=NULL;
			}

			if (organizationUID) {
				const char *p = organizationUID;
				while (*p && isspace(*p)) ++p;
				if (!*p) organizationUID=NULL;
			}

			int organizationNumber = argCount ==  2 ? 0 : nDimensionOrganizations;	// forces assumption of new organization unless recognized
			
			if (organizationLabel != NULL && nDimensionOrganizations > 0 && dimensionOrganizationLabels != NULL) {
//cerr << "option: dimension or multidimension: checking for organization with label" << organizationLabel << endl;
				for (int i=0; i<nDimensionOrganizations; ++i) {
//cerr << "option: dimension or multidimension: checking organization " << i << endl;
					if (dimensionOrganizationLabels[i] != NULL && strcmp(organizationLabel,dimensionOrganizationLabels[i]) == 0) {
//cerr << "option: dimension or multidimension: found organization " << i << endl;
						organizationNumber=i;
						break;
					}
				}
			}
			else if (organizationUID != NULL && nDimensionOrganizations > 0 && dimensionOrganizationUIDs != NULL) {
//cerr << "option: dimension or multidimension: checking for organization with uid" << organizationUID << endl;
				for (int i=0; i<nDimensionOrganizations; ++i) {
//cerr << "option: dimension or multidimension: checking organization " << i << endl;
					if (dimensionOrganizationUIDs[i] != NULL && strcmp(organizationLabel,dimensionOrganizationUIDs[i]) == 0) {
						organizationNumber=i;
//cerr << "option: dimension or multidimension: found organization " << i << endl;
						break;
					}
				}
			}
			
			Tag indexPointerTag;
			if (!staticDictionary.getTag(indexPointerName,indexPointerTag)) {
				cerr << "Dimension index pointer <" << indexPointerName << "> is not a valid data element" << endl;
				bad=true;
			}
			Tag functionalGroupPointerTag;
			if (!staticDictionary.getTag(functionalGroupPointerName,functionalGroupPointerTag)) {
				cerr << "Dimension functional group pointer <" << functionalGroupPointerName << "> is not a valid data element" << endl;
				bad=true;
			}
			if (!bad) {
				while (organizationNumber >= nDimensionOrganizations) {
					if (nDimensionOrganizations < MAX_DIMENSION_ORGANIZATIONS) {
						if (nDimensionOrganizations == 0) {
							Assert(dimensionIndexNames == NULL);
							Assert(dimensionIndexPointers == NULL);
							Assert(dimensionFunctionalGroupPointers == NULL);
							Assert(dimensionOrganizationLabels == NULL);
							Assert(dimensionOrganizationUIDs == NULL);
							Assert(nDimensions == NULL);
							dimensionIndexNames=new const char **[MAX_DIMENSION_ORGANIZATIONS];
							Assert(dimensionIndexNames);
							dimensionIndexPointers=new Tag *[MAX_DIMENSION_ORGANIZATIONS];
							Assert(dimensionIndexPointers);
							dimensionFunctionalGroupPointers=new Tag *[MAX_DIMENSION_ORGANIZATIONS];
							Assert(dimensionFunctionalGroupPointers);
							dimensionOrganizationLabels=new const char *[MAX_DIMENSION_ORGANIZATIONS];
							Assert(dimensionOrganizationLabels);
							dimensionOrganizationUIDs=new const char *[MAX_DIMENSION_ORGANIZATIONS];
							Assert(dimensionOrganizationUIDs);
							nDimensions = new int[MAX_DIMENSION_ORGANIZATIONS];
							Assert(nDimensions);
							for (int i=0; i<MAX_DIMENSION_ORGANIZATIONS; ++i) {
								dimensionIndexNames[i]=NULL;
								dimensionIndexPointers[i]=NULL;
								dimensionFunctionalGroupPointers[i]=NULL;
								dimensionOrganizationLabels[i]=NULL;
								dimensionOrganizationUIDs[i]=NULL;
								nDimensions[i]=0;
							}
						}
						++nDimensionOrganizations;
					}
					else {
						cerr << "Exceeded maximum number of dimension organizations supported (" << MAX_DIMENSION_ORGANIZATIONS << ") - ignoring <" << indexPointerName << ">" << endl;
						bad=true;
					}
				}
			}
			if (!bad) {
				Assert(organizationNumber < nDimensionOrganizations);
				int dimensionNumber = nDimensions[organizationNumber];
				if (dimensionNumber < MAX_DIMENSIONS) {
					if (dimensionNumber == 0) {
						Assert(dimensionIndexNames[organizationNumber] == NULL);
						Assert(dimensionIndexPointers[organizationNumber] == NULL);
						Assert(dimensionFunctionalGroupPointers[organizationNumber] == NULL);
						Assert(dimensionOrganizationLabels[organizationNumber] == NULL);
						Assert(dimensionOrganizationUIDs[organizationNumber] == NULL);
						
						dimensionIndexNames[organizationNumber]=new const char *[MAX_DIMENSIONS];
						Assert(dimensionIndexNames[organizationNumber]);
						dimensionIndexPointers[organizationNumber]=new Tag[MAX_DIMENSIONS];
						Assert(dimensionIndexPointers[organizationNumber]);
						dimensionFunctionalGroupPointers[organizationNumber]=new Tag[MAX_DIMENSIONS];
						Assert(dimensionFunctionalGroupPointers[organizationNumber]);
					}
					dimensionIndexNames[organizationNumber][dimensionNumber]=indexPointerName;
					dimensionIndexPointers[organizationNumber][dimensionNumber]=indexPointerTag;
					dimensionFunctionalGroupPointers[organizationNumber][dimensionNumber]=functionalGroupPointerTag;
					if (dimensionOrganizationLabels[organizationNumber] == NULL) {
						dimensionOrganizationLabels[organizationNumber]=organizationLabel;
					}
					else {
						if (organizationLabel != NULL && dimensionOrganizationLabels[organizationNumber] != NULL
						 && strcmp(organizationLabel,dimensionOrganizationLabels[organizationNumber]) != 0) {
							cerr << "Different label <" << organizationLabel << "> for dimension organization " << organizationNumber
							     << " was <" << dimensionOrganizationLabels[organizationNumber] << ">" << endl;
							bad=true;
						 }
					}
					if (dimensionOrganizationUIDs[organizationNumber] == NULL) {
						dimensionOrganizationUIDs[organizationNumber]=organizationUID;
					}
					else {
						if (organizationUID != NULL && dimensionOrganizationUIDs[organizationNumber] != NULL
						 && strcmp(organizationUID,dimensionOrganizationUIDs[organizationNumber]) != 0) {
							cerr << "Different UID <" << organizationUID << "> for dimension organization " << organizationNumber
							     << " was <" << dimensionOrganizationUIDs[organizationNumber] << ">" << endl;
							bad=true;
						 }
					}
					nDimensions[organizationNumber]++;
				}
				else {
					cerr << "Exceeded maximum number of dimensions supported (" << MAX_DIMENSIONS << ") - ignoring <" << indexPointerName << ">" << endl;
					bad=true;
				}
			}
		}
	}
	
	dicom_input_options.done();
	Assert(!dicom_input_options.filename);

	dicom_output_options.done();

	int numberofinputfiles=!options;

	struct sortentry *sortedfilenamestable = new sortentry [numberofinputfiles];
	Assert(sortedfilenamestable);
	struct sortentry *ptr = sortedfilenamestable;

	const char *filename;

	while(!options && (filename=options())) {
		++options;
		ptr->filename=filename;
//cerr << "option: filename " << filename << endl;
		ptr->value=0;			// leave sort value unused
		++ptr;
	}

	options.done();

	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose|-vv|-veryverbose|-vvv|-veryveryverbose]"
			<< " [-sortby elementname [-descending]]"
			<< " [-nomrmf]"
			<< " [-noctmf]"
			<< " [-makestack [-stackdescending]]"
			<< " [-temporalposition]"
			<< " [-dimension elementname functionalgroupname]*"
			<< " [-multidimension elementname functionalgroupname organizationlabel organizationuid]*"
			<< " [-addreferenced class instance frames|\" \" purposescheme code mng vers [-perframereference]]"
			<< " [-addderivation class instance frames|\" \" derivationscheme code mng vers purposescheme code mng vers [-perframederivation]]"
			<< " [-addsatslab thickness orientX orientY orientZ midpointX midpointY midpointZ]"
			<< " [-addcolorlut numberofentries firstvaluemapped firstred incrred startincrred startdecrred stopred firstgreen incrgreen startincrgreen startdecrgreen stopgreen firstblue incrblue startincrblue startdecrblue stopblue]"
			<< " [-accumulateduration]"
			<< " [-derivedurationfromtiming]"
			<< " [-addnonderivedstuffanyway]"
			<< " [-addpositionstuffanyway]"
			<< " [-addtimingstuffanyway]"
			<< " [-phasecontrast]"
			<< " [-minimalattributes]"
			<< " [-nowindow]"
			<< " [-enhancedcontrast]"
			<< " [-sync]"
			<< " [-copyall]"
			<< " [-irradiationeventuid uid]"
			<< " " << MMsgDC(InputFile) << " ["<< MMsgDC(InputFile) << " ...]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	if (!dicom_output_options.transfersyntaxuid)
		dicom_output_options.transfersyntaxuid=ExplicitVRLittleEndianTransferSyntaxUID;

	bool success=true;
	TextOutputStream log(cerr);

	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	if (sortby) {

		// Make additional pass through images files to get sorttag

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

			sortedfilenamestable[i].value = value;

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
	}

	if (!success) exit(1);

	int *stackID=NULL;
	int *inStackPositionNumber=NULL;
	int *temporalPositionIndex=NULL;

	if (makeStack || addTemporalPosition) {

		// Make additional pass through images files to plan stacks, temporal positions and dimensions
		
		// Step 1 - read all image files and track necessary attributes values ...
		// (and while we are at it, track stuff for deriving temporal position index)

		double reference_tlhc_X;	// these are used for ImagePositionPatient only
		double reference_tlhc_Y;
		double reference_tlhc_Z;
		double reference_normal_X;	// these are used for ImageOrientationPatient only
		double reference_normal_Y;
		double reference_normal_Z;
		double reference_normal_length;

		// Track these for stacks ...
		
		bool encountered_Rows = false;
		bool encountered_Columns = false;
		bool encountered_ImagePositionPatient = false;
		bool encountered_ImageOrientationPatient = false;
		bool encountered_SliceThickness = false;
		bool encountered_PixelSpacing = false;
		
		Uint32 shared_Rows = 0;
		Uint32 shared_Columns = 0;
		double *shared_ImagePositionPatient = NULL;
		double *shared_ImageOrientationPatient = NULL;
		double shared_SliceThickness = 0;
		double *shared_PixelSpacing = NULL;
		
		Uint32 *perFrame_Rows = 0;
		Uint32 *perFrame_Columns = 0;
		double **perFrame_ImagePositionPatient = NULL;
		double **perFrame_ImageOrientationPatient = NULL;
		double *perFrame_SliceThickness = NULL;
		double **perFrame_PixelSpacing = NULL;
		
		// Track these for temporal position index ...
		
		bool encountered_AcquisitionDate = false;
		bool encountered_AcquisitionTime = false;
		bool encountered_ContentDate = false;
		bool encountered_ContentTime = false;
		bool encountered_TriggerTime = false;
		bool encountered_TemporalPositionIndex = false;
        
		char *shared_AcquisitionDate = NULL;
		char *shared_AcquisitionTime = NULL;
		char *shared_ContentDate = NULL;
		char *shared_ContentTime = NULL;
		double shared_TriggerTime = 0;
		Uint32 shared_TemporalPositionIndex = 0;
        
		char **perFrame_AcquisitionDate = NULL;
		char **perFrame_AcquisitionTime = NULL;
		char **perFrame_ContentDate = NULL;
		char **perFrame_ContentTime = NULL;
		double *perFrame_TriggerTime = NULL;
		Uint32 *perFrame_TemporalPositionIndex = NULL;

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

			if (makeStack) {
				trackSingleValuedUint32AttributePerFrame(list,TagFromName(Rows),"Rows",
					encountered_Rows,shared_Rows,perFrame_Rows,
					numberofinputfiles,i,filename,log);

				trackSingleValuedUint32AttributePerFrame(list,TagFromName(Columns),"Columns",
					encountered_Columns,shared_Columns,perFrame_Columns,
					numberofinputfiles,i,filename,log);

				trackMultiValuedDoubleAttributePerFrame(list,TagFromName(ImagePositionPatient),"Image Position (Patient)",3,
					encountered_ImagePositionPatient,shared_ImagePositionPatient,perFrame_ImagePositionPatient,
					numberofinputfiles,i,filename,log);

				trackMultiValuedDoubleAttributePerFrame(list,TagFromName(ImageOrientationPatient),"Image Orientation (Patient)",6,
					encountered_ImageOrientationPatient,shared_ImageOrientationPatient,perFrame_ImageOrientationPatient,
					numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(SliceThickness),"Slice Thickness",
					encountered_SliceThickness,shared_SliceThickness,perFrame_SliceThickness,
					numberofinputfiles,i,filename,log);

				trackMultiValuedDoubleAttributePerFrame(list,TagFromName(PixelSpacing),"Pixel Spacing",2,
					encountered_PixelSpacing,shared_PixelSpacing,perFrame_PixelSpacing,
					numberofinputfiles,i,filename,log);
			}
			
			if (addTemporalPosition) {
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(AcquisitionDate),"Acquisition Date",
                                        encountered_AcquisitionDate,shared_AcquisitionDate,perFrame_AcquisitionDate,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(AcquisitionTime),"Acquisition Time",
                                        encountered_AcquisitionTime,shared_AcquisitionTime,perFrame_AcquisitionTime,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(ContentDate),"Content Date",
                                        encountered_ContentDate,shared_ContentDate,perFrame_ContentDate,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(ContentTime),"Content Time",
                                        encountered_ContentTime,shared_ContentTime,perFrame_ContentTime,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(TriggerTime),"Trigger Time",
                                        encountered_TriggerTime,shared_TriggerTime,perFrame_TriggerTime,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedUint32AttributePerFrame(list,TagFromName(TemporalPositionIndex),"Temporal Position Index",
					encountered_TemporalPositionIndex,shared_TemporalPositionIndex,perFrame_TemporalPositionIndex,
					numberofinputfiles,i,filename,log);
			}
			
			// clean up ... the DICOM input stream goes out of scope with each iteration of the loop

			if (fstr) delete fstr;
		}
		
		if (makeStack) {
		
			// Step 2 ... scan through attribute values that were tracked for each frame and identify the stacks,
			// as well as identifying unique in-stack positions (based on distance along the normal to the stack
			// plane)
		
			StackDescriptor *tableOfStackDescriptors = new StackDescriptor[numberofinputfiles];		// probably nothing like numberofinputfiles stacks, but upper bound
			Assert(tableOfStackDescriptors);
			int numberOfStackDescriptors = 0;
			{ int j; for (j=0; j<numberofinputfiles; ++j) tableOfStackDescriptors[j].distancesAlongNormal=NULL; tableOfStackDescriptors[j].nDistancesAlongNormal=0; }
		
			for (i=0; i < numberofinputfiles; ++i) {
				int useStack;
				int useDistance;
				findFrameInStackDescriptorsOrInsert(i,numberofinputfiles,tableOfStackDescriptors,numberOfStackDescriptors,
					perFrame_Rows,shared_Rows,
					perFrame_Columns,shared_Columns,
					perFrame_ImagePositionPatient,shared_ImagePositionPatient,
					perFrame_ImageOrientationPatient,shared_ImageOrientationPatient,
					perFrame_SliceThickness,shared_SliceThickness,
					perFrame_PixelSpacing,shared_PixelSpacing,
					useStack,useDistance);
//cerr << "Before sort Frame " << dec << i << " stack " << useStack << " in-stack position (unsorted) " << useDistance << endl;
			}
		
			// Step 3 ... for each stack, sort the in-stack positions by distance along the normal
		
			if (tableOfStackDescriptors) {
				int j;
				for (j=0; j<numberOfStackDescriptors; ++j) {
					double *distancesAlongNormal = tableOfStackDescriptors[j].distancesAlongNormal;
					int nDistancesAlongNormal = tableOfStackDescriptors[j].nDistancesAlongNormal;
					Assert(nDistancesAlongNormal);
					Assert(distancesAlongNormal);
		
					qsort((char *)distancesAlongNormal,nDistancesAlongNormal,sizeof(double),
						stackdescending ? doublecomparedescending : doublecompareascending);
				}
			}
		
			// Step 4 ... make another pass through the tracked attribute values, this time assigning a stack identifier
			// and an in-stack position identifier (based on the implicit location in sorted distancesAlongNormal[])

			stackID = new int[numberofinputfiles];
			Assert(stackID);
			inStackPositionNumber = new int[numberofinputfiles];
			Assert(inStackPositionNumber);
			for (i=0; i < numberofinputfiles; ++i) {
				int useStack;
				int useDistance;
				findFrameInStackDescriptorsOrInsert(i,numberofinputfiles,tableOfStackDescriptors,numberOfStackDescriptors,
					perFrame_Rows,shared_Rows,
					perFrame_Columns,shared_Columns,
					perFrame_ImagePositionPatient,shared_ImagePositionPatient,
					perFrame_ImageOrientationPatient,shared_ImageOrientationPatient,
					perFrame_SliceThickness,shared_SliceThickness,
					perFrame_PixelSpacing,shared_PixelSpacing,
					useStack,useDistance);
//cerr << "After sort Frame " << dec << i << " stack " << useStack << " in-stack position (sorted) " << useDistance << " position " << tableOfStackDescriptors[useStack].distancesAlongNormal[useDistance] << endl;
					stackID[i]=useStack+1;
					inStackPositionNumber[i]=useDistance+1;	// standard says these starts from 1
			}

			// Step 5 - clean up ...
		
			if (tableOfStackDescriptors) {
				// Not numberofinputfiles ... since above numberOfStackDescriptors won't have been initialized !
				//{ int j; for (j=0; j<numberOfStackDescriptors; ++j)
				//	if (tableOfStackDescriptors[j].distancesAlongNormal) delete[] tableOfStackDescriptors[j].distancesAlongNormal; }
				//delete[] tableOfStackDescriptors;
			}
		
			// Left with per frame arrays stackID[] and inStackPositionNumber[] which are used later in building Frame Content Macro
		}
		if (addTemporalPosition) {
			if (encountered_TemporalPositionIndex) {
				temporalPositionIndex = new int[numberofinputfiles];
				Assert(temporalPositionIndex);
				for (i=0; i < numberofinputfiles; ++i) {
					temporalPositionIndex[i] = perFrame_TemporalPositionIndex ? perFrame_TemporalPositionIndex[i] : shared_TemporalPositionIndex;
				}
			}
			else {
		
				// Step 2 ... scan through attribute values that were tracked for each frame and identify the temporal positions
			
				bool doItWithTriggerTime = false;
				bool doItWithAcquisitionTime = false;
				bool doItWithAcquisitionTimeAndDate = false;
				bool doItWithContentTime = false;
				bool doItWithContentTimeAndDate = false;

cerr << "encountered_TriggerTime = " << encountered_TriggerTime << endl;
cerr << "perFrame_TriggerTime = " << perFrame_TriggerTime << endl;
cerr << "encountered_AcquisitionTime = " << encountered_AcquisitionTime << endl;
cerr << "perFrame_AcquisitionTime = " << perFrame_AcquisitionTime << endl;
cerr << "encountered_AcquisitionDate = " << encountered_AcquisitionDate << endl;
cerr << "perFrame_AcquisitionDate = " << perFrame_AcquisitionDate << endl;
cerr << "encountered_ContentTime = " << encountered_ContentTime << endl;
cerr << "perFrame_ContentTime = " << perFrame_ContentTime << endl;
cerr << "encountered_ContentDate = " << encountered_ContentDate << endl;
cerr << "perFrame_ContentDate = " << perFrame_ContentDate << endl;
			
				if (encountered_TriggerTime && perFrame_TriggerTime) {
					// different trigger times available
					doItWithTriggerTime=true;
				}
				else if (encountered_AcquisitionTime && perFrame_AcquisitionTime && (!encountered_AcquisitionDate || !perFrame_AcquisitionDate)) {
					// different acquisition times available and missing or same date
					doItWithAcquisitionTime=true;
				}
				else if (encountered_AcquisitionTime && encountered_AcquisitionDate && (perFrame_AcquisitionTime || perFrame_AcquisitionDate)) {
					// both time and date present and either time or date or both vary per frame
					doItWithAcquisitionTimeAndDate=true;
				}
				else if (encountered_ContentTime && perFrame_ContentTime && (!encountered_ContentDate || !perFrame_ContentDate)) {
					// different content times available and missing or same date
					doItWithContentTime=true;
				}
				else if (encountered_ContentTime && encountered_ContentDate && (perFrame_ContentTime || perFrame_ContentDate)) {
					// both time and date present and either time or date or both vary per frame
					doItWithContentTimeAndDate=true;
				}
			
				if (doItWithTriggerTime
				 || doItWithAcquisitionTime
				 || doItWithAcquisitionTimeAndDate
				 || doItWithContentTime
				 || doItWithContentTimeAndDate) {
					double *tableOfTimes = new double[numberofinputfiles];		// probably nothing like numberofinputfiles temporal positions, but upper bound
					Assert(tableOfTimes);
					int numberOfTimes = 0;
					{ int j; for (j=0; j<numberofinputfiles; ++j) tableOfTimes[j]=0; }
		
					for (i=0; i < numberofinputfiles; ++i) {
						int useTemporalPositionIndex = findFrameInTableOfTimesOrInsert(i,numberofinputfiles,tableOfTimes,numberOfTimes,
							perFrame_TriggerTime,shared_TriggerTime,
							perFrame_AcquisitionTime,shared_AcquisitionTime,
							perFrame_AcquisitionDate,shared_AcquisitionDate,
							perFrame_ContentTime,shared_ContentTime,
							perFrame_ContentDate,shared_ContentDate,
							doItWithTriggerTime,doItWithAcquisitionTime,doItWithAcquisitionTimeAndDate,
							doItWithContentTime,doItWithContentTimeAndDate);
//cerr << "Before sort Frame " << dec << i << " useTemporalPositionIndex " << useTemporalPositionIndex << endl;
					}
			
					// Step 3 ... for each temporal position, sort by time
		
					qsort((char *)tableOfTimes,numberOfTimes,sizeof(double),doublecompareascending);
		
					// Step 4 ... make another pass through the tracked attribute values, this time assigning a
					// temporal position identifier (based on the implicit location in sorted times table)

					temporalPositionIndex = new int[numberofinputfiles];
					Assert(temporalPositionIndex);
					for (i=0; i < numberofinputfiles; ++i) {
						int useTemporalPositionIndex = findFrameInTableOfTimesOrInsert(i,numberofinputfiles,tableOfTimes,numberOfTimes,
							perFrame_TriggerTime,shared_TriggerTime,
							perFrame_AcquisitionTime,shared_AcquisitionTime,
							perFrame_AcquisitionDate,shared_AcquisitionDate,
							perFrame_ContentTime,shared_ContentTime,
							perFrame_ContentDate,shared_ContentDate,
							doItWithTriggerTime,doItWithAcquisitionTime,doItWithAcquisitionTimeAndDate,
							doItWithContentTime,doItWithContentTimeAndDate);
//cerr << "After sort Frame " << dec << i << " useTemporalPositionIndex " << useTemporalPositionIndex << " time " << tableOfTimes[useTemporalPositionIndex] << endl;
						temporalPositionIndex[i]=useTemporalPositionIndex+1;	// standard (should) say these start from 1
					}

					// Step 5 - clean up ...
		
					if (tableOfTimes) {
						//delete[] tableOfTimes;
					}
					// Left with per frame array temporalPositionIndex[] which is used later in building Frame Content Macro
				}
				else {
					log << "Cannot add temporal position since no times vary per frame" << endl;
					addTemporalPosition=false;
				}
			}
		}
	}

	if (!success) exit(1);

        bool useExtraPixelDataWritePass = false;
        
        bool addMultiFrameSecondaryCaptureStuff = false;
        bool addMultiFrameMRStuff = false;
        bool addMultiFrameCTStuff = false;

	// iterate through image files to extract and check attribute values ...
 
	// attributes required to have constant values for all image files ...

        Uint16 requiredRows;
        Uint16 requiredColumns;
        Uint16 requiredBitsAllocated;
        Uint16 requiredBitsStored;
        Uint16 requiredHighBit;
        Uint16 requiredPixelRepresentation;
        Uint16 requiredSamplesPerPixel;
        
	{
                const char *requiredPatientID;
                const char *requiredStudyInstanceUID;
                const char *requiredSeriesInstanceUID;
                const char *requiredSOPClassUID;
                const char *requiredPatientName;
                const char *requiredStudyID;
                Uint16 requiredSeriesNumber;
                const char *requiredModality;

                double requiredSliceThickness;

                //Uint16 requiredEchoNumbers;
                double requiredEchoTime;
                double requiredRepetitionTime;
		const char *requiredImagedNucleus;
		double requiredMagneticFieldStrength;
                const char *requiredProtocolName;
                const char *requiredSequenceName;
                const char *requiredMRAcquisitionType;

                double requiredRescaleSlope;
                double requiredRescaleIntercept;
                
		Uint32 studyIDForUIDGeneration = 0;
		Uint32 seriesNumberForUIDGeneration = 0;
		Uint32 instanceNumberForUIDGeneration = 0;

		bool encountered_EchoTime = false;
		bool encountered_RepetitionTime = false;
		bool encountered_InversionTime = false;
		bool encountered_FlipAngle = false;
		bool encountered_SliceThickness = false;
		bool encountered_PixelSpacing = false;
		bool encountered_AcquisitionNumber = false;
		bool encountered_AcquisitionDate = false;
		bool encountered_AcquisitionTime = false;
		bool encountered_AcquisitionDuration = false;
		bool encountered_ImageComments = false;
		bool encountered_ImagePositionPatient = false;
		bool encountered_ImageOrientationPatient = false;
		bool encountered_TriggerTime = false;
		bool encountered_NominalCardiacTriggerDelayTime = false;
		bool encountered_ImageType = false;
		bool encountered_ContentDate = false;
		bool encountered_ContentTime = false;
		bool encountered_ScanningSequence = false;
		bool encountered_SequenceVariant = false;
		bool encountered_ScanOptions = false;
		bool encountered_EchoNumbers = false;
		bool encountered_BodyPartExamined = false;
		bool encountered_PatientName = false;
		bool encountered_StudyDescription = false;
		bool encountered_SeriesDescription = false;
		bool encountered_Laterality = false;
		bool encountered_MRAcquisitionPhaseEncodingStepsOutOfPlane = false;
		bool encountered_InPlanePhaseEncodingDirection = false;
		bool encountered_NumberOfPhaseEncodingSteps = false;
		bool encountered_PercentSampling = false;
		bool encountered_PercentPhaseFieldOfView = false;
		bool encountered_ImagingFrequency = false;
		bool encountered_PixelBandwidth = false;
		bool encountered_ReceiveCoilName = false;
		bool encountered_TransmitCoilName = false;
		bool encountered_Manufacturer = false;
		bool encountered_NumberOfAverages = false;
		bool encountered_EchoTrainLength = false;
		bool encountered_RFEchoTrainLength = false;
		bool encountered_GradientEchoTrainLength = false;
		bool encountered_SAR = false;
		bool encountered_PixelPresentation = false;
		bool encountered_VolumetricProperties = false;
		bool encountered_VolumeBasedCalculationTechnique = false;
		bool encountered_ComplexImageComponent = false;
		bool encountered_AcquisitionContrast = false;
		bool encountered_AcquisitionMatrix = false;
 		bool encountered_WindowCenter = false;
 		bool encountered_WindowWidth = false;
		bool encountered_WindowCenterWidthExplanation = false;
 		bool encountered_DiffusionBValue = false;
 		bool encountered_DiffusionDirectionality = false;
 		bool encountered_GradientOutput = false;
 		bool encountered_GradientOutputType = false;
 		bool encountered_RescaleSlope = false;
 		bool encountered_RescaleIntercept = false;
 		bool encountered_RescaleType = false;
 		bool encountered_RealWorldValueSlope = false;
 		bool encountered_RealWorldValueIntercept = false;
 		bool encountered_Units = false;
 		bool encountered_RealWorldValueFirstValueMapped = false;
 		bool encountered_RealWorldValueLastValueMapped = false;
 		bool encountered_LUTExplanation = false;
 		bool encountered_LUTLabel = false;
 		bool encountered_VelocityEncodingDirection = false;
		bool encountered_VelocityEncodingMinimumValue = false;
		bool encountered_VelocityEncodingMaximumValue = false;
		bool encountered_MetaboliteMapDescription = false;
		bool encountered_ChemicalShiftMinimumIntegrationLimitInppm = false;
		bool encountered_ChemicalShiftMaximumIntegrationLimitInppm = false;
		bool encountered_AcquisitionType = false;
		bool encountered_TubeAngle = false;
		bool encountered_ConstantVolumeFlag = false;
		bool encountered_FluoroscopyFlag = false;
		bool encountered_RotationDirection = false;
		bool encountered_RevolutionTime = false;
		bool encountered_SingleCollimationWidth = false;
		bool encountered_TotalCollimationWidth = false;
		bool encountered_TableHeight = false;
		bool encountered_GantryDetectorTilt = false;
		bool encountered_DataCollectionDiameter = false;
		bool encountered_TableSpeed = false;
		bool encountered_TableFeedPerRotation = false;
		bool encountered_SpiralPitchFactor = false;
		bool encountered_SliceLocation = false;
		bool encountered_TablePosition = false;
		bool encountered_DataCollectionCenterPatient = false;
		bool encountered_ReconstructionTargetCenterPatient = false;
		bool encountered_DistanceSourceToDetector = false;
		bool encountered_DistanceSourceToDataCollectionCenter = false;
		bool encountered_DistanceSourceToPatient = false;
		bool encountered_ReconstructionAlgorithm = false;
		bool encountered_ConvolutionKernel = false;
		bool encountered_ConvolutionKernelGroup = false;
		bool encountered_ReconstructionDiameter = false;
		bool encountered_ReconstructionFieldOfView = false;
		bool encountered_ReconstructionPixelSpacing = false;
		bool encountered_ReconstructionAngle = false;
		bool encountered_ImageFilter = false;
		bool encountered_ExposureTime = false;
		bool encountered_XRayTubeCurrent = false;
		bool encountered_Exposure = false;
		bool encountered_ExposureInuAs = false;
		bool encountered_ExposureTimeInms = false;
		bool encountered_XRayTubeCurrentInmA = false;
		bool encountered_ExposureInmAs = false;
		bool encountered_ExposureModulationType = false;
		bool encountered_EstimatedDoseSaving = false;
		bool encountered_KVP = false;
		bool encountered_CTDIvol = false;
		bool encountered_FocalSpots = false;
		bool encountered_FilterType = false;
		bool encountered_FilterMaterial = false;
		bool encountered_PatientPosition = false;
		bool encountered_ContrastBolusAgent = false;
		bool encountered_ContrastBolusRoute = false;
		bool encountered_ContrastBolusVolume = false;
		bool encountered_ContrastBolusIngredientConcentration = false;
		bool encountered_ContrastBolusAgentAdministered = false;
		bool encountered_ContrastBolusAgentDetected = false;
		bool encountered_ContrastBolusAgentPhase = false;
		bool encountered_CardiacCyclePosition = false;
		bool encountered_RespiratoryCyclePosition = false;
		//bool encountered_TriggerWindow = false;
		bool encountered_RRIntervalTimeNominal = false;
		bool encountered_IrradiationEventUID = false;

		double shared_EchoTime = 0;
		double shared_RepetitionTime = 0;
		double shared_InversionTime = 0;
		double shared_FlipAngle = 0;
		double shared_SliceThickness = 0;
		double *shared_PixelSpacing = NULL;
		Uint32 shared_AcquisitionNumber = 0;
		char *shared_AcquisitionDate = NULL;
		char *shared_AcquisitionTime = NULL;
		double shared_AcquisitionDuration = 0;
		char *shared_ContentDate = NULL;
		char *shared_ContentTime = NULL;
		char *shared_ImageComments = NULL;
		double *shared_ImagePositionPatient = NULL;
		double *shared_ImageOrientationPatient = NULL;
		double shared_TriggerTime = 0;
		double shared_NominalCardiacTriggerDelayTime = 0;
		char **shared_ImageType = NULL;
		char **shared_ScanningSequence = NULL;
		char **shared_SequenceVariant = NULL;
		char **shared_ScanOptions = NULL;
		Uint32 shared_EchoNumbers = 0;
		char *shared_BodyPartExamined = NULL;
		char *shared_PatientName = NULL;
		char *shared_StudyDescription = NULL;
		char *shared_SeriesDescription = NULL;
		char *shared_Laterality = NULL;
		Uint32 shared_MRAcquisitionPhaseEncodingStepsOutOfPlane = 0;
		char *shared_InPlanePhaseEncodingDirection = NULL;
		Uint32 shared_NumberOfPhaseEncodingSteps = 0;
		double shared_PercentSampling = 0;
		double shared_PercentPhaseFieldOfView = 0;
		double shared_ImagingFrequency = 0;
		double shared_PixelBandwidth = 0;
		char *shared_ReceiveCoilName = NULL;
		char *shared_TransmitCoilName = NULL;
		char *shared_Manufacturer = NULL;
		double shared_NumberOfAverages = 0;
		Uint32 shared_EchoTrainLength = 0;
		Uint32 shared_RFEchoTrainLength = 0;
		Uint32 shared_GradientEchoTrainLength = 0;
		double shared_SAR = 0;
		char *shared_PixelPresentation = NULL;
		char *shared_VolumetricProperties = NULL;
		char *shared_VolumeBasedCalculationTechnique = NULL;
		char *shared_ComplexImageComponent = NULL;
		char *shared_AcquisitionContrast = NULL;
		Uint32 *shared_AcquisitionMatrix = NULL;
 		double shared_WindowCenter = 0;
 		double shared_WindowWidth = 0;
		char *shared_WindowCenterWidthExplanation = NULL;
 		double shared_DiffusionBValue = 0;
 		char *shared_DiffusionDirectionality = NULL;
 		double shared_GradientOutput = 0;
 		char *shared_GradientOutputType = NULL;
 		double shared_RescaleSlope = 0;
 		double shared_RescaleIntercept = 0;
 		char * shared_RescaleType = NULL;
 		double shared_RealWorldValueSlope = 0;
 		double shared_RealWorldValueIntercept = 0;
		char *shared_Units = NULL;
		Uint32 shared_RealWorldValueFirstValueMapped = 0;
		Uint32 shared_RealWorldValueLastValueMapped = 0;
		char *shared_LUTExplanation = NULL;
		char *shared_LUTLabel = NULL;
 		double *shared_VelocityEncodingDirection = NULL;
		double shared_VelocityEncodingMinimumValue = 0;
		double shared_VelocityEncodingMaximumValue = 0;
		char *shared_MetaboliteMapDescription = NULL;
		double shared_ChemicalShiftMinimumIntegrationLimitInppm = 0;
		double shared_ChemicalShiftMaximumIntegrationLimitInppm = 0;
 		char *shared_AcquisitionType = NULL;
		double shared_TubeAngle = 0;
 		char *shared_ConstantVolumeFlag = NULL;
 		char *shared_FluoroscopyFlag = NULL;
 		char *shared_RotationDirection = NULL;
		double shared_RevolutionTime = 0;
		double shared_SingleCollimationWidth = 0;
		double shared_TotalCollimationWidth = 0;
		double shared_TableHeight = 0;
		double shared_GantryDetectorTilt = 0;
		double shared_DataCollectionDiameter = 0;
		double shared_TableSpeed = 0;
		double shared_TableFeedPerRotation = 0;
		double shared_SpiralPitchFactor = 0;
		double shared_SliceLocation = 0;
		double shared_TablePosition = 0;
		double *shared_DataCollectionCenterPatient = NULL;
		double *shared_ReconstructionTargetCenterPatient = NULL;
		double shared_DistanceSourceToDetector = 0;
		double shared_DistanceSourceToDataCollectionCenter = 0;
		double shared_DistanceSourceToPatient = 0;
 		char *shared_ReconstructionAlgorithm = NULL;
 		char *shared_ConvolutionKernel = NULL;
 		char *shared_ConvolutionKernelGroup = NULL;
		double shared_ReconstructionDiameter = 0;
		double *shared_ReconstructionFieldOfView = NULL;
		double *shared_ReconstructionPixelSpacing = NULL;
		double shared_ReconstructionAngle = 0;
 		char *shared_ImageFilter = NULL;
		Uint32 shared_ExposureTime = 0;
		Uint32 shared_XRayTubeCurrent = 0;
		Uint32 shared_Exposure = 0;
		Uint32 shared_ExposureInuAs = 0;
		double shared_ExposureTimeInms = 0;
		double shared_XRayTubeCurrentInmA = 0;
		double shared_ExposureInmAs = 0;
 		char *shared_ExposureModulationType = NULL;
		double shared_EstimatedDoseSaving = 0;
		double shared_KVP = 0;
		double shared_CTDIvol = 0;
		double shared_FocalSpots = 0;
 		char *shared_FilterType = NULL;
 		char *shared_FilterMaterial = NULL;
		char *shared_PatientPosition = NULL;
		char **shared_ContrastBolusAgent = NULL;
		char **shared_ContrastBolusRoute = NULL;
		char **shared_ContrastBolusVolume = NULL;
		char **shared_ContrastBolusIngredientConcentration = NULL;
		char **shared_ContrastBolusAgentAdministered = NULL;
		char **shared_ContrastBolusAgentDetected = NULL;
		char **shared_ContrastBolusAgentPhase = NULL;
		char *shared_CardiacCyclePosition = NULL;
		char *shared_RespiratoryCyclePosition = NULL;
		//double shared_TriggerWindow = 0;
		double shared_RRIntervalTimeNominal = 0;
		char *shared_IrradiationEventUID = NULL;

		double *perFrame_EchoTime = NULL;
		double *perFrame_RepetitionTime = NULL;
		double *perFrame_InversionTime = NULL;
		double *perFrame_FlipAngle = NULL;
		double *perFrame_SliceThickness = NULL;
		double **perFrame_PixelSpacing = NULL;
		Uint32 *perFrame_AcquisitionNumber = NULL;
		char **perFrame_AcquisitionDate = NULL;
		char **perFrame_AcquisitionTime = NULL;
		double *perFrame_AcquisitionDuration = NULL;
		char **perFrame_ContentDate = NULL;
		char **perFrame_ContentTime = NULL;
		char **perFrame_ImageComments = NULL;
		double **perFrame_ImagePositionPatient = NULL;
		double **perFrame_ImageOrientationPatient = NULL;
		double *perFrame_TriggerTime = NULL;
		double *perFrame_NominalCardiacTriggerDelayTime = NULL;
		char ***perFrame_ImageType = NULL;
		char ***perFrame_ScanningSequence = NULL;
		char ***perFrame_SequenceVariant = NULL;
		char ***perFrame_ScanOptions = NULL;
		Uint32 *perFrame_EchoNumbers = NULL;
		char **perFrame_BodyPartExamined = NULL;
		char **perFrame_PatientName = NULL;
		char **perFrame_StudyDescription = NULL;
		char **perFrame_SeriesDescription = NULL;
		char **perFrame_Laterality = NULL;
		Uint32 *perFrame_MRAcquisitionPhaseEncodingStepsOutOfPlane = NULL;
		char **perFrame_InPlanePhaseEncodingDirection = NULL;
		Uint32 *perFrame_NumberOfPhaseEncodingSteps = NULL;
		double *perFrame_PercentSampling = NULL;
		double *perFrame_PercentPhaseFieldOfView = NULL;
		double *perFrame_ImagingFrequency = NULL;
		double *perFrame_PixelBandwidth = NULL;
		char **perFrame_ReceiveCoilName = NULL;
		char **perFrame_TransmitCoilName = NULL;
		char **perFrame_Manufacturer = NULL;
		double *perFrame_NumberOfAverages = NULL;
		Uint32 *perFrame_EchoTrainLength = NULL;
		Uint32 *perFrame_RFEchoTrainLength = NULL;
		Uint32 *perFrame_GradientEchoTrainLength = NULL;
		double *perFrame_SAR = NULL;
		char **perFrame_PixelPresentation = NULL;
		char **perFrame_VolumetricProperties = NULL;
		char **perFrame_VolumeBasedCalculationTechnique = NULL;
		char **perFrame_ComplexImageComponent = NULL;
		char **perFrame_AcquisitionContrast = NULL;
		Uint32 **perFrame_AcquisitionMatrix = NULL;
 		double *perFrame_WindowCenter = NULL;
 		double *perFrame_WindowWidth = NULL;
		char **perFrame_WindowCenterWidthExplanation = NULL;
 		double *perFrame_DiffusionBValue = NULL;
 		char **perFrame_DiffusionDirectionality = NULL;
 		double *perFrame_GradientOutput = NULL;
 		char **perFrame_GradientOutputType = NULL;
 		double *perFrame_RescaleSlope = NULL;
 		double *perFrame_RescaleIntercept = NULL;
 		char **perFrame_RescaleType = NULL;
 		double *perFrame_RealWorldValueSlope = NULL;
 		double *perFrame_RealWorldValueIntercept = NULL;
		char **perFrame_Units = NULL;
		Uint32 *perFrame_RealWorldValueFirstValueMapped = NULL;
		Uint32 *perFrame_RealWorldValueLastValueMapped = NULL;
		char **perFrame_LUTExplanation = NULL;
		char **perFrame_LUTLabel = NULL;
 		double **perFrame_VelocityEncodingDirection = NULL;
		double *perFrame_VelocityEncodingMinimumValue = NULL;
		double *perFrame_VelocityEncodingMaximumValue = NULL;
		char **perFrame_MetaboliteMapDescription = NULL;
		double *perFrame_ChemicalShiftMinimumIntegrationLimitInppm = NULL;
		double *perFrame_ChemicalShiftMaximumIntegrationLimitInppm = NULL;
		char **perFrame_AcquisitionType = NULL;
		double *perFrame_TubeAngle = NULL;
		char **perFrame_ConstantVolumeFlag = NULL;
		char **perFrame_FluoroscopyFlag = NULL;
		char **perFrame_RotationDirection = NULL;
		double *perFrame_RevolutionTime = NULL;
		double *perFrame_SingleCollimationWidth = NULL;
		double *perFrame_TotalCollimationWidth = NULL;
		double *perFrame_TableHeight = NULL;
		double *perFrame_GantryDetectorTilt = NULL;
		double *perFrame_DataCollectionDiameter = NULL;
		double *perFrame_TableSpeed = NULL;
		double *perFrame_TableFeedPerRotation = NULL;
		double *perFrame_SpiralPitchFactor = NULL;
		double *perFrame_SliceLocation = NULL;
		double *perFrame_TablePosition = NULL;
		double **perFrame_DataCollectionCenterPatient = NULL;
		double **perFrame_ReconstructionTargetCenterPatient = NULL;
		double *perFrame_DistanceSourceToDetector = NULL;
		double *perFrame_DistanceSourceToDataCollectionCenter = NULL;
		double *perFrame_DistanceSourceToPatient = NULL;
		char **perFrame_ReconstructionAlgorithm = NULL;
		char **perFrame_ConvolutionKernel = NULL;
		char **perFrame_ConvolutionKernelGroup = NULL;
		double *perFrame_ReconstructionDiameter = NULL;
		double **perFrame_ReconstructionFieldOfView = NULL;
		double **perFrame_ReconstructionPixelSpacing = NULL;
		double *perFrame_ReconstructionAngle = NULL;
		char **perFrame_ImageFilter = NULL;
		Uint32 *perFrame_ExposureTime = NULL;
		Uint32 *perFrame_XRayTubeCurrent = NULL;
		Uint32 *perFrame_Exposure = NULL;
		Uint32 *perFrame_ExposureInuAs = NULL;
		double *perFrame_ExposureTimeInms = NULL;
		double *perFrame_XRayTubeCurrentInmA = NULL;
		double *perFrame_ExposureInmAs = NULL;
		char **perFrame_ExposureModulationType = NULL;
		double *perFrame_EstimatedDoseSaving = NULL;
		double *perFrame_KVP = NULL;
		double *perFrame_CTDIvol = NULL;
		double *perFrame_FocalSpots = NULL;
		char **perFrame_FilterType = NULL;
		char **perFrame_FilterMaterial = NULL;
		char **perFrame_PatientPosition = NULL;
		char ***perFrame_ContrastBolusAgent = NULL;
		char ***perFrame_ContrastBolusRoute = NULL;
		char ***perFrame_ContrastBolusVolume = NULL;
		char ***perFrame_ContrastBolusIngredientConcentration = NULL;
		char ***perFrame_ContrastBolusAgentAdministered = NULL;
		char ***perFrame_ContrastBolusAgentDetected = NULL;
		char ***perFrame_ContrastBolusAgentPhase = NULL;
		char **perFrame_CardiacCyclePosition = NULL;
		char **perFrame_RespiratoryCyclePosition = NULL;
		//double *perFrame_TriggerWindow = NULL;
		double *perFrame_RRIntervalTimeNominal = NULL;
		char **perFrame_IrradiationEventUID = NULL;

		int nValues_ImageType = 0;
		int nValues_ScanningSequence = 0;
		int nValues_SequenceVariant = 0;
		int nValues_ScanOptions = 0;
		int nValues_ContrastBolusAgent = 0;
		int nValues_ContrastBolusRoute = 0;
		int nValues_ContrastBolusVolume = 0;
		int nValues_ContrastBolusIngredientConcentration = 0;
		int nValues_ContrastBolusAgentAdministered = 0;
		int nValues_ContrastBolusAgentDetected = 0;
		int nValues_ContrastBolusAgentPhase = 0;

                ManagedAttributeList newList;

                int i;
                for (i=0; i < numberofinputfiles; ++i) {
//cerr << "Frame " << i << " tracking ... " << endl;				

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

                        const char *patientID=getAndCheckStringMatchElseError(i == 0,requiredPatientID,list,
                                TagFromName(PatientID),"Patient ID",filename,log);

                        const char *patientName=getAndCheckStringMatchElseError(i == 0,requiredPatientName,list,
                                TagFromName(PatientName),"Patient Name",filename,log);

                        const char *studyInstanceUID=getAndCheckStringMatchElseError(i == 0,requiredStudyInstanceUID,list,
                                TagFromName(StudyInstanceUID),"Study Instance UID",filename,log);

                        const char *studyID=getAndCheckStringMatchElseError(i == 0,requiredStudyID,list,
                                TagFromName(StudyID),"Study ID",filename,log);
				
			if (i == 0) {
				Attribute *aStudyID = dicom_output_options.replacebeforelist ? (*dicom_output_options.replacebeforelist)[TagFromName(StudyID)] : NULL;
				const char *vStudyIDForUIDGeneration = aStudyID ? AttributeValue(aStudyID,"0") : requiredStudyID /*what is already in the dataset*/;
//cerr << "(*dicom_output_options.replacebeforelist)[TagFromName(StudyID)] = " << ((*dicom_output_options.replacebeforelist)[TagFromName(StudyID)]) << endl;
//cerr << "vStudyIDForUIDGeneration = " << vStudyIDForUIDGeneration << endl;
//cerr << "vStudyIDForUIDGeneration strtol = " << strtol(vStudyIDForUIDGeneration,NULL,0) << endl;
//cerr << "vStudyIDForUIDGeneration atoi = " << atoi(vStudyIDForUIDGeneration) << endl;
				studyIDForUIDGeneration = vStudyIDForUIDGeneration ? atoi(vStudyIDForUIDGeneration) : 0;
			}

                        const char *seriesInstanceUID=getAndCheckStringMatchElseError(i == 0,requiredSeriesInstanceUID,list,
                                TagFromName(SeriesInstanceUID),"Series Instance UID",filename,log);

                        const char *SOPClassUID=getAndCheckStringMatchElseError(i == 0,requiredSOPClassUID,list,
                                TagFromName(SOPClassUID),"SOP Class UID",filename,log);

                        Uint16 seriesNumber=getAndCheckIntegerMatchElseError(i == 0,requiredSeriesNumber,list,
                                TagFromName(SeriesNumber),"Series Number",filename,log);

			if (i == 0) {
				Attribute *aSeriesNumber = dicom_output_options.replacebeforelist ? (*dicom_output_options.replacebeforelist)[TagFromName(SeriesNumber)] : NULL;
				seriesNumberForUIDGeneration = aSeriesNumber ? Uint32(AttributeValue(aSeriesNumber,"0")) : requiredSeriesNumber /*what is already in the dataset*/;
			}
			
			{
				Attribute *aInstanceNumber = dicom_output_options.replacebeforelist ? (*dicom_output_options.replacebeforelist)[TagFromName(InstanceNumber)] : NULL;
				if (!aInstanceNumber) {
					aInstanceNumber=list[TagFromName(InstanceNumber)];	/*what is already in the dataset*/
				}
				instanceNumberForUIDGeneration = aInstanceNumber ? Uint32(AttributeValue(aInstanceNumber,"0")) : 0 ;
			}

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
                                //Uint16 echoNumber=getAndCheckIntegerMatchElseError(i == 0,requiredEchoNumbers,list,
                                //	TagFromName(EchoNumbers),"Echo Number",filename,log);
                                double echoTime=getAndCheckFloatMatchElseError(i == 0,requiredEchoTime,list,
                                        TagFromName(EchoTime),"Echo Time",filename,log);
                                double repetitionTime=getAndCheckFloatMatchElseError(i == 0,requiredRepetitionTime,list,
                                        TagFromName(RepetitionTime),"Repetition Time",filename,log);
                                double magneticFieldStrength=getAndCheckFloatMatchElseError(i == 0,requiredMagneticFieldStrength,list,
                                        TagFromName(MagneticFieldStrength),"Magnetic Field Strength",filename,log);
				const char *imagedNucleus=getAndCheckStringMatchElseError(i == 0,requiredImagedNucleus,list,
					TagFromName(ImagedNucleus),"Imaged Nucleus",filename,log);
				const char *protocolName=getAndCheckStringMatchElseError(i == 0,requiredProtocolName,list,
					TagFromName(ProtocolName),"Protocol Name",filename,log);
				const char *sequenceName=getAndCheckStringMatchElseError(i == 0,requiredSequenceName,list,
					TagFromName(SequenceName),"Sequence Name",filename,log);
				const char *MRAcquisitionType=getAndCheckStringMatchElseError(i == 0,requiredMRAcquisitionType,list,
					TagFromName(MRAcquisitionType),"MR Acquisition Type",filename,log);

                        }

                        if (strcmp(requiredSOPClassUID,CTImageStorageSOPClassUID) == 0) {
                                double rescaleSlope=getAndCheckFloatMatchElseError(i == 0,requiredRescaleSlope,list,
                                        TagFromName(RescaleSlope),"Rescale Slope",filename,log);
                                double rescaleIntercept=getAndCheckFloatMatchElseError(i == 0,requiredRescaleIntercept,list,
                                        TagFromName(RescaleIntercept),"Rescale Intercept",filename,log);
                        }

                        // should also check PixelSpacing, ImageOrientationPatient ...

                        // Get pixel data attribute for later use ...

                        Attribute *apixeldata=list[TagFromName(PixelData)];
                        if (!apixeldata) {
                                log << EMsgDC(MissingAttribute) << " - \"PixelData\"" << endl;
                                success=false;
                                break;
                        }

                        // for the first file, copy everything except pixels, and add frame stuff ...

                        if (i == 0) {
                                // Check and add frame stuff ...

                                Attribute *aNumberOfFrames=list[TagFromName(NumberOfFrames)];
                                if (aNumberOfFrames) {
                                        Uint16 vNumberOfFrames = AttributeValue(aNumberOfFrames);
                                        if (vNumberOfFrames > 1) {
                                                log << "Number Of Frames found in <" << filename
                                                    << "> and greater than 1 - input images cannot be multiframe - aborting" << endl;
                                                success=false;
                                                break;
                                        }
                                        list-=TagFromName(NumberOfFrames);
                                }
                                list+=new IntegerStringAttribute(TagFromName(NumberOfFrames),Uint16(numberofinputfiles));

                                // delete old Instance Number and add new one with fixed value of 1 ...

                                list-=TagFromName(InstanceNumber);
                                list+=new IntegerStringAttribute(TagFromName(InstanceNumber),Uint16(1));

                                // delete old SOP Instance UID ... usualManagedAttributeListWrite() adds new one ...

                                list-=TagFromName(SOPInstanceUID);

                                // check for single-frame SOP classes which have multi-frame equivalent, else leave alone ...

                                const char *newSOPClassUID = 0;
                                if (strcmp(SOPClassUID,MRImageStorageSOPClassUID) == 0) {
                                        if (!noMRMultiFrame) {
						newSOPClassUID=EnhancedMRImageStorageSOPClassUID;
						addMultiFrameMRStuff=true;
						useExtraPixelDataWritePass=true;
					}
                                }
                                else if (strcmp(SOPClassUID,CTImageStorageSOPClassUID) == 0) {
                                        if (!noCTMultiFrame) {
						newSOPClassUID=EnhancedCTImageStorageSOPClassUID;
						addMultiFrameCTStuff=true;
						useExtraPixelDataWritePass=true;
					}
                                }
                                else if (strcmp(SOPClassUID,SecondaryCaptureImageStorageSOPClassUID) == 0) {
                                        const char *photometricInterpretation=getStringValueElseError(list,TagFromName(PhotometricInterpretation),"Photometric Interpretation",filename,log);
                                        if (photometricInterpretation) {
                                                if (strcmp(photometricInterpretation,"RGB") == 0) {
                                                        if (samplesPerPixel == 3 && bitsAllocated == 8 && bitsStored == 8 && highBit == 7 && pixelRepresentation == 0) {	// should check PlanarConfiguration also
                                                                newSOPClassUID=MultiframeTrueColorSecondaryCaptureImageStorageSOPClassUID;
                                                        }
                                                }
                                                else if (strcmp(photometricInterpretation,"MONOCHROME2") == 0) {
                                                        if (samplesPerPixel == 1 && bitsAllocated == 1 && bitsStored == 1 && highBit == 0 && pixelRepresentation == 0) {
                                                                newSOPClassUID=MultiframeSingleBitSecondaryCaptureImageStorageSOPClassUID;
                                                                addMultiFrameSecondaryCaptureStuff=true;
                                                        }
                                                        else if (samplesPerPixel == 1 && bitsAllocated == 8 && bitsStored == 8 && highBit == 7 && pixelRepresentation == 0) {
                                                                newSOPClassUID=MultiframeGrayscaleByteSecondaryCaptureImageStorageSOPClassUID;
                                                                addMultiFrameSecondaryCaptureStuff=true;
                                                        }
                                                        else if (samplesPerPixel == 1 && bitsAllocated == 16 && bitsStored > 8 && bitsStored <= 16 && highBit == (bitsStored-1) && pixelRepresentation == 0) {
                                                                newSOPClassUID=MultiframeGrayscaleWordSecondaryCaptureImageStorageSOPClassUID;
                                                                addMultiFrameSecondaryCaptureStuff=true;
                                                        }
                                                }
                                        }
                                }
                                if (newSOPClassUID) {
                                        list-=TagFromName(SOPClassUID);
                                        list+=new UIStringAttribute(TagFromName(SOPClassUID),newSOPClassUID);
                                }
                                if (addMultiFrameSecondaryCaptureStuff) {
                                        list-=TagFromName(PresentationLUTShape);
                                        list+=new CodeStringAttribute(TagFromName(PresentationLUTShape),"IDENTITY");

                                        if (numberofinputfiles > 1) {
                                                AttributeTagAttribute *aFrameIncrementPointer = new AttributeTagAttribute(TagFromName(FrameIncrementPointer));
                                                aFrameIncrementPointer->addValue(TagFromName(DisplayWindowLabelVector));
                                                list+=aFrameIncrementPointer;

                                                ShortStringAttribute *aDisplayWindowLabelVector = new ShortStringAttribute(TagFromName(DisplayWindowLabelVector));
                                                for (int i=1; i<=numberofinputfiles; ++i) aDisplayWindowLabelVector->addValue((Uint16)i);
                                                list+=aDisplayWindowLabelVector;
                                        }
                                }
                        }
                        
			// capture stuff for each frame before list goes away ...
                        
                        if (addMultiFrameMRStuff || addMultiFrameCTStuff) {
//cerr << "Frame " << i << " tracking ... addMultiFrameMRStuff and addMultiFrameCTStuff" << endl;				
                                // For every frame track the following ...
                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(SliceThickness),"Slice Thickness",
                                        encountered_SliceThickness,shared_SliceThickness,perFrame_SliceThickness,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedDoubleAttributePerFrame(list,TagFromName(PixelSpacing),"Pixel Spacing",2,
                                        encountered_PixelSpacing,shared_PixelSpacing,perFrame_PixelSpacing,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedUint32AttributePerFrame(list,TagFromName(AcquisitionNumber),"Acquisition Number",
                                        encountered_AcquisitionNumber,shared_AcquisitionNumber,perFrame_AcquisitionNumber,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(AcquisitionDate),"Acquisition Date",
                                        encountered_AcquisitionDate,shared_AcquisitionDate,perFrame_AcquisitionDate,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(AcquisitionTime),"Acquisition Time",
                                        encountered_AcquisitionTime,shared_AcquisitionTime,perFrame_AcquisitionTime,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(AcquisitionDuration),"Acquisition Duration",
                                        encountered_AcquisitionDuration,shared_AcquisitionDuration,perFrame_AcquisitionDuration,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(ContentDate),"Content Date",
                                        encountered_ContentDate,shared_ContentDate,perFrame_ContentDate,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(ContentTime),"Content Time",
                                        encountered_ContentTime,shared_ContentTime,perFrame_ContentTime,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(ImageComments),"Image Comments",
                                        encountered_ImageComments,shared_ImageComments,perFrame_ImageComments,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedDoubleAttributePerFrame(list,TagFromName(ImagePositionPatient),"Image Position (Patient)",3,
                                        encountered_ImagePositionPatient,shared_ImagePositionPatient,perFrame_ImagePositionPatient,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedDoubleAttributePerFrame(list,TagFromName(ImageOrientationPatient),"Image Orientation (Patient)",6,
                                        encountered_ImageOrientationPatient,shared_ImageOrientationPatient,perFrame_ImageOrientationPatient,
                                        numberofinputfiles,i,filename,log);
                                        
                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(TriggerTime),"Trigger Time",
                                        encountered_TriggerTime,shared_TriggerTime,perFrame_TriggerTime,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(NominalCardiacTriggerDelayTime),"Nominal Cardiac Trigger Delay Time",
                                        encountered_NominalCardiacTriggerDelayTime,shared_NominalCardiacTriggerDelayTime,perFrame_NominalCardiacTriggerDelayTime,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ImageType),"Image Type",4,nValues_ImageType,
                                        encountered_ImageType,shared_ImageType,perFrame_ImageType,
                                        numberofinputfiles,i,filename,log);
					
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(BodyPartExamined),"Body Part Examined",
                                        encountered_BodyPartExamined,shared_BodyPartExamined,perFrame_BodyPartExamined,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(PatientName),"Patient Name",
                                        encountered_PatientName,shared_PatientName,perFrame_PatientName,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(StudyDescription),"Study Description",
                                        encountered_StudyDescription,shared_StudyDescription,perFrame_StudyDescription,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(SeriesDescription),"Series Description",
                                        encountered_SeriesDescription,shared_SeriesDescription,perFrame_SeriesDescription,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(Laterality),"Laterality",
                                        encountered_Laterality,shared_Laterality,perFrame_Laterality,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(Manufacturer),"Manufacturer",
                                        encountered_Manufacturer,shared_Manufacturer,perFrame_Manufacturer,
                                        numberofinputfiles,i,filename,log);
				
				// The window values are actually potentially multi-valued; we just want to track the first one
				// though we could do all of them :(
				
				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(WindowCenter),"Window Center",
                                        encountered_WindowCenter,shared_WindowCenter,perFrame_WindowCenter,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(WindowWidth),"Window Width",
                                        encountered_WindowWidth,shared_WindowWidth,perFrame_WindowWidth,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(WindowCenterWidthExplanation),"Window Center Width Explanation",
                                        encountered_WindowCenterWidthExplanation,shared_WindowCenterWidthExplanation,perFrame_WindowCenterWidthExplanation,
                                        numberofinputfiles,i,filename,log);
					
				// these aren't in the original single frame CT or MR object, but may be manually inserted into source
				// images to be able to build a correct CT or MR multi-frame ...
				
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(PixelPresentation),"Pixel Presentation",
					encountered_PixelPresentation,shared_PixelPresentation,perFrame_PixelPresentation,
                                        numberofinputfiles,i,filename,log);
					
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(VolumetricProperties),"Volumetric Properties",
					encountered_VolumetricProperties,shared_VolumetricProperties,perFrame_VolumetricProperties,
                                        numberofinputfiles,i,filename,log);
					
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(VolumeBasedCalculationTechnique),"Volume Based Calculation Technique",
					encountered_VolumeBasedCalculationTechnique,shared_VolumeBasedCalculationTechnique,perFrame_VolumeBasedCalculationTechnique,
                                        numberofinputfiles,i,filename,log);
					
				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(RescaleSlope),"Rescale Slope",
                                        encountered_RescaleSlope,shared_RescaleSlope,perFrame_RescaleSlope,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(RescaleIntercept),"Rescale Intercept",
                                        encountered_RescaleIntercept,shared_RescaleIntercept,perFrame_RescaleIntercept,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(RescaleType),"Rescale Type",
 					encountered_RescaleType,shared_RescaleType,perFrame_RescaleType,
					numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(RealWorldValueSlope),"Real World Value Slope",
                                        encountered_RealWorldValueSlope,shared_RealWorldValueSlope,perFrame_RealWorldValueSlope,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(RealWorldValueIntercept),"Real World Value Intercept",
                                        encountered_RealWorldValueIntercept,shared_RealWorldValueIntercept,perFrame_RealWorldValueIntercept,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(Units),"Units",
 					encountered_Units,shared_Units,perFrame_Units,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedUint32AttributePerFrame(list,TagFromName(RealWorldValueFirstValueMapped),"Real World Value First Value Mapped",
                                        encountered_RealWorldValueFirstValueMapped,shared_RealWorldValueFirstValueMapped,perFrame_RealWorldValueFirstValueMapped,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedUint32AttributePerFrame(list,TagFromName(RealWorldValueLastValueMapped),"Real World Value Last Value Mapped",
                                        encountered_RealWorldValueLastValueMapped,shared_RealWorldValueLastValueMapped,perFrame_RealWorldValueLastValueMapped,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(LUTExplanation),"LUT Explanation",
 					encountered_LUTExplanation,shared_LUTExplanation,perFrame_LUTExplanation,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(LUTLabel),"LUT Label",
 					encountered_LUTLabel,shared_LUTLabel,perFrame_LUTLabel,
					numberofinputfiles,i,filename,log);

                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ContrastBolusAgent),"Contrast/Bolus Agent",10,nValues_ContrastBolusAgent,
                                        encountered_ContrastBolusAgent,shared_ContrastBolusAgent,perFrame_ContrastBolusAgent,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ContrastBolusRoute),"Contrast/Bolus Route",10,nValues_ContrastBolusRoute,
                                        encountered_ContrastBolusRoute,shared_ContrastBolusRoute,perFrame_ContrastBolusRoute,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ContrastBolusVolume),"Contrast/Bolus Volume",10,nValues_ContrastBolusVolume,
                                        encountered_ContrastBolusVolume,shared_ContrastBolusVolume,perFrame_ContrastBolusVolume,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ContrastBolusIngredientConcentration),"Contrast/Bolus Ingredient Concentration",
					10,nValues_ContrastBolusIngredientConcentration,
                                        encountered_ContrastBolusIngredientConcentration,shared_ContrastBolusIngredientConcentration,perFrame_ContrastBolusIngredientConcentration,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ContrastBolusAgentAdministered),"Contrast/Bolus Administered",10,nValues_ContrastBolusAgentAdministered,
                                        encountered_ContrastBolusAgentAdministered,shared_ContrastBolusAgentAdministered,perFrame_ContrastBolusAgentAdministered,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ContrastBolusAgentDetected),"Contrast/Bolus Detected",10,nValues_ContrastBolusAgentDetected,
                                        encountered_ContrastBolusAgentDetected,shared_ContrastBolusAgentDetected,perFrame_ContrastBolusAgentDetected,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ContrastBolusAgentPhase),"Contrast/Bolus Phase",10,nValues_ContrastBolusAgentPhase,
                                        encountered_ContrastBolusAgentPhase,shared_ContrastBolusAgentPhase,perFrame_ContrastBolusAgentPhase,
                                        numberofinputfiles,i,filename,log);
					
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(CardiacCyclePosition),"Cardiac Cycle Position",
 					encountered_CardiacCyclePosition,shared_CardiacCyclePosition,perFrame_CardiacCyclePosition,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(RespiratoryCyclePosition),"Respiratory Cycle Position",
 					encountered_RespiratoryCyclePosition,shared_RespiratoryCyclePosition,perFrame_RespiratoryCyclePosition,
					numberofinputfiles,i,filename,log);
					
                                //trackSingleValuedDoubleAttributePerFrame(list,TagFromName(TriggerWindow),"Trigger Window",
                                //        encountered_TriggerWindow,shared_TriggerWindow,perFrame_TriggerWindow,
                                //        numberofinputfiles,i,filename,log);
										
                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(RRIntervalTimeNominal),"R-R Interval Time Nominal",
                                        encountered_RRIntervalTimeNominal,shared_RRIntervalTimeNominal,perFrame_RRIntervalTimeNominal,
                                        numberofinputfiles,i,filename,log);
								
                               trackSingleValuedStringAttributePerFrame(list,TagFromName(IrradiationEventUID),"Irradiation Event UID",
 					encountered_IrradiationEventUID,shared_IrradiationEventUID,perFrame_IrradiationEventUID,
					numberofinputfiles,i,filename,log);
			}
                        if (addMultiFrameCTStuff) {
//cerr << "Frame " << i << " tracking ... addMultiFrameCTStuff" << endl;				
                                // For every frame track the following ...

				trackSingleValuedStringAttributePerFrame(list,TagFromName(AcquisitionType),"Acquisition Type",
					encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(TubeAngle),"Tube Angle",
                                        encountered_TubeAngle,shared_TubeAngle,perFrame_TubeAngle,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(ConstantVolumeFlag),"Constant Volume Flag",
					encountered_ConstantVolumeFlag,shared_ConstantVolumeFlag,perFrame_ConstantVolumeFlag,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(FluoroscopyFlag),"Fluoroscopy Flag",
					encountered_FluoroscopyFlag,shared_FluoroscopyFlag,perFrame_FluoroscopyFlag,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(RotationDirection),"Rotation Direction",
					encountered_RotationDirection,shared_RotationDirection,perFrame_RotationDirection,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(RevolutionTime),"Revolution Time",
                                        encountered_RevolutionTime,shared_RevolutionTime,perFrame_RevolutionTime,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(SingleCollimationWidth),"Single Collimation Width",
                                        encountered_SingleCollimationWidth,shared_SingleCollimationWidth,perFrame_SingleCollimationWidth,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(TotalCollimationWidth),"Total Collimation Width",
                                        encountered_TotalCollimationWidth,shared_TotalCollimationWidth,perFrame_TotalCollimationWidth,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(TableHeight),"Table Height",
                                        encountered_TableHeight,shared_TableHeight,perFrame_TableHeight,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(GantryDetectorTilt),"Gantry/Detector Tilt",
					encountered_GantryDetectorTilt,shared_GantryDetectorTilt,perFrame_GantryDetectorTilt,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(DataCollectionDiameter),"Data Collection Diameter",
					encountered_DataCollectionDiameter,shared_DataCollectionDiameter,perFrame_DataCollectionDiameter,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(TableSpeed),"Table Speed",
					encountered_TableSpeed,shared_TableSpeed,perFrame_TableSpeed,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(TableFeedPerRotation),"Table Feed per Rotation",
					encountered_TableFeedPerRotation,shared_TableFeedPerRotation,perFrame_TableFeedPerRotation,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(SpiralPitchFactor),"Spiral Pitch Factor",
					encountered_SpiralPitchFactor,shared_SpiralPitchFactor,perFrame_SpiralPitchFactor,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(SliceLocation),"Slice Location",
					encountered_SliceLocation,shared_SliceLocation,perFrame_SliceLocation,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(TablePosition),"Table Position",
					encountered_TablePosition,shared_TablePosition,perFrame_TablePosition,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedDoubleAttributePerFrame(list,TagFromName(DataCollectionCenterPatient),"Data Collection Center (Patient)",3,
                                        encountered_DataCollectionCenterPatient,shared_DataCollectionCenterPatient,perFrame_DataCollectionCenterPatient,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedDoubleAttributePerFrame(list,TagFromName(ReconstructionTargetCenterPatient),"Reconstruction Target Center (Patient)",3,
                                        encountered_ReconstructionTargetCenterPatient,shared_ReconstructionTargetCenterPatient,perFrame_ReconstructionTargetCenterPatient,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(DistanceSourceToDetector),"Distance Source to Detector",
                                        encountered_DistanceSourceToDetector,shared_DistanceSourceToDetector,perFrame_DistanceSourceToDetector,
					numberofinputfiles,i,filename,log);

                                 trackSingleValuedDoubleAttributePerFrame(list,TagFromName(DistanceSourceToDataCollectionCenter),"Distance Source to Data Collection Center",
                                       encountered_DistanceSourceToDataCollectionCenter,shared_DistanceSourceToDataCollectionCenter,perFrame_DistanceSourceToDataCollectionCenter,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(DistanceSourceToPatient),"Distance Source to Patient",
                                        encountered_DistanceSourceToPatient,shared_DistanceSourceToPatient,perFrame_DistanceSourceToPatient,
					numberofinputfiles,i,filename,log);

 				trackSingleValuedStringAttributePerFrame(list,TagFromName(ReconstructionAlgorithm),"Reconstruction Algorithm",
                                       encountered_ReconstructionAlgorithm,shared_ReconstructionAlgorithm,perFrame_ReconstructionAlgorithm,
					numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(ConvolutionKernel),"Convolution Kernel",
                                        encountered_ConvolutionKernel,shared_ConvolutionKernel,perFrame_ConvolutionKernel,
					numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(ConvolutionKernelGroup),"Convolution Kernel Group",
                                        encountered_ConvolutionKernelGroup,shared_ConvolutionKernelGroup,perFrame_ConvolutionKernelGroup,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(ReconstructionDiameter),"Reconstruction Diameter",
                                        encountered_ReconstructionDiameter,shared_ReconstructionDiameter,perFrame_ReconstructionDiameter,
					numberofinputfiles,i,filename,log);

                                trackMultiValuedDoubleAttributePerFrame(list,TagFromName(ReconstructionFieldOfView),"Reconstruction Field of View",2,
                                        encountered_ReconstructionFieldOfView,shared_ReconstructionFieldOfView,perFrame_ReconstructionFieldOfView,
					numberofinputfiles,i,filename,log);

                                trackMultiValuedDoubleAttributePerFrame(list,TagFromName(ReconstructionPixelSpacing),"Reconstruction Pixel Spacing",2,
                                        encountered_ReconstructionPixelSpacing,shared_ReconstructionPixelSpacing,perFrame_ReconstructionPixelSpacing,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(ReconstructionAngle),"Reconstruction Angle",
                                        encountered_ReconstructionAngle,shared_ReconstructionAngle,perFrame_ReconstructionAngle,
					numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(ImageFilter),"Image Filter",
                                        encountered_ImageFilter,shared_ImageFilter,perFrame_ImageFilter,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedUint32AttributePerFrame(list,TagFromName(ExposureTime),"Exposure Time",
                                        encountered_ExposureTime,shared_ExposureTime,perFrame_ExposureTime,
					numberofinputfiles,i,filename,log);
					
                                trackSingleValuedUint32AttributePerFrame(list,TagFromName(XRayTubeCurrent),"XRay Tube Current",
                                        encountered_XRayTubeCurrent,shared_XRayTubeCurrent,perFrame_XRayTubeCurrent,
					numberofinputfiles,i,filename,log);
					
                                trackSingleValuedUint32AttributePerFrame(list,TagFromName(Exposure),"Exposure",
                                        encountered_Exposure,shared_Exposure,perFrame_Exposure,
					numberofinputfiles,i,filename,log);
					
                                trackSingleValuedUint32AttributePerFrame(list,TagFromName(ExposureInuAs),"Exposure in uAs",
                                        encountered_ExposureInuAs,shared_ExposureInuAs,perFrame_ExposureInuAs,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(ExposureTimeInms),"Exposure Time in ms",
                                        encountered_ExposureTimeInms,shared_ExposureTimeInms,perFrame_ExposureTimeInms,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(XRayTubeCurrentInmA),"XRay Tube Current in mA",
                                        encountered_XRayTubeCurrentInmA,shared_XRayTubeCurrentInmA,perFrame_XRayTubeCurrentInmA,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(ExposureInmAs),"Exposure in mAs",
                                        encountered_ExposureInmAs,shared_ExposureInmAs,perFrame_ExposureInmAs,
					numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(ExposureModulationType),"Exposure Modulation Type",
                                        encountered_ExposureModulationType,shared_ExposureModulationType,perFrame_ExposureModulationType,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(EstimatedDoseSaving),"Estimated Dose Saving",
                                        encountered_EstimatedDoseSaving,shared_EstimatedDoseSaving,perFrame_EstimatedDoseSaving,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(KVP),"KVP",
                                        encountered_KVP,shared_KVP,perFrame_KVP,
					numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(FocalSpots),"Focal Spot(s)",
                                        encountered_FocalSpots,shared_FocalSpots,perFrame_FocalSpots,
					numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(FilterType),"Filter Type",
                                        encountered_FilterType,shared_FilterType,perFrame_FilterType,
					numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(FilterMaterial),"Filter Material",
                                        encountered_FilterMaterial,shared_FilterMaterial,perFrame_FilterMaterial,
					numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(PatientPosition),"Patient Position",
                                        encountered_PatientPosition,shared_PatientPosition,perFrame_PatientPosition,
					numberofinputfiles,i,filename,log);
			}
			
                        if (addMultiFrameMRStuff) {
//cerr << "Frame " << i << " tracking ... addMultiFrameMRStuff" << endl;				
                                // For every frame track the following ...
                                
                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(EchoTime),"Echo Time",
                                        encountered_EchoTime,shared_EchoTime,perFrame_EchoTime,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(RepetitionTime),"Repetition Time",
                                        encountered_RepetitionTime,shared_RepetitionTime,perFrame_RepetitionTime,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(InversionTime),"Inversion Time",
                                        encountered_InversionTime,shared_InversionTime,perFrame_InversionTime,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(FlipAngle),"Flip Angle",
                                        encountered_FlipAngle,shared_FlipAngle,perFrame_FlipAngle,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ScanningSequence),"Scanning Sequence",5,nValues_ScanningSequence,
                                        encountered_ScanningSequence,shared_ScanningSequence,perFrame_ScanningSequence,
                                        numberofinputfiles,i,filename,log);
					
                                trackMultiValuedStringAttributePerFrame(list,TagFromName(SequenceVariant),"Sequence Variant",8,nValues_SequenceVariant,
                                        encountered_SequenceVariant,shared_SequenceVariant,perFrame_SequenceVariant,
                                        numberofinputfiles,i,filename,log);
					
                                trackMultiValuedStringAttributePerFrame(list,TagFromName(ScanOptions),"Scan Options",9,nValues_ScanOptions,
                                        encountered_ScanOptions,shared_ScanOptions,perFrame_ScanOptions,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedUint32AttributePerFrame(list,TagFromName(EchoNumbers),"Echo Number(s)",
                                        encountered_EchoNumbers,shared_EchoNumbers,perFrame_EchoNumbers,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedUint32AttributePerFrame(list,TagFromName(MRAcquisitionPhaseEncodingStepsOutOfPlane),"MR Acquisition Phase Encoding Steps out-of-plane",
                                        encountered_MRAcquisitionPhaseEncodingStepsOutOfPlane,shared_MRAcquisitionPhaseEncodingStepsOutOfPlane,perFrame_MRAcquisitionPhaseEncodingStepsOutOfPlane,
                                        numberofinputfiles,i,filename,log);

                               trackSingleValuedStringAttributePerFrame(list,TagFromName(InPlanePhaseEncodingDirection),"In-plane Phase Encoding Direction",
                                        encountered_InPlanePhaseEncodingDirection,shared_InPlanePhaseEncodingDirection,perFrame_InPlanePhaseEncodingDirection,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedUint32AttributePerFrame(list,TagFromName(NumberOfPhaseEncodingSteps),"Number of Phase Encoding Steps",
                                        encountered_NumberOfPhaseEncodingSteps,shared_NumberOfPhaseEncodingSteps,perFrame_NumberOfPhaseEncodingSteps,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(PercentSampling),"Percent Sampling",
                                        encountered_PercentSampling,shared_PercentSampling,perFrame_PercentSampling,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(PercentPhaseFieldOfView),"Percent Phase Field of View",
                                        encountered_PercentPhaseFieldOfView,shared_PercentPhaseFieldOfView,perFrame_PercentPhaseFieldOfView,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(ImagingFrequency),"Imaging Frequency",
                                        encountered_ImagingFrequency,shared_ImagingFrequency,perFrame_ImagingFrequency,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(PixelBandwidth),"Pixel Bandwidth",
                                        encountered_PixelBandwidth,shared_PixelBandwidth,perFrame_PixelBandwidth,
                                        numberofinputfiles,i,filename,log);
					
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(ReceiveCoilName),"Receive Coil Name",
                                        encountered_ReceiveCoilName,shared_ReceiveCoilName,perFrame_ReceiveCoilName,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(TransmitCoilName),"Transmit Coil Name",
                                        encountered_TransmitCoilName,shared_TransmitCoilName,perFrame_TransmitCoilName,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(NumberOfAverages),"Number of Averages",
                                        encountered_NumberOfAverages,shared_NumberOfAverages,perFrame_NumberOfAverages,
                                        numberofinputfiles,i,filename,log);

                                 trackSingleValuedUint32AttributePerFrame(list,TagFromName(EchoTrainLength),"Echo Train Length",
                                        encountered_EchoTrainLength,shared_EchoTrainLength,perFrame_EchoTrainLength,
                                        numberofinputfiles,i,filename,log);

                                 trackSingleValuedUint32AttributePerFrame(list,TagFromName(RFEchoTrainLength),"RF Echo Train Length",
                                        encountered_RFEchoTrainLength,shared_RFEchoTrainLength,perFrame_RFEchoTrainLength,
                                        numberofinputfiles,i,filename,log);

                                 trackSingleValuedUint32AttributePerFrame(list,TagFromName(GradientEchoTrainLength),"Gradient Echo Train Length",
                                        encountered_GradientEchoTrainLength,shared_GradientEchoTrainLength,perFrame_GradientEchoTrainLength,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(SAR),"SAR",
                                        encountered_SAR,shared_SAR,perFrame_SAR,
                                        numberofinputfiles,i,filename,log);

                                trackMultiValuedUint32AttributePerFrame(list,TagFromName(AcquisitionMatrix),"Acquisition Matrix",4,
					encountered_AcquisitionMatrix,shared_AcquisitionMatrix,perFrame_AcquisitionMatrix,
                                        numberofinputfiles,i,filename,log);

				// these aren't in the original single frame MR object, but may be manually inserted into source
				// images to be able to build a correct MR multi-frame ...
				
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(ComplexImageComponent),"Complex Image Component",
 					encountered_ComplexImageComponent,shared_ComplexImageComponent,perFrame_ComplexImageComponent,
                                        numberofinputfiles,i,filename,log);
					
                                trackSingleValuedStringAttributePerFrame(list,TagFromName(AcquisitionContrast),"Acquisition Contrast",
					encountered_AcquisitionContrast,shared_AcquisitionContrast,perFrame_AcquisitionContrast,
					numberofinputfiles,i,filename,log);
					
				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(DiffusionBValue),"Diffusion b-value",
                                        encountered_DiffusionBValue,shared_DiffusionBValue,perFrame_DiffusionBValue,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedStringAttributePerFrame(list,TagFromName(DiffusionDirectionality),"Diffusion Directionality",
 					encountered_DiffusionDirectionality,shared_DiffusionDirectionality,perFrame_DiffusionDirectionality,
					numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(GradientOutput),"Gradient Output",
                                        encountered_GradientOutput,shared_GradientOutput,perFrame_GradientOutput,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(GradientOutputType),"Gradient Output Type",
					encountered_GradientOutputType,shared_GradientOutputType,perFrame_GradientOutputType,
                                        numberofinputfiles,i,filename,log);
					
				trackMultiValuedDoubleAttributePerFrame(list,TagFromName(VelocityEncodingDirection),"Velocity Encoding Direction",3,
					encountered_VelocityEncodingDirection,shared_VelocityEncodingDirection,perFrame_VelocityEncodingDirection,
					numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(VelocityEncodingMinimumValue),"Velocity Encoding Minimum Value",
                                        encountered_VelocityEncodingMinimumValue,shared_VelocityEncodingMinimumValue,perFrame_VelocityEncodingMinimumValue,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedDoubleAttributePerFrame(list,TagFromName(VelocityEncodingMaximumValue),"Velocity Encoding Maximum Value",
                                        encountered_VelocityEncodingMaximumValue,shared_VelocityEncodingMaximumValue,perFrame_VelocityEncodingMaximumValue,
                                        numberofinputfiles,i,filename,log);

				trackSingleValuedStringAttributePerFrame(list,TagFromName(MetaboliteMapDescription),"Metabolite Map Description",
					encountered_MetaboliteMapDescription,shared_MetaboliteMapDescription,perFrame_MetaboliteMapDescription,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(ChemicalShiftMinimumIntegrationLimitInppm),"Chemical Shift Minimum Integration Limit in ppm",
                                        encountered_ChemicalShiftMinimumIntegrationLimitInppm,shared_ChemicalShiftMinimumIntegrationLimitInppm,perFrame_ChemicalShiftMinimumIntegrationLimitInppm,
                                        numberofinputfiles,i,filename,log);

                                trackSingleValuedDoubleAttributePerFrame(list,TagFromName(ChemicalShiftMaximumIntegrationLimitInppm),"Chemical Shift Maximum Integration Limit in ppm",
                                        encountered_ChemicalShiftMaximumIntegrationLimitInppm,shared_ChemicalShiftMaximumIntegrationLimitInppm,perFrame_ChemicalShiftMaximumIntegrationLimitInppm,
                                        numberofinputfiles,i,filename,log);
			}
                        
                        if (i == 0) {
//cerr << "Frame 0 so do the copying of attributes ... " << endl;				
                                // delete any Data Set Trailing Padding which would otherwise follow pixel data ...

                                list-=TagFromName(DataSetTrailingPadding);

                                // delete PixelData ... written later ...

                                list-=TagFromName(PixelData);

                                if (copyAll) {
					newList+=list;	// Copy (actually move) everything from old list for first frame to new list ...

				}
				else {
					// Selectively copy what is needed from old list for first frame to new list ...
//cerr << "Selectively copy what is needed from old list" << endl;				
					copyPatientModule(newList,list);
					copyGeneralStudyModule(newList,list);
					copyPatientStudyModule(newList,list);
					copyGeneralSeriesModule(newList,list,addMultiFrameMRStuff || addMultiFrameCTStuff);
					copyFrameOfReferenceModule(newList,list);
					copySynchronizationFrameOfReferenceModule(newList,list);
					copyGeneralEquipmentModule(newList,list);
					copyImagePixelModule(newList,list);
					if (!addEnhancedContrastBolusModuleAndMacro) {
						copyContrastBolusModule(newList,list);
					}
					copySoftcopyPresentationLUTModule(newList,list);
					copyAcquisitionContextModule(newList,list);
					copySOPCommonModule(newList,list);
				
					if (addMultiFrameSecondaryCaptureStuff) {
						copySCEquipmentModule(newList,list);
						copyGeneralImageModule(newList,list);
						copyCineModule(newList,list);
						copyMultiFrameModule(newList,list);
						copyFramePointersModule(newList,list);
						copySCImageModule(newList,list);
						copySCMultiFrameImageModule(newList,list);
						copySCMultiFrameVectorModule(newList,list);
					}
					else if (addMultiFrameMRStuff || addMultiFrameCTStuff) {
						// for Enhanced MR or CT Image Module ...
						copyAttributeIfNotAlreadyPresent(TagFromName(LossyImageCompression),newList,list);
						copyAttributeIfNotAlreadyPresent(TagFromName(LossyImageCompressionRatio),newList,list);
						copyAttributeIfNotAlreadyPresent(TagFromName(IconImageSequence),newList,list);
						copyAttributeIfNotAlreadyPresent(TagFromName(BurnedInAnnotation),newList,list);
						if (!newList[TagFromName(BurnedInAnnotation)]) {
							newList+=new CodeStringAttribute(TagFromName(BurnedInAnnotation),"NO");
						}
					}
					else {	// do not know what we are making ...
						copyGeneralImageModule(newList,list);
						copyCineModule(newList,list);
						copyMultiFrameModule(newList,list);
						copyFramePointersModule(newList,list);
					}
				}
				// Regardless, cannot use (old) list after this point
                                
                                if (!useExtraPixelDataWritePass) {
//cerr << "Write of newList on 1st frame" << endl;
                                        if (!usualManagedAttributeListWrite(newList,dout,dicom_output_options,log,veryverbose)) {
                                                success=false;
                                                break;
                                        }

                                        // Now add "empty" pixel data attribute without pixels but correct value length ...

                                        OtherUnspecifiedLargeAttributeDummy *dummyPixelData = new
                                                OtherUnspecifiedLargeAttributeDummy(TagFromName(PixelData),
                                                        requiredRows,requiredColumns,numberofinputfiles,requiredSamplesPerPixel,
                                                        dout.getTransferSyntaxToWriteDataSet(),
                                                        0 /* let encoding rules calculate bytesinword */,
                                                        requiredBitsAllocated,requiredBitsStored,requiredHighBit,
                                                        0xffffffff /* let encoding rules calculate length */);
                                        Assert(dummyPixelData);

                                        dummyPixelData->writeBase(dout);

                                        if (veryverbose) {
                                                dummyPixelData->writeBase(log,&staticDictionary,false);
                                                log << "[]" << endl;
                                        }
                                }
                        }

                        if (!useExtraPixelDataWritePass && apixeldata) {
//cerr << "Writing pixel data on first pass (non-deferred)" << endl;
                                if (!apixeldata->isOtherData()) {
                                        log << EMsgDC(PixelDataIncorrectVR) << endl;
                                        success=false;
                                        break;
                                }
                                else {
                                        // Check for compatible transfer syntax and append to pixel data ...
                                        TransferSyntax *its=din.getTransferSyntaxToReadDataSet();
                                        Assert(its);
                                        TransferSyntax *ots=dout.getTransferSyntaxToWriteDataSet();
                                        Assert(ots);

                                        if (its->isEncapsulated()) {
                                                log << "Can't read encapsulated transfer syntax from file <" << filename << ">" << endl;
                                                success=false;
                                                break;
                                        }
                                        else if (its->getEndian() != ots->getEndian()) {
                                                log << "Input byte order in input file <" << filename << "> differs from output - not supported" << endl;
                                                success=false;
                                                break;
                                        }
                                        else {
                                                OtherUnspecifiedLargeAttributeBase *opixeldata = apixeldata->castToOtherData();
                                                Assert(opixeldata);
                                                opixeldata->writeRaw(dout);

                                                if (verbose) log << "wrote frame " << i
							 << " from filename <" << filename << ">" << endl;
                                        }
                                }
                        }
                        
                        if (i == numberofinputfiles-1) {
                                if (addMultiFrameMRStuff || addMultiFrameCTStuff) {
//cerr << "Adding MF stuff" << endl;
					// Keep track of this in case needed to generate CT Position Sequence (used to generate position along slice normals)
					double *reference_ImagePositionPatient = perFrame_ImagePositionPatient
						? perFrame_ImagePositionPatient[0]
						: (encountered_ImagePositionPatient ? shared_ImagePositionPatient : NULL);
						
					// Build the MultiFrame Dimension Module ...
//cerr << "Build the MultiFrame Dimension Module" << endl;

					// DimensionOrganizationSequence is Type 2 ... always add it
					SequenceAttribute *aDimensionOrganizationSequence=new SequenceAttribute(TagFromName(DimensionOrganizationSequence));
					Assert(aDimensionOrganizationSequence);
					newList-=TagFromName(DimensionOrganizationSequence);
					newList+=aDimensionOrganizationSequence;
					if (!minimalAttributesOnly && nDimensionOrganizations) {
						// but only create and add a UID if there really are any dimensions
						for (int ido=0; ido<nDimensionOrganizations; ++ido) {
							Assert(dimensionOrganizationUIDs);
							if (dimensionOrganizationUIDs[ido] == NULL || strlen(dimensionOrganizationUIDs[ido]) == 0) {
								dimensionOrganizationUIDs[ido] = StrDup(GeneratedDimensionOrganizationUID(dicom_output_options.stamp,
									studyIDForUIDGeneration,seriesNumberForUIDGeneration,ido));
							}
							AttributeList *iDimensionOrganizationSequence = new AttributeList();
							Assert(iDimensionOrganizationSequence);
							(*aDimensionOrganizationSequence)+=iDimensionOrganizationSequence;
							(*iDimensionOrganizationSequence)+=new UIStringAttribute(TagFromName(DimensionOrganizationUID),dimensionOrganizationUIDs[ido]);
						}
					}
					// DimensionIndexSequence is also Type 2 ... always add it
					SequenceAttribute *aDimensionIndexSequence=new SequenceAttribute(TagFromName(DimensionIndexSequence));
					Assert(aDimensionIndexSequence);
					newList-=TagFromName(DimensionIndexSequence);
					newList+=aDimensionIndexSequence;
					if (!minimalAttributesOnly) {
						for (int ido=0; ido<nDimensionOrganizations; ++ido) {
							Assert(nDimensions);
							for (int d=0; d<nDimensions[ido]; ++d) {
								AttributeList *iDimensionIndexSequence = new AttributeList();
								Assert(iDimensionIndexSequence);
								(*aDimensionIndexSequence)+=iDimensionIndexSequence;
								Assert(dimensionIndexPointers[0]);
								(*iDimensionIndexSequence)+=new AttributeTagAttribute(TagFromName(DimensionIndexPointer),dimensionIndexPointers[ido][d]);
								Assert(dimensionFunctionalGroupPointers);
								(*iDimensionIndexSequence)+=new AttributeTagAttribute(TagFromName(FunctionalGroupPointer),dimensionFunctionalGroupPointers[ido][d]);
								Assert(dimensionOrganizationUIDs);
								Assert(dimensionOrganizationUIDs[ido]);
								Assert(strlen(dimensionOrganizationUIDs[ido]));
								(*iDimensionIndexSequence)+=new UIStringAttribute(TagFromName(DimensionOrganizationUID),dimensionOrganizationUIDs[ido]);
							}
						}
					}
					
					if (addSynchronizationModule) {
//cerr << "Build the Synchronization Module" << endl;
						// The SynchronizationFrameOfReferenceUID will be added during the usual list write
						// because of the presence of the following attributes of the module
						// (can't do it now because will be removed if -removeinstanceuid option)

						if (!newList[TagFromName(SynchronizationTrigger)]) {
			 				newList+=new CodeStringAttribute(TagFromName(SynchronizationTrigger),"NO TRIGGER");
						}
						if (!newList[TagFromName(AcquisitionTimeSynchronized)]) {
							newList+=new CodeStringAttribute(TagFromName(AcquisitionTimeSynchronized),"Y");
						}
					}
					
					// Build the common part of the Multi-frame Functional Groups Module (as refactored by Sup 58) ...
//cerr << "Build the common part of the Multi-frame Functional Groups Module" << endl;

					newList-=TagFromName(InstanceNumber);
					newList+=new IntegerStringAttribute(TagFromName(InstanceNumber),Uint16(1));

					addEarliestContentDateAndTime(newList,numberofinputfiles,
						encountered_ContentDate,shared_ContentDate,perFrame_ContentDate,
						encountered_ContentTime,shared_ContentTime,perFrame_ContentTime,
						log);

					newList-=TagFromName(NumberOfFrames);
					newList+=new IntegerStringAttribute(TagFromName(NumberOfFrames),Uint16(numberofinputfiles));
/*
                Element <ConcatenationFrameOffsetNumber> not present
                Element <RepresentativeFrameNumber> not present
                Element <ConcatenationUID> not present
                Element <InConcatenationNumber> not present
                Element <InConcatenationTotalNumber> not present
*/
					
					// Build the Enhanced MR and CT Image Modules ...
//cerr << "Build the Enhanced MR and CT Image Modules" << endl;
					
					bool imageIsDerived = makeCTMRImageDescriptionMacroAttributesAndAppendToList(addMultiFrameMRStuff,
						newList,false /* frame level */,0,numberofinputfiles,
						encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
						encountered_PixelPresentation,shared_PixelPresentation,perFrame_PixelPresentation,
						encountered_VolumetricProperties,shared_VolumetricProperties,perFrame_VolumetricProperties,
						encountered_VolumeBasedCalculationTechnique,shared_VolumeBasedCalculationTechnique,perFrame_VolumeBasedCalculationTechnique,
						encountered_ComplexImageComponent,shared_ComplexImageComponent,perFrame_ComplexImageComponent,
						encountered_AcquisitionContrast,shared_AcquisitionContrast,perFrame_AcquisitionContrast,
						log);
//cerr << "imageIsDerived= " << (imageIsDerived ? "T" : "F") << endl;

					if (!newList[TagFromName(LossyImageCompression)])					// since might have been copied from 1st file in copyGeneralImageModule()
						newList+=new CodeStringAttribute(TagFromName(LossyImageCompression),"00");	// let's just assume they weren't ... should really check each frame :(

					// Build the MR Image and Spectroscopy Instance Macro, and the same attributes that are in the Enhanced CT Image Module
//cerr << "Build the MR Image and Spectroscopy Instance Macro" << endl;
					
					if (encountered_AcquisitionNumber && !perFrame_AcquisitionNumber) {
						newList-=TagFromName(AcquisitionNumber);
						if (!minimalAttributesOnly) newList+=new IntegerStringAttribute(TagFromName(AcquisitionNumber),shared_AcquisitionNumber);
					}
					
					if (!imageIsDerived || addNonDerivedStuffAnyway) {
						if (addMultiFrameMRStuff) {
							copyImagedNucleusIntoResonantNucleusIfNotAlreadyPresent(newList,list);
							copyAttributeIfNotAlreadyPresent(TagFromName(MagneticFieldStrength),newList,list);
							copyAttributeIfNotAlreadyPresent(TagFromName(KSpaceFiltering),newList,list);
							if (!newList[TagFromName(KSpaceFiltering)]) {
								newList+=new CodeStringAttribute(TagFromName(KSpaceFiltering),"NONE");
							}
						}
						addEarliestAcquisitionDateAndTime(newList,numberofinputfiles,
							encountered_AcquisitionDate,shared_AcquisitionDate,perFrame_AcquisitionDate,
							encountered_AcquisitionTime,shared_AcquisitionTime,perFrame_AcquisitionTime,
							log);
					
						double use_AcquisitionDuration;
						if (addMultiFrameMRStuff && deriveDurationFromTiming) {
							// should do better if varies per frame than just use frame 0 values :(
							use_AcquisitionDuration=computeAcquisitionDurationInMilliSecondsFromTiming(
								encountered_RepetitionTime,shared_RepetitionTime,perFrame_RepetitionTime,
								encountered_NumberOfAverages,shared_NumberOfAverages,perFrame_NumberOfAverages,
								encountered_AcquisitionMatrix,shared_AcquisitionMatrix,perFrame_AcquisitionMatrix,
								encountered_EchoTrainLength,shared_EchoTrainLength,perFrame_EchoTrainLength,
								encountered_ScanningSequence,shared_ScanningSequence,perFrame_ScanningSequence,nValues_ScanningSequence,
								0)/1000;
						}
						else {
							use_AcquisitionDuration = encountered_AcquisitionDuration ? (perFrame_AcquisitionDuration ? 0 : shared_AcquisitionDuration) : 0;
						}
						if (accumulateDuration) use_AcquisitionDuration=use_AcquisitionDuration*numberofinputfiles;
						newList+=(minimalAttributesOnly && !addMultiFrameMRStuff)
							? new FloatDoubleAttribute(TagFromName(AcquisitionDuration))	// is Type 2C for CT, 1C for MR
							: new FloatDoubleAttribute(TagFromName(AcquisitionDuration),use_AcquisitionDuration);
					}
					
					// Add ReferencedRawDataSequence derived from AcquisitionNumber
					if (!minimalAttributesOnly && encountered_AcquisitionNumber) {
						SequenceAttribute *aReferencedRawDataSequence=new SequenceAttribute(TagFromName(ReferencedRawDataSequence));
						Assert(aReferencedRawDataSequence);
						newList-=TagFromName(ReferencedRawDataSequence);
						newList+=aReferencedRawDataSequence;
						
						int useAcquisitionNumber = perFrame_AcquisitionNumber ? perFrame_AcquisitionNumber[0] : shared_AcquisitionNumber;
						{
							AttributeList *iReferencedRawDataSequence = new AttributeList();
							Assert(iReferencedRawDataSequence);
							(*aReferencedRawDataSequence)+=iReferencedRawDataSequence;
							
							Attribute *aStudyInstanceUID=new UIStringAttribute(TagFromName(StudyInstanceUID),
								GeneratedStudyInstanceUID(dicom_output_options.stamp,studyIDForUIDGeneration));
							Assert(aStudyInstanceUID);
							(*iReferencedRawDataSequence)+=aStudyInstanceUID;
								
							SequenceAttribute *aReferencedSeriesSequence=new SequenceAttribute(TagFromName(ReferencedSeriesSequence));
							Assert(aReferencedSeriesSequence);
							(*iReferencedRawDataSequence)+=aReferencedSeriesSequence;

							AttributeList *iReferencedSeriesSequence = new AttributeList();
							Assert(iReferencedSeriesSequence);
							(*aReferencedSeriesSequence)+=iReferencedSeriesSequence;
							
							Attribute *aSeriesInstanceUID=new UIStringAttribute(TagFromName(SeriesInstanceUID),
								GeneratedSeriesInstanceUID(dicom_output_options.stamp,studyIDForUIDGeneration,seriesNumberForUIDGeneration));
							Assert(aSeriesInstanceUID);
							(*iReferencedSeriesSequence)+=aSeriesInstanceUID;
								
							SequenceAttribute *aReferencedSOPSequence=new SequenceAttribute(TagFromName(ReferencedSOPSequence));
							Assert(aReferencedSOPSequence);
							(*iReferencedSeriesSequence)+=aReferencedSOPSequence;

							AttributeList *iReferencedSOPSequence = new AttributeList();
							Assert(iReferencedSOPSequence);
							(*aReferencedSOPSequence)+=iReferencedSOPSequence;
							
							Attribute *aReferencedSOPInstanceUID=new UIStringAttribute(TagFromName(ReferencedSOPInstanceUID),
								GeneratedRawDataUID(dicom_output_options.stamp,studyIDForUIDGeneration,useAcquisitionNumber));
							Assert(aReferencedSOPInstanceUID);
							(*iReferencedSOPSequence)+=aReferencedSOPInstanceUID;
							
							Attribute *aReferencedSOPClassUID=new UIStringAttribute(TagFromName(ReferencedSOPClassUID),RawDataStorageSOPClassUID);
							Assert(aReferencedSOPClassUID);
							(*iReferencedSOPSequence)+=aReferencedSOPClassUID;
						}
					}
/*
                Sequence <ReferencedWaveformSequence> not present
                Sequence <ReferencedImageEvidenceSequence> not present
                Sequence <SourceImageEvidenceSequence> not present
                Sequence <ReferencedGrayscalePresentationStateSequence> not present
*/
					newList-=TagFromName(ContentQualification);
					newList+=new CodeStringAttribute(TagFromName(ContentQualification),"PRODUCT");

					if (addMultiFrameMRStuff) {
						// ResonantNucleus copied from ImagedNucleus earlier
						// MagneticFieldStrength copied earlier

						newList-=TagFromName(ApplicableSafetyStandardAgency);
						newList+=new CodeStringAttribute(TagFromName(ApplicableSafetyStandardAgency),"FDA");

						if (minimalAttributesOnly) {
							// remove anything optional that might have been copied earlier from 1st file
							newList-=TagFromName(ApplicableSafetyStandardDescription);
						}
					}
					
					if (minimalAttributesOnly) {
						// remove anything optional that might have been copied earlier from 1st file
						newList-=TagFromName(ImageComments);
					}
					
					// SamplesPerPixel should already be 1
					// PhotometricInterpretation should already be MONOCHROME2
					// LargestMonochromePixelValue is no longer used
					// BitsAllocated has to be 8 or 16 for MR; 16 for CT
					// BitStored has to be 8, 12 or 16 for MR; 12 or 16 for CT
					{
						Uint16 vBitsStored = getIntegerValueElseError(newList,TagFromName(BitsStored),"Bits Stored",filename,log);
						Uint16 nBitsStored = vBitsStored;
						if (addMultiFrameMRStuff) {
							if (nBitsStored < 8) nBitsStored=8;
							else if (nBitsStored > 8 && nBitsStored < 12) nBitsStored=12;
							else if (nBitsStored > 12 && nBitsStored < 16) nBitsStored=16;
						}
						else {
							if (nBitsStored < 16) nBitsStored=16;
							else if (nBitsStored <= 12) nBitsStored=12;
							else if (nBitsStored <= 16) nBitsStored=16;
						}
						if (nBitsStored != vBitsStored) {
							if (verbose) log << "Changing Bits Stored from " << dec << vBitsStored << " to " << nBitsStored << endl;
							newList-=TagFromName(BitsStored);
							newList+=new UnsignedShortAttribute(TagFromName(BitsStored),nBitsStored);
						}
					}
					// HighBit has to be one less than BitStored
					{
						Uint16 vBitsStored = getIntegerValueElseError(newList,TagFromName(BitsStored),"Bits Stored",filename,log);
						Uint16 vHighBit = getIntegerValueElseError(newList,TagFromName(HighBit),"High Bit",filename,log);
						Uint16 nHighBit = vHighBit;
						if (nHighBit != vBitsStored-1) nHighBit=vBitsStored-1;
						
						if (nHighBit != vHighBit) {
							if (verbose) log << "Changing High Bit from " << dec << vHighBit << " to " << nHighBit << endl;
							newList-=TagFromName(HighBit);
							newList+=new UnsignedShortAttribute(TagFromName(HighBit),nHighBit);
						}
					}
					
					if (minimalAttributesOnly) {
						// remove anything optional that might have been copied earlier from 1st file
						newList-=TagFromName(SpacingBetweenSlices);	// an MR thing
						newList-=TagFromName(IconImageSequence);
					}
					
					// LossyImageCompression	copied earlier from 1st file
					// LossyImageCompressionRatio	copied earlier from 1st file
					
					// Set a flag that indicates whether or not to add the MR-specific stuff
					// which is required if value 1 of Image Type is ORIGINAL or MIXED
					// but also add it if we encountered Scanning Sequence in the input
					// (i.e. make it even for DERIVED if we may have enough information to make it from)
					
					//bool addMROriginalOrMixedStuff;
					//{
					//	const char *imageTypeValue1 = AttributeValue(newList[TagFromName(ImageType)]);
					//	addMROriginalOrMixedStuff = imageTypeValue1 && (
					//				   strcmp(imageTypeValue1,"ORIGINAL") == 0
					//				|| strcmp(imageTypeValue1,"MIXED") == 0
					//				// || encountered_ScanningSequence
					//				);
					//}
 					
					//if (addMROriginalOrMixedStuff)
					if (addMultiFrameMRStuff && (!imageIsDerived || addNonDerivedStuffAnyway)) {
						makeMRPulseSequenceModule(&newList,numberofinputfiles,
								requiredSequenceName,requiredProtocolName,requiredMRAcquisitionType,
								encountered_ScanningSequence,shared_ScanningSequence,perFrame_ScanningSequence,nValues_ScanningSequence,
								encountered_SequenceVariant,shared_SequenceVariant,perFrame_SequenceVariant,nValues_SequenceVariant,
								encountered_ScanOptions,shared_ScanOptions,perFrame_ScanOptions,nValues_ScanOptions,
								encountered_EchoNumbers,shared_EchoNumbers,perFrame_EchoNumbers,
								encountered_VelocityEncodingDirection,shared_VelocityEncodingDirection,perFrame_VelocityEncodingDirection,
								phaseContrast);
					}
 					// Build the Softcopy Presentation LUT Module ...
//cerr << "Build the Softcopy Presentation LUT Module" << endl;
					newList+=new CodeStringAttribute(TagFromName(PresentationLUTShape),"IDENTITY");

					if (addSupplementalPaletteColorLUT) {
						makeSupplementalPaletteColorLUT(
							&newList,
							minimalAttributesOnly,
							addSupplementalPaletteColorLUT_numberOfEntries,
							addSupplementalPaletteColorLUT_firstValueMapped,
							addSupplementalPaletteColorLUT_firstRedValue,
							addSupplementalPaletteColorLUT_incrementRedValue,
							addSupplementalPaletteColorLUT_entryToStartIncrementingRedValue,
							addSupplementalPaletteColorLUT_entryToStartDecrementingRedValue,
							addSupplementalPaletteColorLUT_entryToStopChangingRedValue,
							addSupplementalPaletteColorLUT_firstGreenValue,
							addSupplementalPaletteColorLUT_incrementGreenValue,
							addSupplementalPaletteColorLUT_entryToStartIncrementingGreenValue,
							addSupplementalPaletteColorLUT_entryToStartDecrementingGreenValue,
							addSupplementalPaletteColorLUT_entryToStopChangingGreenValue,
							addSupplementalPaletteColorLUT_firstBlueValue,
							addSupplementalPaletteColorLUT_incrementBlueValue,
							addSupplementalPaletteColorLUT_entryToStartIncrementingBlueValue,
							addSupplementalPaletteColorLUT_entryToStartDecrementingBlueValue,
							addSupplementalPaletteColorLUT_entryToStopChangingBlueValue);
					}

					if (addEnhancedContrastBolusModuleAndMacro) {
						makeEnhancedContrastBolusModule(&newList,
						encountered_ContrastBolusAgent,shared_ContrastBolusAgent,perFrame_ContrastBolusAgent,nValues_ContrastBolusAgent,
						encountered_ContrastBolusRoute,shared_ContrastBolusRoute,perFrame_ContrastBolusRoute,nValues_ContrastBolusRoute,
						encountered_ContrastBolusVolume,shared_ContrastBolusVolume,perFrame_ContrastBolusVolume,nValues_ContrastBolusVolume,
						encountered_ContrastBolusIngredientConcentration,shared_ContrastBolusIngredientConcentration,
							perFrame_ContrastBolusIngredientConcentration,nValues_ContrastBolusIngredientConcentration);
					}
					
					// Add the attributes tracked per frame to either the shared or per-frame function groups ...
					
					const char *vEchoPulseSequence = NULL;					// Need this for making echo train lengths
					{
						Attribute *a = newList[TagFromName(EchoPulseSequence)];		// filled in as GRADIENT, SPIN or BOTH by makeMRPulseSequenceModule
						if (a) {							// can be misssing, e.g. derived
							vEchoPulseSequence = AttributeValue(a);
						}
					}
										
					bool frameAnatomySequenceDoneAtSharedLevel = false;
					//bool frameIsDerived = false;
                                        
                                        SequenceAttribute *aAcquisitionContextSequence=new SequenceAttribute(TagFromName(AcquisitionContextSequence));
                                        Assert(aAcquisitionContextSequence);
                                        newList-=TagFromName(AcquisitionContextSequence);
                                        newList+=aAcquisitionContextSequence;
                                        
                                        SequenceAttribute *aPerFrameFunctionalGroupsSequence=new SequenceAttribute(TagFromName(PerFrameFunctionalGroupsSequence));
                                        Assert(aPerFrameFunctionalGroupsSequence);
                                        newList-=TagFromName(PerFrameFunctionalGroupsSequence);
                                        newList+=aPerFrameFunctionalGroupsSequence;
                                
                                        SequenceAttribute *aSharedFunctionalGroupsSequence=new SequenceAttribute(TagFromName(SharedFunctionalGroupsSequence));
                                        Assert(aSharedFunctionalGroupsSequence);
                                        newList-=TagFromName(SharedFunctionalGroupsSequence);
                                        newList+=aSharedFunctionalGroupsSequence;
                                        
                                        AttributeList *iSharedFunctionalGroupsSequence = new AttributeList();
                                        Assert(iSharedFunctionalGroupsSequence);
                                        (*aSharedFunctionalGroupsSequence)+=iSharedFunctionalGroupsSequence;
//cerr << "adding shared functional groups" << endl;
                                        if (perFrame_SliceThickness == NULL
					 && perFrame_PixelSpacing == NULL) {
						AttributeList *iPixelMeasuresSequence = makeNewSequenceAttributeWithItem(
							iSharedFunctionalGroupsSequence,TagFromName(PixelMeasuresSequence));
                                                if (encountered_SliceThickness)
							(*iPixelMeasuresSequence)+=new DecimalStringAttribute(TagFromName(SliceThickness),shared_SliceThickness);
                                                if (encountered_PixelSpacing)
							(*iPixelMeasuresSequence)+=new DecimalStringAttribute(TagFromName(PixelSpacing),shared_PixelSpacing[0],shared_PixelSpacing[1]);
                                        }
                                        if (perFrame_ImagePositionPatient == NULL) {
						AttributeList *iPlanePositionSequence = makeNewSequenceAttributeWithItem(
							iSharedFunctionalGroupsSequence,TagFromName(PlanePositionSequence));
                                                if (encountered_ImagePositionPatient && (!imageIsDerived || addNonDerivedStuffAnyway || addPositionStuffAnyway))
							(*iPlanePositionSequence)+=new DecimalStringAttribute(TagFromName(ImagePositionPatient),
								shared_ImagePositionPatient[0],shared_ImagePositionPatient[1],shared_ImagePositionPatient[2]);
                                        }
                                         if (perFrame_ImageOrientationPatient == NULL) {
						AttributeList *iPlaneOrientationSequence = makeNewSequenceAttributeWithItem(
							iSharedFunctionalGroupsSequence,TagFromName(PlaneOrientationSequence));
                                                if (encountered_ImageOrientationPatient && (!imageIsDerived || addNonDerivedStuffAnyway || addPositionStuffAnyway))
							(*iPlaneOrientationSequence)+=new DecimalStringAttribute(TagFromName(ImageOrientationPatient),
								shared_ImageOrientationPatient[0],shared_ImageOrientationPatient[1],shared_ImageOrientationPatient[2],
								shared_ImageOrientationPatient[3],shared_ImageOrientationPatient[4],shared_ImageOrientationPatient[5]);
                                        }
                                        if (perFrame_NominalCardiacTriggerDelayTime == NULL
					 && perFrame_TriggerTime == NULL
					 && perFrame_RRIntervalTimeNominal == NULL
					 /*&& perFrame_TriggerWindow == NULL*/) {
                                                if (encountered_TriggerTime || encountered_NominalCardiacTriggerDelayTime || encountered_RRIntervalTimeNominal/* || encountered_TriggerWindow*/) {
							AttributeList *iCardiacSynchronizationSequence = makeNewSequenceAttributeWithItem(
								iSharedFunctionalGroupsSequence,TagFromName(CardiacSynchronizationSequence));
							double use_NominalCardiacTriggerDelayTime = encountered_TriggerTime ?  shared_TriggerTime : shared_NominalCardiacTriggerDelayTime;
							(*iCardiacSynchronizationSequence)+=new FloatDoubleAttribute(TagFromName(NominalCardiacTriggerDelayTime),use_NominalCardiacTriggerDelayTime);
							if (encountered_RRIntervalTimeNominal) {
								(*iCardiacSynchronizationSequence)+=new FloatDoubleAttribute(TagFromName(RRIntervalTimeNominal),shared_RRIntervalTimeNominal);
							}
							//if (encountered_TriggerWindow) {
							//	(*iCardiacSynchronizationSequence)+=new IntegerStringAttribute(TagFromName(TriggerWindow),Uint16(shared_TriggerWindow));
							//}
						}
					}
                    if ((perFrame_WindowCenter == NULL
					 && perFrame_WindowWidth == NULL
					 && perFrame_WindowCenterWidthExplanation == NULL) || nowindow) {
						if (minimalAttributesOnly || nowindow) {
							// Do NOT add empty FrameVOILUTSequence ... changed from Type 2 to Type 1 by Sup 83, but is U macro in CT and MR
							//(void)makeNewSequenceAttributeWithoutItem(iSharedFunctionalGroupsSequence,TagFromName(FrameVOILUTSequence));
						}
						else {
							AttributeList *iFrameVOILUTSequence = makeNewSequenceAttributeWithItem(
								iSharedFunctionalGroupsSequence,TagFromName(FrameVOILUTSequence));
							(*iFrameVOILUTSequence)+=new DecimalStringAttribute(TagFromName(WindowCenter),encountered_WindowCenter ? shared_WindowCenter : defaultWindowCenter);
							(*iFrameVOILUTSequence)+=new DecimalStringAttribute(TagFromName(WindowWidth),encountered_WindowWidth ? shared_WindowWidth : defaultWindowWidth);
							if (encountered_WindowCenterWidthExplanation) {
								(*iFrameVOILUTSequence)+=new LongStringAttribute(TagFromName(WindowCenterWidthExplanation),shared_WindowCenterWidthExplanation);
							}
						}
                    }
					{
						frameAnatomySequenceDoneAtSharedLevel=makeFrameAnatomySequence(iSharedFunctionalGroupsSequence,
							false /*frameLevel*/,0,
							encountered_BodyPartExamined,shared_BodyPartExamined,perFrame_BodyPartExamined,
							encountered_StudyDescription,shared_StudyDescription,perFrame_StudyDescription,
							encountered_SeriesDescription,shared_SeriesDescription,perFrame_SeriesDescription,
							encountered_ImageComments,shared_ImageComments,perFrame_ImageComments,
							encountered_PatientName,shared_PatientName,perFrame_PatientName,
							encountered_Laterality,shared_Laterality,perFrame_Laterality);
					}
                                        if (perFrame_RescaleSlope == NULL
					 && perFrame_RescaleIntercept == NULL
					 && perFrame_RescaleType == NULL) {
//cerr << "adding shared PixelValueTransformationSequence" << endl;
						AttributeList *iPixelValueTransformationSequence = makeNewSequenceAttributeWithItem(
							iSharedFunctionalGroupsSequence,TagFromName(PixelValueTransformationSequence));
						double use_intercept = encountered_RescaleIntercept ? shared_RescaleIntercept : 0.0;
						double use_slope = encountered_RescaleSlope ? shared_RescaleSlope : 1.0;
						if (use_slope == 0) {
							use_slope=1.0;	// a slope of zero means it was probably missing for that frame
							use_intercept=0.0;
						}
						const char *use_type = encountered_RescaleType ? shared_RescaleType : NULL;
						use_type=fixRescaleTypeForCTIfNecessary(use_type,addMultiFrameCTStuff,imageIsDerived);
//cerr << "adding RescaleIntercept" << endl;
                                                (*iPixelValueTransformationSequence)+=new DecimalStringAttribute(TagFromName(RescaleIntercept),use_intercept);
//cerr << "adding RescaleSlope" << endl;
                                                (*iPixelValueTransformationSequence)+=new DecimalStringAttribute(TagFromName(RescaleSlope),use_slope);
//cerr << "adding RescaleType = " << use_type << endl;
						(*iPixelValueTransformationSequence)+=new LongStringAttribute(TagFromName(RescaleType),use_type);
                                        }
					if (addDerivation && !forceDerivationPerFrame) {
						makeDerivationImageSequence(
							iSharedFunctionalGroupsSequence,
							minimalAttributesOnly,
							addDerivation_derivationCodeSequenceCodingSchemeDesignator,
							addDerivation_derivationCodeSequenceCodeValue,
							addDerivation_derivationCodeSequenceCodeMeaning,
							addDerivation_derivationCodeSequenceCodingSchemeVersion,
							addDerivation_referencedSOPClassUID,
							addDerivation_referencedSOPInstanceUID,
							addDerivation_referencedFrameNumbers,
							addDerivation_purposeOfReferenceCodeSequenceCodingSchemeDesignator,
							addDerivation_purposeOfReferenceCodeSequenceCodeValue,
							addDerivation_purposeOfReferenceCodeSequenceCodeMeaning,
							addDerivation_purposeOfReferenceCodeSequenceCodingSchemeVersion);
					}
					if (addReferenced && !forceReferencedPerFrame) {
						makeReferencedImageSequence(
							iSharedFunctionalGroupsSequence,
							minimalAttributesOnly,
							addReferenced_referencedSOPClassUID,
							addReferenced_referencedSOPInstanceUID,
							addReferenced_referencedFrameNumbers,
							addReferenced_purposeOfReferenceCodeSequenceCodingSchemeDesignator,
							addReferenced_purposeOfReferenceCodeSequenceCodeValue,
							addReferenced_purposeOfReferenceCodeSequenceCodeMeaning,
							addReferenced_purposeOfReferenceCodeSequenceCodingSchemeVersion);
					}
					if (addMultiFrameMRStuff) {
						if (addSatSlab_numberOfSlabs > 0) {
							makeMRSpatialSaturationSequence(
								iSharedFunctionalGroupsSequence,
								minimalAttributesOnly,
								addSatSlab_numberOfSlabs,
								addSatSlab_thickness,	
								addSatSlab_orientX,addSatSlab_orientY,addSatSlab_orientZ,	
								addSatSlab_midpointX,addSatSlab_midpointY,addSatSlab_midpointZ);
						}
						else {
							// we may need to add an empty MRSpatialSaturationSequence anyway
							// since the condition on the macro is based on SpatialPresaturation being SLAB
							// and ImageType value 1 ORIGINAL or MIXED
							// 
							// achieve this by scanning for any frame that may trigger SpatialPresaturation
							// to be set to slab
						
							if (!imageIsDerived || addNonDerivedStuffAnyway) {
								bool needSpatialPresaturationRegardless = false;
								if (encountered_ScanOptions) {
									if (shared_ScanOptions && arrayOfStringValuesContains(shared_ScanOptions,nValues_ScanOptions,"SP")) {
										needSpatialPresaturationRegardless=true;
									}
									else if (perFrame_ScanOptions) {
										int i;
										for (int i=0; i<numberofinputfiles; ++i) {
											if (perFrame_ScanOptions[i] && arrayOfStringValuesContains(perFrame_ScanOptions[i],nValues_ScanOptions,"SP")) {
												needSpatialPresaturationRegardless=true;
												break;
											}
										}
									}
								}
								if (needSpatialPresaturationRegardless) {
									(void)makeNewSequenceAttributeWithoutItem(iSharedFunctionalGroupsSequence,TagFromName(MRSpatialSaturationSequence));
								}
							}
						}
					}
					if (perFrame_RealWorldValueSlope == NULL
					 && perFrame_RealWorldValueIntercept == NULL
					 && perFrame_Units == NULL
					 && perFrame_RealWorldValueFirstValueMapped == NULL
					 && perFrame_RealWorldValueLastValueMapped == NULL
					 && perFrame_LUTExplanation == NULL
					 && perFrame_LUTLabel == NULL) {
//cerr << "adding shared RealWorldValueMappingSequence" << endl;
						makeRealWorldValueMappingSequence(iSharedFunctionalGroupsSequence,0,pixelRepresentation,
							encountered_RealWorldValueSlope,shared_RealWorldValueSlope,perFrame_RealWorldValueSlope,
							encountered_RealWorldValueIntercept,shared_RealWorldValueIntercept,perFrame_RealWorldValueIntercept,
							encountered_Units,shared_Units,perFrame_Units,
							encountered_RealWorldValueFirstValueMapped,shared_RealWorldValueFirstValueMapped,perFrame_RealWorldValueFirstValueMapped,
							encountered_RealWorldValueLastValueMapped,shared_RealWorldValueLastValueMapped,perFrame_RealWorldValueLastValueMapped,
							encountered_LUTExplanation,shared_LUTExplanation,perFrame_LUTExplanation,
							encountered_LUTLabel,shared_LUTLabel,perFrame_LUTLabel);
					}
					if (perFrame_ImageType == NULL
					 && perFrame_PixelPresentation == NULL
					 && perFrame_VolumetricProperties == NULL
					 && perFrame_VolumeBasedCalculationTechnique == NULL
					 && perFrame_ComplexImageComponent == NULL
					 && perFrame_AcquisitionContrast == NULL) {
//cerr << "adding shared MR or CT Image Frame Type Macro" << endl;
						AttributeList *iImageFrameTypeSequence = makeNewSequenceAttributeWithItem(
							iSharedFunctionalGroupsSequence,
							addMultiFrameMRStuff ? TagFromName(MRImageFrameTypeSequence) : TagFromName(CTImageFrameTypeSequence));
							
						// include the MR Image Description Macro, using Frame Type not Image Type at this level
						// (the same macro is also used in the Enhanced MR Image Module)
							
						/* frameIsDerived = */ makeCTMRImageDescriptionMacroAttributesAndAppendToList(addMultiFrameMRStuff,
							*iImageFrameTypeSequence,true /* frame level */,0,numberofinputfiles,
							encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
							encountered_PixelPresentation,shared_PixelPresentation,perFrame_PixelPresentation,
							encountered_VolumetricProperties,shared_VolumetricProperties,perFrame_VolumetricProperties,
							encountered_VolumeBasedCalculationTechnique,shared_VolumeBasedCalculationTechnique,perFrame_VolumeBasedCalculationTechnique,
							encountered_ComplexImageComponent,shared_ComplexImageComponent,perFrame_ComplexImageComponent,
							encountered_AcquisitionContrast,shared_AcquisitionContrast,perFrame_AcquisitionContrast,
							log);							
					}
					if (addEnhancedContrastBolusModuleAndMacro) {
						// regardless of original or derived ...
						if (perFrame_ContrastBolusAgent == NULL
						 && perFrame_ContrastBolusRoute == NULL
						 && perFrame_ContrastBolusVolume == NULL
						 && perFrame_ContrastBolusIngredientConcentration == NULL
						 && perFrame_ContrastBolusAgentAdministered == NULL
						 && perFrame_ContrastBolusAgentDetected == NULL
						 && perFrame_ContrastBolusAgentPhase == NULL) {
							makeContrastBolusUsageSequence(iSharedFunctionalGroupsSequence,0,
								encountered_ContrastBolusAgent,shared_ContrastBolusAgent,perFrame_ContrastBolusAgent,nValues_ContrastBolusAgent,
								encountered_ContrastBolusRoute,shared_ContrastBolusRoute,perFrame_ContrastBolusRoute,nValues_ContrastBolusRoute,
								encountered_ContrastBolusVolume,shared_ContrastBolusVolume,perFrame_ContrastBolusVolume,nValues_ContrastBolusVolume,
								encountered_ContrastBolusIngredientConcentration,shared_ContrastBolusIngredientConcentration,
									perFrame_ContrastBolusIngredientConcentration,nValues_ContrastBolusIngredientConcentration,
								encountered_ContrastBolusAgentAdministered,shared_ContrastBolusAgentAdministered,
									perFrame_ContrastBolusAgentAdministered,nValues_ContrastBolusAgentAdministered,
								encountered_ContrastBolusAgentDetected,shared_ContrastBolusAgentDetected,
									perFrame_ContrastBolusAgentDetected,nValues_ContrastBolusAgentDetected,
								encountered_ContrastBolusAgentPhase,shared_ContrastBolusAgentPhase,
									perFrame_ContrastBolusAgentPhase,nValues_ContrastBolusAgentPhase);
						}
					}
					if (addMultiFrameCTStuff) {
						{
							// Irradiation Event Identification is mandatory whether derived or not
							if (perFrame_IrradiationEventUID == NULL) {
//cerr << "adding per-frame IrradiationEventIdentificationSequence" << endl;
								AttributeList *iIrradiationEventIdentificationSequence = makeNewSequenceAttributeWithItem(
									iSharedFunctionalGroupsSequence,TagFromName(IrradiationEventIdentificationSequence));
								(*iIrradiationEventIdentificationSequence)+=new UIStringAttribute(TagFromName(IrradiationEventUID),
									encountered_IrradiationEventUID
										? shared_IrradiationEventUID
										: StrDup(GeneratedIrradiationEventUID(dicom_output_options.stamp,
											studyIDForUIDGeneration,seriesNumberForUIDGeneration,instanceNumberForUIDGeneration))
									);
							}
						}
						if (!imageIsDerived || addNonDerivedStuffAnyway) {	// NB. not !frameIsDerived, since values may be shared but frame type value 1 varies per-frame
//cerr << "Adding CT-specific stuff for non-derived image at shared level" << endl;
							if (perFrame_AcquisitionType == NULL
							 && perFrame_TubeAngle == NULL
							 && perFrame_ConstantVolumeFlag == NULL
							 && perFrame_FluoroscopyFlag == NULL) {
								makeCTAcquisitionTypeSequence(iSharedFunctionalGroupsSequence,0,false,
									encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
									encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType,
									encountered_TubeAngle,shared_TubeAngle,perFrame_TubeAngle,
									encountered_ConstantVolumeFlag,shared_ConstantVolumeFlag,perFrame_ConstantVolumeFlag,
									encountered_FluoroscopyFlag,shared_FluoroscopyFlag,perFrame_FluoroscopyFlag);
							}
							if (perFrame_RevolutionTime == NULL
							 && perFrame_SingleCollimationWidth == NULL
							 && perFrame_TotalCollimationWidth == NULL
							 && perFrame_TableHeight == NULL
							 && perFrame_GantryDetectorTilt == NULL
							 && perFrame_DataCollectionDiameter == NULL) {
								makeCTAcquisitionDetailsSequence(iSharedFunctionalGroupsSequence,0,false,
									encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
									encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType,
									encountered_RotationDirection,shared_RotationDirection,perFrame_RotationDirection,
									encountered_RevolutionTime,shared_RevolutionTime,perFrame_RevolutionTime,
									encountered_SingleCollimationWidth,shared_SingleCollimationWidth,perFrame_SingleCollimationWidth,
									encountered_TotalCollimationWidth,shared_TotalCollimationWidth,perFrame_TotalCollimationWidth,
									encountered_TableHeight,shared_TableHeight,perFrame_TableHeight,
									encountered_GantryDetectorTilt,shared_GantryDetectorTilt,perFrame_GantryDetectorTilt,
									encountered_DataCollectionDiameter,shared_DataCollectionDiameter,perFrame_DataCollectionDiameter);
							}
							if (perFrame_TableSpeed == NULL
							 && perFrame_TableFeedPerRotation == NULL
							 && perFrame_SpiralPitchFactor == NULL) {
								makeCTTableDynamicsSequence(iSharedFunctionalGroupsSequence,0,false,
									encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
									encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType,
									encountered_TableSpeed,shared_TableSpeed,perFrame_TableSpeed,
									encountered_TableFeedPerRotation,shared_TableFeedPerRotation,perFrame_TableFeedPerRotation,
									encountered_SpiralPitchFactor,shared_SpiralPitchFactor,perFrame_SpiralPitchFactor);
							}
							if (perFrame_PatientPosition == NULL
							 && perFrame_PixelSpacing == NULL
							 && perFrame_SliceLocation == NULL
							 && perFrame_TablePosition == NULL
							 && perFrame_ImagePositionPatient == NULL
							 && perFrame_ImageOrientationPatient == NULL
							 && perFrame_DataCollectionCenterPatient == NULL
							 && perFrame_ReconstructionTargetCenterPatient == NULL) {
								makeCTPositionSequence(iSharedFunctionalGroupsSequence,0,false,
									rows,columns,
									reference_ImagePositionPatient,
									encountered_PatientPosition,shared_PatientPosition,perFrame_PatientPosition,
									encountered_PixelSpacing,shared_PixelSpacing,perFrame_PixelSpacing,
									encountered_SliceLocation,shared_SliceLocation,perFrame_SliceLocation,
									encountered_TablePosition,shared_TablePosition,perFrame_TablePosition,
									encountered_ImagePositionPatient,shared_ImagePositionPatient,perFrame_ImagePositionPatient,
									encountered_ImageOrientationPatient,shared_ImageOrientationPatient,perFrame_ImageOrientationPatient,
									encountered_DataCollectionCenterPatient,shared_DataCollectionCenterPatient,perFrame_DataCollectionCenterPatient,
									encountered_ReconstructionTargetCenterPatient,shared_ReconstructionTargetCenterPatient,perFrame_ReconstructionTargetCenterPatient);
							}
							if (perFrame_DistanceSourceToDetector == NULL
							 && perFrame_DistanceSourceToDataCollectionCenter == NULL) {
								makeCTGeometrySequence(iSharedFunctionalGroupsSequence,0,false,
									encountered_DistanceSourceToDetector,shared_DistanceSourceToDetector,perFrame_DistanceSourceToDetector,
									encountered_DistanceSourceToDataCollectionCenter,shared_DistanceSourceToDataCollectionCenter,
																perFrame_DistanceSourceToDataCollectionCenter,
									encountered_DistanceSourceToPatient,shared_DistanceSourceToPatient,perFrame_DistanceSourceToPatient);
							}
							if (perFrame_ReconstructionAlgorithm == NULL
							 && perFrame_ConvolutionKernel == NULL
							 && perFrame_ConvolutionKernelGroup == NULL
							 && perFrame_ReconstructionDiameter == NULL
							 && perFrame_ReconstructionFieldOfView == NULL
							 && perFrame_ReconstructionPixelSpacing == NULL
							 && perFrame_ReconstructionAngle == NULL
							 && perFrame_ImageFilter == NULL) {
								makeCTReconstructionSequence(iSharedFunctionalGroupsSequence,0,false,
									rows,columns,
									encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
									encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType,
									encountered_PixelSpacing,shared_PixelSpacing,perFrame_PixelSpacing,
									encountered_ReconstructionAlgorithm,shared_ReconstructionAlgorithm,perFrame_ReconstructionAlgorithm,
									encountered_ConvolutionKernel,shared_ConvolutionKernel,perFrame_ConvolutionKernel,
									encountered_ConvolutionKernelGroup,shared_ConvolutionKernelGroup,perFrame_ConvolutionKernelGroup,
									encountered_ReconstructionDiameter,shared_ReconstructionDiameter,perFrame_ReconstructionDiameter,
									encountered_ReconstructionFieldOfView,shared_ReconstructionFieldOfView,perFrame_ReconstructionFieldOfView,
									encountered_ReconstructionPixelSpacing,shared_ReconstructionPixelSpacing,perFrame_ReconstructionPixelSpacing,
									encountered_ReconstructionAngle,shared_ReconstructionAngle,perFrame_ReconstructionAngle,
									encountered_ImageFilter,shared_ImageFilter,perFrame_ImageFilter);
							}
							if (perFrame_ExposureTime == NULL
							 && perFrame_XRayTubeCurrent == NULL
							 && perFrame_Exposure == NULL
							 && perFrame_ExposureInuAs == NULL
							 && perFrame_ExposureTimeInms == NULL
							 && perFrame_XRayTubeCurrentInmA == NULL
							 && perFrame_ExposureInmAs == NULL
							 && perFrame_ExposureModulationType == NULL
							 && perFrame_EstimatedDoseSaving == NULL
							 && perFrame_CTDIvol == NULL) {
								makeCTExposureSequence(iSharedFunctionalGroupsSequence,0,false,
									encountered_ExposureTime,shared_ExposureTime,perFrame_ExposureTime,
									encountered_XRayTubeCurrent,shared_XRayTubeCurrent,perFrame_XRayTubeCurrent,
									encountered_Exposure,shared_Exposure,perFrame_Exposure,
									encountered_ExposureInuAs,shared_ExposureInuAs,perFrame_ExposureInuAs,
									encountered_ExposureTimeInms,shared_ExposureTimeInms,perFrame_ExposureTimeInms,
									encountered_XRayTubeCurrentInmA,shared_XRayTubeCurrentInmA,perFrame_XRayTubeCurrentInmA,
									encountered_ExposureInmAs,shared_ExposureInmAs,perFrame_ExposureInmAs,
									encountered_ExposureModulationType,shared_ExposureModulationType,perFrame_ExposureModulationType,
									encountered_EstimatedDoseSaving,shared_EstimatedDoseSaving,perFrame_EstimatedDoseSaving,
									encountered_CTDIvol,shared_CTDIvol,perFrame_CTDIvol);
							}
							if (perFrame_KVP == NULL
							 && perFrame_FocalSpots == NULL
							 && perFrame_FilterType == NULL
							 && perFrame_FilterMaterial == NULL) {
								makeCTXRayDetailsSequence(iSharedFunctionalGroupsSequence,0,false,
									encountered_KVP,shared_KVP,perFrame_KVP,
									encountered_FocalSpots,shared_FocalSpots,perFrame_FocalSpots,
									encountered_FilterType,shared_FilterType,perFrame_FilterType,
									encountered_FilterMaterial,shared_FilterMaterial,perFrame_FilterMaterial);
							}
						}
 					}
					if (addMultiFrameMRStuff) {
						// if we need it regardless or we saw one of them
						// && the descriptors are not varying on per-frame basis, add a shared MR Metabolite Map Sequence
						if (((shared_ImageType != NULL && nValues_ImageType >= 3 && strcmp(shared_ImageType[2],"METABOLITE_MAP") == 0)
						      || encountered_MetaboliteMapDescription
						      || encountered_ChemicalShiftMinimumIntegrationLimitInppm
						      || encountered_ChemicalShiftMaximumIntegrationLimitInppm)
						    && perFrame_MetaboliteMapDescription == NULL
						    && perFrame_ChemicalShiftMinimumIntegrationLimitInppm == NULL
						    && perFrame_ChemicalShiftMaximumIntegrationLimitInppm == NULL) {
							AttributeList *iMRMetaboliteMapSequence = makeNewSequenceAttributeWithItem(
								iSharedFunctionalGroupsSequence,TagFromName(MRMetaboliteMapSequence));
							
							const char *use_MetaboliteMapDescription = encountered_MetaboliteMapDescription ? shared_MetaboliteMapDescription : "";
							(*iMRMetaboliteMapSequence)+=new ShortTextAttribute(TagFromName(MetaboliteMapDescription),use_MetaboliteMapDescription);
							addMetaboliteMapCodeSequence(iMRMetaboliteMapSequence,use_MetaboliteMapDescription);
							
							if (encountered_ChemicalShiftMinimumIntegrationLimitInppm && encountered_ChemicalShiftMaximumIntegrationLimitInppm) { // both are Type 1
								AttributeList *iChemicalShiftSequence = makeNewSequenceAttributeWithItem(
									iMRMetaboliteMapSequence,TagFromName(ChemicalShiftSequence));
								(*iChemicalShiftSequence)+=new FloatDoubleAttribute(TagFromName(ChemicalShiftMinimumIntegrationLimitInppm),
									shared_ChemicalShiftMinimumIntegrationLimitInppm);
								(*iChemicalShiftSequence)+=new FloatDoubleAttribute(TagFromName(ChemicalShiftMaximumIntegrationLimitInppm),
									shared_ChemicalShiftMaximumIntegrationLimitInppm);
							}
						}					

						//if (addMROriginalOrMixedStuff) {
						if (!imageIsDerived || addNonDerivedStuffAnyway) {	// NB. not !frameIsDerived, since values may be shared but frame type value 1 varies per-frame
//cerr << "Adding MR-specific stuff for non-derived image at shared level" << endl;
							if (perFrame_RepetitionTime == NULL
							 && perFrame_FlipAngle == NULL
							 && perFrame_EchoTrainLength == NULL
							 && perFrame_RFEchoTrainLength == NULL
							 && perFrame_GradientEchoTrainLength == NULL
							 && perFrame_SAR == NULL) {
								makeMRTimingAndRelatedParametersSequence(iSharedFunctionalGroupsSequence,0,false,vEchoPulseSequence,
									encountered_RepetitionTime,shared_RepetitionTime,perFrame_RepetitionTime,
									encountered_FlipAngle,shared_FlipAngle,perFrame_FlipAngle,
									encountered_EchoTrainLength,shared_EchoTrainLength,perFrame_EchoTrainLength,
									encountered_RFEchoTrainLength,shared_RFEchoTrainLength,perFrame_RFEchoTrainLength,
									encountered_GradientEchoTrainLength,shared_GradientEchoTrainLength,perFrame_GradientEchoTrainLength,
									encountered_SAR,shared_SAR,perFrame_SAR,
									encountered_GradientOutputType,shared_GradientOutputType,perFrame_GradientOutputType,
									encountered_GradientOutput,shared_GradientOutput,perFrame_GradientOutput);
							}
							if (perFrame_EchoTime == NULL) {
								AttributeList *iMREchoSequence = makeNewSequenceAttributeWithItem(
									iSharedFunctionalGroupsSequence,TagFromName(MREchoSequence));
								if (encountered_EchoTime)
									(*iMREchoSequence)+=new FloatDoubleAttribute(TagFromName(EffectiveEchoTime),shared_EchoTime);
							}
							if (perFrame_InPlanePhaseEncodingDirection == NULL
							 && perFrame_NumberOfPhaseEncodingSteps == NULL
							 && perFrame_PercentSampling == NULL
							 && perFrame_PercentPhaseFieldOfView == NULL
							 && perFrame_AcquisitionMatrix == NULL) {
								makeMRFOVGeometrySequence(iSharedFunctionalGroupsSequence,0,false,requiredMRAcquisitionType,
									encountered_MRAcquisitionPhaseEncodingStepsOutOfPlane,shared_MRAcquisitionPhaseEncodingStepsOutOfPlane,
									perFrame_MRAcquisitionPhaseEncodingStepsOutOfPlane,
									encountered_InPlanePhaseEncodingDirection,shared_InPlanePhaseEncodingDirection,perFrame_InPlanePhaseEncodingDirection,
									encountered_NumberOfPhaseEncodingSteps,shared_NumberOfPhaseEncodingSteps,perFrame_NumberOfPhaseEncodingSteps,
									encountered_PercentSampling,shared_PercentSampling,perFrame_PercentSampling,
									encountered_PercentPhaseFieldOfView,shared_PercentPhaseFieldOfView,perFrame_PercentPhaseFieldOfView,
									encountered_AcquisitionMatrix,shared_AcquisitionMatrix,perFrame_AcquisitionMatrix);
							}
						
							if (perFrame_ScanningSequence == NULL
							 && perFrame_SequenceVariant == NULL
							 && perFrame_ScanOptions == NULL) {
								char **useScanningSequence = encountered_ScanningSequence ? shared_ScanningSequence : NULL;
								char **useSequenceVariant = encountered_SequenceVariant ? shared_SequenceVariant : NULL;
								char **useScanOptions = encountered_ScanOptions ? shared_ScanOptions : NULL;
								
								makeMRModifierSequence(iSharedFunctionalGroupsSequence,0,false,
									encountered_InversionTime,shared_InversionTime,perFrame_InversionTime,
									useScanningSequence && arrayOfStringValuesContains(useScanningSequence,nValues_ScanningSequence,"GR"),
									useScanningSequence && arrayOfStringValuesContains(useScanningSequence,nValues_ScanningSequence,"IR"),
									useScanOptions && arrayOfStringValuesContains(useScanOptions,nValues_ScanOptions,"FC"),
									useSequenceVariant && arrayOfStringValuesContains(useSequenceVariant,nValues_SequenceVariant,"SP"),
									useScanOptions && arrayOfStringValuesContains(useScanOptions,nValues_ScanOptions,"SP"),
									useScanOptions && arrayOfStringValuesContains(useScanOptions,nValues_ScanOptions,"PFF"),
									useScanOptions && arrayOfStringValuesContains(useScanOptions,nValues_ScanOptions,"PFP"));
							}
							if (perFrame_SequenceVariant == NULL
							 && perFrame_ImagingFrequency == NULL
							 && perFrame_PixelBandwidth == NULL) {
								char  **useSequenceVariant =  encountered_SequenceVariant ? shared_SequenceVariant : NULL;
								double useImagingFrequency = encountered_ImagingFrequency ? shared_ImagingFrequency : 0;
								double   usePixelBandwidth =   encountered_PixelBandwidth ? shared_PixelBandwidth : 0;
								
								makeMRImagingModifierSequence(iSharedFunctionalGroupsSequence,false,
									useSequenceVariant && arrayOfStringValuesContains(useSequenceVariant,nValues_SequenceVariant,"MTC"),
									useImagingFrequency,
									usePixelBandwidth);
							}
							if (perFrame_ReceiveCoilName == NULL) {
								AttributeList *iMRReceiveCoilSequence = makeNewSequenceAttributeWithItem(
									iSharedFunctionalGroupsSequence,TagFromName(MRReceiveCoilSequence));
								const char *useReceiveCoilName = encountered_ReceiveCoilName && shared_ReceiveCoilName ? shared_ReceiveCoilName : "NONAME";
								const char *useManufacturer = minimalAttributesOnly ? "" : (encountered_Manufacturer && !perFrame_Manufacturer && shared_Manufacturer ? shared_Manufacturer : "");
								(*iMRReceiveCoilSequence)+=new ShortStringAttribute(TagFromName(ReceiveCoilName),useReceiveCoilName);
								(*iMRReceiveCoilSequence)+=new LongStringAttribute(TagFromName(ReceiveCoilManufacturerName),useManufacturer);
								(*iMRReceiveCoilSequence)+=new CodeStringAttribute(TagFromName(QuadratureReceiveCoil),
									encountered_ReceiveCoilName && findInStringRegardlessOfCase(useReceiveCoilName,"QUAD") ? "YES" : "NO");
								(*iMRReceiveCoilSequence)+=new CodeStringAttribute(TagFromName(ReceiveCoilType),
									encountered_ReceiveCoilName ? makeCoilTypeFromName(useReceiveCoilName) : "UNKNOWN");	// default is not a standard defined term
							}
							if (perFrame_TransmitCoilName == NULL) {
								AttributeList *iMRTransmitCoilSequence = makeNewSequenceAttributeWithItem(
									iSharedFunctionalGroupsSequence,TagFromName(MRTransmitCoilSequence));
								const char *useTransmitCoilName = encountered_TransmitCoilName && shared_TransmitCoilName ? shared_TransmitCoilName : "NONAME";
								const char *useManufacturer = minimalAttributesOnly ? "" : (encountered_Manufacturer && !perFrame_Manufacturer && shared_Manufacturer ? shared_Manufacturer : "");
								(*iMRTransmitCoilSequence)+=new ShortStringAttribute(TagFromName(TransmitCoilName),useTransmitCoilName);
								(*iMRTransmitCoilSequence)+=new LongStringAttribute(TagFromName(TransmitCoilManufacturerName),useManufacturer);
								(*iMRTransmitCoilSequence)+=new CodeStringAttribute(TagFromName(TransmitCoilType),
									encountered_TransmitCoilName ? makeCoilTypeFromName(useTransmitCoilName) : "UNKNOWN");	// default is not a standard defined term
							}
							if (perFrame_NumberOfAverages == NULL) {
								AttributeList *iMRAveragesSequence = makeNewSequenceAttributeWithItem(
									iSharedFunctionalGroupsSequence,TagFromName(MRAveragesSequence));
								(*iMRAveragesSequence)+=new DecimalStringAttribute(TagFromName(NumberOfAverages),encountered_NumberOfAverages ? shared_NumberOfAverages : double(1));
							}
							// Do we need MRDiffusionSequence in Shared Functional Groups ?
							if (encountered_AcquisitionContrast && perFrame_AcquisitionContrast == NULL
							 && shared_AcquisitionContrast != NULL && strcmp(shared_AcquisitionContrast,"DIFFUSION") == 0
							 && perFrame_DiffusionBValue == NULL && perFrame_DiffusionDirectionality == NULL) {
								double use_DiffusionBValue = encountered_DiffusionBValue ? shared_DiffusionBValue : 0;
								const char *use_DiffusionDirectionality = encountered_DiffusionDirectionality ? shared_DiffusionDirectionality : "ISOTROPIC";
								makeMRDiffusionSequence(iSharedFunctionalGroupsSequence,false,
									use_DiffusionBValue,
									use_DiffusionDirectionality);
							}
							if (phaseContrast) {
								if (perFrame_VelocityEncodingDirection == NULL
								 && perFrame_VelocityEncodingMinimumValue == NULL
								 && perFrame_VelocityEncodingMaximumValue == NULL) {
									double   *useVelocityEncodingDirection = encountered_VelocityEncodingDirection    ? shared_VelocityEncodingDirection : NULL;
									double useVelocityEncodingMinimumValue = encountered_VelocityEncodingMinimumValue ? shared_VelocityEncodingMinimumValue : 0;
									double useVelocityEncodingMaximumValue = encountered_VelocityEncodingMaximumValue ? shared_VelocityEncodingMaximumValue : 0;
									makeMRVelocityEncodingSequence(iSharedFunctionalGroupsSequence,false,
										useVelocityEncodingDirection,useVelocityEncodingMinimumValue,useVelocityEncodingMaximumValue);
								}
							}
						}
					}
					
//cerr << "adding per-frame functional groups" << endl;
					int j;
                                        for (j=0; j<numberofinputfiles; ++j) {
//cerr << "added new item to PerFrameFunctionalGroupsSequence: frame " << dec << j << " filename " << filename << endl;
                                                AttributeList *iPerFrameFunctionalGroupsSequence = new AttributeList();
                                                Assert(iPerFrameFunctionalGroupsSequence);
                                                (*aPerFrameFunctionalGroupsSequence)+=iPerFrameFunctionalGroupsSequence;
						if (perFrame_NominalCardiacTriggerDelayTime != NULL
						 || perFrame_TriggerTime != NULL
						 || perFrame_RRIntervalTimeNominal != NULL
						 /*|| perFrame_TriggerWindow != NULL*/) {
//cerr << "adding per-frame CardiacSynchronizationSequence" << endl;
							AttributeList *iCardiacSynchronizationSequence = makeNewSequenceAttributeWithItem(
								iPerFrameFunctionalGroupsSequence,TagFromName(CardiacSynchronizationSequence));
							double use_TriggerTime = perFrame_TriggerTime ? perFrame_TriggerTime[j] : shared_TriggerTime;
							double use_NominalCardiacTriggerDelayTime = perFrame_NominalCardiacTriggerDelayTime ? perFrame_NominalCardiacTriggerDelayTime[j] : shared_NominalCardiacTriggerDelayTime;
							use_NominalCardiacTriggerDelayTime = encountered_TriggerTime ?  use_TriggerTime : use_NominalCardiacTriggerDelayTime;
							(*iCardiacSynchronizationSequence)+=new FloatDoubleAttribute(TagFromName(NominalCardiacTriggerDelayTime),use_NominalCardiacTriggerDelayTime);

							double use_RRIntervalTimeNominal = perFrame_RRIntervalTimeNominal ? perFrame_RRIntervalTimeNominal[j] : shared_RRIntervalTimeNominal;
							if (encountered_RRIntervalTimeNominal) {
								(*iCardiacSynchronizationSequence)+=new FloatDoubleAttribute(TagFromName(RRIntervalTimeNominal),shared_RRIntervalTimeNominal);
							}
							
							//double use_TriggerWindow = perFrame_TriggerWindow ? perFrame_TriggerWindow[j] : shared_TriggerWindow;
							//if (encountered_TriggerWindow) {
							//	(*iCardiacSynchronizationSequence)+=new IntegerStringAttribute(TagFromName(TriggerWindow),Uint16(use_TriggerWindow));
							//}
						}
						if ((perFrame_WindowCenter != NULL
						 || perFrame_WindowWidth != NULL
						 || perFrame_WindowCenterWidthExplanation != NULL) && !nowindow) {
//cerr << "adding per-frame FrameVOILUTSequence" << endl;
							AttributeList *iFrameVOILUTSequence = makeNewSequenceAttributeWithItem(
								iPerFrameFunctionalGroupsSequence,TagFromName(FrameVOILUTSequence));
							(*iFrameVOILUTSequence)+=new DecimalStringAttribute(TagFromName(WindowCenter),
								encountered_WindowCenter ? (perFrame_WindowCenter ? perFrame_WindowCenter[j] : shared_WindowCenter) : defaultWindowCenter);
							(*iFrameVOILUTSequence)+=new DecimalStringAttribute(TagFromName(WindowWidth),
								encountered_WindowWidth ? (perFrame_WindowWidth ? perFrame_WindowWidth[j] : shared_WindowWidth) : defaultWindowWidth);
							const char *use_explanation = encountered_WindowCenterWidthExplanation
								? (perFrame_WindowCenterWidthExplanation ? perFrame_WindowCenterWidthExplanation[j] : shared_WindowCenterWidthExplanation)
								: NULL;
							if (use_explanation) {
								(*iFrameVOILUTSequence)+=new LongStringAttribute(TagFromName(WindowCenterWidthExplanation),use_explanation);
							}
						}
						if (perFrame_RescaleSlope != NULL
						 || perFrame_RescaleIntercept != NULL
						 || perFrame_RescaleType != NULL) {
//cerr << "adding per-frame PixelValueTransformationSequence" << endl;
							AttributeList *iPixelValueTransformationSequence = makeNewSequenceAttributeWithItem(
								iPerFrameFunctionalGroupsSequence,TagFromName(PixelValueTransformationSequence));
							double use_intercept = encountered_RescaleIntercept ? (perFrame_RescaleIntercept ? perFrame_RescaleIntercept[j] : shared_RescaleIntercept) : 0.0;
							double use_slope = encountered_RescaleSlope ? (perFrame_RescaleSlope ? perFrame_RescaleSlope[j] : shared_RescaleSlope) : 1.0;
							if (use_slope == 0) {
								use_slope=1.0;	// a slope of zero means it was probably missing for that frame
								use_intercept=0.0;
							}
							const char *use_type = encountered_RescaleType ? (perFrame_RescaleType ? perFrame_RescaleType[j] : shared_RescaleType) : NULL;
							use_type=fixRescaleTypeForCTIfNecessary(use_type,addMultiFrameCTStuff,imageIsDerived);
//cerr << "adding RescaleIntercept" << endl;
							(*iPixelValueTransformationSequence)+=new DecimalStringAttribute(TagFromName(RescaleIntercept),use_intercept);
//cerr << "adding RescaleSlope" << endl;
							(*iPixelValueTransformationSequence)+=new DecimalStringAttribute(TagFromName(RescaleSlope),use_slope);
//cerr << "adding RescaleType = " << use_type << endl;
							(*iPixelValueTransformationSequence)+=new LongStringAttribute(TagFromName(RescaleType),use_type);
						}
						if (perFrame_RealWorldValueSlope != NULL
						 || perFrame_RealWorldValueIntercept != NULL
						 || perFrame_Units != NULL
						 || perFrame_RealWorldValueFirstValueMapped != NULL
						 || perFrame_RealWorldValueLastValueMapped != NULL
						 || perFrame_LUTExplanation != NULL
						 || perFrame_LUTLabel != NULL
						) {
//cerr << "adding per-frame RealWorldValueMappingSequence" << endl;
							makeRealWorldValueMappingSequence(iPerFrameFunctionalGroupsSequence,j,pixelRepresentation,
								encountered_RealWorldValueSlope,shared_RealWorldValueSlope,perFrame_RealWorldValueSlope,
								encountered_RealWorldValueIntercept,shared_RealWorldValueIntercept,perFrame_RealWorldValueIntercept,
								encountered_Units,shared_Units,perFrame_Units,
								encountered_RealWorldValueFirstValueMapped,shared_RealWorldValueFirstValueMapped,perFrame_RealWorldValueFirstValueMapped,
								encountered_RealWorldValueLastValueMapped,shared_RealWorldValueLastValueMapped,perFrame_RealWorldValueLastValueMapped,
								encountered_LUTExplanation,shared_LUTExplanation,perFrame_LUTExplanation,
								encountered_LUTLabel,shared_LUTLabel,perFrame_LUTLabel);
						}
						if (addDerivation && forceDerivationPerFrame) {
							makeDerivationImageSequence(
								iPerFrameFunctionalGroupsSequence,
								minimalAttributesOnly,
								addDerivation_derivationCodeSequenceCodingSchemeDesignator,
								addDerivation_derivationCodeSequenceCodeValue,
								addDerivation_derivationCodeSequenceCodeMeaning,
								addDerivation_derivationCodeSequenceCodingSchemeVersion,
								addDerivation_referencedSOPClassUID,
								addDerivation_referencedSOPInstanceUID,
								addDerivation_referencedFrameNumbers,
								addDerivation_purposeOfReferenceCodeSequenceCodingSchemeDesignator,
								addDerivation_purposeOfReferenceCodeSequenceCodeValue,
								addDerivation_purposeOfReferenceCodeSequenceCodeMeaning,
								addDerivation_purposeOfReferenceCodeSequenceCodingSchemeVersion);
						}
						if (addReferenced && forceReferencedPerFrame) {
							makeReferencedImageSequence(
								iPerFrameFunctionalGroupsSequence,
								minimalAttributesOnly,
								addReferenced_referencedSOPClassUID,
								addReferenced_referencedSOPInstanceUID,
								addReferenced_referencedFrameNumbers,
								addReferenced_purposeOfReferenceCodeSequenceCodingSchemeDesignator,
								addReferenced_purposeOfReferenceCodeSequenceCodeValue,
								addReferenced_purposeOfReferenceCodeSequenceCodeMeaning,
								addReferenced_purposeOfReferenceCodeSequenceCodingSchemeVersion);
						}
						if (!frameAnatomySequenceDoneAtSharedLevel) {
//cerr << "adding per-frame FrameAnatomySequence" << endl;
							makeFrameAnatomySequence(iPerFrameFunctionalGroupsSequence,
								true /*frameLevel*/,j,
								encountered_BodyPartExamined,shared_BodyPartExamined,perFrame_BodyPartExamined,
								encountered_StudyDescription,shared_StudyDescription,perFrame_StudyDescription,
								encountered_SeriesDescription,shared_SeriesDescription,perFrame_SeriesDescription,
								encountered_ImageComments,shared_ImageComments,perFrame_ImageComments,
								encountered_PatientName,shared_PatientName,perFrame_PatientName,
								encountered_Laterality,shared_Laterality,perFrame_Laterality);
						}
						if (addMultiFrameMRStuff) {
							if (perFrame_MetaboliteMapDescription != NULL
							 || perFrame_ChemicalShiftMinimumIntegrationLimitInppm != NULL
							 || perFrame_ChemicalShiftMaximumIntegrationLimitInppm != NULL) {
								AttributeList *iMRMetaboliteMapSequence = makeNewSequenceAttributeWithItem(
									iPerFrameFunctionalGroupsSequence,TagFromName(MRMetaboliteMapSequence));
								const char *use_MetaboliteMapDescription = perFrame_MetaboliteMapDescription
									? perFrame_MetaboliteMapDescription[j] : (encountered_MetaboliteMapDescription
										? shared_MetaboliteMapDescription : "");
								double useChemicalShiftMinimumIntegrationLimitInppm = perFrame_ChemicalShiftMinimumIntegrationLimitInppm
										? perFrame_ChemicalShiftMinimumIntegrationLimitInppm[j] : shared_ChemicalShiftMinimumIntegrationLimitInppm;
								double useChemicalShiftMaximumIntegrationLimitInppm = perFrame_ChemicalShiftMaximumIntegrationLimitInppm
										? perFrame_ChemicalShiftMaximumIntegrationLimitInppm[j] : shared_ChemicalShiftMaximumIntegrationLimitInppm;
									
								(*iMRMetaboliteMapSequence)+=new ShortTextAttribute(TagFromName(MetaboliteMapDescription),use_MetaboliteMapDescription);
								addMetaboliteMapCodeSequence(iMRMetaboliteMapSequence,use_MetaboliteMapDescription);
								if (encountered_ChemicalShiftMinimumIntegrationLimitInppm && encountered_ChemicalShiftMaximumIntegrationLimitInppm) { // both are Type 1
									AttributeList *iChemicalShiftSequence = makeNewSequenceAttributeWithItem(
										iMRMetaboliteMapSequence,TagFromName(ChemicalShiftSequence));
									(*iChemicalShiftSequence)+=new FloatDoubleAttribute(TagFromName(ChemicalShiftMinimumIntegrationLimitInppm),
										useChemicalShiftMinimumIntegrationLimitInppm);
									(*iChemicalShiftSequence)+=new FloatDoubleAttribute(TagFromName(ChemicalShiftMaximumIntegrationLimitInppm),
										useChemicalShiftMaximumIntegrationLimitInppm);
								}
							}
						}
						
						bool frameIsDerived = imageIsDerived;	// may be over-ridden if there is a per-frame (rather than shared) MR Image Frame Type Macro
						
						if (perFrame_ImageType != NULL
						 || perFrame_PixelPresentation != NULL
						 || perFrame_VolumetricProperties != NULL
						 || perFrame_VolumeBasedCalculationTechnique != NULL
						 || perFrame_ComplexImageComponent != NULL
						 || perFrame_AcquisitionContrast != NULL) {
//cerr << "adding per-frame MR or CT Image Frame Type Macro" << endl;
							AttributeList *iImageFrameTypeSequence = makeNewSequenceAttributeWithItem(
								iPerFrameFunctionalGroupsSequence,
								addMultiFrameMRStuff ? TagFromName(MRImageFrameTypeSequence) : TagFromName(CTImageFrameTypeSequence));
							
							// include the Common CT MR and MR Image Description Macro, using Frame Type not Image Type at this level
							// (the same macro is also used in the Enhanced MR and CT Image Modules)
							
							frameIsDerived = makeCTMRImageDescriptionMacroAttributesAndAppendToList(addMultiFrameMRStuff,
								*iImageFrameTypeSequence,true /* frame level */,j,numberofinputfiles,
								encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
								encountered_PixelPresentation,shared_PixelPresentation,perFrame_PixelPresentation,
								encountered_VolumetricProperties,shared_VolumetricProperties,perFrame_VolumetricProperties,
								encountered_VolumeBasedCalculationTechnique,shared_VolumeBasedCalculationTechnique,perFrame_VolumeBasedCalculationTechnique,
								encountered_ComplexImageComponent,shared_ComplexImageComponent,perFrame_ComplexImageComponent,
								encountered_AcquisitionContrast,shared_AcquisitionContrast,perFrame_AcquisitionContrast,
								log);							
						}

						// The Frame Content Macro is always present
//cerr << "adding per-frame Frame Content Macro" << endl;
						{
							AttributeList *iFrameContentSequence = makeNewSequenceAttributeWithItem(
								iPerFrameFunctionalGroupsSequence,TagFromName(FrameContentSequence));
                                                        
                                                        if (!minimalAttributesOnly && encountered_AcquisitionNumber) {
                                                                (*iFrameContentSequence)+=new UnsignedShortAttribute(TagFromName(FrameAcquisitionNumber),
                                                                        perFrame_AcquisitionNumber ? perFrame_AcquisitionNumber[j] : shared_AcquisitionNumber);
                                                        }
                                                        
                                                        if (!frameIsDerived || addNonDerivedStuffAnyway || addTimingStuffAnyway) {
								double use_AcquisitionDuration;
								if (addMultiFrameMRStuff && deriveDurationFromTiming) {
									use_AcquisitionDuration=computeAcquisitionDurationInMilliSecondsFromTiming(
										encountered_RepetitionTime,shared_RepetitionTime,perFrame_RepetitionTime,
										encountered_NumberOfAverages,shared_NumberOfAverages,perFrame_NumberOfAverages,
										encountered_AcquisitionMatrix,shared_AcquisitionMatrix,perFrame_AcquisitionMatrix,
										encountered_EchoTrainLength,shared_EchoTrainLength,perFrame_EchoTrainLength,
										encountered_ScanningSequence,shared_ScanningSequence,perFrame_ScanningSequence,nValues_ScanningSequence,
										j);
//cerr << "use_AcquisitionDuration: derived " << use_AcquisitionDuration << endl;
								}
								else {
									use_AcquisitionDuration = encountered_AcquisitionDuration ? (perFrame_AcquisitionDuration ? perFrame_AcquisitionDuration[j] : shared_AcquisitionDuration) : 0;
									use_AcquisitionDuration=use_AcquisitionDuration*1000;	// image level is seconds, frame level is milliseconds
								}
								if (accumulateDuration) use_AcquisitionDuration=use_AcquisitionDuration*numberofinputfiles;
//cerr << "use_AcquisitionDuration: " << use_AcquisitionDuration << endl;
								if (encountered_AcquisitionDate && encountered_AcquisitionTime) {
									char *ad = perFrame_AcquisitionDate ? perFrame_AcquisitionDate[j] : shared_AcquisitionDate;
//cerr << "AcquisitionDate: " << ad << endl;
									char *at = perFrame_AcquisitionTime ? perFrame_AcquisitionTime[j] : shared_AcquisitionTime;
//cerr << "AcquisitionTime: " << at << endl;
									Uint32 acquisitionTime = Time(at);	// gets time in milliseconds
//cerr << "acquisitionTime: " << acquisitionTime << endl;
									Uint32 referenceTime = (Uint32)(acquisitionTime + (use_AcquisitionDuration+0.5)/2);
//cerr << "referenceTime: " << referenceTime << endl;
									if (accumulateDuration) {
										acquisitionTime=(Uint32)(acquisitionTime+use_AcquisitionDuration*j);
										referenceTime=(Uint32)(referenceTime+use_AcquisitionDuration*j);
									}
//cerr << "acquisitionTime: accumulated " << acquisitionTime << endl;
//cerr << "referenceTime: accumulated " << referenceTime << endl;
									char *adt = makeDateTimeString(ad,getStringFromTime(Time(acquisitionTime)));
//cerr << "AcquisitionDateTime: " << adt << endl;
									char *rdt = makeDateTimeString(ad,getStringFromTime(Time(referenceTime)));
//cerr << "ReferenceDateTime: " << rdt << endl;
									(*iFrameContentSequence)+=new DateTimeStringAttribute(TagFromName(FrameReferenceDateTime),rdt);
									(*iFrameContentSequence)+=new DateTimeStringAttribute(TagFromName(FrameAcquisitionDateTime),adt);
								}
								(*iFrameContentSequence)+=new FloatDoubleAttribute(TagFromName(FrameAcquisitionDuration),use_AcquisitionDuration);
                                                        }

                                                        if (!minimalAttributesOnly && encountered_ImageComments) {
                                                                Attribute *aFrameComments=new LongTextAttribute(TagFromName(FrameComments));
                                                                Assert(aFrameComments);
                                                                (*iFrameContentSequence)+=aFrameComments;
                                                                char *useImageComments = perFrame_ImageComments ? perFrame_ImageComments[j] : shared_ImageComments;
                                                                if (useImageComments && strlen(useImageComments) > 0) aFrameComments->addValue(useImageComments);
                                                        }

                                                        if (!minimalAttributesOnly && encountered_CardiacCyclePosition) {
                                                                Attribute *aCardiacCyclePosition=new CodeStringAttribute(TagFromName(CardiacCyclePosition));
                                                                Assert(aCardiacCyclePosition);
                                                                (*iFrameContentSequence)+=aCardiacCyclePosition;
                                                                char *useCardiacCyclePosition = perFrame_CardiacCyclePosition ? perFrame_CardiacCyclePosition[j] : shared_CardiacCyclePosition;
                                                                if (useCardiacCyclePosition && strlen(useCardiacCyclePosition) > 0) aCardiacCyclePosition->addValue(useCardiacCyclePosition);
                                                        }

                                                        if (!minimalAttributesOnly && encountered_RespiratoryCyclePosition) {
                                                                Attribute *aRespiratoryCyclePosition=new CodeStringAttribute(TagFromName(RespiratoryCyclePosition));
                                                                Assert(aRespiratoryCyclePosition);
                                                                (*iFrameContentSequence)+=aRespiratoryCyclePosition;
                                                                char *useRespiratoryCyclePosition = perFrame_RespiratoryCyclePosition ? perFrame_RespiratoryCyclePosition[j] : shared_RespiratoryCyclePosition;
                                                                if (useRespiratoryCyclePosition && strlen(useRespiratoryCyclePosition) > 0) aRespiratoryCyclePosition->addValue(useRespiratoryCyclePosition);
                                                        }

							if (!minimalAttributesOnly && makeStack && stackID && inStackPositionNumber) {
								(*iFrameContentSequence)+=new ShortStringAttribute(TagFromName(StackID),Uint32(stackID[j]));
								(*iFrameContentSequence)+=new UnsignedLongAttribute(TagFromName(InStackPositionNumber),inStackPositionNumber[j]);
							}
							if (!minimalAttributesOnly && addTemporalPosition && temporalPositionIndex) {
								(*iFrameContentSequence)+=new UnsignedLongAttribute(TagFromName(TemporalPositionIndex),temporalPositionIndex[j]);
							}
							
							// DimensionIndexValues can not be done yet ... have to first do functional groups for every frame, then add it later
                                                }

                                                if (perFrame_SliceThickness != NULL
						 || perFrame_PixelSpacing != NULL) {
//cerr << "adding per-frame PixelMeasuresSequence" << endl;
							AttributeList *iPixelMeasuresSequence = makeNewSequenceAttributeWithItem(
								iPerFrameFunctionalGroupsSequence,TagFromName(PixelMeasuresSequence));
							// NB. This is not predicated on any original or derived stuff,
							// since that is what the standard says, unlike the Plane Position or Orientation Macros
							{
								(*iPixelMeasuresSequence)+=new DecimalStringAttribute(TagFromName(SliceThickness),
									perFrame_SliceThickness ? perFrame_SliceThickness[j] : shared_SliceThickness);
                                                                
								Attribute *aPixelSpacing=new DecimalStringAttribute(TagFromName(PixelSpacing));
								Assert(aPixelSpacing);
								(*iPixelMeasuresSequence)+=aPixelSpacing;
								if (perFrame_PixelSpacing) {
									Assert(perFrame_PixelSpacing[j]);
									aPixelSpacing->addValue(perFrame_PixelSpacing[j][0]);
									aPixelSpacing->addValue(perFrame_PixelSpacing[j][1]);
								}
								else {
									Assert(shared_PixelSpacing);
									aPixelSpacing->addValue(shared_PixelSpacing[0]);
									aPixelSpacing->addValue(shared_PixelSpacing[1]);
								}
							}
                                                }
                                                if (perFrame_ImagePositionPatient != NULL) {
//cerr << "adding per-frame PlanePositionSequence" << endl;
							AttributeList *iPlanePositionSequence = makeNewSequenceAttributeWithItem(
								iPerFrameFunctionalGroupsSequence,TagFromName(PlanePositionSequence));
                                                        Assert(perFrame_ImagePositionPatient[j]);
                                                        if (!frameIsDerived || addNonDerivedStuffAnyway || addPositionStuffAnyway) (*iPlanePositionSequence)+=new DecimalStringAttribute(TagFromName(ImagePositionPatient),
                                                               perFrame_ImagePositionPatient[j][0],perFrame_ImagePositionPatient[j][1],perFrame_ImagePositionPatient[j][2]);
                                                }
                                                if (perFrame_ImageOrientationPatient != NULL) {
//cerr << "adding per-frame PlaneOrientationSequence" << endl;
							AttributeList *iPlaneOrientationSequence = makeNewSequenceAttributeWithItem(
								iPerFrameFunctionalGroupsSequence,TagFromName(PlaneOrientationSequence));
                                                        Assert(perFrame_ImageOrientationPatient[j]);
                                                        if (!frameIsDerived || addNonDerivedStuffAnyway || addPositionStuffAnyway) (*iPlaneOrientationSequence)+=new DecimalStringAttribute(TagFromName(ImageOrientationPatient),
                                                         	perFrame_ImageOrientationPatient[j][0],perFrame_ImageOrientationPatient[j][1],perFrame_ImageOrientationPatient[j][2],
                                                                perFrame_ImageOrientationPatient[j][3],perFrame_ImageOrientationPatient[j][4],perFrame_ImageOrientationPatient[j][5]);
                                                }
						if (addEnhancedContrastBolusModuleAndMacro) {
							// regardless of original or derived ...
							if (perFrame_ContrastBolusAgent != NULL
							 || perFrame_ContrastBolusRoute != NULL
							 || perFrame_ContrastBolusVolume != NULL
							 || perFrame_ContrastBolusIngredientConcentration != NULL
							 || perFrame_ContrastBolusAgentAdministered != NULL
							 || perFrame_ContrastBolusAgentDetected != NULL
							 || perFrame_ContrastBolusAgentPhase != NULL)
							 {
								makeContrastBolusUsageSequence(iPerFrameFunctionalGroupsSequence,j,
									encountered_ContrastBolusAgent,shared_ContrastBolusAgent,perFrame_ContrastBolusAgent,nValues_ContrastBolusAgent,
									encountered_ContrastBolusRoute,shared_ContrastBolusRoute,perFrame_ContrastBolusRoute,nValues_ContrastBolusRoute,
									encountered_ContrastBolusVolume,shared_ContrastBolusVolume,perFrame_ContrastBolusVolume,nValues_ContrastBolusVolume,
									encountered_ContrastBolusIngredientConcentration,shared_ContrastBolusIngredientConcentration,
										perFrame_ContrastBolusIngredientConcentration,nValues_ContrastBolusIngredientConcentration,
									encountered_ContrastBolusAgentAdministered,shared_ContrastBolusAgentAdministered,
										perFrame_ContrastBolusAgentAdministered,nValues_ContrastBolusAgentAdministered,
									encountered_ContrastBolusAgentDetected,shared_ContrastBolusAgentDetected,
										perFrame_ContrastBolusAgentDetected,nValues_ContrastBolusAgentDetected,
									encountered_ContrastBolusAgentPhase,shared_ContrastBolusAgentPhase,
										perFrame_ContrastBolusAgentPhase,nValues_ContrastBolusAgentPhase);
							}
						}
						if (addMultiFrameCTStuff) {
							{
								// Irradiation Event Identification is mandatory whether derived or not
								if (perFrame_IrradiationEventUID != NULL) {
//cerr << "adding per-frame IrradiationEventIdentificationSequence" << endl;
									AttributeList *iIrradiationEventIdentificationSequence = makeNewSequenceAttributeWithItem(
										iPerFrameFunctionalGroupsSequence,TagFromName(IrradiationEventIdentificationSequence));
									(*iIrradiationEventIdentificationSequence)+=new UIStringAttribute(TagFromName(IrradiationEventUID),perFrame_IrradiationEventUID[j]);
								}
							}
							if (!imageIsDerived || addNonDerivedStuffAnyway) {
								// need to add these regardless of whether original or mixed
								// and in the case of mixed the methods will add empty sequences for th derived frames
//cerr << "Adding CT-specific stuff for non-derived image at per-frame level" << endl;
								if (perFrame_AcquisitionType != NULL
								 || perFrame_TubeAngle != NULL
								 || perFrame_ConstantVolumeFlag != NULL
								 || perFrame_FluoroscopyFlag != NULL) {
//cerr << "adding per-frame CTAcquisitionTypeSequence" << endl << flush;
									makeCTAcquisitionTypeSequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
										encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType,
										encountered_TubeAngle,shared_TubeAngle,perFrame_TubeAngle,
										encountered_ConstantVolumeFlag,shared_ConstantVolumeFlag,perFrame_ConstantVolumeFlag,
										encountered_FluoroscopyFlag,shared_FluoroscopyFlag,perFrame_FluoroscopyFlag);
								}
								if (perFrame_RotationDirection != NULL
								 || perFrame_RevolutionTime != NULL
								 || perFrame_SingleCollimationWidth != NULL
								 || perFrame_TotalCollimationWidth != NULL
								 || perFrame_TableHeight != NULL
								 || perFrame_GantryDetectorTilt != NULL
								 || perFrame_DataCollectionDiameter != NULL) {
//cerr << "adding per-frame CTAcquisitionDetailsSequence" << endl << flush;
									makeCTAcquisitionDetailsSequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
										encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType,
										encountered_RotationDirection,shared_RotationDirection,perFrame_RotationDirection,
										encountered_RevolutionTime,shared_RevolutionTime,perFrame_RevolutionTime,
										encountered_SingleCollimationWidth,shared_SingleCollimationWidth,perFrame_SingleCollimationWidth,
										encountered_TotalCollimationWidth,shared_TotalCollimationWidth,perFrame_TotalCollimationWidth,
										encountered_TableHeight,shared_TableHeight,perFrame_TableHeight,
										encountered_GantryDetectorTilt,shared_GantryDetectorTilt,perFrame_GantryDetectorTilt,
										encountered_DataCollectionDiameter,shared_DataCollectionDiameter,perFrame_DataCollectionDiameter);
								}
								if (perFrame_TableSpeed != NULL
								 || perFrame_TableFeedPerRotation != NULL
								 || perFrame_SpiralPitchFactor != NULL) {
//cerr << "adding per-frame CTTableDynamicsSequence" << endl << flush;
									makeCTTableDynamicsSequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
										encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType,
										encountered_TableSpeed,shared_TableSpeed,perFrame_TableSpeed,
										encountered_TableFeedPerRotation,shared_TableFeedPerRotation,perFrame_TableFeedPerRotation,
										encountered_SpiralPitchFactor,shared_SpiralPitchFactor,perFrame_SpiralPitchFactor);
								}
								if (perFrame_PatientPosition != NULL
								 || perFrame_PixelSpacing != NULL
								 || perFrame_SliceLocation != NULL
								 || perFrame_TablePosition != NULL
								 || perFrame_ImagePositionPatient != NULL
								 || perFrame_ImageOrientationPatient != NULL
								 || perFrame_DataCollectionCenterPatient != NULL
								 || perFrame_ReconstructionTargetCenterPatient != NULL) {
//cerr << "adding per-frame CTPositionSequence" << endl << flush;
									makeCTPositionSequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										rows,columns,
										reference_ImagePositionPatient,
										encountered_PatientPosition,shared_PatientPosition,perFrame_PatientPosition,
										encountered_PixelSpacing,shared_PixelSpacing,perFrame_PixelSpacing,
										encountered_SliceLocation,shared_SliceLocation,perFrame_SliceLocation,
										encountered_TablePosition,shared_TablePosition,perFrame_TablePosition,
										encountered_ImagePositionPatient,shared_ImagePositionPatient,perFrame_ImagePositionPatient,
										encountered_ImageOrientationPatient,shared_ImageOrientationPatient,perFrame_ImageOrientationPatient,
										encountered_DataCollectionCenterPatient,shared_DataCollectionCenterPatient,perFrame_DataCollectionCenterPatient,
										encountered_ReconstructionTargetCenterPatient,shared_ReconstructionTargetCenterPatient,
											perFrame_ReconstructionTargetCenterPatient);
								}
								if (perFrame_DistanceSourceToDetector != NULL
								 || perFrame_DistanceSourceToDataCollectionCenter != NULL) {
									makeCTGeometrySequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										encountered_DistanceSourceToDetector,shared_DistanceSourceToDetector,perFrame_DistanceSourceToDetector,
										encountered_DistanceSourceToDataCollectionCenter,shared_DistanceSourceToDataCollectionCenter,
																	perFrame_DistanceSourceToDataCollectionCenter,
										encountered_DistanceSourceToPatient,shared_DistanceSourceToPatient,perFrame_DistanceSourceToPatient);
								}
								if (perFrame_ReconstructionAlgorithm != NULL
								 || perFrame_ConvolutionKernel != NULL
								 || perFrame_ConvolutionKernelGroup != NULL
								 || perFrame_ReconstructionDiameter != NULL
								 || perFrame_ReconstructionFieldOfView != NULL
								 || perFrame_ReconstructionPixelSpacing != NULL
								 || perFrame_ReconstructionAngle != NULL
								 || perFrame_ImageFilter != NULL) {
									makeCTReconstructionSequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										rows,columns,
										encountered_ImageType,shared_ImageType,perFrame_ImageType,nValues_ImageType,
										encountered_AcquisitionType,shared_AcquisitionType,perFrame_AcquisitionType,
										encountered_PixelSpacing,shared_PixelSpacing,perFrame_PixelSpacing,
										encountered_ReconstructionAlgorithm,shared_ReconstructionAlgorithm,perFrame_ReconstructionAlgorithm,
										encountered_ConvolutionKernel,shared_ConvolutionKernel,perFrame_ConvolutionKernel,
										encountered_ConvolutionKernelGroup,shared_ConvolutionKernelGroup,perFrame_ConvolutionKernelGroup,
										encountered_ReconstructionDiameter,shared_ReconstructionDiameter,perFrame_ReconstructionDiameter,
										encountered_ReconstructionFieldOfView,shared_ReconstructionFieldOfView,perFrame_ReconstructionFieldOfView,
										encountered_ReconstructionPixelSpacing,shared_ReconstructionPixelSpacing,perFrame_ReconstructionPixelSpacing,
										encountered_ReconstructionAngle,shared_ReconstructionAngle,perFrame_ReconstructionAngle,
										encountered_ImageFilter,shared_ImageFilter,perFrame_ImageFilter);
								}
								if (perFrame_ExposureTime != NULL
								 || perFrame_XRayTubeCurrent != NULL
								 || perFrame_Exposure != NULL
								 || perFrame_ExposureInuAs != NULL
								 || perFrame_ExposureTimeInms != NULL
								 || perFrame_XRayTubeCurrentInmA != NULL
								 || perFrame_ExposureInmAs != NULL
								 || perFrame_ExposureModulationType != NULL
								 || perFrame_EstimatedDoseSaving != NULL
								 || perFrame_CTDIvol != NULL) {
									makeCTExposureSequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										encountered_ExposureTime,shared_ExposureTime,perFrame_ExposureTime,
										encountered_XRayTubeCurrent,shared_XRayTubeCurrent,perFrame_XRayTubeCurrent,
										encountered_Exposure,shared_Exposure,perFrame_Exposure,
										encountered_ExposureInuAs,shared_ExposureInuAs,perFrame_ExposureInuAs,
										encountered_ExposureTimeInms,shared_ExposureTimeInms,perFrame_ExposureTimeInms,
										encountered_XRayTubeCurrentInmA,shared_XRayTubeCurrentInmA,perFrame_XRayTubeCurrentInmA,
										encountered_ExposureInmAs,shared_ExposureInmAs,perFrame_ExposureInmAs,
										encountered_ExposureModulationType,shared_ExposureModulationType,perFrame_ExposureModulationType,
										encountered_EstimatedDoseSaving,shared_EstimatedDoseSaving,perFrame_EstimatedDoseSaving,
										encountered_CTDIvol,shared_CTDIvol,perFrame_CTDIvol);
								}
								if (perFrame_KVP != NULL
								 || perFrame_FocalSpots != NULL
								 || perFrame_FilterType != NULL
								 || perFrame_FilterMaterial != NULL) {
									makeCTXRayDetailsSequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										encountered_KVP,shared_KVP,perFrame_KVP,
										encountered_FocalSpots,shared_FocalSpots,perFrame_FocalSpots,
										encountered_FilterType,shared_FilterType,perFrame_FilterType,
										encountered_FilterMaterial,shared_FilterMaterial,perFrame_FilterMaterial);
								}
							}
						}
						if (addMultiFrameMRStuff) {
							//if (addMROriginalOrMixedStuff) {
							//if (!frameIsDerived || addNonDerivedStuffAnyway) {
							if (!imageIsDerived || addNonDerivedStuffAnyway) {
								// need to add these regardless of whether original or mixed
								// and in the case of mixed the methods will add empty sequences for th derived frames
//cerr << "Adding MR-specific stuff for non-derived image at per-frame level" << endl;
								if (perFrame_RepetitionTime != NULL
								 || perFrame_FlipAngle != NULL
								 || perFrame_EchoTrainLength != NULL
								 || perFrame_RFEchoTrainLength != NULL
								 || perFrame_GradientEchoTrainLength != NULL
								 || perFrame_SAR != NULL) {
//cerr << "adding per-frame MRTimingAndRelatedParametersSequence" << endl << flush;
									makeMRTimingAndRelatedParametersSequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,vEchoPulseSequence,
										encountered_RepetitionTime,shared_RepetitionTime,perFrame_RepetitionTime,
										encountered_FlipAngle,shared_FlipAngle,perFrame_FlipAngle,
										encountered_EchoTrainLength,shared_EchoTrainLength,perFrame_EchoTrainLength,
										encountered_RFEchoTrainLength,shared_RFEchoTrainLength,perFrame_RFEchoTrainLength,
										encountered_GradientEchoTrainLength,shared_GradientEchoTrainLength,perFrame_GradientEchoTrainLength,
										encountered_SAR,shared_SAR,perFrame_SAR,
										encountered_GradientOutputType,shared_GradientOutputType,perFrame_GradientOutputType,
										encountered_GradientOutput,shared_GradientOutput,perFrame_GradientOutput);
								}
								if (perFrame_EchoTime != NULL) {
//cerr << "adding per-frame MREchoSequence" << endl;
									AttributeList *iMREchoSequence = makeNewSequenceAttributeWithItem(
										iPerFrameFunctionalGroupsSequence,TagFromName(MREchoSequence));
									/* if (!frameIsDerived || addNonDerivedStuffAnyway) */
										(*iMREchoSequence)+=new FloatDoubleAttribute(TagFromName(EffectiveEchoTime),perFrame_EchoTime[j]);
								}
//cerr << "adding per-frame MRFOVGeometrySequence" << endl;
								if (perFrame_InPlanePhaseEncodingDirection != NULL
								 || perFrame_NumberOfPhaseEncodingSteps != NULL
								 || perFrame_PercentSampling != NULL
								 || perFrame_PercentPhaseFieldOfView != NULL
								 || perFrame_AcquisitionMatrix != NULL) {
									makeMRFOVGeometrySequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										requiredMRAcquisitionType,
										encountered_MRAcquisitionPhaseEncodingStepsOutOfPlane,shared_MRAcquisitionPhaseEncodingStepsOutOfPlane,
										perFrame_MRAcquisitionPhaseEncodingStepsOutOfPlane,
										encountered_InPlanePhaseEncodingDirection,shared_InPlanePhaseEncodingDirection,perFrame_InPlanePhaseEncodingDirection,
										encountered_NumberOfPhaseEncodingSteps,shared_NumberOfPhaseEncodingSteps,perFrame_NumberOfPhaseEncodingSteps,
										encountered_PercentSampling,shared_PercentSampling,perFrame_PercentSampling,
										encountered_PercentPhaseFieldOfView,shared_PercentPhaseFieldOfView,perFrame_PercentPhaseFieldOfView,
										encountered_AcquisitionMatrix,shared_AcquisitionMatrix,perFrame_AcquisitionMatrix);
								}
//cerr << "adding per-frame MRModifierSequence" << endl;
								if (perFrame_ScanningSequence != NULL
								 || perFrame_SequenceVariant != NULL
								 || perFrame_ScanOptions != NULL) {
									char **useScanningSequence = encountered_ScanningSequence ? 
										(perFrame_ScanningSequence ? perFrame_ScanningSequence[j] : shared_ScanningSequence) : NULL;
									char **useSequenceVariant = encountered_SequenceVariant ? 
										(perFrame_SequenceVariant ? perFrame_SequenceVariant[j] : shared_SequenceVariant) : NULL;
									char **useScanOptions = encountered_ScanOptions ? 
										(perFrame_ScanOptions ? perFrame_ScanOptions[j] : shared_ScanOptions) : NULL;
								
									makeMRModifierSequence(iPerFrameFunctionalGroupsSequence,j,frameIsDerived && !addNonDerivedStuffAnyway,
										encountered_InversionTime,shared_InversionTime,perFrame_InversionTime,
										useScanningSequence && arrayOfStringValuesContains(useScanningSequence,nValues_ScanningSequence,"GR"),
										useScanningSequence && arrayOfStringValuesContains(useScanningSequence,nValues_ScanningSequence,"IR"),
										useScanOptions && arrayOfStringValuesContains(useScanOptions,nValues_ScanOptions,"FC"),
										useSequenceVariant && arrayOfStringValuesContains(useSequenceVariant,nValues_SequenceVariant,"SP"),
										useScanOptions && arrayOfStringValuesContains(useScanOptions,nValues_ScanOptions,"SP"),
										useScanOptions && arrayOfStringValuesContains(useScanOptions,nValues_ScanOptions,"PFF"),
										useScanOptions && arrayOfStringValuesContains(useScanOptions,nValues_ScanOptions,"PFP"));
								}
//cerr << "adding per-frame MRImagingModifierSequence" << endl;
								if (perFrame_SequenceVariant != NULL
								 || perFrame_ImagingFrequency != NULL
								 || perFrame_PixelBandwidth != NULL) {
									char  **useSequenceVariant =  encountered_SequenceVariant ?
										(perFrame_SequenceVariant ? perFrame_SequenceVariant[j] : shared_SequenceVariant) : NULL;
									double useImagingFrequency = encountered_ImagingFrequency ?
										(perFrame_ImagingFrequency ? perFrame_ImagingFrequency[j] : shared_ImagingFrequency) : 0;
									double   usePixelBandwidth =   encountered_PixelBandwidth ?
										(perFrame_PixelBandwidth ? perFrame_PixelBandwidth[j] : shared_PixelBandwidth) : 0;
								
									makeMRImagingModifierSequence(iPerFrameFunctionalGroupsSequence,frameIsDerived && !addNonDerivedStuffAnyway,
										useSequenceVariant && arrayOfStringValuesContains(useSequenceVariant,nValues_SequenceVariant,"MTC"),
										useImagingFrequency,
										usePixelBandwidth);
								}
//cerr << "adding per-frame MRReceiveCoilSequence" << endl;
								if (perFrame_ReceiveCoilName != NULL) {
									AttributeList *iMRReceiveCoilSequence = makeNewSequenceAttributeWithItem(
										iPerFrameFunctionalGroupsSequence,TagFromName(MRReceiveCoilSequence));
									/* if (!frameIsDerived || addNonDerivedStuffAnyway) */ {
										const char *useReceiveCoilName = perFrame_ReceiveCoilName[j] ? perFrame_ReceiveCoilName[j] : "NONAME";
										const char *useManufacturer = encountered_Manufacturer ? (perFrame_Manufacturer ? (perFrame_Manufacturer[j] ? perFrame_Manufacturer[j] : "") : shared_Manufacturer) : "";
										(*iMRReceiveCoilSequence)+=new ShortStringAttribute(TagFromName(ReceiveCoilName),useReceiveCoilName);
										(*iMRReceiveCoilSequence)+=new LongStringAttribute(TagFromName(ReceiveCoilManufacturerName),useManufacturer);
										(*iMRReceiveCoilSequence)+=new CodeStringAttribute(TagFromName(QuadratureReceiveCoil),
											findInStringRegardlessOfCase(useReceiveCoilName,"QUAD") ? "YES" : "NO");
										(*iMRReceiveCoilSequence)+=new CodeStringAttribute(TagFromName(ReceiveCoilType),
											makeCoilTypeFromName(useReceiveCoilName));
									}
								}
//cerr << "adding per-frame MRTransmitCoilSequence" << endl;
								if (perFrame_TransmitCoilName != NULL) {
									AttributeList *iMRTransmitCoilSequence = makeNewSequenceAttributeWithItem(
										iPerFrameFunctionalGroupsSequence,TagFromName(MRTransmitCoilSequence));
									/* if (!frameIsDerived || addNonDerivedStuffAnyway) */ {
										const char *useTransmitCoilName = perFrame_TransmitCoilName[j] ? perFrame_TransmitCoilName[j] : "NONAME";
										const char *useManufacturer = encountered_Manufacturer ? (perFrame_Manufacturer ? (perFrame_Manufacturer[j] ? perFrame_Manufacturer[j] : "") : shared_Manufacturer) : "";
										(*iMRTransmitCoilSequence)+=new ShortStringAttribute(TagFromName(TransmitCoilName),useTransmitCoilName);
										(*iMRTransmitCoilSequence)+=new LongStringAttribute(TagFromName(TransmitCoilManufacturerName),useManufacturer);
										(*iMRTransmitCoilSequence)+=new CodeStringAttribute(TagFromName(TransmitCoilType),
											makeCoilTypeFromName(useTransmitCoilName));
									}
								}
//cerr << "adding per-frame NumberOfAverages" << endl;
								if (perFrame_NumberOfAverages != NULL) {
									AttributeList *iMRAveragesSequence = makeNewSequenceAttributeWithItem(
										iPerFrameFunctionalGroupsSequence,TagFromName(MRAveragesSequence));
									/* if (!frameIsDerived || addNonDerivedStuffAnyway) */ (*iMRAveragesSequence)+=new DecimalStringAttribute(TagFromName(NumberOfAverages),perFrame_NumberOfAverages[j]);
								}
								// Do we need MRDiffusionSequence in Per-Frame Functional Groups ?
								if ((encountered_AcquisitionContrast && perFrame_AcquisitionContrast != NULL && strcmp(perFrame_AcquisitionContrast[j],"DIFFUSION") == 0)
								 || perFrame_DiffusionBValue != NULL
								 || perFrame_DiffusionDirectionality != NULL) {
									double use_DiffusionBValue = encountered_DiffusionBValue ? (perFrame_DiffusionBValue ? perFrame_DiffusionBValue[j] : shared_DiffusionBValue) : 0;
									const char *use_DiffusionDirectionality = encountered_DiffusionDirectionality ? (perFrame_DiffusionDirectionality ? perFrame_DiffusionDirectionality[j] : shared_DiffusionDirectionality) : "ISOTROPIC";
									makeMRDiffusionSequence(iPerFrameFunctionalGroupsSequence,frameIsDerived && !addNonDerivedStuffAnyway,
										use_DiffusionBValue,
										use_DiffusionDirectionality);
								}
								if (phaseContrast) {
									if (perFrame_VelocityEncodingDirection != NULL
									 || perFrame_VelocityEncodingMinimumValue != NULL
									 || perFrame_VelocityEncodingMaximumValue != NULL) {
										double   *useVelocityEncodingDirection = encountered_VelocityEncodingDirection    ? (perFrame_VelocityEncodingDirection ? perFrame_VelocityEncodingDirection[j] : shared_VelocityEncodingDirection) : NULL;
										double useVelocityEncodingMinimumValue = encountered_VelocityEncodingMinimumValue ? (perFrame_VelocityEncodingMinimumValue ? perFrame_VelocityEncodingMinimumValue[j] : shared_VelocityEncodingMinimumValue) : 0;
										double useVelocityEncodingMaximumValue = encountered_VelocityEncodingMaximumValue ? (perFrame_VelocityEncodingMaximumValue ? perFrame_VelocityEncodingMaximumValue[j] : shared_VelocityEncodingMaximumValue) : 0;
										makeMRVelocityEncodingSequence(iPerFrameFunctionalGroupsSequence,frameIsDerived && !addNonDerivedStuffAnyway,
										useVelocityEncodingDirection,useVelocityEncodingMinimumValue,useVelocityEncodingMaximumValue);
									}
								}
							}
						}
					}
					
					// Now need to pass through all functional groups for all frames building dimension index values ...
					
					if (nDimensionOrganizations) {
						
						// Step 1 ... build an organizations * dimensions * inputfiles array of all the double values, without trying to ascertain uniqueness yet
						
						double ***dimensionValues = new double**[nDimensionOrganizations];
						Assert(dimensionValues);
						for (int ido=0; ido<nDimensionOrganizations; ++ido) {
							dimensionValues[ido] = new double*[nDimensions[ido]];
							Assert(dimensionValues[ido]);
							for (int d=0; d<nDimensions[ido]; ++d) {
								dimensionValues[ido][d] = new double[numberofinputfiles];
								Assert(dimensionValues[ido][d]);
							}
						}
						
						AttributeList **iPerFrameFunctionalGroupsSequence;
						int n = aPerFrameFunctionalGroupsSequence->getLists(&iPerFrameFunctionalGroupsSequence);
						Assert(n == numberofinputfiles);
                                                Assert(iPerFrameFunctionalGroupsSequence);
	
						for (j=0; j<numberofinputfiles; ++j) {
							AttributeList *thisFramesFunctionalGroupItem = iPerFrameFunctionalGroupsSequence[j];
							Assert(thisFramesFunctionalGroupItem);
							for (int ido=0; ido<nDimensionOrganizations; ++ido) {
								for (int d=0; d<nDimensions[ido]; ++d) {
									SequenceAttribute *aFunctionalGroup =
										(SequenceAttribute *)((*thisFramesFunctionalGroupItem)[dimensionFunctionalGroupPointers[ido][d]]);
									Assert(aFunctionalGroup);
									AttributeList **iFunctionalGroup;
									int n = aFunctionalGroup->getLists(&iFunctionalGroup);
									Assert(n >= 1);					// might be more than one item, say ContrastBolusUsageSequence
									Assert(iFunctionalGroup);
									AttributeList *ifg = iFunctionalGroup[0];	// we use only the first item
									Assert(ifg);
									Attribute *a = (*ifg)[dimensionIndexPointers[ido][d]];
									Assert(a);
									double v;
									if (a->isNumeric()) {
										v=AttributeValue(a);
									}
									else {
										// specifically to deal with StackID which is SH VR even though numeric,
										// or FrameType, which is defined terms in a particular order
										// or ContrastBolusAgentPhase, which is defined terms in a particular order
										v=getNumericValueFromNonNumericAttributeForDimensionSorting(a);
									}
									dimensionValues[ido][d][j]=v;
//cerr << "frame " << dec << j << " organization " << ido <<  " dimension " << d << " value " << v << endl;
								}
							}
						}
						
						// Step 2 ... for each dimension, for each frame whose parent dimensions all have the same value, find unique values and sort

						int ***dimensionIndices = new int**[nDimensionOrganizations];
						Assert(dimensionIndices);
						for (int ido=0; ido<nDimensionOrganizations; ++ido) {
							dimensionIndices[ido]=new int*[nDimensions[ido]];
							Assert(dimensionIndices[ido]);
							for (int d=0; d<nDimensions[ido]; ++d) {
								dimensionIndices[ido][d] = new int[numberofinputfiles];
								Assert(dimensionIndices[ido][d]);
							}
						
							int *partitionedSet = new int[numberofinputfiles];
							Assert(partitionedSet);
							for (int i=0; i<numberofinputfiles; ++i) partitionedSet[i]=i;
							partitionFramesByValuesForDimensions(numberofinputfiles,0,
								nDimensions[ido],dimensionValues[ido],dimensionIndices[ido],numberofinputfiles,partitionedSet);
						}
						
						// Step 3 ... insert the Dimension Index Values to each per-frame functional group sequence
						
						for (j=0; j<numberofinputfiles; ++j) {
							AttributeList *thisFramesFunctionalGroupItem = iPerFrameFunctionalGroupsSequence[j];
							Assert(thisFramesFunctionalGroupItem);
							
							// find Frame Content Sequence to add the Dimension Index Values to ...
							AttributeList *iFrameContentSequence;
							{
								SequenceAttribute *aFrameContentSequence = (SequenceAttribute *)((*thisFramesFunctionalGroupItem)[TagFromName(FrameContentSequence)]);
								Assert(aFrameContentSequence);
								AttributeList **frameContentSequenceItems;
								int n = aFrameContentSequence->getLists(&frameContentSequenceItems);
								Assert(n == 1);
								Assert(frameContentSequenceItems);
								iFrameContentSequence = frameContentSequenceItems[0];
								Assert(iFrameContentSequence);
							}

							Attribute *aDimensionIndexValues = new UnsignedLongAttribute(TagFromName(DimensionIndexValues));
							Assert(aDimensionIndexValues);
							(*iFrameContentSequence)+=aDimensionIndexValues;
							
							for (int ido=0; ido<nDimensionOrganizations; ++ido) {
								for (int d=0; d<nDimensions[ido]; ++d) {
									int index = dimensionIndices[ido][d][j];
//cerr << "frame " << dec << j << " organization " << ido << " dimension " << d << " index " << index << " value " << dimensionValues[ido][d][j] << endl;
									aDimensionIndexValues->addValue(Uint32(index)+1);	// indices are numbered from 1
								}
							}
						}
					}
				}
				
				// why delay adding evidence sequences until here (after last frame read and processed) ?
				// in case one day we gather this dynamically and merge, rather than just using command
				// line arguments
				
				if (addReferenced) makeReferencedImageEvidenceSequence(
					&newList,
					minimalAttributesOnly,
					addReferenced_studyInstanceUID,
					addReferenced_seriesInstanceUID,
					addReferenced_referencedSOPClassUID,
					addReferenced_referencedSOPInstanceUID);
				if (addDerivation) makeSourceImageEvidenceSequence(
					&newList,
					minimalAttributesOnly,
					addDerivation_studyInstanceUID,
					addDerivation_seriesInstanceUID,
					addDerivation_referencedSOPClassUID,
					addDerivation_referencedSOPInstanceUID);
			
                                if (useExtraPixelDataWritePass) {
                                        // we have deferred writing newList until now ... so do it now ...
//cerr << "Deferred write of newList" << endl;
                                        if (!usualManagedAttributeListWrite(newList,dout,dicom_output_options,log,veryverbose)) {
                                                success=false;
                                                break;
                                        }
                                }
				
                        }

                        // clean up ... the DICOM input stream goes out of scope with each iteration of the loop

                        if (fstr) delete fstr;
                }
        }
        
        if (useExtraPixelDataWritePass) {
//cerr << "Entering extra Pixel Data write pass" << endl;
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

                        // Get pixel data attribute for later use ...

                        Attribute *apixeldata=list[TagFromName(PixelData)];
                        if (!apixeldata) {
                                log << EMsgDC(MissingAttribute) << " - \"PixelData\"" << endl;
                                success=false;
                                break;
                        }

                        if (i == 0) {
                                 // Now add "empty" pixel data attribute without pixels but correct value length ...

                                OtherUnspecifiedLargeAttributeDummy *dummyPixelData = new
                                        OtherUnspecifiedLargeAttributeDummy(TagFromName(PixelData),
                                                requiredRows,requiredColumns,numberofinputfiles,requiredSamplesPerPixel,
                                                dout.getTransferSyntaxToWriteDataSet(),
                                                0 /* let encoding rules calculate bytesinword */,
                                                requiredBitsAllocated,requiredBitsStored,requiredHighBit,
                                                0xffffffff /* let encoding rules calculate length */);
                                Assert(dummyPixelData);

                                dummyPixelData->writeBase(dout);

                                if (veryverbose) {
                                        dummyPixelData->writeBase(log,&staticDictionary,false);
                                        log << "[]" << endl;
                                }
                        }

                        if (apixeldata) {
                                if (!apixeldata->isOtherData()) {
                                        log << EMsgDC(PixelDataIncorrectVR) << endl;
                                        success=false;
                                        break;
                                }
                                else {
                                        // Check for compatible transfer syntax and append to pixel data ...
                                        TransferSyntax *its=din.getTransferSyntaxToReadDataSet();
                                        Assert(its);
                                        TransferSyntax *ots=dout.getTransferSyntaxToWriteDataSet();
                                        Assert(ots);

                                        if (its->isEncapsulated()) {
                                                log << "Can't read encapsulated transfer syntax from file <" << filename << ">" << endl;
                                                success=false;
                                                break;
                                        }
                                        else if (its->getEndian() != ots->getEndian()) {
                                                log << "Input byte order in input file <" << filename << "> differs from output - not supported" << endl;
                                                success=false;
                                                break;
                                        }
                                        else {
                                                OtherUnspecifiedLargeAttributeBase *opixeldata = apixeldata->castToOtherData();
                                                Assert(opixeldata);
                                                opixeldata->writeRaw(dout);

                                                if (verbose) log << "wrote frame " << i
							 << " from filename <" << filename << ">" << endl;
                                        }
                                }
                        }

                        // clean up ... the DICOM input stream goes out of scope with each iteration of the loop

                        if (fstr) delete fstr;
                }
        }
        
	if (numberofinputfiles && sortedfilenamestable) delete[] sortedfilenamestable;

	return success ? 0 : 1;
}

	


