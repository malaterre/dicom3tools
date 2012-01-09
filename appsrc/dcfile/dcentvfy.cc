#include <fstream.h>

#include "attrmxls.h"
#include "mesgtext.h"
#include "dcopt.h"
//#include "sopclu.h"
#include "elmconst.h"
#include "attrval.h"
#include "attrseq.h"

#include "iodcomp.h"

static const char *
getStringValueElseDefault(AttributeList &list,Tag tag,const char *def)
{
	Attribute *a=list[tag];
	if (a && a->getVM() > 0) {
		return AttributeValue(a);
	}
	else {
		return def;
	}
}

class RecordBase {
protected:
	RecordBase *sibling;
	RecordBase *child;
	const char *primaryKey;
public:
	RecordBase(const char *key,RecordBase *nextsib)
		{
			primaryKey=key;
			Assert(primaryKey);
			
			child=0;
			sibling=nextsib;
		}

	virtual ~RecordBase()
		{
			if (child) delete child;
			if (sibling) delete sibling;
			if (primaryKey) delete[] primaryKey;
		}

	const char *getPrimaryKey() { return primaryKey; }

	RecordBase *getSibling()	{ return sibling; }
	RecordBase *getChild()	{ return child; }
	
	void setChild(RecordBase *newchild)
		{
			child=newchild;
		}

	bool matches(const char *key)
		{
			Assert(key);
			Assert(primaryKey);
			return strcmp(key,primaryKey) == 0 ? true : false;
		}

	virtual const char *getInformationEntity() = 0;
};

class InstanceRecord : public RecordBase {
	const char *filename;
	AttributeList *list;
	CompositeIOD *iod;
public:
	InstanceRecord(const char *uid,InstanceRecord *nextsib,AttributeList *srclist,const char *name)
		: RecordBase(uid,nextsib)
		{
			list=srclist;
			Assert(list);
			filename=name;
			Assert(filename);
			iod = selectCompositeIOD(list,NULL);	// this has the side effect of setting the IE for each attribute
		}

	const char *getInformationEntity() { return "Instance"; }
	
	AttributeList *getAttributeList() { return list; }
	
	CompositeIOD *getCompositeIOD() { return iod; }
	
	const char *getFileName() { return filename; }
};

class SeriesRecord : public RecordBase {
public:
	SeriesRecord(const char *uid,SeriesRecord *nextsib)
		: RecordBase(uid,nextsib)
		{
		}

	const char *getInformationEntity() { return "Series"; }
};

class StudyRecord : public RecordBase {
public:
	StudyRecord(const char *uid,StudyRecord *nextsib)
		: RecordBase(uid,nextsib)
		{
		}

	const char *getInformationEntity() { return "Study"; }
};

class PatientRecord : public RecordBase {
public:
	PatientRecord(const char *patientID,PatientRecord *nextsib)
		: RecordBase(patientID,nextsib)
		{
		}

	const char *getInformationEntity() { return "Patient"; }
};

static bool singleNumericAttributeValuesAreEqual(Attribute *attr1,Attribute *attr2,Float64 &value1,Float64 &value2,Uint16 i,bool verbose,TextOutputStream& log) {
	attr1->getValue(i,value1);
	attr2->getValue(i,value2);
	if (verbose) log << "diffAttributes(): attr1 value " << dec << (i+1) << " = " << value1 << endl;
	if (verbose) log << "diffAttributes(): attr2 value " << dec << (i+1) << " = " << value2 << endl;
	return value1 == value2;
}

