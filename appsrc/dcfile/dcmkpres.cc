#include "attrtype.h"
#include "attrothr.h"
#include "attrmxls.h"
#include "attrseq.h"
#include "attrval.h"
#include "sopclu.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "ioopt.h"
#include "dcopt.h"

static bool
createPresentationState(ManagedAttributeList &input_list,ManagedAttributeList &output_list,TextOutputStream &log,
	const char *plutshape)
{
	bool success = true;

	// Fetch necessary attributes to reference image ...

	char *vStudyInstanceUIDOfReferencedImage=0;
	{
		Attribute *aStudyInstanceUIDOfReferencedImage=input_list[TagFromName(StudyInstanceUID)];
		if (aStudyInstanceUIDOfReferencedImage)
			vStudyInstanceUIDOfReferencedImage=AttributeValue(aStudyInstanceUIDOfReferencedImage);
		if (!vStudyInstanceUIDOfReferencedImage || !*vStudyInstanceUIDOfReferencedImage)  {
			log << EMsgDC(MissingAttributeInReferencedImage) << " - Study Instance UID" << endl;
			success=false;
		}
	}

	char *vSeriesInstanceUIDOfReferencedImage=0;
	{
		Attribute *aSeriesInstanceUIDOfReferencedImage=input_list[TagFromName(SeriesInstanceUID)];
		if (aSeriesInstanceUIDOfReferencedImage)
			vSeriesInstanceUIDOfReferencedImage=AttributeValue(aSeriesInstanceUIDOfReferencedImage);
		if (!vSeriesInstanceUIDOfReferencedImage || !*vSeriesInstanceUIDOfReferencedImage)  {
			log << EMsgDC(MissingAttributeInReferencedImage) << " - Series Instance UID" << endl;
			success=false;
		}
	}

	char *vSOPInstanceUIDOfReferencedImage=0;
	{
		Attribute *aSOPInstanceUIDOfReferencedImage=input_list[TagFromName(SOPInstanceUID)];
		if (aSOPInstanceUIDOfReferencedImage)
			vSOPInstanceUIDOfReferencedImage=AttributeValue(aSOPInstanceUIDOfReferencedImage);
		if (!vSOPInstanceUIDOfReferencedImage || !*vSOPInstanceUIDOfReferencedImage)  {
			log << EMsgDC(MissingAttributeInReferencedImage) << " - SOP Instance UID" << endl;
			success=false;
		}
	}

	char *vSOPClassUIDOfReferencedImage=0;
	{
		Attribute *aSOPClassUIDOfReferencedImage=input_list[TagFromName(SOPClassUID)];
		if (aSOPClassUIDOfReferencedImage)
			vSOPClassUIDOfReferencedImage=AttributeValue(aSOPClassUIDOfReferencedImage);
		if (!vSOPClassUIDOfReferencedImage || !*vSOPClassUIDOfReferencedImage)  {
			log << EMsgDC(MissingAttributeInReferencedImage) << " - SOP Class UID" << endl;
			success=false;
		}
	}

	Uint16 vRowsOfReferencedImage=0;
	{
		Attribute *aRowsOfReferencedImage=input_list[TagFromName(Rows)];
		if (aRowsOfReferencedImage)
			vRowsOfReferencedImage=AttributeValue(aRowsOfReferencedImage);
		else {
			log << EMsgDC(MissingAttributeInReferencedImage) << " - Rows" << endl;
			success=false;
		}
	}

	Uint16 vColumnsOfReferencedImage=0;
	{
		Attribute *aColumnsOfReferencedImage=input_list[TagFromName(Columns)];
		if (aColumnsOfReferencedImage)
			vColumnsOfReferencedImage=AttributeValue(aColumnsOfReferencedImage);
		else {
			log << EMsgDC(MissingAttributeInReferencedImage) << " - Columns" << endl;
			success=false;
		}
	}

	if (!success) return false;

	char *vPatientNameOfReferencedImage=0;
	{
		Attribute *aPatientNameOfReferencedImage=input_list[TagFromName(PatientName)];
		if (aPatientNameOfReferencedImage)
			vPatientNameOfReferencedImage=AttributeValue(aPatientNameOfReferencedImage);
	}

	char *vPatientIDOfReferencedImage=0;
	{
		Attribute *aPatientIDOfReferencedImage=input_list[TagFromName(PatientID)];
		if (aPatientIDOfReferencedImage)
			vPatientIDOfReferencedImage=AttributeValue(aPatientIDOfReferencedImage);
	}

	char *vPatientBirthDateOfReferencedImage=0;
	{
		Attribute *aPatientBirthDateOfReferencedImage=input_list[TagFromName(PatientBirthDate)];
		if (aPatientBirthDateOfReferencedImage)
			vPatientBirthDateOfReferencedImage=AttributeValue(aPatientBirthDateOfReferencedImage);
	}

	char *vPatientSexOfReferencedImage=0;
	{
		Attribute *aPatientSexOfReferencedImage=input_list[TagFromName(PatientSex)];
		if (aPatientSexOfReferencedImage)
			vPatientSexOfReferencedImage=AttributeValue(aPatientSexOfReferencedImage);
	}

	char *vStudyIDOfReferencedImage=0;
	{
		Attribute *aStudyIDOfReferencedImage=input_list[TagFromName(StudyID)];
		if (aStudyIDOfReferencedImage)
			vStudyIDOfReferencedImage=AttributeValue(aStudyIDOfReferencedImage);
	}

	char *vStudyDateOfReferencedImage=0;
	{
		Attribute *aStudyDateReferencedImage=input_list[TagFromName(StudyDate)];
		if (aStudyDateReferencedImage)
			vStudyDateOfReferencedImage=AttributeValue(aStudyDateReferencedImage);
	}

	char *vStudyTimeOfReferencedImage=0;
	{
		Attribute *aStudyTimeOfReferencedImage=input_list[TagFromName(StudyTime)];
		if (aStudyTimeOfReferencedImage)
			vStudyTimeOfReferencedImage=AttributeValue(aStudyTimeOfReferencedImage);
	}

	char *vReferringPhysicianNameOfReferencedImage=0;
	{
		Attribute *aReferringPhysicianNameOfReferencedImage=input_list[TagFromName(ReferringPhysicianName)];
		if (aReferringPhysicianNameOfReferencedImage)
			vReferringPhysicianNameOfReferencedImage=AttributeValue(aReferringPhysicianNameOfReferencedImage);
	}

	char *vAccessionNumberOfReferencedImage=0;
	{
		Attribute *aAccessionNumberOfReferencedImage=input_list[TagFromName(AccessionNumber)];
		if (aAccessionNumberOfReferencedImage)
			vAccessionNumberOfReferencedImage=AttributeValue(aAccessionNumberOfReferencedImage);
	}

	// various Type 1 and Type 2 attributes for mandatory Presentation State modules ...

	// Patient Module

	output_list+=new PersonNameAttribute(TagFromName(PatientName),vPatientNameOfReferencedImage ? vPatientNameOfReferencedImage : "^^^^");
	output_list+=new LongStringAttribute(TagFromName(PatientID),vPatientIDOfReferencedImage ? vPatientIDOfReferencedImage : "");
	output_list+=new DateStringAttribute(TagFromName(PatientBirthDate),vPatientBirthDateOfReferencedImage ? vPatientBirthDateOfReferencedImage : "");
	output_list+=new CodeStringAttribute(TagFromName(PatientSex),vPatientSexOfReferencedImage ? vPatientSexOfReferencedImage : "");

	// General Study Module
	output_list+=new ShortStringAttribute(TagFromName(StudyID),vStudyIDOfReferencedImage ? vStudyIDOfReferencedImage : "");
	output_list+=new DateStringAttribute(TagFromName(StudyDate),vStudyDateOfReferencedImage ? vStudyDateOfReferencedImage : "");
	output_list+=new TimeStringAttribute(TagFromName(StudyTime),vStudyTimeOfReferencedImage ? vStudyTimeOfReferencedImage : "");
	output_list+=new PersonNameAttribute(TagFromName(ReferringPhysicianName),vReferringPhysicianNameOfReferencedImage ? vReferringPhysicianNameOfReferencedImage : "^^^^");
	output_list+=new ShortStringAttribute(TagFromName(AccessionNumber),vAccessionNumberOfReferencedImage ? vAccessionNumberOfReferencedImage : "");

	output_list+=new UIStringAttribute(TagFromName(StudyInstanceUID),vStudyInstanceUIDOfReferencedImage);

	// General Series Module

	output_list+=new IntegerStringAttribute(TagFromName(SeriesNumber));

	// General Equipment Module

	output_list+=new LongStringAttribute(TagFromName(Manufacturer));

	// SOP Common Module

	output_list+=new IntegerStringAttribute(TagFromName(InstanceNumber),Uint16(1));

	output_list+=new UIStringAttribute(TagFromName(SOPClassUID),GrayscaleSoftcopyPresentationStateStorageSOPClassUID);

	// Presentation Series Module

	output_list+=new CodeStringAttribute(TagFromName(Modality),"PR");

	// Presentation State Module

	output_list+=new CodeStringAttribute(TagFromName(ContentLabel),"TESTSTATE");
	output_list+=new LongStringAttribute(TagFromName(ContentDescription));
	output_list+=new DateStringAttribute(TagFromName(PresentationCreationDate),vStudyDateOfReferencedImage ? vStudyDateOfReferencedImage : "");
	output_list+=new TimeStringAttribute(TagFromName(PresentationCreationTime),vStudyTimeOfReferencedImage ? vStudyTimeOfReferencedImage : "");
	output_list+=new PersonNameAttribute(TagFromName(ContentCreatorName));

	{
		SequenceAttribute *aReferencedSeriesSequence = new SequenceAttribute(TagFromName(ReferencedSeriesSequence));
		Assert(aReferencedSeriesSequence);

		AttributeList *lReferencedSeriesSequence = new AttributeList();
		Assert(lReferencedSeriesSequence);

			(*lReferencedSeriesSequence)+=new UIStringAttribute(TagFromName(SeriesInstanceUID),vSeriesInstanceUIDOfReferencedImage);

				SequenceAttribute *aReferencedImageSequence = new SequenceAttribute(TagFromName(ReferencedImageSequence));
				Assert(aReferencedImageSequence);

				AttributeList *lReferencedImageSequence = new AttributeList();
				Assert(lReferencedImageSequence);

				(*lReferencedImageSequence)+=new UIStringAttribute(TagFromName(ReferencedSOPClassUID),vSOPClassUIDOfReferencedImage);
				(*lReferencedImageSequence)+=new UIStringAttribute(TagFromName(ReferencedSOPInstanceUID),vSOPInstanceUIDOfReferencedImage);

				(*aReferencedImageSequence)+=lReferencedImageSequence;

			(*lReferencedSeriesSequence)+=aReferencedImageSequence;

		(*aReferencedSeriesSequence)+=lReferencedSeriesSequence;

		output_list+=aReferencedSeriesSequence;

	}

	// Displayed Area Module

	{
		SequenceAttribute *aDisplayedAreaSelectionSequence = new SequenceAttribute(TagFromName(DisplayedAreaSelectionSequence));
		Assert(aDisplayedAreaSelectionSequence);

		AttributeList *lDisplayedAreaSelectionSequence = new AttributeList();
		Assert(lDisplayedAreaSelectionSequence);

			Attribute *aDisplayedAreaTopLeftHandCorner = new SignedLongAttribute(TagFromName(DisplayedAreaTopLeftHandCorner));
			Assert(aDisplayedAreaTopLeftHandCorner);
			aDisplayedAreaTopLeftHandCorner->addValue(Uint16(1));
			aDisplayedAreaTopLeftHandCorner->addValue(Uint16(1));

			Attribute *aDisplayedAreaBottomRightHandCorner = new SignedLongAttribute(TagFromName(DisplayedAreaBottomRightHandCorner));
			Assert(aDisplayedAreaBottomRightHandCorner);
			aDisplayedAreaBottomRightHandCorner->addValue(Uint16(vColumnsOfReferencedImage));
			aDisplayedAreaBottomRightHandCorner->addValue(Uint16(vRowsOfReferencedImage));

			Attribute *aPresentationSizeMode = new CodeStringAttribute(TagFromName(PresentationSizeMode),"SCALE TO FIT");
			Assert(aPresentationSizeMode);

			Attribute *aPresentationPixelAspectRatio = new IntegerStringAttribute(TagFromName(PresentationPixelAspectRatio));
			Assert(aPresentationPixelAspectRatio);
			aPresentationPixelAspectRatio->addValue(Uint16(1));
			aPresentationPixelAspectRatio->addValue(Uint16(1));

		(*lDisplayedAreaSelectionSequence)+=aDisplayedAreaTopLeftHandCorner;
		(*lDisplayedAreaSelectionSequence)+=aDisplayedAreaBottomRightHandCorner;
		(*lDisplayedAreaSelectionSequence)+=aPresentationSizeMode;
		(*lDisplayedAreaSelectionSequence)+=aPresentationPixelAspectRatio;

		(*aDisplayedAreaSelectionSequence)+=lDisplayedAreaSelectionSequence;

		output_list+=aDisplayedAreaSelectionSequence;
	}

	// Softcopy Presentation LUT Module

	if (plutshape) {
		output_list+=new CodeStringAttribute(TagFromName(PresentationLUTShape),plutshape);
	}

	return true;
}

