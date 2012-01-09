#include "attrtype.h"
#include "attrlsln.h"
#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "uidgen.h"
#include "sopclu.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "bnchar.h"

static bool
loopOverListsInSequences(Attribute *a,bool (*func)(AttributeList &))
{
	bool succeeded=true;
	if (strcmp(a->getVR(),"SQ") == 0) {
		AttributeList **al;
		int n;
		if ((n=a->getLists(&al)) > 0) {
			int i;
			for (i=0; i<n; ++i) {
				if (!(*func)(*al[i])) succeeded=false;
			}
			delete [] al;
		}
	}
	return succeeded;
}

static bool
loopOverListsInSequencesWithFlag(Attribute *a,bool flag,bool (*func)(AttributeList &,bool flag))
{
	bool succeeded=true;
	if (strcmp(a->getVR(),"SQ") == 0) {
		AttributeList **al;
		int n;
		if ((n=a->getLists(&al)) > 0) {
			int i;
			for (i=0; i<n; ++i) {
				if (!(*func)(*al[i],flag)) succeeded=false;
			}
			delete [] al;
		}
	}
	return succeeded;
}

static bool
loopOverListsInSequencesWithLog(Attribute *a,TextOutputStream &log,
		bool (*func)(AttributeList &,TextOutputStream &))
{
	bool succeeded=true;
	if (strcmp(a->getVR(),"SQ") == 0) {
		AttributeList **al;
		int n;
		if ((n=a->getLists(&al)) > 0) {
			int i;
			for (i=0; i<n; ++i) {
				if (!(*func)(*al[i],log)) succeeded=false;
			}
			delete [] al;
		}
	}
	return succeeded;
}

static bool
loopOverListsInSequencesWithSpecificCharacterSetInfoAndLog(Attribute *a,SpecificCharacterSetInfo *specificCharacterSetInfo,TextOutputStream &log,
		bool (*func)(AttributeList &,SpecificCharacterSetInfo *specificCharacterSetInfo,TextOutputStream &))
{
	bool succeeded=true;
	if (strcmp(a->getVR(),"SQ") == 0) {
		AttributeList **al;
		int n;
		if ((n=a->getLists(&al)) > 0) {
			int i;
			for (i=0; i<n; ++i) {
				if (!(*func)(*al[i],specificCharacterSetInfo,log)) succeeded=false;
			}
			delete [] al;
		}
	}
	return succeeded;
}

static bool
validateVR(AttributeList& list,SpecificCharacterSetInfo *parentSpecificCharacterSetInfo,TextOutputStream &log)
{
	SpecificCharacterSetInfo *specificCharacterSetInfo = list.getSpecificCharacterSetInfo();
	if (!specificCharacterSetInfo) {
		Attribute *aSpecificCharacterSet = list[TagFromName(SpecificCharacterSet)];
		if (aSpecificCharacterSet) {
			specificCharacterSetInfo = new SpecificCharacterSetInfo(aSpecificCharacterSet);		// always use it if present (even if zero length ... overrides parent with default)
		}
		else {
			specificCharacterSetInfo = parentSpecificCharacterSetInfo ? parentSpecificCharacterSetInfo : new SpecificCharacterSetInfo();	// create default if not inherited from parent
		}
		list.setSpecificCharacterSetInfo(specificCharacterSetInfo);
	}
	bool succeeded=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithSpecificCharacterSetInfoAndLog(a,specificCharacterSetInfo,log,&::validateVR))
			succeeded=false;
		if (a->getVL() > 0 && !a->validateVR(log,specificCharacterSetInfo,list.getDictionary()))
			succeeded=false;
		++listi;
	}
	return succeeded;
}

bool
ManagedAttributeList::validateVR(TextOutputStream &log)
{
//cerr << "ManagedAttributeList::validateVR" << endl;
	return ::validateVR(*this,NULL,log);
}

static bool
validateRetired(AttributeList& list,TextOutputStream &log)
{
	bool succeeded=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithLog(a,log,&::validateRetired))
			succeeded=false;
		if (!a->validateRetired(log,list.getDictionary()))
			succeeded=false;
		++listi;
	}
	return succeeded;
}

bool
ManagedAttributeList::validateRetired(TextOutputStream &log)
{
//cerr << "ManagedAttributeList::validateRetired" << endl;
	return ::validateRetired(*this,log);
}

static bool
validateUsed(AttributeList& list,TextOutputStream &log)
{
	bool succeeded=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		Tag t = a->getTag();
		if (!t.isPrivateGroup()) {
			if (!::loopOverListsInSequencesWithLog(a,log,&::validateUsed))
				succeeded=false;
		}
		bool used = a->isUsed();
		if (!used && t.isRepeatingGroup()) {
			Attribute *ab = list[t.getRepeatingBase()];
			if (ab) {
				used = ab->isUsed();
			}
		}
		if (!used) {
			if (!t.isPrivateGroup() && !t.isLengthElementOrLengthToEnd() && t != TagFromName(DataSetTrailingPadding)) {
				ElementDictionary *dictionary = list.getDictionary();
				if (dictionary) {
					const char *vrd = dictionary->getValueRepresentation(t);
					if (!vrd) {
						log << EMsgDC(AttributeIsNotALegalStandardAttribute) << " - ";
						t.write(log,dictionary);
						log << endl;
						succeeded=false;
					}
				}
				log << WMsgDC(AttributeIsNotUsedInIOD) << " - ";
				t.write(log,dictionary);
				log << endl;
				succeeded=false;
			}
		}
		++listi;
	}
	return succeeded;
}