static bool
diffAttributes(Attribute *attr1,Attribute *attr2,
	const char *element,const char *ie,const char *filename1,const char *filename2,
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict)
{
	bool success = true;
	if (verbose) log << "diffAttributes():" << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">" << endl;
	Assert(attr1 != attr2);
	if (attr1 && attr2) {
		const char *vr1 = attr1->getVR();
		const char *vr2 = attr1->getVR();
		if (!attr1->isUnknown() && !attr2->isUnknown() && strcmp(vr1,vr2) != 0) {
			log << EMsgDC(DifferentValueRepresentationInOneInstanceComparedToTheOther)
				<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
				<< " - " << MMsgDC(ValueRepresentation) << " "
				<< " " << vr1 << " versus " << vr2
				<< endl;
			success = false;
		}
		int vm1 = attr1->getVM();
		if (verbose) log << "diffAttributes(): vm1 = " << dec << vm1 << endl;
		int vm2 = attr2->getVM();
		if (verbose) log << "diffAttributes(): vm2 = " << dec << vm2 << endl;
		if (vm1 == vm2) {
			for (Uint16 i=0; i<vm1; ++i) {
				if (attr1->isString() && attr2->isString()) {
					char *value1;
					char *value2;
					attr1->getValue(i,value1);
					attr2->getValue(i,value2);
					if (verbose) log << "diffAttributes(): attr1 value " << dec << (i+1) << " = " << value1 << endl;
					if (verbose) log << "diffAttributes(): attr2 value " << dec << (i+1) << " = " << value2 << endl;
					if (value1 && value2) {
						if (strcmp(value1,value2) != 0) {
							if (attr1->isNumeric() && attr2->isNumeric()
							 || (strcmp(vr1,"TM") == 0 &&  strcmp(vr2,"TM") == 0)) {
								Float64 fvalue1;
								Float64 fvalue2;
								if (singleNumericAttributeValuesAreEqual(attr1,attr2,fvalue1,fvalue2,i,verbose,log)) {
									log << WMsgDC(NumericStringAttributeHasEqualValueButDifferentEncodingInOneInstanceComparedToTheOther)
										<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
										<< " " << MMsgDC(Value) << " " << dec << (i+1)
										<< " <" << value1 << "> [" << fvalue1 << "] versus <" << value2 << "> [" << fvalue2 << "]"
										<< endl;
								}
								else {
									log << EMsgDC(NumericStringAttributeHasDifferentValueInOneInstanceComparedToTheOther)
										<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
										<< " " << MMsgDC(Value) << " " << dec << (i+1)
										<< " <" << value1 << "> versus <" << value2 << ">"
										<< endl;
									success = false;
								}
							}
							else {
								log << EMsgDC(StringAttributeHasDifferentValueInOneInstanceComparedToTheOther)
									<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
									<< " " << MMsgDC(Value) << " " << dec << (i+1)
									<< " <" << value1 << "> versus <" << value2 << ">"
									<< endl;
								success = false;
							}
						}
					}
					else if (value1 || value2) {
						log << WMsgDC(ValuePresentInOneInstanceButNotTheOther)
							<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
							<< " " << MMsgDC(Value) << " " << dec << (i+1)
							<< endl;
					}
					// else OK ... both null
				}
				else if (attr1->isNumeric() && attr2->isNumeric()) {
					Float64 value1;
					Float64 value2;
					if (!singleNumericAttributeValuesAreEqual(attr1,attr2,value1,value2,i,verbose,log)) {
						log << EMsgDC(NumericAttributeHasDifferentValueInOneInstanceComparedToTheOther)
							<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
							<< " " << MMsgDC(Value) << " " << dec << (i+1)
							<< " <" << value1 << "> versus <" << value2 << ">"
							<< endl;
						success = false;
					}
				}
				// else do not attempt to compare
			}
		}
		else {
			if (vm1 == 0 || vm2 == 0) {
				log << WMsgDC(MissingValueInOneInstanceComparedToTheOtherOKIfType2)
					<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
					<< endl;
			}
			else {
				log << EMsgDC(AttributeHasDifferentValueMultiplicityInOneInstanceComparedToTheOther)
					<< " - " << dec << vm1 << " versus " << vm2
					<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
					<< endl;
				success = false;
			}
		}
	}
	else if (attr1 || attr2) {
		log << WMsgDC(AttributePresentInOneInstanceButNotTheOther) << " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">" << endl;
	}
	// else OK ... both empty
	return success;
}

const char *getElementName(Tag t,ElementDictionary *dict) {
	return  (t.isPrivateGroup() ? "--private--" : dict->getKeyword(t));
}

