#include <fstream.h>

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

static ElementDictionary staticDictionary;

// Define a funky derived OX class to be able to writeBase() and not activateSource() ...

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


// structure to store filenames

struct filenameentry {
	const char *filename;
	AttributeList *list;
};

static void
dumpfilenamestable(filenameentry *table,int n,const char *name,ostream &log)
{
	int i;
	for (i=0; i<n; ++i) {
		log << name << "[" << i << "] = " << table[i].filename << endl;
	}
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

	dicom_input_options.done();
	Assert(!dicom_input_options.filename);

	dicom_output_options.done();

	int numberofinputfiles=!options;

	struct filenameentry *filenamestable = new filenameentry [numberofinputfiles];
	Assert(filenamestable);
	struct filenameentry *ptr = filenamestable;

	const char *filename;

	while(!options && (filename=options())) {
		++options;
		ptr->filename=filename;
		ptr->list=0;
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

	// attributes required to have constant values for all SR files ...

	const char *requiredPatientID;
	const char *requiredStudyInstanceUID;
	const char *requiredSeriesInstanceUID;
	const char *requiredSOPClassUID;
	const char *requiredPatientName;
	const char *requiredStudyID;
	Uint16 requiredSeriesNumber;
	const char *requiredModality;

	ManagedAttributeList newlist;
	SequenceAttribute *aFirstContentSequence;

	// iterate through image files to extract and check attributes values ...

	int i;
	for (i=0; i < numberofinputfiles; ++i) {
		// open and read each image file ...

		const char *filename=filenamestable[i].filename;
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

		ManagedAttributeList *list = new ManagedAttributeList();
		Assert(list);
		filenamestable[i].list=list;

		if (veryveryverbose) log << "******** While reading ... " << filename << " ... ********" << endl; 
		list->read(din,&log,veryveryverbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags);

		if (!list->good()) {
			log << list->errors()
			    << EMsgDC(DatasetReadFailed) << endl;
			success=false;
			break;
		}

		// extract and check that attributes values are consistent for all image files ...

		const char *patientID=getAndCheckStringMatchElseError(i == 0,requiredPatientID,*list,
			TagFromName(PatientID),"Patient ID",filename,log);

		const char *patientName=getAndCheckStringMatchElseError(i == 0,requiredPatientName,*list,
			TagFromName(PatientName),"Patient Name",filename,log);

		const char *studyInstanceUID=getAndCheckStringMatchElseError(i == 0,requiredStudyInstanceUID,*list,
			TagFromName(StudyInstanceUID),"Study Instance UID",filename,log);

		const char *studyID=getAndCheckStringMatchElseError(i == 0,requiredStudyID,*list,
			TagFromName(StudyID),"Study ID",filename,log);

		const char *seriesInstanceUID=getAndCheckStringMatchElseError(i == 0,requiredSeriesInstanceUID,*list,
			TagFromName(SeriesInstanceUID),"Series Instance UID",filename,log);

		const char *SOPClassUID=getAndCheckStringMatchElseError(i == 0,requiredSOPClassUID,*list,
			TagFromName(SOPClassUID),"SOP Class UID",filename,log);

		Uint16 seriesNumber=getAndCheckIntegerMatchElseError(i == 0,requiredSeriesNumber,*list,
			TagFromName(SeriesNumber),"Series Number",filename,log);

		const char *modality=getAndCheckStringMatchElseError(i == 0,requiredModality,*list,
			TagFromName(Modality),"Modality",filename,log);

		// for the first file, copy everything ...

		if (i == 0) {
			// delete old Instance Number and add new one with fixed value of 1 ...

			(*list)-=TagFromName(InstanceNumber);
			(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),Uint16(1));

			// delete old SOP Instance UID ... usualManagedAttributeListWrite() adds new one ...

			(*list)-=TagFromName(SOPInstanceUID);

			// delete any Data Set Trailing Padding  ...

			(*list)-=TagFromName(DataSetTrailingPadding);

			aFirstContentSequence=(SequenceAttribute *)((*list)[TagFromName(ContentSequence)]);
			Assert(aFirstContentSequence);

			newlist+=(*list);
		}
		else {
			SequenceAttribute *aContentSequence=(SequenceAttribute *)((*list)[TagFromName(ContentSequence)]); 
			Assert(aContentSequence);
			AttributeList **itemLists;
			int nLists=aContentSequence->getLists(&itemLists);
			int i;
			for (i=0; i<nLists; ++i) {
				AttributeList *itemList = itemLists[i];
				Assert(itemList);
				Assert(aFirstContentSequence);
				(*aFirstContentSequence)+=itemList;
			}
		}

		// clean up ... the DICOM input stream goes out of scope with each iteration of the loop

		if (fstr) delete fstr;
	}
cerr << "About to write:" << endl;

	if (!usualManagedAttributeListWrite(newlist,dout,dicom_output_options,log,veryverbose)) {
		success=false;
	}

	if (numberofinputfiles && filenamestable) delete[] filenamestable;

	return success ? 0 : 1;
}

	