bool
ManagedAttributeList::validateUsed(TextOutputStream &log)
{
//cerr << "ManagedAttributeList::validateUsed" << endl;
	return ::validateUsed(*this,log);
}

static bool
removeLengths(AttributeList& list)
{
	bool succeeded=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		Tag tag=a->getTag();
		if (tag.isLengthElementOrLengthToEnd()) {
			list-=(tag);
			listi.first();
		}
		else {
			if (!::loopOverListsInSequences(a,&::removeLengths))
				succeeded=false;
			++listi;
		}
	}
	return succeeded;
}

bool
ManagedAttributeList::removeLengths(void)
{
//cerr << "ManagedAttributeList::removeLengths" << endl;
	return ::removeLengths(*this);
}

bool
ManagedAttributeList::removeMetaHeader(bool leaveuid)
{
//cerr << "ManagedAttributeList::removeMetaHeader" << endl;
//cerr << "ManagedAttributeList::removeMetaHeader - leaveuid" << (leaveuid ? "T" : "F") << endl;
	bool succeeded=true;
	AttributeListIterator listi(*this);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		Tag tag=a->getTag();
		if (tag.isMetaheaderGroup()
		 && !(leaveuid && (tag == TagFromName(MediaStorageSOPClassUID)
			        || tag == TagFromName(MediaStorageSOPInstanceUID) ))
		) {
//TextOutputStream terr(cerr);
//terr << "ManagedAttributeList::removeMetaHeader: removing";
//a->write(terr,getDictionary());
//terr << endl;
			(*this)-=(tag);
			listi.first();
		}
		else {
			// doesn't recurse through sequences
			++listi;
		}
	}
	return succeeded;
}

bool
ManagedAttributeList::removeInstanceUID(void)
{
//cerr << "ManagedAttributeList::removeInstanceUID" << endl;

	// Only top level Instance UID's other than references

	bool succeeded=true;
	AttributeListIterator listi(*this);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		const char *vr=a->getVR();
		Assert(vr);
		Tag tag=a->getTag();
		if (strcmp(vr,"UI") == 0
		 && (tag == TagFromName(SOPInstanceUID)
		  || tag == TagFromName(StudyInstanceUID)
		  || tag == TagFromName(SeriesInstanceUID)
		  || tag == TagFromName(FrameOfReferenceUID)
		  || tag == TagFromName(SynchronizationFrameOfReferenceUID)
		  || tag == TagFromName(MediaStorageSOPInstanceUID)
		 )
		) {
			(*this)-=(tag);
			listi.first();
		}
		else {
			// doesn't recurse through sequences
			++listi;
		}
	}
	return succeeded;
}

static bool
removePrivate(AttributeList& list)
{
	bool succeeded=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		Tag tag=a->getTag();
		if (tag.isPrivateGroup()) {
			list-=(tag);
			listi.first();
		}
		else {
			if (!::loopOverListsInSequences(a,&::removePrivate))
				succeeded=false;
			++listi;
		}
	}
	return succeeded;
}

void
ManagedAttributeList::removeCommandGroup(void)
{
	AttributeListIterator listi(*this);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		Tag tag=a->getTag();
		if (tag.getGroup() == 0x0000) {
			(*this)-=(tag);
			listi.first();
		}
		else {
			break;
		}
		// no need for ++listi, since either broke or restarted
	}
}

bool
ManagedAttributeList::removePrivate(void)
{
//cerr << "ManagedAttributeList::removePrivate" << endl;
	return ::removePrivate(*this);
}