static bool
addLinearLUT(/*Managed*/AttributeList &output_list,TextOutputStream &log,bool useOWForLUTs,
	bool ismodalitylut,bool isvoilut,bool ispresentationlut,const char *explanation,const char *luttype,
	Uint32 numberofentries,
	Uint16 firstvaluemapped,
	Uint16 inputbitsallocated,
	Uint16 outputbitsallocated,
	Uint16 firstlutentry,
	Int16  lutentryincrement)
{
cerr << "addLinearLUT(): numberofentries =" << numberofentries << endl;
cerr << "addLinearLUT(): firstvaluemapped =" << firstvaluemapped << endl;
cerr << "addLinearLUT(): inputbitsallocated =" << inputbitsallocated << endl;
cerr << "addLinearLUT(): outputbitsallocated =" << outputbitsallocated << endl;
cerr << "addLinearLUT(): firstlutentry =" << firstlutentry << endl;
cerr << "addLinearLUT(): lutentryincrement =" << lutentryincrement << endl;

	Assert(inputbitsallocated <= 16);
	Assert(outputbitsallocated <= 16);
	Assert(numberofentries <= 65536);

	Attribute *aLUTDescriptor = new UnsignedShortAttribute(TagFromName(LUTDescriptor));
	Assert(aLUTDescriptor);
	aLUTDescriptor->addValue(numberofentries == 65536 ? 0 : numberofentries);
	aLUTDescriptor->addValue(firstvaluemapped);
	aLUTDescriptor->addValue(outputbitsallocated);

	Uint16 *values = NULL;
	Attribute *aLUTData = NULL;
	if (useOWForLUTs) {
		aLUTData = new OtherWordSmallNonPixelAttribute(TagFromName(LUTData));
		values = new Uint16[numberofentries];
	}
	else {
		aLUTData = new UnsignedShortAttribute(TagFromName(LUTData));
	}
	Assert(aLUTData);
	
	Int16 value;
	Uint32 left;
	Uint32 index;

	if (outputbitsallocated <= 8) {

		// pack 8 bits into 16 bit words, 1st 8 bits into low part of word

		for (value=firstlutentry,left=numberofentries,index=0; left > 0; left-=2,++index) {
			Uint16 usevalue;
			Uint16 usevalue1;
			Uint16 usevalue2;

			if (outputbitsallocated > inputbitsallocated) {
				unsigned shiftleftforvalue=outputbitsallocated-inputbitsallocated;
				Assert(inputbitsallocated >= shiftleftforvalue);				// doesn't support multiple replications
				unsigned shiftrightforfill=inputbitsallocated-shiftleftforvalue;

				usevalue1 = (value<<shiftleftforvalue) | (Uint16(value)>>shiftrightforfill);	// unsigned to avoid sign replication in fill
				value+=lutentryincrement;
				usevalue2 = (value<<shiftleftforvalue) | (Uint16(value)>>shiftrightforfill);	// unsigned to avoid sign replication in fill
			}
			else {
				usevalue1 = value>>(inputbitsallocated-outputbitsallocated);	// no sign extension :( ?
				value+=lutentryincrement;
				usevalue2 = value>>(inputbitsallocated-outputbitsallocated);	// no sign extension :( ?
			}
			value+=lutentryincrement;

			usevalue=(Uint16((usevalue1 & 0xff) | (usevalue2 << 8)));
			if (useOWForLUTs) {
				values[index]=usevalue;
			}
			else {
				aLUTData->addValue(usevalue);
			}
		}
	}
	else {
		for (value=firstlutentry,left=numberofentries; left > 0; --left,++index) {
			Uint16 usevalue;

			if (outputbitsallocated > inputbitsallocated) {
				unsigned shiftleftforvalue=outputbitsallocated-inputbitsallocated;
				Assert(inputbitsallocated >= shiftleftforvalue);				// doesn't support multiple replications
				unsigned shiftrightforfill=inputbitsallocated-shiftleftforvalue;

				usevalue = (value<<shiftleftforvalue) | (Uint16(value)>>shiftrightforfill);	// unsigned to avoid sign replication in fill
			}
			else {
				usevalue = value>>(inputbitsallocated-outputbitsallocated);	// no sign extension :( ?
			}
			value+=lutentryincrement;
//cerr << "addLinearLUT(): adding usevalue =" << usevalue << " value =" << value << endl;
			if (useOWForLUTs) {
				values[index]=usevalue;
			}
			else {
				aLUTData->addValue(usevalue);
			}
		}
	}
	if (useOWForLUTs) {
		aLUTData->setValue(values,numberofentries);
	}

	SequenceAttribute *aLUTSequence = 0;
	if      (ismodalitylut)     aLUTSequence = new SequenceAttribute(TagFromName(ModalityLUTSequence));
	else if (isvoilut)          aLUTSequence = new SequenceAttribute(TagFromName(VOILUTSequence));
	else if (ispresentationlut) aLUTSequence = new SequenceAttribute(TagFromName(PresentationLUTSequence));
	else Assert(0);

	Assert(aLUTSequence);

	AttributeList *lLUTSequence = new AttributeList();
	Assert(lLUTSequence);

	(*lLUTSequence)+=aLUTData;
	(*lLUTSequence)+=aLUTDescriptor;
	if (ismodalitylut) {
		(*lLUTSequence)+=new LongStringAttribute(TagFromName(ModalityLUTType),luttype ? luttype : "");
	}
	else {
		Assert(luttype == 0);
	}
	(*lLUTSequence)+=new LongStringAttribute(TagFromName(LUTExplanation),explanation ? explanation : "");

	(*aLUTSequence)+=lLUTSequence;

	output_list+=aLUTSequence;

	return true;
}

