#include <fstream.h>

#include "attrmxls.h"
#include "attrnew.h"
#include "attrval.h"
#include "attrseq.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "dcstream.h"
#include "elmconst.h"

static const char *options_output_key[] = {
	"key",
	"k",
	0
};

// Same in dckey.cc

static void dumpCodeSequenceAsString(SequenceAttribute *as,TextOutputStream &log) {
	AttributeList **itemLists;
	int nItems = as->getLists(&itemLists);
	for (int i=0; i<nItems; ++i) {
		AttributeList *thisItemList = itemLists[i];
		if (thisItemList) {
			Attribute *aCodeValue = (*thisItemList)[TagFromName(CodeValue)];
			Attribute *aCodingSchemeDesignator = (*thisItemList)[TagFromName(CodingSchemeDesignator)];
			Attribute *aCodeMeaning = (*thisItemList)[TagFromName(CodeMeaning)];
			if (aCodeValue || aCodingSchemeDesignator || aCodeMeaning) {
				const char *vCodeValue = AttributeValue(aCodeValue,"");
				const char *vCodingSchemeDesignator = AttributeValue(aCodingSchemeDesignator,"");
				const char *vCodeMeaning = AttributeValue(aCodeMeaning,"");
				log << "('"
				    << vCodeValue
				    << "','"
				    << vCodingSchemeDesignator
				    << "','"
				    << vCodeMeaning
				    << "')"
				;
			}
		}
	}
}

static Attribute *
findFirstInstanceOfAttributeNestedAnywhereInList(Tag tag,AttributeList& list) {
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		if (a->isSequence()) {
			SequenceAttribute *as = (SequenceAttribute *)a;
			AttributeList **itemLists;
			int nItems = as->getLists(&itemLists);
			for (int i=0; i<nItems; ++i) {
				AttributeList *thisItemList = itemLists[i];
				if (thisItemList) {
					Attribute *aFound = (*thisItemList)[tag];
					if (aFound) {
						return aFound;
					}
				}
			}
		}
		++listi;
	}
	return NULL;
}

static void
writeQuoteRemovedDecimalConvertedData(Attribute *a,TextOutputStream &stream)
{
	int n = a->getVM();
	for (int i=0; i<n; ++i) {
		char *string;
		Uint32 l = strlen(string);
		if (a->getValue((Uint16)i,string)) {
			l = strlen(string);
		}
		else {
			l = 0;
		}
		const char *p = string;
		while (l--) {
			char c = *p++;
			if (c != '"') {
				stream << c;
			}
		}
		++i;
		if (!i) {
			stream << "\\";
		}
	}
}			

