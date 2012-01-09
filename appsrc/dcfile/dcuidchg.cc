#include <fstream.h>

#include "attrmxls.h"
#include "attrtype.h"
#include "attrseq.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"

#include "hash.h"
#include "uidgen.h"

class UIDEntryString {
	HashKeyString key;
	const char *value;
public:
	UIDEntryString(void)		{}
	UIDEntryString(const char *s,const char *v)
					{ key=HashKeyString(s); value=v; }
	UIDEntryString(UIDEntryString *e)
					{ key=e->key; value=e->value; }

	HashKeyString	getKey(void) const	{ return key; }
	const char *	getValue(void) const	{ return value; }

	bool operator==(UIDEntryString e)
					{ return key == e.getKey(); }
};

class UIDEntryStringList : public SimpleList<UIDEntryString>
{
public:
	~UIDEntryStringList() {}	// only because buggy g++ 2.7.0 freaks
};

class UIDEntryStringListIterator : public SimpleListIterator<UIDEntryString>
{
public:
	UIDEntryStringListIterator(void)
		: SimpleListIterator<UIDEntryString>() {}
	UIDEntryStringListIterator(UIDEntryStringList& list)
		: SimpleListIterator<UIDEntryString>(list) {}
};

class UIDHashMap : public OpenHashTable <UIDEntryString,
					HashKeyString,
					UIDEntryStringList,
					UIDEntryStringListIterator>
{
public:
	UIDHashMap(unsigned long size)
		: OpenHashTable<UIDEntryString,
				HashKeyString,
				UIDEntryStringList,
				UIDEntryStringListIterator>(size)
		{}

	// supply virtual functions for OpenHashTable ...

	unsigned long	hash(const HashKeyString &key,unsigned long size)
		{
			const unsigned nchars = 10;
			const unsigned shift  = 4;

			unsigned n=nchars;
			unsigned long value=0;
			const char *s=key.getString();
			while (n-- && *s) {
				value=(value<<shift)|*s++;
			}
			return value%size; 
		}

	HashKeyString key(const UIDEntryString &e)	{ return e.getKey(); }
};

static bool
isAttributeToBeReplaced(Attribute *a) {
		Tag tag = a->getTag();
		return tag != TagFromName(SOPClassUID)
			&& tag != TagFromName(AffectedSOPClassUID)
			&& tag != TagFromName(MediaStorageSOPClassUID)
			&& tag != TagFromName(OriginalSpecializedSOPClassUID)
			&& tag != TagFromName(ReferencedRelatedGeneralSOPClassUIDInFile)
			&& tag != TagFromName(ReferencedSOPClassUID)
			&& tag != TagFromName(ReferencedSOPClassUIDInFile)
			&& tag != TagFromName(RelatedGeneralSOPClassUID)
			&& tag != TagFromName(RequestedSOPClassUID)
			&& tag != TagFromName(RelatedGeneralSOPClassUID)
			&& tag != TagFromName(SOPClassesInStudy)
			&& tag != TagFromName(SOPClassesSupported)
			&& tag != TagFromName(TransferSyntaxUID)
			&& tag != TagFromName(EncryptedContentTransferSyntaxUID)
			&& tag != TagFromName(MACCalculationTransferSyntaxUID)
			&& tag != TagFromName(ReferencedTransferSyntaxUIDInFile)
			&& tag != TagFromName(CodingSchemeUID)
			&& tag != TagFromName(ContextGroupExtensionCreatorUID)
			&& tag != TagFromName(PrivateInformationCreatorUID)
			&& tag != TagFromName(PrivateRecordUID);
}

static bool
loopOverListsInSequences(Attribute *a,UIDHashMap *uidHashMap,const char *uidstamp,ofstream *mapfstr,TextOutputStream &log,
		bool (*func)(AttributeList &,UIDHashMap *,const char *uidstamp,ofstream *mapfstr,TextOutputStream &))
{
	bool succeeded=true;
	if (strcmp(a->getVR(),"SQ") == 0) {
		AttributeList **al;
		int n;
		if ((n=a->getLists(&al)) > 0) {
			int i;
			for (i=0; i<n; ++i) {
				if (!(*func)(*al[i],uidHashMap,uidstamp,mapfstr,log)) succeeded=false;
			}
			delete [] al;
		}
	}
	return succeeded;
}

