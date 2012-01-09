#include "attrmxls.h"
#include "attrtype.h"
#include "attrval.h"
#include "attrseq.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "sopclu.h"
#include "elmconst.h"

static void
copyRelevantAttributes(ManagedAttributeList& list,ManagedAttributeList& newList,bool verbose,TextOutputStream& log) {


	newList+= new ShortStringAttribute(TagFromName(StudyID),(char *)AttributeValue(list[TagFromName(StudyID)],""));
	newList+= new LongStringAttribute(TagFromName(StudyDescription),(char *)AttributeValue(list[TagFromName(StudyDescription)],""));
	newList+= new ShortStringAttribute(TagFromName(AccessionNumber),(char *)AttributeValue(list[TagFromName(AccessionNumber)],""));
	newList+= new UIStringAttribute(TagFromName(StudyInstanceUID),(char *)AttributeValue(list[TagFromName(StudyInstanceUID)],""));
	newList+= new PersonNameAttribute(TagFromName(PatientName),(char *)AttributeValue(list[TagFromName(PatientName)],""));
	newList+= new LongStringAttribute(TagFromName(PatientID),(char *)AttributeValue(list[TagFromName(PatientID)],""));
	newList+= new IntegerStringAttribute(TagFromName(SeriesNumber),(Uint32)AttributeValue(list[TagFromName(SeriesNumber)],1)+1000);
	newList+= new IntegerStringAttribute(TagFromName(InstanceNumber),(Uint32)AttributeValue(list[TagFromName(InstanceNumber)],1));
	newList+= new DateStringAttribute(TagFromName(PatientBirthDate),(char *)AttributeValue(list[TagFromName(PatientBirthDate)],""));
	newList+= new CodeStringAttribute(TagFromName(PatientSex),(char *)AttributeValue(list[TagFromName(PatientSex)],""));
	newList+= new DateStringAttribute(TagFromName(StudyDate),(char *)AttributeValue(list[TagFromName(StudyDate)],""));
	newList+= new TimeStringAttribute(TagFromName(StudyTime),(char *)AttributeValue(list[TagFromName(StudyTime)],""));
	newList+= new PersonNameAttribute(TagFromName(ReferringPhysicianName),(char *)AttributeValue(list[TagFromName(ReferringPhysicianName)],""));
	newList+= new LongStringAttribute(TagFromName(Manufacturer),(char *)AttributeValue(list[TagFromName(Manufacturer)],""));
	newList+= new DateStringAttribute(TagFromName(ContentDate),(char *)AttributeValue(list[TagFromName(ContentDate)],""));
	newList+= new TimeStringAttribute(TagFromName(ContentTime),(char *)AttributeValue(list[TagFromName(ContentTime)],""));

	// should copy these but lazy for now (since not likely in Osiris output):(

	newList+= new SequenceAttribute(TagFromName(PerformedProcedureCodeSequence));
	newList+= new SequenceAttribute(TagFromName(ReferencedPerformedProcedureStepSequence));
}

static Uint16 findGroupContainingOverlayID(AttributeList *list,Uint32 id) {
	Uint16 g=0x6001;
	Attribute *a;
	while ((a=(*list)[Tag(g,0x1000)])) {
		Uint32 vOverlayID;
		a->getValue(0,vOverlayID);
		if (id == vOverlayID) return g;
		g+=0x0002;
	}
	return 0;
}

static Attribute *
newSingleCodedSequenceAttribute(Tag tag,char *cv,char *csd,char *cm) {
	SequenceAttribute *a = new SequenceAttribute(tag);
	Assert(a);
	AttributeList *list = new AttributeList();
	Assert(list);
	(*a)+=list;
	(*list)+=new ShortStringAttribute(TagFromName(CodeValue),cv);
	(*list)+=new ShortStringAttribute(TagFromName(CodingSchemeDesignator),csd);
	(*list)+=new LongStringAttribute(TagFromName(CodeMeaning),cm);
	return a;
}

