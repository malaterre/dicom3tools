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

// Same in dctable.cc

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

int
main(int argc,char **argv)
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool showfilename=options.get("filename");

	bool noerror=options.get("noerror");
	bool ignorereaderrors=options.get("ignorereaderrors");
	bool brief=options.get("brief");
	bool describe=options.get("describe");
	bool decimal=options.get("decimal") || options.get("d");

	bool bad=false;

	// identical to dctable.cc; should refactor this, perhaps as AttributeTag::read() ? :(
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
			<< " [-describe]"
			<< " [-noerror]"
			<< " [-ignorereaderrors]"
			<< " [-brief]"
			<< " [-decimal|d]"
			<< " [-key|k elementname|(0xgggg,0xeeee)]"
			<< " [-verbose|v]"
			<< " [-filename]"
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
	TextOutputStream  log(cerr);
	
	if (showfilename) {
		const char *filenameused = input_opener.getFilename();
		cerr << "Filename: \"" << (filenameused && strlen(filenameused) > 0 ? filenameused : "-") << "\"" << endl;
	}

	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit,
		true/*useStopAtTag*/,TagFromName(PixelData));

	if (!list.good()) {
		log << list.errors()
		    << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	if (success || ignorereaderrors) {
		ElementDictionary *dict=list.getDictionary();
		Assert(dict);
		AttributeListIterator listi(*keylist);
		while (!listi) {
			Attribute *ak=listi();
			Assert(ak);
			Tag tag=ak->getTag();
			Attribute *ad=list[tag];
			if (!describe && brief) {
				const char *keyword=0;
				if (dict) {
					keyword=dict->getKeyword(tag);	// not description; want without spaces
				}
				if (keyword) {
					log << keyword;
				}
				else {
					log << "(";
					writeZeroPaddedHexNumber(log,tag.getGroup(),4);
					log << ",";
					writeZeroPaddedHexNumber(log,tag.getElement(),4);
					log << ") ";
				}
				log << "=";
			}
			if (ad) {
				if (describe) {
					ad->write(log,dict);
				}
				else {
					if (decimal) {
						log << double(AttributeValue(ad,double(0)));
					}
					else {
						if (ad->isSequence()) {
							SequenceAttribute *as = (SequenceAttribute *)ad;
							dumpCodeSequenceAsString(as,log);
						}
						else {
							ad->writeData(log);
						}
					}
				}
			}
			else {
				if (!noerror && !brief) {
					log << EMsgDC(NotFound) << " - ";
					tag.write(log,dict);
				}
			}
			log << endl;
			++listi;
		}
	}

	return success ? 0 : 1;
}