bool
ManagedAttributeList::addMetaHeader(TransferSyntax *ts,const char *stamp)
{
//cerr << "ManagedAttributeList::addMetaHeader" << endl;
	Attribute *a;

	operator-=(TagFromName(TransferSyntaxUID));
	//if (!operator[](TagFromName(TransferSyntaxUID))) {
//cerr << "ManagedAttributeList::addMetaHeader - TransferSyntaxUID" << endl;
		Assert(ts);
		const char *tsuid=ts->getUID();
		Assert(tsuid);
		a=new UIStringAttribute(TagFromName(TransferSyntaxUID),tsuid);
		Assert(a);
		operator+=(a);
	//}

	operator-=(TagFromName(FileMetaInformationVersion));
	//if (!operator[](TagFromName(FileMetaInformationVersion))) {
//cerr << "ManagedAttributeList::addMetaHeader - FileMetaInformationVersion" << endl;
		a=new OtherByteSmallNonPixelAttribute(TagFromName(FileMetaInformationVersion));
		Assert(a);
		unsigned char values[2];
		values[0]=0x00;
		values[1]=0x01;
		a->setValue(values,2);
		operator+=(a);
	//}

	operator-=(TagFromName(MediaStorageSOPClassUID));
	//if (!operator[](TagFromName(MediaStorageSOPClassUID))) {
//cerr << "ManagedAttributeList::addMetaHeader - MediaStorageSOPClassUID" << endl;
		char *classuid=AttributeValue(operator[](TagFromName(SOPClassUID)));
		if (classuid) {
			a=new UIStringAttribute(TagFromName(MediaStorageSOPClassUID));
			Assert(a);
			a->addValue(String_Use(classuid));	// correct: AttributeValue() returns a copy
			operator+=(a);
		}
		else if (operator[](TagFromName(DirectoryRecordSequence))) {
			// we are doing a DICOMDIR
			operator+=(new UIStringAttribute(TagFromName(MediaStorageSOPClassUID),MediaStorageDirectoryStorageSOPClassUID));
		}
		else {
			errorstream << EMsgDC(MissingSOPClassUIDBuildingMetaHeader) << endl;
			good_flag=false;
			return false;
		}
	//}

	operator-=(TagFromName(MediaStorageSOPInstanceUID));
	//if (!operator[](TagFromName(MediaStorageSOPInstanceUID))) {
//cerr << "ManagedAttributeList::addMetaHeader - MediaStorageSOPInstanceUID" << endl;
		char *instanceuid=AttributeValue(operator[](TagFromName(SOPInstanceUID)));
		if (instanceuid) {
			a=new UIStringAttribute(TagFromName(MediaStorageSOPInstanceUID));
			Assert(a);
			a->addValue(String_Use(instanceuid));	// correct: AttributeValue() returns a copy
			operator+=(a);
		}
		else if (operator[](TagFromName(DirectoryRecordSequence))) {
			// we are doing a DICOMDIR, so just make up a new InstanceUID ...
			operator+=(new UIStringAttribute(TagFromName(MediaStorageSOPClassUID),StrDup(GeneratedDirInstanceUID(stamp))));
		}
		else {
			errorstream << EMsgDC(MissingSOPInstanceUIDBuildingMetaHeader) << endl;
			good_flag=false;
			return false;
		}
	//}

#ifndef DEFAULTIMPLEMENTATIONCLASSUID
#define DEFAULTIMPLEMENTATIONCLASSUID "0.0.0.0"
#endif
	operator-=(TagFromName(ImplementationClassUID));
	//if (!operator[](TagFromName(ImplementationClassUID))) {
//cerr << "ManagedAttributeList::addMetaHeader - ImplementationClassUID" << endl;
		a=new UIStringAttribute(TagFromName(ImplementationClassUID));
		Assert(a);
		a->addValue(DEFAULTIMPLEMENTATIONCLASSUID);
		operator+=(a);
	//}

#ifndef DEFAULTIMPLEMENTATIONVERSIONNAME
#define DEFAULTIMPLEMENTATIONVERSIONNAME "NOTSPECIFIED"
#endif
	operator-=(TagFromName(ImplementationVersionName));
	//if (!operator[](TagFromName(ImplementationVersionName))) {
//cerr << "ManagedAttributeList::addMetaHeader - ImplementationVersionName" << endl;
		a=new ShortStringAttribute(TagFromName(ImplementationVersionName));
		Assert(a);
		a->addValue(DEFAULTIMPLEMENTATIONVERSIONNAME);
		operator+=(a);
	//}

#ifndef DEFAULTSOURCEAPPLICATIONENTITYTITLE
#define DEFAULTSOURCEAPPLICATIONENTITYTITLE "NOTSPECIFIED"
#endif
	operator-=(TagFromName(SourceApplicationEntityTitle));
	//if (!operator[](TagFromName(SourceApplicationEntityTitle))) {
//cerr << "ManagedAttributeList::addMetaHeader - SourceApplicationEntityTitle" << endl;
		a=new ApplicationEntityAttribute(TagFromName(SourceApplicationEntityTitle));
		Assert(a);
		a->addValue(DEFAULTSOURCEAPPLICATIONENTITYTITLE);
		operator+=(a);
	//}

	return true;
}


static const char *uidstamp;
static const char *instanceCreationDate;
static const char *instanceCreationTime;
static const char *timezoneOffsetFromUTC;