class OurGeneratedUID : public GeneratedUID {
public:
	OurGeneratedUID
			(const char *s,unsigned counter)
		: GeneratedUID(s)
		{
			setUnsigned(counter);
		}
};

static int uidcounter = 0;

static bool
changeUIDs(AttributeList& list,UIDHashMap *uidHashMap,const char *uidstamp,ofstream *mapfstr,TextOutputStream &log)
{
	bool succeeded=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequences(a,uidHashMap,uidstamp,mapfstr,log,&::changeUIDs))
			succeeded=false;
		const char *vr = a->getVR();
		if (vr && strcmp(vr,"UI") == 0) {
			int n = a->getVM();
			int i;
			for (int i=0; i<n; ++i) {
				char *originalUID;
				if (a->getValue(i,originalUID) && originalUID && strlen(originalUID) && isAttributeToBeReplaced(a)) {
//cerr << "Original UID = " << originalUID << endl;
					const char *replacementUID = NULL;
					UIDEntryString *entry = (*uidHashMap)[originalUID];
					if (entry) {
//cerr << "Found entry = " << entry->getKey().getString() << ", " << entry->getValue() << endl;
						replacementUID = entry->getValue();
					}
					else {
						replacementUID = StrDup(OurGeneratedUID(uidstamp,uidcounter++));
//cerr << "Add entry = " << originalUID << ", " << replacementUID << endl;
						(*uidHashMap)+=new UIDEntryString(StrDup(originalUID),replacementUID);
						if (mapfstr) {
							(*mapfstr) << "\"" << originalUID << "\" \"" << replacementUID << "\"" << endl;
						}
					}
//cerr << "Replacement UID = " << replacementUID << endl;
					a->setValue(i,replacementUID);
				}
			}
		}
		//	succeeded=false;
		++listi;
	}
	return succeeded;
}

static void addUIDChangedToDeidentificationMethod(AttributeList& list)
{
	const char *newvalue = "replace uids (dcuidchg)";
	Attribute *aDeidentificationMethod = list[TagFromName(DeidentificationMethod)];
	if (aDeidentificationMethod) {
		aDeidentificationMethod->addValue(newvalue);
	}
	else {
		aDeidentificationMethod = new LongStringAttribute(TagFromName(DeidentificationMethod),newvalue);
		list+=aDeidentificationMethod;
	}
}

static const char *
makeNewFileName(const char *outdir,const char *filename) {
	Assert(outdir);
	Assert(filename);
	size_t outdirLength = strlen(outdir);
	size_t filenameLength = strlen(filename);
	const char *filenameLastSeparator = strrchr(filename,'/');
	if (filenameLastSeparator != NULL) {
		filename = filenameLastSeparator+1;
		filenameLength = strlen(filename);
//cerr << "makeNewFileName(): found last component " << filename << endl;
	}
	size_t length = outdirLength + 1 + filenameLength;
	char *string=new char[length+1];
	strcpy(string,outdir);
	strcat(string,"/");
	strcat(string,filename);
	string[length]=0;
	return string;
}

static void handleOneFile(UIDHashMap *uidHashMap,const char *outdir,const char *filename,
			DicomInputOptions &dicom_input_options,
			DicomOutputOptions &dicom_output_options,
			bool listnames,bool updatedescriptor,bool success,bool verbose,
			ofstream *mapfstr,
			TextOutputStream &log) {

		Assert(filename);
		if (listnames) log << "Reading \"" << filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
		ifstream *ifstr=new ifstream(filename,ios::in|ios::binary);
#else
		ifstream *ifstr=new ifstream(filename);
#endif
		if (!ifstr || !*ifstr || !ifstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileReadOpenFailed);
			if (filename) cerr <<" - \"" << filename << "\"";
			cerr << endl;
			success=false;
			return;
		}

		DicomInputStream din(*(istream *)ifstr,
			dicom_input_options.transfersyntaxuid,
			dicom_input_options.usemetaheader);

		ManagedAttributeList list;

		if (verbose) log << "******** While reading ... " << filename << " ... ********" << endl;
		list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

		if (!list.good()) {
			log << list.errors()
			    << EMsgDC(DatasetReadFailed) << endl;
			success=false;
			return;
		}

		// do NOT close input file yet, else deferred pixel data read for write will fail
		
		changeUIDs(list,uidHashMap,dicom_output_options.stamp,mapfstr,log);

		if (updatedescriptor) {
			addUIDChangedToDeidentificationMethod(list);
		}

		const char *outfilename = makeNewFileName(outdir,filename);
		Assert(outfilename);
		if (listnames) log << "Writing \"" << outfilename << "\"" << endl;
		