bool
compareInformationEntitiesOfAttributeLists(AttributeList *list1,AttributeList *list2,const char *ie,const char *filename1,const char *filename2,bool verbose,TextOutputStream &log,ElementDictionary *dict) {
	if (verbose) log << "Comparing attribute lists for IE " << ie << endl;
	bool success = true;
	Assert(ie);
	InformationEntity ieWanted = getInformationEntityFromDescription(ie);
	Assert(ieWanted != UnknownIE);
	
	Assert(list1);
	Assert(list2);
	Assert(list1 != list2);
	AttributeListIterator listi1(*list1);
	AttributeListIterator listi2(*list2);
	Attribute *a1 = !listi1 ? listi1() : NULL;
	Attribute *a2 = !listi2 ? listi2() : NULL;
	while (a1 && a2) {
		if (verbose) log << "Comparing attribute lists -- looping" << endl;
		Tag t1 = a1->getTag();
		Tag t2 = a2->getTag();
		if (verbose) { log << "("; writeZeroPaddedHexNumber(log,t1.getGroup(),4); log << ","; writeZeroPaddedHexNumber(log,t1.getElement(),4); log << ") " << endl; }
		if (verbose) { log << "("; writeZeroPaddedHexNumber(log,t2.getGroup(),4); log << ","; writeZeroPaddedHexNumber(log,t2.getElement(),4); log << ") " << endl; }
		InformationEntity ie1 = a1->getInformationEntity();
		InformationEntity ie2 = a2->getInformationEntity();
		if (verbose) log << "IE1=" << describeInformationEntity(ie1) << endl;
		if (verbose) log << "IE2=" << describeInformationEntity(ie1) << endl;
		if (t1 == t2) {
			if (verbose) log << "Checking attribute isPrivateGroup()" << t1.isPrivateGroup() << endl;
			const char *element = getElementName(t1,dict);
			if (verbose) log << "Checking attribute " << element << endl;
			if (verbose) log << "Checking attribute " << element << " -  IE1=" << describeInformationEntity(ie1) << " versus IE2=" << describeInformationEntity(ie2) << endl;
			if (ie1 == ieWanted || ie2 == ieWanted) {	// do not need to check for private tag, since there would be no IE information
				if (verbose) log << "Attribute is candidate this IE " << element << endl;
				if (ie1 != ie2) {
					log << WMsgDC(DifferentInformationEntitiesForAttributeInOneInstanceComparedToTheOther)
						<< " - " << MMsgDC(Element) << "=<" << element << "> "
						<< MMsgDC(IE) << " <" << describeInformationEntity(ie1) << "> versus <" << describeInformationEntity(ie2)
						<< "> for file <" << filename1 << "> versus <" << filename2 << ">"
						<< endl;
				}
				else {
					if (a1->isSequence() || a2->isSequence()) {
						if (verbose) log << "Checking Sequence Attribute " << element << endl;
						if (a1->isSequence() && a2->isSequence()) {
							SequenceAttribute *sa1 = (SequenceAttribute *)a1;
							SequenceAttribute *sa2 = (SequenceAttribute *)a2;
							AttributeList **sal1;
							AttributeList **sal2;
							int n1 = sa1->getLists(&sal1);
							int n2 = sa2->getLists(&sal2);
							if (n1 == n2) {
								if (n1 > 0) {
									for (int item=0; item <n1; ++item) {
										if (verbose) log << "Checking Sequence Attribute " << element << " item " << dec << (item+1) << endl;
										AttributeList *l1 = sal1[item];
										AttributeList *l2 = sal2[item];
										bool ourSucess = compareInformationEntitiesOfAttributeLists(l1,l2,ie,filename1,filename2,verbose,log,dict);
										if (!ourSucess) success = false;
									}
								}
							}
							else {
								log << EMsgDC(SequenceHasDifferentNumberOfItemsInOneInstanceComparedToTheOther)
									<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
									<< " - " << n1 << " versus " << n2
									<< endl;
								success = false;
							}
						}
						else {
							log << EMsgDC(SequenceHasDifferentValueRepresentationInOneInstanceComparedToTheOther)
								<< " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">"
								<< " - " << MMsgDC(ValueRepresentation) << " "
								<< " " << a1->getVR() << " versus " << a2->getVR()
								<< endl;
							success = false;
						}
					}
					else {
						bool ourSucess = diffAttributes(a1,a2,element,ie,filename1,filename2,verbose,log,dict);
						if (!ourSucess) success = false;
					}
				}
			}
			else {
				if (verbose) log << "Ignoring Attribute in both lists but not for wanted IE " << element << endl;
			}
			++listi1; a1 = !listi1 ? listi1() : NULL;
			++listi2; a2 = !listi2 ? listi2() : NULL;
		}
		else if (t1 < t2) {
			const char *element = getElementName(t1,dict);
			if (ie1 == ieWanted) {	// do not need to check for private tag, since there would be no IE information
				log << WMsgDC(AttributePresentInOneInstanceButNotTheOther) << " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">" << endl;
			}
			else {
				if (verbose) log << "Ignoring Attribute present only in first list not for wanted IE " << element << endl;
			}
			++listi1; a1 = !listi1 ? listi1() : NULL;
		}
		else if (t1 > t2) {
			const char *element = getElementName(t2,dict);
			if (ie2 == ieWanted) {	// do not need to check for private tag, since there would be no IE information
				log << WMsgDC(AttributePresentInOneInstanceButNotTheOther) << " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">" << endl;
			}
			else {
				if (verbose) log << "Ignoring Attribute present only in second list not for wanted IE " << element << endl;
			}
			++listi2; a2 = !listi2 ? listi2() : NULL;
		}
		else {
			Assert(0);
		}
	}
	while (a1) {
		if (verbose) log << "Checking residual attributes in first attribute list -- looping" << endl;
		Tag t1 = a1->getTag();
		InformationEntity ie1 = a1->getInformationEntity();
		const char *element = getElementName(t1,dict);
		if (verbose) log << "Checking residual attributes in first attribute list -- have element " << element << endl;
		if (ie1 == ieWanted) {
			log << WMsgDC(AttributePresentInOneInstanceButNotTheOther) << " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">" << endl;
		}
		++listi1; a1 = !listi1 ? listi1() : NULL;
	}
	while (a2) {
		if (verbose) log << "Checking residual attributes in second attribute list -- looping" << endl;
		Tag t2 = a2->getTag();
		InformationEntity ie2 = a2->getInformationEntity();
		const char *element = getElementName(t2,dict);
		if (verbose) log << "Checking residual attributes in second attribute list -- have element " << element << endl;
		if (ie2 == ieWanted) {
			log << WMsgDC(AttributePresentInOneInstanceButNotTheOther) << " - " << MMsgDC(Element) << "=<" << element << "> " << MMsgDC(IE) << "=<" << ie << "> for file <" << filename1 << "> versus <" << filename2 << ">" << endl;
		}
		++listi2; a2 = !listi2 ? listi2() : NULL;
	}
	if (verbose) log << "Done comparing attribute lists for IE " << ie << endl;
	return success;
}