static bool
addDicom(AttributeList& list,bool disambiguateseriesbydescription)
{
//cerr << "addDicom:" << endl;
//cerr << "addDicom: disambiguateseriesbydescription = " << disambiguateseriesbydescription << endl;
	bool succeeded=true;

	// Patient and Study attributes will not both be present
	// in a Normalized IOD, but are always present in a
	// Composite IOD (even if empty, all these are Type 2 in
	// all modules), including the Basic Study Descriptor IOD

	// Don't do anything for Normalized IOD's ...

	bool ispatient = list[TagFromName(PatientName)] != 0
		      || list[TagFromName(PatientID)] != 0
		      || list[TagFromName(PatientSex)] != 0
		      || list[TagFromName(PatientBirthDate)] != 0;
	bool isstudy = list[TagFromName(StudyID)] != 0
		    || list[TagFromName(StudyDate)] != 0
		    || list[TagFromName(StudyTime)] != 0;

	if (!(ispatient && isstudy)) return true;

	// Must be a Composite IOD ...

	// AccessionNumber is Type 2 so add zero length if not present

	if (!list[TagFromName(AccessionNumber)]) {
		list+=new ShortStringAttribute(TagFromName(AccessionNumber));
	}

	Attribute *astudy    = list[TagFromName(StudyID)];
	Attribute *apatient  = list[TagFromName(PatientName)];
	Attribute *amodality = list[TagFromName(Modality)];
	Attribute *aseries   = list[TagFromName(SeriesNumber)];
	Attribute *ainstance = list[TagFromName(InstanceNumber)];
	Attribute *aInConcatenationNumber = list[TagFromName(InConcatenationNumber)];

	char *studystring = AttributeValue(astudy);
	Uint16 study = studystring ? atoi(studystring) : 0;
	Uint16 series = AttributeValue(aseries);
	Uint16 instance = AttributeValue(ainstance);
	Int32 inConcatenationNumber = aInConcatenationNumber ? Int32(AttributeValue(aInConcatenationNumber)) : -1;
//cerr << "addDicom: inConcatenationNumber " << dec << inConcatenationNumber << endl;
	
	if (disambiguateseriesbydescription) {
//cerr << "addDicom: disambiguateseriesbydescription - series number was " << dec << series << endl;
		Attribute *aSeriesDescription    = list[TagFromName(SeriesDescription)];
		char *seriesDescriptionString = AttributeValue(aSeriesDescription);
		if (seriesDescriptionString != NULL) {
			const char *p = seriesDescriptionString;
			while (*p) {
				series+=*p++;	// crude hash by adding character values
			}
		}
//cerr << "addDicom: disambiguateseriesbydescription - series number now " << dec << series << endl;
	}

	// The SOPClassUID and SOPInstanceUID are required
	// in all Composite IOD's.

	// SOPClassUID

	if (!list[TagFromName(SOPClassUID)]) {
		const char *modality;
		const char *uid;
		if (amodality && (modality=AttributeValue(amodality))) {
			if (strcmp(modality,"CR") == 0)
				uid=ComputedRadiographyImageStorageSOPClassUID;
			else if (strcmp(modality,"CT") == 0)
				uid=CTImageStorageSOPClassUID;
			else if (strcmp(modality,"MR") == 0)
				uid=MRImageStorageSOPClassUID;
			else // can't be sure ... multiple choices
				uid=SecondaryCaptureImageStorageSOPClassUID;
		}
		else {
			uid=SecondaryCaptureImageStorageSOPClassUID;
		}
		list+=(new UIStringAttribute(TagFromName(SOPClassUID),uid));
	}

	// SOPInstanceUID

	if (!list[TagFromName(SOPInstanceUID)]) {
		list+=(new UIStringAttribute(
			TagFromName(SOPInstanceUID),
			GeneratedSOPInstanceUID(uidstamp, study,series,instance,inConcatenationNumber)
		));
	}

	// StudyInstanceUID

	if (!list[TagFromName(StudyInstanceUID)] && astudy) {
		list+=(new UIStringAttribute(
			TagFromName(StudyInstanceUID),
			GeneratedStudyInstanceUID(uidstamp,study)
		));
	}

	// SeriesInstanceUID

	if (!list[TagFromName(SeriesInstanceUID)] && aseries) {
		list+=(new UIStringAttribute(
			TagFromName(SeriesInstanceUID),
			GeneratedSeriesInstanceUID(uidstamp,study,series)
		));
	}

	// FrameOfReferenceUID

	if (!list[TagFromName(FrameOfReferenceUID)]
	  && list[TagFromName(PositionReferenceIndicator)]) {
		list+=(new UIStringAttribute(
			TagFromName(FrameOfReferenceUID),
			GeneratedFrameOfReferenceUID(uidstamp,
				study,series)
		));
	}

	// SynchronizationFrameOfReferenceUID

	if (!list[TagFromName(SynchronizationFrameOfReferenceUID)]
	  && (list[TagFromName(SynchronizationTrigger)] || list[TagFromName(AcquisitionTimeSynchronized)])) {
		list+=(new UIStringAttribute(
			TagFromName(SynchronizationFrameOfReferenceUID),
			GeneratedSynchronizationFrameOfReferenceUID(uidstamp,
				study,series)
		));
	}

	// InstanceCreatorUID

#ifndef DEFAULTINSTANCECREATORUID
#define DEFAULTINSTANCECREATORUID "0.0.0.0"
#endif
	if (!list[TagFromName(InstanceCreatorUID)]) {
		list+=(new UIStringAttribute(
			TagFromName(InstanceCreatorUID),
			DEFAULTINSTANCECREATORUID
		));
	}

	// InstanceCreationDate

	if (!list[TagFromName(InstanceCreationDate)]) {
		list+=(new DateStringAttribute(
			TagFromName(InstanceCreationDate),
			instanceCreationDate ? Date(instanceCreationDate,DateOrderMonthMiddleYearFirst) : Date()
		));
	}

	// InstanceCreationTime

	if (!list[TagFromName(InstanceCreationTime)]) {
		list+=(new TimeStringAttribute(
			TagFromName(InstanceCreationTime),
			instanceCreationTime ? Time(instanceCreationTime) : Time()
		));
	}

	// TimezoneOffsetFromUTC

	if (!list[TagFromName(TimezoneOffsetFromUTC)]) {
		list+=(new ShortStringAttribute(
			TagFromName(TimezoneOffsetFromUTC),
			timezoneOffsetFromUTC ? timezoneOffsetFromUTC : Date().getTZOffset()
		));
	}

	if (studystring) delete[] studystring;

	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithFlag(a,disambiguateseriesbydescription,&::addDicom))
			succeeded=false;
		++listi;
	}

	return succeeded;
}

