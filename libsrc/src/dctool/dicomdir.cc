#include "attr.h"
#include "attrmxls.h"
#include "attrval.h"
#include "hash.h"
#include "elmconst.h"
#include "mesgtext.h"

class Entry {
	Uint32 inputoffset;
	AttributeList * dicomdirrecord;
public:
	Entry(void)				{}
	Entry(Uint32 o,AttributeList * i)	{ inputoffset=o; dicomdirrecord=i; }
	Entry(Entry *e)				{ inputoffset=e->inputoffset; dicomdirrecord=e->dicomdirrecord; }

	Uint32	getInputOffset(void) const		{ return inputoffset; }
	AttributeList *	getDicomdirRecord(void) const	{ return dicomdirrecord; }

	bool operator==(Entry e)		{ return inputoffset == e.getInputOffset(); }
};


class EntryList : public SimpleList<Entry>
{
public:
	~EntryList() {}	// only because buggy g++ 2.7.0 freaks
};

class EntryListIterator : public SimpleListIterator<Entry>
{
public:
	EntryListIterator(void)
		: SimpleListIterator<Entry>() {}
	EntryListIterator(EntryList& list)
		: SimpleListIterator<Entry>(list) {}
};

class IndexDicomdirRecordsByInputOffset : public OpenHashTable <Entry,
			Uint32,
			EntryList,
			EntryListIterator>
{
public:
	IndexDicomdirRecordsByInputOffset(unsigned long size)
		: OpenHashTable<Entry,
			Uint32,
			EntryList,
			EntryListIterator>(size)
		{}

	// supply virtual functions for OpenHashTable ...

	unsigned long hash(const Uint32 &inputoffset,unsigned long size)
		{
			return inputoffset%size; 
		}

	Uint32 key(const Entry &e)		{ return e.getInputOffset(); }
};

#define DICOMDIRRECORDSINDEXSIZE	131

static void
indent(TextOutputStream &log,unsigned depth)
{
	while (depth--) log << "\t";
}