static void
addLesionItem(SequenceAttribute *aMeasurementsContentSequence,char *vROIType,char *vLabel,int nContourPoints,Float32 *vContourPoints,
		const char *vReferencedSOPClassUID,const char *vReferencedSOPInstanceUID) {
	Assert(aMeasurementsContentSequence);
	AttributeList *list = new AttributeList();
	Assert(list);
	(*aMeasurementsContentSequence)+=list;
	(*list)+=new CodeStringAttribute(TagFromName(ValueType),"TEXT");
	(*list)+=new CodeStringAttribute(TagFromName(RelationshipType),"CONTAINS");
	(*list)+=newSingleCodedSequenceAttribute(TagFromName(ConceptNameCodeSequence),"333301","99RADPHARM","Lesion");

	// parse out label

	char *nameFromLabel;
	char *valueFromLabel;
	char *unitsFromLabel;

	int lLabel=strlen(vLabel);
	char *equalsPosition = strrchr(vLabel,'=');
	if (equalsPosition) {
		int lNameFromLabel=equalsPosition-vLabel;
		nameFromLabel=new char[lNameFromLabel];
		strncpy(nameFromLabel,vLabel,equalsPosition-vLabel);

		char *lastSpacePosition = strrchr(vLabel,' ');
		if (lastSpacePosition && lastSpacePosition > equalsPosition) {
			int lUnitsFromLabel=lLabel-(lastSpacePosition-vLabel);
			unitsFromLabel=new char[lUnitsFromLabel];
			strncpy(unitsFromLabel,lastSpacePosition+1,lUnitsFromLabel);
		}
		else {
			unitsFromLabel=0;
			lastSpacePosition=vLabel+lLabel-1;
		}
		int lValueFromLabel=lastSpacePosition-equalsPosition-1;
		valueFromLabel=new char[lValueFromLabel];
		strncpy(valueFromLabel,equalsPosition+1,lValueFromLabel);
	}
	else {
		nameFromLabel=vLabel;
		valueFromLabel=0;
		unitsFromLabel=0;
	}
cerr << "nameFromLabel = <" << nameFromLabel << ">" << endl;
cerr << "valueFromLabel = <" << valueFromLabel << ">" << endl;
cerr << "unitsFromLabel = <" << unitsFromLabel << ">" << endl;

	(*list)+=new UnlimitedTextAttribute(TagFromName(TextValue),nameFromLabel);

	AttributeList *listToAddScoordTo;

	if (valueFromLabel && unitsFromLabel) {
		SequenceAttribute *aNumericContentSequence = new SequenceAttribute(TagFromName(ContentSequence));
		(*list)+=aNumericContentSequence;
		AttributeList *ncsl = new AttributeList();
		Assert(ncsl);
		(*aNumericContentSequence)+=ncsl;
		listToAddScoordTo=ncsl;
		(*ncsl)+=new CodeStringAttribute(TagFromName(ValueType),"NUM");
		(*ncsl)+=new CodeStringAttribute(TagFromName(RelationshipType),"HAS PROPERTIES");
		(*ncsl)+=newSingleCodedSequenceAttribute(TagFromName(ConceptNameCodeSequence),"121206","DCM","Distance");

		SequenceAttribute *aMeasuredValueSequence = new SequenceAttribute(TagFromName(MeasuredValueSequence));
		(*ncsl)+=aMeasuredValueSequence;
		AttributeList *mvsl = new AttributeList();
		Assert(mvsl);
		(*aMeasuredValueSequence)+=mvsl;
		(*mvsl)+=new DecimalStringAttribute(TagFromName(NumericValue),valueFromLabel);
		(*mvsl)+=newSingleCodedSequenceAttribute(TagFromName(MeasurementUnitsCodeSequence),unitsFromLabel,"UCUM",unitsFromLabel);
	}
	else {
		listToAddScoordTo=list;
	}

	Assert(listToAddScoordTo);
	SequenceAttribute *aScoordContentSequence = new SequenceAttribute(TagFromName(ContentSequence));
	(*listToAddScoordTo)+=aScoordContentSequence;
	AttributeList *scol = new AttributeList();
	Assert(scol);
	(*aScoordContentSequence)+=scol;
	(*scol)+=new CodeStringAttribute(TagFromName(ValueType),"SCOORD");
	(*scol)+=new CodeStringAttribute(TagFromName(RelationshipType),"INFERRED FROM");
	// no ConceptNameCodeSequence

	if (vROIType) {
		if (strcmp(vROIType,"CALLIPER") == 0) {
			(*scol)+=new CodeStringAttribute(TagFromName(GraphicType),"POLYLINE");
			Attribute *aGraphicData = new FloatSingleAttribute(TagFromName(GraphicData));
			Assert(aGraphicData);
			(*scol)+=aGraphicData;
			if (nContourPoints >= 4 && vContourPoints) {
				aGraphicData->addValue(vContourPoints[0]);
				aGraphicData->addValue(vContourPoints[1]);
				aGraphicData->addValue(vContourPoints[2]);
				aGraphicData->addValue(vContourPoints[3]);
			}
		}
		else if (strcmp(vROIType,"ELLIPSE") == 0) {
			(*scol)+=new CodeStringAttribute(TagFromName(GraphicType),"ELLIPSE");
			Attribute *aGraphicData = new FloatSingleAttribute(TagFromName(GraphicData));
			Assert(aGraphicData);
			(*scol)+=aGraphicData;
			if (nContourPoints >= 4 && vContourPoints) {
				aGraphicData->addValue(vContourPoints[0]);
				aGraphicData->addValue(vContourPoints[1]);
				aGraphicData->addValue(vContourPoints[2]);
				aGraphicData->addValue(vContourPoints[3]);
			}
		}
	}	


	SequenceAttribute *aImageContentSequence = new SequenceAttribute(TagFromName(ContentSequence));
	(*scol)+=aImageContentSequence;
	AttributeList *imgl = new AttributeList();
	Assert(imgl);
	(*aImageContentSequence)+=imgl;
	(*imgl)+=new CodeStringAttribute(TagFromName(ValueType),"IMAGE");
	(*imgl)+=new CodeStringAttribute(TagFromName(RelationshipType),"SELECTED FROM");
	// no ConceptNameCodeSequence

	SequenceAttribute *aReferencedSOPSequence = new SequenceAttribute(TagFromName(ReferencedSOPSequence));
	(*imgl)+=aReferencedSOPSequence;
	AttributeList *rsopl = new AttributeList();
	Assert(rsopl);
	(*aReferencedSOPSequence)+=rsopl;
	(*rsopl)+=new UIStringAttribute(TagFromName(ReferencedSOPClassUID),vReferencedSOPClassUID);
	(*rsopl)+=new UIStringAttribute(TagFromName(ReferencedSOPInstanceUID),vReferencedSOPInstanceUID);

}