bool
ManagedAttributeList::addDicom(bool disambiguateseriesbydescription)
{
//cerr << "ManagedAttributeList::addDicom" << endl;
//cerr << "ManagedAttributeList::addDicom: disambiguateseriesbydescription = " << disambiguateseriesbydescription << endl;
	::uidstamp = getUIDStamp();
	::instanceCreationDate = getInstanceCreationDate();
//cerr << "ManagedAttributeList::addDicom instanceCreationDate=" << ::instanceCreationDate << endl;
	::instanceCreationTime = getInstanceCreationTime();
	::timezoneOffsetFromUTC = getTimezoneOffsetFromUTC();
	return ::addDicom(*this,disambiguateseriesbydescription);
}

bool
ManagedAttributeList::addDisclaimer(void)
{
//cerr << "ManagedAttributeList::addDisclaimer" << endl;
	const char *disclaimer=MMsgDC(DisclaimerMessage);
	Attribute *aImageComments=(*this)[TagFromName(ImageComments)];
	if (aImageComments) {
		char *sImageComments=0;
		// LT so VM should always be 1
		if (aImageComments->getVM() == 1
		 && aImageComments->getValue(0,sImageComments)
		 && sImageComments
		 && strstr(sImageComments,disclaimer) == 0) {
			ostrstream o;
			o << disclaimer << "\n" << sImageComments << ends;
			char *s=o.str();
			if (s) {
				aImageComments->setValue(0,s);
				delete[] s;
			}
		}
	}
	else {
		(*this)+=new LongTextAttribute(TagFromName(ImageComments),disclaimer);
	}
		
	return true;
}

bool
ManagedAttributeList::addLengths(
	TransferSyntax *metats,TransferSyntax *datats,bool allgroups,bool toend)
{
//cerr << "ManagedAttributeList::addLengths" << endl;
	// Fixup and/or insert group lengths

	// insert a dummy LengthToEnd element in case not present ...
	// (otherwise the length of group 0008 would be wrong)

	if (toend)
		operator+=(new UnsignedLongAttribute(TagFromName(LengthToEnd),Uint32(0)));

	// each (nnnn,0000) group length element
	// contains the length in bytes of the rest of the group
	// not including the length element itself

	bool metaseen=false;
	bool dothisgroup=true;
	Uint16 doinggroup=0;
	Uint32 length=0;
	AttributeList lengthlist;
	AttributeListIterator listi(*this);
	while (1) {
		Attribute *a;
		Tag tag;
		if (!listi) {
			a=listi();
			Assert(a);
			tag=a->getTag();
		}
		if (!!listi || tag.getGroup() > doinggroup) {
			if (length && dothisgroup && (allgroups || doinggroup == 0x0002)) {
				lengthlist+=new UnsignedLongAttribute(Tag(doinggroup,0),length);
			}
			if (!!listi)
				break;
			else {
				doinggroup=tag.getGroup();
				dothisgroup=true;
				length=0;
			}
		}
		else if (tag.getGroup() == doinggroup) {
                        Uint32 add=lengthOfEntireAttribute(metats,datats,a);
			if (add == 0xffffffff)
				dothisgroup=false;
			else
				length+=add;
			++listi;
		}
		else
			++listi;

		if (!allgroups) {
			if (tag.isMetaheaderGroup())
				metaseen=true;
			else
				if (metaseen) break;
		}
	}

	operator+=(lengthlist);

	if (toend) {
		listi.first();
		length=0;
		while (!listi) {
			Attribute *a = listi();
			Assert(a);
			Tag tag = a->getTag();
			if (tag > TagFromName(LengthToEnd)) {
                                Uint32 add=lengthOfEntireAttribute(metats,datats,a);
		                if (add == 0xffffffff)
			                Assert(0);              // could just break silently and not add, but let's flag the problem
		                else
			                length+=add;
			 }
                        ++listi;
		}

		operator-=(TagFromName(LengthToEnd));
		operator+=(new UnsignedLongAttribute(TagFromName(LengthToEnd),length));
	}

	return true;
}

static bool
getOffsetToAttributeValue(Tag matchtag,AttributeList& list,
	TransferSyntax *metats,TransferSyntax *datats,bool metaflag,
	Uint32& length)
{
//cerr << "getOffsetToPixelData" << endl;

	length=metaflag ? 132 : 0;

	AttributeListIterator listi(list);
	while (!listi) {
//cerr << "getOffsetToPixelData: length = hex " << hex << length << dec << endl;
		Attribute *a=listi();
		Assert(a);
		Tag tag=a->getTag();
		if (tag == matchtag) {
			length+=lengthOfFixedPartOfAttribute(metats,datats,a);
			return true;
		}
                Uint32 add=lengthOfEntireAttribute(metats,datats,a);
		if (add == 0xffffffff)
			Assert(0);
		else
			length+=add;
		++listi;
	}

	return false;
}