static bool
addLinearModalityLUT(ManagedAttributeList &output_list,TextOutputStream &log,bool useOWForLUTs,
	const char *explanation,
	Uint32 numberofentries,
	Uint16 firstvaluemapped,
	Uint16 inputbitsallocated,
	Uint16 outputbitsallocated,
	Uint16 firstlutentry,
	Int16  lutentryincrement)
{

	return addLinearLUT(output_list,log,useOWForLUTs,
		true,false,false,explanation,"US",
		numberofentries,
		firstvaluemapped,
		inputbitsallocated,
		outputbitsallocated,
		firstlutentry,
		lutentryincrement);
}

static bool
addLinearPresentationLUT(ManagedAttributeList &output_list,TextOutputStream &log,bool useOWForLUTs,
	const char *explanation,
	Uint32 numberofentries,
	Uint16 firstvaluemapped,
	Uint16 inputbitsallocated,
	Uint16 outputbitsallocated,
	Uint16 firstlutentry,
	Int16  lutentryincrement)
{

	return addLinearLUT(output_list,log,useOWForLUTs,
		false,false,true,explanation,0,
		numberofentries,
		firstvaluemapped,
		inputbitsallocated,
		outputbitsallocated,
		firstlutentry,
		lutentryincrement);
}

static bool
addSoftcopyLinearVOILUT(ManagedAttributeList &output_list,TextOutputStream &log,bool useOWForLUTs,
	const char *explanation,
	Uint32 numberofentries,
	Uint16 firstvaluemapped,
	Uint16 inputbitsallocated,
	Uint16 outputbitsallocated,
	Uint16 firstlutentry,
	Int16  lutentryincrement)
{
	// In Presentation State, VOI LUT Sequence is nested in Softcopy VOI LUT Sequence ...

	SequenceAttribute *aSoftcopyVOILUTSequence = new SequenceAttribute(TagFromName(SoftcopyVOILUTSequence));
	Assert(aSoftcopyVOILUTSequence);

	output_list+=aSoftcopyVOILUTSequence;

	AttributeList *lSoftcopyVOILUTSequence = new AttributeList();
	Assert(lSoftcopyVOILUTSequence);

	(*aSoftcopyVOILUTSequence)+=lSoftcopyVOILUTSequence;

	return addLinearLUT(*lSoftcopyVOILUTSequence,log,useOWForLUTs,
		false,true,false,explanation,0,
		numberofentries,
		firstvaluemapped,
		inputbitsallocated,
		outputbitsallocated,
		firstlutentry,
		lutentryincrement);
}