#ifdef USEBINARYFLAGFOROUTPUTOPENMODE
		ofstream *ofstr=new ofstream(outfilename,
			ios::out|ios::trunc|ios::binary);
#else
		ofstream *ofstr=new ofstream(outfilename,ios::out|ios::trunc);
#endif
		if (!ofstr || !*ofstr || !ofstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileWriteOpenFailed);
			if (outfilename) cerr <<" - \"" << outfilename << "\"";
			cerr << endl;
			success=false;
			return;
		}
		
		DicomOutputStream dout(*(ostream *)ofstr,
			dicom_output_options.transfersyntaxuid,
			dicom_output_options.usemetaheader,
			dicom_output_options.useimplicitmetaheader,
			dicom_output_options.addtiff);

		if (!usualManagedAttributeListWrite(list,dout,
			dicom_output_options,log,verbose)) success=false;
		
		if (ofstr) {
			ofstr->close();
			delete ofstr;
		}
		if (outfilename) {
			delete outfilename;
		}
		// Now can close input file
		if (ifstr) {
			ifstr->close();
			delete ifstr;
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
	bool listnames=options.get("l");
	bool updatedescriptor=options.get("descriptor");

	const char *outdir=0;
	bad = !options.get("outdir",outdir) || bad;

	dicom_input_options.done();
	Assert(!dicom_input_options.filename);

	dicom_output_options.done();

	const char *filelistfile=0;
	(void)(options.get("filelist",filelistfile) || options.get("f",filelistfile));

	const char *mapfile=0;
	(void)(options.get("map",mapfile));

	int numberofinputfiles=!options;

	const char **listoffilenames = new const char * [numberofinputfiles];
	const char **ptr = listoffilenames;
	const char *filename;

	while(!options && (filename=options())) {
		++options;
		*ptr++=filename;
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
			<< " -outdir dirname"
			<< " [-v|-verbose]"
			<< " [-l]"
			<< " [-f|-filelist filename]"
			<< " [-map filenametowriteuidmap]"
			<< " [-descriptor]"
			<< " " << MMsgDC(InputFile) << " ["<< MMsgDC(InputFile) << " ...]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	if (!dicom_output_options.transfersyntaxuid)
		dicom_output_options.transfersyntaxuid=ExplicitVRLittleEndianTransferSyntaxUID;

	bool success=true;
	TextOutputStream log(cerr);

	UIDHashMap *uidHashMap = new UIDHashMap(1000);

	ofstream *mapfstr=NULL;
	if (mapfile) {
		mapfstr=new ofstream(mapfile,ios::out|ios::trunc);
		Assert(mapfstr);
	}
	
	int i;
	for (i=0; i < numberofinputfiles; ++i) {
		handleOneFile(uidHashMap,outdir,listoffilenames[i],dicom_input_options,dicom_output_options,listnames,updatedescriptor,success,verbose,mapfstr,log);
	}

	if (filelistfile) {
		ifstream *flfstr=new ifstream(filelistfile);
		if (!flfstr || !*flfstr || !flfstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileReadOpenFailed);
			if (filelistfile) cerr <<" - \"" << filename << "\"";
			cerr << endl;
			bad=true;
		}
		else {
			while (flfstr->peek() != EOF) {
				const int lineBufferSize=2048;
				char lineBuffer[lineBufferSize];
				flfstr->getline(lineBuffer,2048);
				if (strlen(lineBuffer))
					handleOneFile(uidHashMap,outdir,lineBuffer,dicom_input_options,dicom_output_options,listnames,updatedescriptor,success,verbose,mapfstr,log);
				// else skip blank lines
			}
		}
	}
	
	if (mapfstr) {
		mapfstr->close();
		delete mapfstr;
	}

	if (numberofinputfiles && listoffilenames) delete[] listoffilenames;
	return success ? 0 : 1;
}