int
main(int argc,char **argv)
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	bool describe=options.get("describe");

	bool nofilename=options.get("nofilename");
	
	bool recurseToFindIt=options.get("recurse") || options.get("r");
	
	bool ignorereaderrors=options.get("ignorereaderrors");
	
	bool noembeddedquotes=options.get("noembeddedquotes");

	bool bad=false;

	// identical to dckey.cc; should refactor this, perhaps as AttributeTag::read() ? :(
	ElementDictionary optdict;
	AttributeList *keylist=0;
	const char *keyarg;
	while (options.get(options_output_key,keyarg)) {
		Assert(keyarg);
		Tag tag;
		if (!getAttributeTagFromStringHexForm(keyarg,tag)	// handle "-k (0xgggg,0xeeee)" case
		 && !optdict.getTag(keyarg,tag)) {			// handle "-k keyword" case
			bad=true;
			cerr << "-" << options_output_key[0] << ": "
			     << EMsgDC(UnrecognizedElement)
			     << " - \"" << keyarg << "\""
			     << endl;
		}
		else {
			const char *vr=optdict.getValueRepresentation(tag);
			if (!vr) vr="UN";
			Attribute *a=newAttribute(vr,tag);
			Assert(a);
			if (!keylist) {
				keylist=new AttributeList;
				Assert(keylist);
			}
			(*keylist)+=a;
		}
	}

	dicom_input_options.done();
	Assert(!dicom_input_options.filename);

	int numberofinputfiles=!options;
	const char **listoffilenames = new const char * [numberofinputfiles];
	const char **ptr = listoffilenames;
	const char *filename;

	while(!options && (filename=options())) {
		++options;
		*ptr++=filename;
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
			<< " [-ignorereaderrors]"
			<< " [-describe]"
			<< " [-nofilename]"
			<< " [-noembeddedquotes]"
			<< " [-key|k elementname|(0xgggg,0xeeee)]"
			<< " [-recurse|r]"
			<< " [-verbose|v]"
			<< " " << MMsgDC(InputFile) << " ["<< MMsgDC(InputFile) << " ...]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		return 1;
	}

	bool success=true;
	TextOutputStream log(cerr);
	TextOutputStream outstream(cout);

	if (describe) {
		bool addtab=false;
		if (!nofilename) {
			outstream << "Filename";
			addtab=true;
		}
		AttributeListIterator listi(*keylist);
		while (!listi) {
			Attribute *ak=listi();
			Assert(ak);
			if (addtab) {
				outstream << "\t";
			}
			addtab=true;
			Tag tag=ak->getTag();
			const char *desc=optdict.getDescription(tag);
			if (desc) {
				outstream << "\"" << desc << "\"";
			}
			else {
				outstream << "(";
				writeZeroPaddedHexNumber(outstream,tag.getGroup(),4);
				outstream << ",";
				writeZeroPaddedHexNumber(outstream,tag.getElement(),4);
				outstream << ") " << flush;
			}
			++listi;
		}
		outstream << endl;
	}

	int i;
	for (i=0; i < numberofinputfiles; ++i) {
		const char *filename=listoffilenames[i];
		Assert(filename);
		if (verbose) log << "Reading \"" << filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
		ifstream *fstr=new ifstream(filename,ios::in|ios::binary);
#else
		ifstream *fstr=new ifstream(filename);
#endif
		if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
			log << AMsgDC(FileReadOpenFailed);
			if (filename) log <<" - \"" << filename << "\"";
			success=false;
			if (fstr) delete fstr;
			break;
		}

		DicomInputStream din(*(istream *)fstr,
			dicom_input_options.transfersyntaxuid,
			dicom_input_options.usemetaheader);

		ManagedAttributeList list;

		if (verbose) log << "******** While reading ... " << filename << " ... ********" << endl; 
		list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

		if (!list.good()) {
			log << list.errors()
			    << filename << ": " << EMsgDC(DatasetReadFailed) << endl;
			success=false;
			if (!ignorereaderrors) {
				if (fstr) delete fstr;
				break;
			}
		}
		{
			bool addtab=false;
			if (!nofilename) {
				outstream << "\"" << filename << "\"";
				addtab=true;
			}
			ElementDictionary *dict=list.getDictionary();
			Assert(dict);
			AttributeListIterator listi(*keylist);
			while (!listi) {
				Attribute *ak=listi();
				Assert(ak);
				if (addtab) {
					outstream << "\t";
				}
				addtab=true;
				Tag tag=ak->getTag();
				Attribute *ad=list[tag];
				if (!ad && recurseToFindIt) {
					ad = findFirstInstanceOfAttributeNestedAnywhereInList(tag,list);
				}
				if (ad) {
					outstream << "\"";
					if (ad->isSequence()) {
						SequenceAttribute *as = (SequenceAttribute *)ad;
						dumpCodeSequenceAsString(as,outstream);
					}
					else {
						if (noembeddedquotes) {
							writeQuoteRemovedDecimalConvertedData(ad,outstream);
						}
						else {
							ad->writeData(outstream);
						}
					}
					outstream << "\"";
				}
				else {
					outstream << "\"\"";
				}
				++listi;
			}
			outstream << endl;
		}

		if (fstr) delete fstr;
	}

	if (numberofinputfiles && listoffilenames) delete[] listoffilenames;

	return success ? 0 : 1;
}