bool
ManagedAttributeList::addTiffTrailer(DicomOutputStream& stream,bool metaflag)
{
//cerr << "ManagedAttributeList::addTiffTrailer" << endl;

	TransferSyntax *metats=stream.getTransferSyntaxToWriteMetaHeader();
	TransferSyntax *datats=stream.getTransferSyntaxToWriteDataSet();

	Attribute *a;

	if ((a=(*this)[TagFromName(Columns)])) {
		Uint32 ImageWidth = AttributeValue(a);
//cerr << "ManagedAttributeList::addTiffTrailer - ImageWidth " << dec << ImageWidth << endl;
		if ((a=(*this)[TagFromName(Rows)])) {
			Uint32 ImageLength = AttributeValue(a);
//cerr << "ManagedAttributeList::addTiffTrailer - ImageLength " << dec << ImageLength << endl;
			if ((a=(*this)[TagFromName(BitsAllocated)])) {
				Uint16 BitsPerSample = AttributeValue(a);
//cerr << "ManagedAttributeList::addTiffTrailer - BitsPerSample " << dec << BitsPerSample << endl;
				Assert(BitsPerSample ==8 || BitsPerSample == 16);
				if ((a=(*this)[TagFromName(PhotometricInterpretation)])) {
					Uint16 PhotometricInterpretation;
					Uint16 SamplesPerPixel;
					Uint16 PlanarConfiguration=0;	// flag not to send it

					const char *s = AttributeValue(a);
					if (s && strcmp(s,"MONOCHROME1") == 0) PhotometricInterpretation=0;
					else if (s && strcmp(s,"MONOCHROME2") == 0) PhotometricInterpretation=1;
					else if (s && strcmp(s,"RGB") == 0) PhotometricInterpretation=2;
					else if (s && strcmp(s,"PALETTE COLOR") == 0) PhotometricInterpretation=3;
					else PhotometricInterpretation=99;

					SamplesPerPixel=(PhotometricInterpretation == 2) ? 3 : 1;
						
					if (PhotometricInterpretation == 2) {
						SamplesPerPixel=3;
						if ((a=(*this)[TagFromName(PlanarConfiguration)])) {
							PlanarConfiguration=AttributeValue(a);
							++PlanarConfiguration;
						}
						else PlanarConfiguration=1;	// blech :(
					}
					else {
						SamplesPerPixel=1;
					}

//cerr << "ManagedAttributeList::addTiffTrailer - PhotometricInterpretation " << dec << PhotometricInterpretation << endl;
//cerr << "ManagedAttributeList::addTiffTrailer - SamplesPerPixel " << dec << SamplesPerPixel << endl;
//cerr << "ManagedAttributeList::addTiffTrailer - PlanarConfiguration " << dec << PlanarConfiguration << endl;

					if (PhotometricInterpretation != 99) {
						if ((a=(*this)[TagFromName(PixelData)])) {
							Uint32 StripOffset;
							if (getOffsetToAttributeValue(TagFromName(PixelData),*this,metats,datats,metaflag,StripOffset)) {

//cerr << "ManagedAttributeList::addTiffTrailer - StripOffset hex " << hex << StripOffset << dec << endl;

								Assert(datats);
								Endian endian=datats->getEndian();
								unsigned char ifd[2+132+4];
								unsigned char *ptr=ifd;
								Uint16 entries = 10 + (PlanarConfiguration ? 1 : 0);

								insert16incr(entries,ptr,endian);	// Entry count 10 or 11 (each 12 bytes)

								insert16incr(0x00fe,ptr,endian);	// NewSubfileType
								insert16incr(0x0004,ptr,endian);	// - type   LONG
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr(0x0000,ptr,endian);	// - value  0

								insert16incr(0x0100,ptr,endian);	// ImageWidth
								insert16incr(0x0004,ptr,endian);	// - type   LONG
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr(ImageWidth,ptr,endian);	// - value

								insert16incr(0x0101,ptr,endian);	// ImageLength
								insert16incr(0x0004,ptr,endian);	// - type   LONG
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr(ImageLength,ptr,endian);	// - value

								insert16incr(0x0102,ptr,endian);	// BitsPerSample
								insert16incr(0x0003,ptr,endian);	// - type   SHORT
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr(BitsPerSample,ptr,endian);	// - value  None

								insert16incr(0x0103,ptr,endian);	// Compression
								insert16incr(0x0003,ptr,endian);	// - type   SHORT
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr(0x0001,ptr,endian);	// - value  None

								insert16incr(0x0106,ptr,endian);	// Photometric Interpretation
								insert16incr(0x0003,ptr,endian);	// - type   SHORT
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr(PhotometricInterpretation,ptr,endian);	// - value

								insert16incr(0x0111,ptr,endian);	// StripOffsets
								insert16incr(0x0004,ptr,endian);	// - type   LONG
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr(StripOffset,ptr,endian);	// - value

								insert16incr(0x0115,ptr,endian);	// SamplesPerPixel
								insert16incr(0x0003,ptr,endian);	// - type   SHORT
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr(SamplesPerPixel,ptr,endian);	// - value

								insert16incr(0x0116,ptr,endian);	// RowsPerStrip
								insert16incr(0x0004,ptr,endian);	// - type   LONG
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr(ImageLength,ptr,endian);	// - value

								insert16incr(0x0117,ptr,endian);	// StripByteCounts
								insert16incr(0x0004,ptr,endian);	// - type   LONG
								insert32incr(0x0001,ptr,endian);	// - length 1
								insert32incr((Uint32)ImageLength*ImageWidth*((BitsPerSample-1)/8+1)*SamplesPerPixel,ptr,endian);	// - value

								if (PlanarConfiguration) {
									insert16incr(0x011c,ptr,endian);	// PlanarConfiguration
									insert16incr(0x0003,ptr,endian);	// - type   SHORT
									insert32incr(0x0001,ptr,endian);	// - length 1
									insert32incr(PlanarConfiguration,ptr,endian);	// - value  None
								}

								insert32incr(0,ptr,endian);		// Offset of next IFD (none)

//cerr << "ptr-ifd = " << dec << ptr-ifd << endl;
								Assert(ptr-ifd == 2 + 12*entries + 4);

								OtherByteSmallNonPixelAttributeBase *apad=
									new OtherByteSmallNonPixelAttributeBase(
										TagFromName(DataSetTrailingPadding));
								Assert(apad);
								apad->setValue(ifd,ptr-ifd);

								if ((a=(*this)[TagFromName(DataSetTrailingPadding)])) (*this)-=a;
								(*this)+=apad;

								Uint32 IFDOffset;
								if (getOffsetToAttributeValue(TagFromName(DataSetTrailingPadding),
										*this,metats,datats,metaflag,IFDOffset)) {
//cerr << "ManagedAttributeList::addTiffTrailer - IFDOffset hex " << hex << IFDOffset << dec << endl;
									stream.setIFDOffset(IFDOffset);
									return true;
								}
								Assert(0);
							}
						}
					}
				}
			}
		}
	}

	return false;
}