static void
copyOverlayAttributesIntoContent(ManagedAttributeList& list,ManagedAttributeList& newList,bool verbose,TextOutputStream& log,const char *useuid) {
	newList+= new SequenceAttribute(TagFromName(VerifyingObserverSequence));

// goal is::
// : CONTAINER: (333300,99RADPHARM,"Measurements")  [SEPARATE]
//	>HAS OBS CONTEXT: PNAME: (121008,DCM,"Person 0bserver Name")  = "Smith^John^^Dr^"
//      >CONTAINS: TEXT: (000444,LNdemo,"Lesion")  = "1"
//               >>HAS PROPERTIES: NUM: (121206,DCM,"Distance")  = 130 (mm,UCUM,"mm")
//                        >>>INFERRED FROM: SCOORD
//                                >>>>SELECTED FROM: IMAGE:
//      >CONTAINS: TEXT: (000444,LNdemo,"Lesion")  = "1"
//                >>INFERRED FROM: SCOORD
//                        >>>SELECTED FROM: IMAGE:


	newList+=new CodeStringAttribute(TagFromName(ValueType),"CONTAINER");
	newList+=new CodeStringAttribute(TagFromName(ContinuityOfContent),"SEPARATE");

	SequenceAttribute *aContentSequence = new SequenceAttribute(TagFromName(ContentSequence));
	newList+=aContentSequence;

	newList+=newSingleCodedSequenceAttribute(TagFromName(ConceptNameCodeSequence),"333300","99RADPHARM","Measurements");

	const char *vReferencedSOPClassUID = AttributeValue(list[TagFromName(SOPClassUID)]);
	const char *vReferencedSOPInstanceUID = useuid ? useuid : AttributeValue(list[TagFromName(SOPInstanceUID)]);

	Attribute *a = list[Tag(0x6001,0x10c0)];	// UIN Overlay Sequence
	if (a && a->isSequence()) {
		SequenceAttribute *aSeq=(SequenceAttribute *)a;
		AttributeList **itemLists;
		int nLists=aSeq->getLists(&itemLists);
		if (nLists == 1) {
			AttributeList *itemList = itemLists[0];
cerr << "search groups" << endl;
			Uint16 g=0x6001;
			while ((*itemList)[Tag(g,0x0010)]) {
cerr << "group =0x" << hex << g << dec << endl;
				Uint32 vOverlayID = AttributeValue((*itemList)[Tag(g,0x1000)]);
cerr << "Overlay ID = " << vOverlayID << endl;
				char *vROIType = AttributeValue((*itemList)[Tag(g,0x10B0)],"");
cerr << "ROI Type = " << vROIType << endl;
				// this mess is because it is an LT not an IS (unlike OverlayID) ...
				char *sAttachedAnnotation = AttributeValue((*itemList)[Tag(g,0x10B2)]);
				Uint32 vAttachedAnnotation = sAttachedAnnotation ?(Uint32)atof(sAttachedAnnotation) : 0;
cerr << "Attached Annotation = " << vAttachedAnnotation << endl;
				Uint16 groupOfAttachedAnnotation = vAttachedAnnotation ? findGroupContainingOverlayID(itemList,vAttachedAnnotation) : 0;
cerr << "Attached Annotation group =0x" << hex << groupOfAttachedAnnotation << dec << endl;

				Attribute *aContourPoints = (*itemList)[Tag(g,0x10BA)];
				if (vROIType && strcmp(vROIType,"ANNOTATION") != 0 && groupOfAttachedAnnotation && aContourPoints) {
					char *vLabel = AttributeValue((*itemList)[Tag(groupOfAttachedAnnotation,0x10A0)],"");
cerr << "Label = " << vLabel << endl;
					int nContourPoints = aContourPoints->getVM();
					Float32 vContourPoints[nContourPoints];
					int i;
					for (i=0; i<nContourPoints; ++i) {
						aContourPoints->getValue(i,*(vContourPoints+i));
cerr << "Contour point[" << i << "] = " << vContourPoints[i] << endl;
					}

					addLesionItem(aContentSequence,vROIType,vLabel,nContourPoints,vContourPoints,
						vReferencedSOPClassUID,vReferencedSOPInstanceUID);
				}

				g+=0x0002;
			}
		}
	}
}