static bool
addSoftcopyWindowCenterWidth(ManagedAttributeList &output_list,TextOutputStream &log,
	const char *explanation,
	double windowcenter,
	double windowwidth)
{
	// In Presentation State, window is nested in Softcopy VOI LUT Sequence ...

	SequenceAttribute *aSoftcopyVOILUTSequence = new SequenceAttribute(TagFromName(SoftcopyVOILUTSequence));
	Assert(aSoftcopyVOILUTSequence);

	output_list+=aSoftcopyVOILUTSequence;

	AttributeList *lSoftcopyVOILUTSequence = new AttributeList();
	Assert(lSoftcopyVOILUTSequence);

	(*aSoftcopyVOILUTSequence)+=lSoftcopyVOILUTSequence;

	Attribute *aWindowCenter = new DecimalStringAttribute(TagFromName(WindowCenter),windowcenter);
	Assert(aWindowCenter);
	(*lSoftcopyVOILUTSequence)+=aWindowCenter;

	Attribute *aWindowWidth = new DecimalStringAttribute(TagFromName(WindowWidth),windowwidth);
	Assert(aWindowWidth);
	(*lSoftcopyVOILUTSequence)+=aWindowWidth;

	Attribute *aWindowCenterWidthExplanation = new LongStringAttribute(TagFromName(WindowCenterWidthExplanation),explanation);
	Assert(aWindowCenterWidthExplanation);
	(*lSoftcopyVOILUTSequence)+=aWindowCenterWidthExplanation;

	return true;
}