bool
compareAllDeepestChildrenOfCurrentRecord(RecordBase *record,InstanceRecord *&compareWith,const char *ie,bool verbose,bool veryverbose,TextOutputStream &log) {
	bool success = true;
	RecordBase *child = record->getChild();
	if (child) {
		while (child) {
			bool ourSucess = compareAllDeepestChildrenOfCurrentRecord(child,compareWith,ie,verbose,veryverbose,log);
			if (!ourSucess) success = false;
			child = child->getSibling();
		}
	}
	else {
		//if (veryverbose) log << "Comparing: record.getInformationEntity() " << record->getInformationEntity() << endl;
		Assert(strcmp(record->getInformationEntity(),"Instance") == 0);
		// no child ... we assume all leaves are instances (since they could not be created otherwise)
		InstanceRecord *testInstance = (InstanceRecord *)record;
		AttributeList *list = testInstance->getAttributeList();
		Assert(list);
		ElementDictionary *dict = list->getDictionary();
		Assert(dict);
		if (compareWith) {
			if (verbose) log << "Comparing " << compareWith->getFileName() << " with " << testInstance->getFileName() << " for IE " << ie << endl;
			if (verbose) log << "Comparing primary key " << testInstance->getPrimaryKey() << endl;
			bool ourSucess = compareInformationEntitiesOfAttributeLists(list,compareWith->getAttributeList(),ie,testInstance->getFileName(),compareWith->getFileName(),veryverbose,log,dict);
			if (!ourSucess) success = false;
		}
		else {
			compareWith = testInstance;
			if (verbose) log << "Choosing basis for comparison to be " << compareWith->getFileName() << " for IE " << ie << endl;
			if (verbose) log << "Choosing basis for comparison primary key " << testInstance->getPrimaryKey() << endl;
		}
	}
	return success;
}