static void
addDeNovoAttributes(ManagedAttributeList& newList,bool verbose,TextOutputStream& log) {
	newList+=new UIStringAttribute(TagFromName(SOPClassUID),EnhancedSRStorageSOPClassUID);
	newList+=new CodeStringAttribute(TagFromName(Modality),"SR");
	newList+=new CodeStringAttribute(TagFromName(CompletionFlag),"COMPLETE");
	newList+=new CodeStringAttribute(TagFromName(VerificationFlag),"UNVERIFIED");
}

static void
addNewSeriesAndInstanceUIDs(ManagedAttributeList& list,bool verbose,TextOutputStream& log) {
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool ignorereaderrors=options.get("ignorereaderrors");

	const char *useSOPInstanceUID=0;
	options.get("useuid",useSOPInstanceUID);

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
			<< " [-useuid SOPInstanceUID]"
			<< " [-v|-verbose]"
			<< " [-ignorereaderrors]"
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
		ManagedAttributeList newList;

		copyRelevantAttributes(list,newList,verbose,log);
		copyOverlayAttributesIntoContent(list,newList,verbose,log,useSOPInstanceUID);
		addDeNovoAttributes(newList,verbose,log);
		addNewSeriesAndInstanceUIDs(newList,verbose,log);

		if (!usualManagedAttributeListWrite(newList,dout,
			dicom_output_options,log,verbose)) success=false;
	}

	return success ? 0 : 1;
}

	





