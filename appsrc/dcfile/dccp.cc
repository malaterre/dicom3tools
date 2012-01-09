#include "attrmxls.h"
#include "attrseq.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"

void
keepOnlySelectedLUTSequenceItem(AttributeList &list,Tag lutSequenceTag,int whichLUTItemToKeep,TextOutputStream &log) {
	Attribute *aLUTSequence = list[lutSequenceTag];
	if (aLUTSequence) {
		AttributeList **items;
		int nLUTItems=aLUTSequence->getLists(&items);
		if (whichLUTItemToKeep >= 0 && whichLUTItemToKeep < nLUTItems) {
			AttributeList *itemToKeep = items[whichLUTItemToKeep];
			SequenceAttribute *newLUTSequence = new SequenceAttribute(lutSequenceTag);
			Assert(newLUTSequence);
			(*newLUTSequence)+=itemToKeep;
			list-=lutSequenceTag;
			list+=newLUTSequence;
		}
		else {
			log << WMsgDC("Missing LUT Sequence Item") << " - " << lutSequenceTag << " # " << whichLUTItemToKeep << endl;
		}
	}
	else {
		log << WMsgDC("Missing LUT Sequence") << " - " << lutSequenceTag << endl;
	}
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool ignorereaderrors=options.get("ignorereaderrors");
	bool removecommandgroup=options.get("removecommandgroup");

	int whichVOILUTitem=0;
	bool selectVOILUTitem=options.get("selectvoilutitem",whichVOILUTitem);

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
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose]"
			<< " [-ignorereaderrors]"
			<< " [-removecommandgroup]"
			<< " [-selectvoilutitem 0..n-1]"
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

	bool success=true;
	TextOutputStream  log(cerr);
	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	if (!list.good()) {
		if (!ignorereaderrors) {
			log << list.errors();
			success=false;
		}
		log << EMsgDC(DatasetReadFailed) << endl;
	}

	if (success) {
		// Don't write list warnings yet ...
		// done in usualManagedAttributeListWrite ...
		
		if (removecommandgroup) {
			list.removeCommandGroup();
		}
		
		if (selectVOILUTitem) {
			keepOnlySelectedLUTSequenceItem(list,TagFromName(VOILUTSequence),whichVOILUTitem,log);
		}

		if (!usualManagedAttributeListWrite(list,dout,
			dicom_output_options,log,verbose)) success=false;
	}

	return success ? 0 : 1;
}