static bool
setOutputEncoding(AttributeList& list,TransferSyntax *ts,ostream &errorstream)
{
//cerr << "::setOutputEncoding" << endl;
	bool succeeded=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		if (strcmp(a->getVR(),"SQ") == 0) {
			AttributeList **al;
			int n;
			if ((n=a->getLists(&al)) > 0) {
				int i;
				for (i=0; i<n; ++i) {
					if (!setOutputEncoding(*al[i],ts,errorstream))
						succeeded=false;
				}
				delete [] al;
			}
		}

		if (a->getTag() == TagFromName(PixelData)) {
//TextOutputStream terr(cerr);
//terr << "::setOutputEncoding: setting";
//a->write(terr,0);
//terr << endl;
			Attribute *aba=list[TagFromName(BitsAllocated)];
			Attribute *abs=list[TagFromName(BitsStored)];
			Attribute *abh=list[TagFromName(HighBit)];

			Uint16 bitsAllocated=0;
			Uint16 bitsStored=0;
			Uint16 highBit=0;

			if (!aba)
				errorstream << WMsgDC(MissingAttribute) << " - BitsAllocated" << endl;
			if (!abs)
				errorstream << WMsgDC(MissingAttribute) << " - BitsStored" << endl;
			if (!abh)
				errorstream << WMsgDC(MissingAttribute) << " - HighBit" << endl;

			if (!aba && abs && (bitsStored=AttributeValue(abs)) != 0) {
				bitsAllocated=((bitsStored-1u)/8u+1u)*8u;
				highBit=AttributeValue(abh,bitsStored-1u);
			}
			else if (aba && (bitsAllocated=AttributeValue(aba)) != 0) {
				bitsStored=AttributeValue(abs,bitsAllocated);
				highBit=AttributeValue(abh,bitsStored-1u);
			}
			else {
				errorstream << EMsgDC(InsufficientInformationToSetEncoding) << endl;
				succeeded=false;
			}

			// bytesinword == 0 -> determine from ts
			if (bitsAllocated && bitsStored)
				a->setOutputEncoding(ts,0,
					bitsAllocated,bitsStored,highBit,0xffffffff);
		}
		++listi;
	}

	return succeeded;
}

bool
ManagedAttributeList::setOutputEncoding(TransferSyntax *ts)
{
//cerr << "ManagedAttributeList::setOutputEncoding" << endl;
	return ::setOutputEncoding(*this,ts,errorstream);
}

ManagedAttributeList::ManagedAttributeList(void)
	: ReadableAttributeList()
{
	flags=none;
	uidstamp=0;
	instanceCreationDate=0;
	instanceCreationTime=0;
	timezoneOffsetFromUTC=0;
	preparedstream=0;
	dictionary=new ElementDictionary();
}

ManagedAttributeList::~ManagedAttributeList(void)
{
#ifdef TRACE_DESTRUCTORS
cerr << "ManagedAttributeList::~ManagedAttributeList" << endl;
#endif
}