static bool
traverseDepthFirst(IndexDicomdirRecordsByInputOffset *index,Uint32 offset,
	TextOutputStream &log,bool verbose,bool veryverbose,bool showrecordinfo,bool showabstract,bool showpaths,bool showdescription,unsigned depth)
{
	while (offset) {
		Entry *entry=(*index)[offset];
		if (entry == 0) {
			log << EMsgDC(DirectoryOffsetInvalid)
			    << " - " << hex << offset << dec
			    << endl;
			return false;
		}
		AttributeList *list=entry->getDicomdirRecord();
		Assert(list);

		char *type = AttributeValue((*list)[TagFromName(DirectoryRecordType)]);
		
		if (showabstract) {
			indent(log,depth);
			
			if (showrecordinfo) log << hex << offset << dec << ": ";

			log << (type ? type : "-none-");
			
			if (strcmp(type,"PATIENT") == 0) {
				char *name = AttributeValue((*list)[TagFromName(PatientName)]);
				char *id = AttributeValue((*list)[TagFromName(PatientID)]);
				log << " " << (name ? name : "") << " " << (id ? id : "");
			}
			else if (strcmp(type,"STUDY") == 0) {
				char *date = AttributeValue((*list)[TagFromName(StudyDate)]);
				char *time = AttributeValue((*list)[TagFromName(StudyTime)]);
				char *id = AttributeValue((*list)[TagFromName(StudyID)]);
				char *num = AttributeValue((*list)[TagFromName(AccessionNumber)]);
				char *desc = showdescription ? AttributeValue((*list)[TagFromName(StudyDescription)]) : (char *)NULL;
				log << " " << (id ? id : "") << " " << (num ? num : "")
				    << " " << (date ? date : "") << " " << (time ? time : "")
				    << " " << (desc ? desc : "");
			}
			else if (strcmp(type,"SERIES") == 0) {
				char *mod = AttributeValue((*list)[TagFromName(Modality)]);
				char *num = AttributeValue((*list)[TagFromName(SeriesNumber)]);
				char *desc = showdescription ? AttributeValue((*list)[TagFromName(SeriesDescription)]) : (char *)NULL;
				log << " " << (num ? num : "") << " " << (mod ? mod : "")
				    << " " << (desc ? desc : "");
			}
			else if (strcmp(type,"IMAGE") == 0) {
				char *num = AttributeValue((*list)[TagFromName(InstanceNumber)]);
				log << " " << (num ? num : "");
			}
			else if (strcmp(type,"STUDY COMPONENT") == 0) {
				char *mod = AttributeValue((*list)[TagFromName(Modality)]);
				char *desc = AttributeValue((*list)[TagFromName(StudyDescription)]);
				// should also do ProcedureCodeSequence
				log << " " << (mod ? mod : "") << " " << (desc ? desc : "");
			}
			else if (strcmp(type,"MRDR") == 0) {
				char *num = AttributeValue((*list)[TagFromName(NumberOfReferences)]);
				log << " (" << (num ? num : "") << ")";
			}

			log << endl;
		}
		
		Attribute *aFilename = (*list)[TagFromName(ReferencedFileID)];
		if (aFilename) {
			if (showabstract) {
				indent(log,depth);
				log << " -> ";
				aFilename->writeData(log);
				log << endl;
			}
			if (showpaths) {
				int vm = aFilename->getVM();
				const char *prefix="";
				for (int i=0; i<vm; ++i) {
					log << prefix;
					char *string;
					if (aFilename->getValue(i,string) && string != NULL) {
						if (strlen(string) > 0) {
							log << string;
						}
						delete[] string;
					}
					prefix="/";
				}
				if (vm > 0) {
					log << endl;
				}
			}
		}

		if (verbose) {
			AttributeListIterator i(*list);
			while (!i) {
				Attribute *a=i();
				Assert(a);
				indent(log,depth);
				a->write(log,list->getDictionary(),veryverbose);
				log << endl;
				++i;
			}
		}

		Uint32 mrdroffset = AttributeValue((*list)[TagFromName(MRDRDirectoryRecordOffset)],double(0));
		if (mrdroffset && !traverseDepthFirst(index,mrdroffset,log,verbose,veryverbose,showrecordinfo,showabstract,showpaths,showdescription,depth+1)) return false;

		Uint32 lower = AttributeValue((*list)[TagFromName(OffsetOfReferencedLowerLevelDirectoryEntity)],double(0));
		if (lower && !traverseDepthFirst(index,lower,log,verbose,veryverbose,showrecordinfo,showabstract,showpaths,showdescription,depth+1)) return false;

		offset=AttributeValue((*list)[TagFromName(OffsetOfTheNextDirectoryRecord)],double(0));
	}
	return true;
}