bool checkNonLeafRecordAndChilden(RecordBase *record,bool verbose,bool veryverbose,TextOutputStream &log) {
	bool success = true;
	RecordBase *child = record->getChild();
	if (child) {
		InstanceRecord *compareWith = NULL;
		const char *ie = record->getInformationEntity();
		Assert(ie);
		const char *key = record->getPrimaryKey();
		Assert(ie);
		if (verbose) log << "Checking IE " << ie << " identified by " << key << endl;
		bool ourSucess = compareAllDeepestChildrenOfCurrentRecord(record,compareWith,ie,verbose,veryverbose,log);
		if (!ourSucess) success = false;
		while (child) {
			bool ourSucess = checkNonLeafRecordAndChilden(child,verbose,veryverbose,log);
			if (!ourSucess) success = false;
			child = child->getSibling();
		}
	}
	// else do not check leaf records themselves (i.e., instances)
	return success;
}

bool
checkAllRecords(RecordBase *record,bool verbose,bool veryverbose,TextOutputStream &log) {
	bool success = true;
	while (record) {
		bool ourSucess = checkNonLeafRecordAndChilden(record,verbose,veryverbose,log);
		if (!ourSucess) success = false;
		record = record->getSibling();
	}
	return success;
}

static bool readOneFile(const char *filename,PatientRecord *&headPatient,DicomInputOptions &dicom_input_options,bool success,bool verbose,bool veryverbose,TextOutputStream &log) {
	Assert(filename);
	if (verbose) log << "Reading \"" << filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
	ifstream *fstr=new ifstream(filename,ios::in|ios::binary);
#else
	ifstream *fstr=new ifstream(filename);
#endif
	if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
		cerr << AMsgDC(FileReadOpenFailed);
		if (filename) cerr <<" - \"" << filename << "\"";
		return false;
	}

	DicomInputStream din(*(istream *)fstr,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);

	ManagedAttributeList *list = new ManagedAttributeList();
	Assert(list);

	list->read(din,&log,false,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	if (!list->good()) {
		log << list->errors()
			<< EMsgDC(DatasetReadFailed) << endl;
		return false;
	}

	const char *patientID=getStringValueElseDefault(*list,TagFromName(PatientID),"");
	Assert(patientID);

	PatientRecord *ptrPatient=headPatient;
	while (ptrPatient && !ptrPatient->matches(patientID)) ptrPatient=(PatientRecord *)ptrPatient->getSibling();

	StudyRecord *headStudy,*ptrStudy;
	if (ptrPatient) {
		ptrStudy=headStudy=(StudyRecord *)ptrPatient->getChild();
	}
	else {
		if (verbose) log << filename << " Detected new patient identified by " << patientID << endl;
		ptrPatient=headPatient=new PatientRecord(patientID,headPatient);
		ptrStudy=headStudy=0;
	}

	const char *studyInstanceUID=getStringValueElseDefault(*list,TagFromName(StudyInstanceUID),"");
	Assert(studyInstanceUID);

	while (ptrStudy && !ptrStudy->matches(studyInstanceUID)) ptrStudy=(StudyRecord *)ptrStudy->getSibling();

	SeriesRecord *headSeries,*ptrSeries;
	if (ptrStudy) {
		ptrSeries=headSeries=(SeriesRecord *)ptrStudy->getChild();
	}
	else {
		if (verbose) log << filename << " Detected new study identified by " << studyInstanceUID << endl;
		ptrStudy=headStudy=new StudyRecord(studyInstanceUID,headStudy);
		Assert(ptrPatient);
		ptrPatient->setChild(headStudy);
		ptrSeries=headSeries=0;
	}

	const char *seriesInstanceUID=getStringValueElseDefault(*list,TagFromName(SeriesInstanceUID),"");
	Assert(seriesInstanceUID);

	while (ptrSeries && !ptrSeries->matches(seriesInstanceUID)) ptrSeries=(SeriesRecord *)ptrSeries->getSibling();

	InstanceRecord *headInstance,*ptrInstance;
	if (ptrSeries) {
		ptrInstance=headInstance=(InstanceRecord *)ptrSeries->getChild();
	}
	else {
		if (verbose) log << filename << " Detected new series identified by " << seriesInstanceUID << endl;
		ptrSeries=headSeries=new SeriesRecord(seriesInstanceUID,headSeries);
		Assert(ptrStudy);
		ptrStudy->setChild(headSeries);
		ptrInstance=headInstance=0;
	}

	const char *sopInstanceUID=getStringValueElseDefault(*list,TagFromName(SOPInstanceUID),"");
	Assert(sopInstanceUID);

	while (ptrInstance && !ptrInstance->matches(sopInstanceUID)) ptrInstance=(InstanceRecord *)ptrInstance->getSibling();

	if (ptrInstance) {
		log << "Error - duplicate SOP instance " << sopInstanceUID << " within Series for file <" << filename << "> versus <" << ptrInstance->getFileName() << ">" << endl;
	}
	else {
		ptrInstance=headInstance=new InstanceRecord(sopInstanceUID,headInstance,list,StrDup(filename));
		Assert(ptrSeries);
		ptrSeries->setChild(headInstance);

		if (!ptrInstance->getCompositeIOD()) {
			const char *sopClassUID=getStringValueElseDefault(*list,TagFromName(SOPClassUID),"");
			log << "Warning - unrecognized SOP Class " << sopClassUID << " so cannot determine information entity of attributes within file <" << filename << ">" << endl;
		}
	}

	if (verbose) log << filename << " Done processing file for SOPInstanceUID " << sopInstanceUID << endl;
	if (fstr) {
		fstr->close();
		delete fstr;
	}
	return true;
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool veryverbose=options.get("veryverbose") || options.get("vv");
	if (veryverbose) verbose=true;
	
	bool bad=false;

	dicom_input_options.done();
	
	const char *filelistfile=0;
	(void)(options.get("filelist",filelistfile) || options.get("f",filelistfile));

	int numberofinputfiles=!options;


	const char **listoffilenames = new const char * [numberofinputfiles];
	const char **ptr = listoffilenames;
	const char *filename;

	while(!options && (filename=options())) {
		++options;
		*ptr++=filename;
	}

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
			<< " [-v|-verbose|-vv|-veryverbose]"
			<< " [-f|-filelist filename]"
			<< " " << MMsgDC(InputFile) << " ["<< MMsgDC(InputFile) << " ...]"
			<< endl;
		return 1;
	}

	bool success=true;
	TextOutputStream log(cerr);

	PatientRecord *headPatient = NULL;

	int i;
	for (i=0; i < numberofinputfiles; ++i) {
		bool thisFileSucceeded = readOneFile(listoffilenames[i],headPatient,dicom_input_options,success,verbose,veryverbose,log);
		if (!thisFileSucceeded) success=false;
	}

	if (filelistfile) {
		ifstream *flfstr=new ifstream(filelistfile);
		if (!flfstr || !*flfstr || !flfstr->rdbuf()->is_open()) {
			log << AMsgDC(FileReadOpenFailed);
			if (filelistfile) log <<" - \"" << filename << "\"";
			log << endl;
			bad=true;
		}
		else {
			while (flfstr->peek() != EOF) {
				const int lineBufferSize=2048;
				char lineBuffer[lineBufferSize];
				flfstr->getline(lineBuffer,2048);
				if (strlen(lineBuffer)) {
					bool thisFileSucceeded = readOneFile(lineBuffer,headPatient,dicom_input_options,success,verbose,veryverbose,log);
					if (!thisFileSucceeded) success=false;
				}
				// else skip blank lines
			}
		}
	}
	
	if (success) {
		success = checkAllRecords(headPatient,verbose,veryverbose,log);
	}

	return success ? 0 : 1;
}

