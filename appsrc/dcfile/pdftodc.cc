#include <ctype.h>

#include "attrtype.h"
#include "attrothr.h"
#include "attrmxls.h"
#include "attrseq.h"
#include "transynu.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "ioopt.h"
#include "dcopt.h"
#include "sopclu.h"

// addCodeSequenceItemToItemList() is copied from appsrc/dcfile/dcmulti.cc and probably should be factored out somewhere

static void addCodeSequenceItemToItemList(
	AttributeList *i,
	const char *csd,
	const char *cv,
	const char *cm,
	const char *csvn)
{
	Assert(i);
	//cm = lookUpAndFillInCodeMeaning(csd,cv,cm,csvn);
	if (csd  && strlen(csd))  (*i)+=new ShortStringAttribute(TagFromName(CodingSchemeDesignator),csd);
	if (cv   && strlen(cv))   (*i)+=new ShortStringAttribute(TagFromName(CodeValue),cv);
	if (cm   && strlen(cm))   (*i)+=new  LongStringAttribute(TagFromName(CodeMeaning),cm);
	if (csvn && strlen(csvn)) (*i)+=new ShortStringAttribute(TagFromName(CodingSchemeVersion),csvn);
}

// removeLeadingSpaces() is copied from libsrc/src/attrtyps.cc and probably should be factored out somewhere

static const char*
removeLeadingSpaces(const char *from)
{
	if (from) while (*from && isspace(*from)) ++from;
	return from;
}

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	InputOptions		input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	bool addConceptNameCodeSequence = false;
	const char *addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodingSchemeDesignator;
	const char *addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodeValue;
	const char *addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodeMeaning;
	const char *addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodingSchemeVersion;
	{
		const char *addConceptNameCodeSequenceArgs[4];
		int n = options.get("conceptname",addConceptNameCodeSequenceArgs,4);
//cerr << "addConceptNameCodeSequence returns " << dec << n << endl;
		if (n > 0) {
			if (n == 4) {
//cerr << "option: addConceptNameCodeSequence: addConceptNameCodeSequenceArgs[0] " << addConceptNameCodeSequenceArgs[0] << endl;
//cerr << "option: addConceptNameCodeSequence: addConceptNameCodeSequenceArgs[1] " << addConceptNameCodeSequenceArgs[1] << endl;
//cerr << "option: addConceptNameCodeSequence: addConceptNameCodeSequenceArgs[2] " << addConceptNameCodeSequenceArgs[2] << endl;
//cerr << "option: addConceptNameCodeSequence: addConceptNameCodeSequenceArgs[3] " << addConceptNameCodeSequenceArgs[3] << endl;
				addConceptNameCodeSequence=true;
				addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodingSchemeDesignator = removeLeadingSpaces(addConceptNameCodeSequenceArgs[0]);
				             addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodeValue = removeLeadingSpaces(addConceptNameCodeSequenceArgs[1]);
				           addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodeMeaning = removeLeadingSpaces(addConceptNameCodeSequenceArgs[2]);
				   addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodingSchemeVersion = removeLeadingSpaces(addConceptNameCodeSequenceArgs[3]);
			}
			else {
				cerr << "addreferenced needs 4 arguments, not " << dec << n << endl;
				bad=true;
			}
		}
	}

	input_options.done();
	dicom_output_options.done();
	options.done();

	InputOpenerFromOptions input_opener(
		options,input_options.filename,cin);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< input_options.usage()
			<< dicom_output_options.usage()
			<< " [-conceptname scheme code meaning version]"
			<< " [-v|-verbose]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	istream in(input_opener);
	OurStreamPos pdfFileLength = OurStreamPos(0);
	in.seekg(0,ios::end);
	pdfFileLength = in.tellg();
	in.seekg(0,ios::beg);
	
	BinaryInputStream bin(in,NoEndian);

	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	ManagedAttributeList list;

	TransferSyntax transfersyntax(
		dicom_output_options.transfersyntaxuid
		? dicom_output_options.transfersyntaxuid
		: DefaultTransferSyntaxUID);

	TextOutputStream log (cerr);
	if (verbose) log << "Converting ..." << endl;

	// various Type 1 and Type 2 attributes for mandatory Encapsulated Document modules ...

	list+=new PersonNameAttribute(TagFromName(PatientName),"^^^^");
	list+=new LongStringAttribute(TagFromName(PatientID));
	list+=new DateStringAttribute(TagFromName(PatientBirthDate));
	list+=new CodeStringAttribute(TagFromName(PatientSex));
	list+=new ShortStringAttribute(TagFromName(StudyID));
	list+=new DateStringAttribute(TagFromName(StudyDate));
	list+=new TimeStringAttribute(TagFromName(StudyTime));
	list+=new PersonNameAttribute(TagFromName(ReferringPhysicianName),"^^^^");
	list+=new ShortStringAttribute(TagFromName(AccessionNumber));
	list+=new IntegerStringAttribute(TagFromName(SeriesNumber),Int32(1));
	list+=new IntegerStringAttribute(TagFromName(InstanceNumber),Int32(1));
	list+=new DateStringAttribute(TagFromName(ContentDate));
	list+=new TimeStringAttribute(TagFromName(ContentTime));
	list+=new DateTimeStringAttribute(TagFromName(AcquisitionDateTime));

	list+=new UIStringAttribute(TagFromName(SOPClassUID),EncapsulatedPDFStorageSOPClassUID);
	list+=new CodeStringAttribute(TagFromName(Modality),"OT");
	list+=new CodeStringAttribute(TagFromName(ConversionType),"WSD");
	list+=new CodeStringAttribute(TagFromName(BurnedInAnnotation),"YES");

	list+=new LongStringAttribute(TagFromName(Manufacturer));
	
	list+=new ShortTextAttribute(TagFromName(DocumentTitle));
	
	SequenceAttribute *aConceptNameCodeSequence = new SequenceAttribute(TagFromName(ConceptNameCodeSequence));
	Assert(aConceptNameCodeSequence);
	list+=aConceptNameCodeSequence;
	if (addConceptNameCodeSequence) {
		AttributeList *iConceptNameCodeSequence = new AttributeList();
		Assert(iConceptNameCodeSequence);
		(*aConceptNameCodeSequence)+=iConceptNameCodeSequence;

		addCodeSequenceItemToItemList(iConceptNameCodeSequence,
			addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodingSchemeDesignator,
			addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodeValue,
			addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodeMeaning,
			addConceptNameCodeSequence_purposeOfReferenceCodeSequenceCodingSchemeVersion);
	}

	list+=new LongStringAttribute(TagFromName(MIMETypeOfEncapsulatedDocument),"application/pdf");
	list+=new OtherByteLargeNonPixelAttributeBase(TagFromName(EncapsulatedDocument),bin,OurStreamPos(0),pdfFileLength);		// pdfFileLength may be odd length; will be handled

	if (!usualManagedAttributeListWrite(list,dout,
		dicom_output_options,log,verbose)) return 1;

	return 0;
}