bool
ManagedAttributeList::read(DicomInputStream& stream,
		TextOutputStream *log,
		bool verbose,
		Uint32 length,
		bool metaheadercheck,
		bool uselengthtoend,
		bool ignoretagsoutoforder,
		bool useUSVRForLUTDataIfNotExplicit,
		bool useStopAtTag,
		Tag stopAtTag)
{
	return ReadableAttributeList::read(
		stream,getDictionary(),
		log,verbose,length,
		metaheadercheck,uselengthtoend,ignoretagsoutoforder,useUSVRForLUTDataIfNotExplicit,
		false/*forceImplicit*/,
		useStopAtTag,stopAtTag);
}

bool
ManagedAttributeList::clean(flag_types fset,flag_types freset)
{
//cerr << "ManagedAttributeList::clean" << endl;
	set(fset);
	reset(freset);

	return
	    removeLengths()
	 && removeMetaHeader(flags&metaheader)
	 && (!(flags&removeinstanceuid) || removeInstanceUID())
	 && (!(flags&removeprivate) || removePrivate());
}

bool
ManagedAttributeList::prepare(DicomOutputStream& stream,
	flag_types fset,flag_types freset,const char *stamp,
	const char *creationDate,const char *creationTime,
	const char *tzOffset)
{
//cerr << "ManagedAttributeList::prepare" << endl;
	set(fset);
	reset(freset);
	if (stamp) uidstamp=stamp;
	if (creationDate) instanceCreationDate=creationDate;
//cerr << "ManagedAttributeList::prepare instanceCreationDate=" << instanceCreationDate << endl;
	if (creationTime) instanceCreationTime=creationTime;
	if (tzOffset) timezoneOffsetFromUTC=tzOffset;
	preparedstream=&stream;

//cerr << "ManagedAttributeList::prepare flags = 0x" << hex << flags << dec << endl;
//cerr << "ManagedAttributeList::prepare flags&disambiguateseriesbydescription = 0x" << hex << (flags&disambiguateseriesbydescription) << dec << endl;
	return 
	(!(flags&adddicom) || addDicom(flags&disambiguateseriesbydescription ? true : false))
	 && (!(flags&adddisclaimer) || addDisclaimer())
	 && setOutputEncoding(stream.getTransferSyntaxToWriteDataSet())
	 && setValueRepresentation()
	 && (!(flags&metaheader) || addMetaHeader(stream.getTransferSyntaxToWriteDataSet(),stamp));
}

bool
ManagedAttributeList::finalize(DicomOutputStream& stream)
{
//cerr << "ManagedAttributeList::finalize" << endl;
	Assert(preparedstream == &stream);

	// Flags already available from prepare

	return addLengths(stream.getTransferSyntaxToWriteMetaHeader(),
			  stream.getTransferSyntaxToWriteDataSet(),
			  flags&addlengths,flags&addlengthtoend)
	    && (!(flags&addtiff) || addTiffTrailer(stream,flags&metaheader));
}

DicomOutputStream&
ManagedAttributeList::write(DicomOutputStream& stream)
{
//cerr << "ManagedAttributeList::write" << endl;
	Assert(preparedstream == &stream);

	AttributeListIterator i(*this);
	while (!i) {
//cerr << "ManagedAttributeList::write: looping on next attribute" << endl;
		Attribute *a=i();
		Assert(a);
		if (a->getTag().isMetaheaderGroup()) {
			if (flags&metaheader) {
				stream.writingMetaHeader();
//TextOutputStream terr(cerr);
//terr << "ManagedAttributeList::write: writing";
//a->write(terr,getDictionary());
//terr << endl;
				stream << *a;
			}
		}
		else {
			if (flags&dataset) {
				stream.writingDataSet();
//TextOutputStream terr(cerr);
//terr << "ManagedAttributeList::write: writing";
//a->write(terr,getDictionary());
//terr << endl;
				stream << *a;
			}
		}
		++i;
	}
//cerr << "ManagedAttributeList::write return" << endl;
	return stream;
}

DicomOutputStream& operator<<
	(DicomOutputStream& stream,ManagedAttributeList& list)
{
	return list.write(stream);	// leave flags alone
}

TextOutputStream&
ManagedAttributeList::write(TextOutputStream& stream,bool verbose,bool showUsedAndIE)
{
//TextOutputStream terr(cerr);
//terr << "ManagedAttributeList::write" << endl;
	AttributeListIterator i(*this);
//terr << "ManagedAttributeList::have iterator" << endl;
	while (!i) {
//terr << "ManagedAttributeList::passed test" << endl;
		Attribute *a=i();
//terr << "ManagedAttributeList::have attribute" << endl;
		Assert(a);
//terr << "ManagedAttributeList::attribute not null" << endl;
//terr << "ManagedAttributeList::tag = "; a->getTag().write(terr); terr << endl;
//a->write(terr,getDictionary());
//terr << endl;
		a->write(stream,getDictionary(),verbose,showUsedAndIE);
		stream << endl;
		++i;
	}
	return stream;
}

TextOutputStream& operator<<
	(TextOutputStream& stream,ManagedAttributeList& list)
{
	return list.write(stream);
}