bool
parseDicomdir(ManagedAttributeList &list,TextOutputStream &log,bool verbose,bool veryverbose,bool showrecordinfo,bool showabstract,bool showpaths,bool showdescription)
{
	IndexDicomdirRecordsByInputOffset *DicomdirRecordsIndex;

	DicomdirRecordsIndex = new
		IndexDicomdirRecordsByInputOffset(DICOMDIRRECORDSINDEXSIZE);

	Assert(DicomdirRecordsIndex);

	// NB. The byteoffset from 0 (including the preamble and DICM) is
	// recorded with the start of each attribute. The offset for the
	// first attribute of each DICOMDIR record is actually 8 bytes larger
	// than the value contained in the "pointers" contained within the
	// records, because the pointer is directed at the 8 byte long Item
	// element that preceeds each item in the directory record sequence,
	// and is not recorded in our internal structure. The input offset
	// values entered in this table are the "corrected" values, ie. those
	// that match the directory record pointer values, ie. 8 bytes less
	// than the byteoffset value stored in the attribute internal structure.

	Uint32 vRootDirectoryFirstRecord = 0;
	Attribute *aRootDirectoryFirstRecord=list[TagFromName(OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity)];
	if (!aRootDirectoryFirstRecord)
		log << WMsgDC(MissingAttribute)
		    << " - \"RootDirectoryFirstRecord\""
		    << endl;
	else
		vRootDirectoryFirstRecord=AttributeValue(aRootDirectoryFirstRecord);

	if (veryverbose | showrecordinfo)
		log << "RootDirectoryFirstRecord = " << hex << vRootDirectoryFirstRecord << dec << endl;

	Uint32 vRootDirectoryLastRecord = 0;
	Attribute *aRootDirectoryLastRecord=list[TagFromName(OffsetOfTheLastDirectoryRecordOfTheRootDirectoryEntity)];
	if (!aRootDirectoryLastRecord)
		log << WMsgDC(MissingAttribute)
		    << " - \"RootDirectoryLastRecord\""
		    << endl;
	else
		vRootDirectoryLastRecord=AttributeValue(aRootDirectoryLastRecord);

	if (veryverbose | showrecordinfo)
		log << "RootDirectoryLastRecord = " << hex << vRootDirectoryLastRecord << dec << endl;

	// The directory record sequence is returned as
	// an array of pointers to attribute lists

	int nDirectoryRecordSequence = 0;
	AttributeList ** vDirectoryRecordSequence = 0;
	Attribute *aDirectoryRecordSequence=list[TagFromName(DirectoryRecordSequence)];
	if (!aDirectoryRecordSequence)
		log << WMsgDC(MissingAttribute)
		    << " - \"DirectoryRecordSequence\""
		    << endl;
	else
		nDirectoryRecordSequence=aDirectoryRecordSequence->getLists(&vDirectoryRecordSequence);

	Assert(nDirectoryRecordSequence==0 || vDirectoryRecordSequence);

	if (veryverbose | showrecordinfo)
		log << "Number of records = " << dec << nDirectoryRecordSequence << endl;

	int i;
	AttributeList **iptr;

	for (i=0,iptr=vDirectoryRecordSequence; i<nDirectoryRecordSequence; ++i,++iptr) {
		AttributeList *ptr = *iptr;
		Assert(ptr);
#ifdef CRAP
		// The DirectoryRecordSequence attribute is ALWAYS the 1st in the item,
		// so find it and use its byteoffset - 8 as the input offset of this record ...

		Attribute *aNextDirectoryRecordOffset=(*ptr)[TagFromName(NextDirectoryRecordOffset)];
#else
		// The proceeding is NOT true ... what if there is a group length ? (eg. ECR 97 Philips)
		// so take 1st element in item ...

		AttributeListIterator i(*ptr);
		// i.first() is redundant
		Attribute *aNextDirectoryRecordOffset=i.value();
#endif
		if (!aNextDirectoryRecordOffset)
			log << WMsgDC(MissingAttribute)
			    << " - \"NextDirectoryRecordOffset\""
			    << endl;
		else {
			Uint32 inputoffset=aNextDirectoryRecordOffset->getByteOffset()-8;
			*DicomdirRecordsIndex+=Entry(inputoffset,ptr);
			if (veryverbose) {
				log << "Offset = " << hex << inputoffset << dec << endl;
				AttributeListIterator i(*ptr);
				while (!i) {
					Attribute *a=i();
					Assert(a);
					log << "\t";
					a->write(log,ptr->getDictionary(),true);
					log << endl;
					++i;
				}
			}
		}
	}

	if (veryverbose) log << endl;

	// Each directory record has a "next" entry at the same level
	// and a "lower-level" entry ... do a depth-first traversal
	// descending recursively ...

	return traverseDepthFirst(DicomdirRecordsIndex,vRootDirectoryFirstRecord,log,verbose,veryverbose,showrecordinfo,showabstract,showpaths,showdescription,0);
}