static bool
addLinearVOILUT(ManagedAttributeList &output_list,TextOutputStream &log,bool useOWForLUTs,
	const char *explanation,
	Uint32 numberofentries,
	Uint16 firstvaluemapped,
	Uint16 inputbitsallocated,
	Uint16 outputbitsallocated,
	Uint16 firstlutentry,
	Int16  lutentryincrement)
{
	return addLinearLUT(output_list,log,useOWForLUTs,
		false,true,false,explanation,0,
		numberofentries,
		firstvaluemapped,
		inputbitsallocated,
		outputbitsallocated,
		firstlutentry,
		lutentryincrement);
}

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool useOWForLUTs=options.get("ow");
	bool imageRatherThanPresentationState=options.get("image");
	bool verbose=options.get("verbose") || options.get("v");

	const char *plutshape = 0;

	options.get("presentationlutshape",plutshape);

	bool addlinearmodalitylut=false;
	Uint32 addlinearmodalitylut_numberofentries;
	Uint16 addlinearmodalitylut_firstvaluemapped;
	Uint16 addlinearmodalitylut_inputbitsallocated;
	Uint16 addlinearmodalitylut_outputbitsallocated;
	Uint16 addlinearmodalitylut_firstlutentry;
	Int16  addlinearmodalitylut_lutentryincrement;
	{
		long addlinearmodalitylut_parameters[6];
		addlinearmodalitylut=options.get("addlinearmodalitylut",addlinearmodalitylut_parameters,6) == 6;
		if (addlinearmodalitylut) {
			addlinearmodalitylut_numberofentries=addlinearmodalitylut_parameters[0];
			addlinearmodalitylut_firstvaluemapped=addlinearmodalitylut_parameters[1];
			addlinearmodalitylut_inputbitsallocated=addlinearmodalitylut_parameters[2];
			addlinearmodalitylut_outputbitsallocated=addlinearmodalitylut_parameters[3];
			addlinearmodalitylut_firstlutentry=addlinearmodalitylut_parameters[4];
			addlinearmodalitylut_lutentryincrement=addlinearmodalitylut_parameters[5];
		}
	}

	bool addlinearpresentationlut=false;
	Uint32 addlinearpresentationlut_numberofentries;
	Uint16 addlinearpresentationlut_firstvaluemapped;
	Uint16 addlinearpresentationlut_inputbitsallocated;
	Uint16 addlinearpresentationlut_outputbitsallocated;
	Uint16 addlinearpresentationlut_firstlutentry;
	Int16  addlinearpresentationlut_lutentryincrement;
	{
		long addlinearpresentationlut_parameters[6];
		addlinearpresentationlut=options.get("addlinearpresentationlut",addlinearpresentationlut_parameters,6) == 6;
		if (addlinearpresentationlut) {
			addlinearpresentationlut_numberofentries=addlinearpresentationlut_parameters[0];
			addlinearpresentationlut_firstvaluemapped=addlinearpresentationlut_parameters[1];
			addlinearpresentationlut_inputbitsallocated=addlinearpresentationlut_parameters[2];
			addlinearpresentationlut_outputbitsallocated=addlinearpresentationlut_parameters[3];
			addlinearpresentationlut_firstlutentry=addlinearpresentationlut_parameters[4];
			addlinearpresentationlut_lutentryincrement=addlinearpresentationlut_parameters[5];
		}
	}

	bool addsoftcopylinearvoilut=false;
	Uint32 addsoftcopylinearvoilut_numberofentries;
	Uint16 addsoftcopylinearvoilut_firstvaluemapped;
	Uint16 addsoftcopylinearvoilut_inputbitsallocated;
	Uint16 addsoftcopylinearvoilut_outputbitsallocated;
	Uint16 addsoftcopylinearvoilut_firstlutentry;
	Int16  addsoftcopylinearvoilut_lutentryincrement;
	{
		long addsoftcopylinearvoilut_parameters[6];
		addsoftcopylinearvoilut=options.get("addsoftcopylinearvoilut",addsoftcopylinearvoilut_parameters,6) == 6;
		if (addsoftcopylinearvoilut) {
			addsoftcopylinearvoilut_numberofentries=addsoftcopylinearvoilut_parameters[0];
			addsoftcopylinearvoilut_firstvaluemapped=addsoftcopylinearvoilut_parameters[1];
			addsoftcopylinearvoilut_inputbitsallocated=addsoftcopylinearvoilut_parameters[2];
			addsoftcopylinearvoilut_outputbitsallocated=addsoftcopylinearvoilut_parameters[3];
			addsoftcopylinearvoilut_firstlutentry=addsoftcopylinearvoilut_parameters[4];
			addsoftcopylinearvoilut_lutentryincrement=addsoftcopylinearvoilut_parameters[5];
		}
	}

	bool addsoftcopywindowcenterwidth=false;
	const char *addsoftcopywindowcenterwidth_explanation;
	double addsoftcopywindowcenterwidth_center;
	double addsoftcopywindowcenterwidth_width;
	{
		const char *addsoftcopywindowcenterwidth_parameters[3];
		addsoftcopywindowcenterwidth=options.get("addsoftcopywindowcenterwidth",addsoftcopywindowcenterwidth_parameters,3) == 3;
		if (addsoftcopywindowcenterwidth) {
			addsoftcopywindowcenterwidth_center=atof(addsoftcopywindowcenterwidth_parameters[0]);
			addsoftcopywindowcenterwidth_width=atof(addsoftcopywindowcenterwidth_parameters[1]);
			addsoftcopywindowcenterwidth_explanation=addsoftcopywindowcenterwidth_parameters[2];
		}
	}

	if (!plutshape && !addlinearpresentationlut) {
		plutshape="IDENTITY";
	}

	bool addlinearvoilut=false;
	Uint32 addlinearvoilut_numberofentries;
	Uint16 addlinearvoilut_firstvaluemapped;
	Uint16 addlinearvoilut_inputbitsallocated;
	Uint16 addlinearvoilut_outputbitsallocated;
	Uint16 addlinearvoilut_firstlutentry;
	Int16  addlinearvoilut_lutentryincrement;
	{
		long addlinearvoilut_parameters[6];
		addlinearvoilut=options.get("addlinearvoilut",addlinearvoilut_parameters,6) == 6;
		if (addlinearvoilut) {
			addlinearvoilut_numberofentries=addlinearvoilut_parameters[0];
			addlinearvoilut_firstvaluemapped=addlinearvoilut_parameters[1];
			addlinearvoilut_inputbitsallocated=addlinearvoilut_parameters[2];
			addlinearvoilut_outputbitsallocated=addlinearvoilut_parameters[3];
			addlinearvoilut_firstlutentry=addlinearvoilut_parameters[4];
			addlinearvoilut_lutentryincrement=addlinearvoilut_parameters[5];
		}
	}

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
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-ow]"
			<< " [-addlinearmodalitylut nentries 1stvaluemapped inbits outbits 1stlutentry lutentryincr]"
			<< " [-addlinearpresentationlut nentries 1stvaluemapped inbits outbits 1stlutentry lutentryincr]"
			<< " [-addsoftcopylinearvoilut nentries 1stvaluemapped inbits outbits 1stlutentry lutentryincr]"
			<< " [-addsoftcopywindowcenterwidth center width explanation]"
			<< " [-presentationlutshape IDENTITY|INVERSE]"
			<< " [-image]"
			<< " [-addlinearvoilut nentries 1stvaluemapped inbits outbits 1stlutentry lutentryincr]"
			<< " [-v|-verbose]"
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

	ManagedAttributeList input_list;
	ManagedAttributeList *output_list = NULL;

	bool success=true;
	TextOutputStream  log(cerr);
	if (verbose) log << "******** While reading ... ********" << endl; 
	input_list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	if (!input_list.good()) {
		log << input_list.errors()
		    << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	else {
		if (imageRatherThanPresentationState) {
			output_list=&input_list;
		}
		else {
			output_list=new ManagedAttributeList();
			success=createPresentationState(input_list,*output_list,log,plutshape);
		}
		
		if (success && addlinearmodalitylut) {
			success=addLinearModalityLUT(*output_list,log,useOWForLUTs,0 /* no explanation */,
				addlinearmodalitylut_numberofentries,
				addlinearmodalitylut_firstvaluemapped,
				addlinearmodalitylut_inputbitsallocated,
				addlinearmodalitylut_outputbitsallocated,
				addlinearmodalitylut_firstlutentry,
				addlinearmodalitylut_lutentryincrement);
		}
		if (success && addlinearpresentationlut) {
			success=addLinearPresentationLUT(*output_list,log,useOWForLUTs,0 /* no explanation */,
				addlinearpresentationlut_numberofentries,
				addlinearpresentationlut_firstvaluemapped,
				addlinearpresentationlut_inputbitsallocated,
				addlinearpresentationlut_outputbitsallocated,
				addlinearpresentationlut_firstlutentry,
				addlinearpresentationlut_lutentryincrement);
		}
		if (success && addsoftcopylinearvoilut) {
			success=addSoftcopyLinearVOILUT(*output_list,log,useOWForLUTs,0 /* no explanation */,
				addsoftcopylinearvoilut_numberofentries,
				addsoftcopylinearvoilut_firstvaluemapped,
				addsoftcopylinearvoilut_inputbitsallocated,
				addsoftcopylinearvoilut_outputbitsallocated,
				addsoftcopylinearvoilut_firstlutentry,
				addsoftcopylinearvoilut_lutentryincrement);
		}
		if (success && addsoftcopywindowcenterwidth) {
			success=addSoftcopyWindowCenterWidth(*output_list,log,
				addsoftcopywindowcenterwidth_explanation,
				addsoftcopywindowcenterwidth_center,
				addsoftcopywindowcenterwidth_width);
		}
		if (success && addlinearvoilut) {
			success=addLinearVOILUT(*output_list,log,useOWForLUTs,0 /* no explanation */,
				addlinearvoilut_numberofentries,
				addlinearvoilut_firstvaluemapped,
				addlinearvoilut_inputbitsallocated,
				addlinearvoilut_outputbitsallocated,
				addlinearvoilut_firstlutentry,
				addlinearvoilut_lutentryincrement);
		}
	}

	if (success && !usualManagedAttributeListWrite(*output_list,dout,dicom_output_options,log,verbose)) success=false;

	return success ? 0 : 1;
}

