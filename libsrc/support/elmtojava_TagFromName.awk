# create XML from element dictionary template 

NR==1	{
	print "/* Copyright (c) 2001-2011, David A. Clunie DBA Pixelmed Publishing. All rights reserved. */"
	print ""
	print "// Automatically generated from template - EDITS WILL BE LOST"
	print ""
	print "package com.pixelmed.dicom;"
	print ""
	print "public class TagFromName {"
	print ""
	print "private static final String identString = \"@(#) $Header: /userland/cvs/dicom3tools/libsrc/support/elmtojava_TagFromName.awk,v 1.4 2011/05/04 21:56:48 dclunie Exp $\";"
	print ""
}

/^[ 	]*[#]/	{}

/^[ 	]*[(]/ {

	match($0,"[(][0-9a-fA-FxX][0-9a-fA-FxX][0-9a-fA-FxX][0-9a-fA-FxX],");
	group=substr($0,RSTART+1,4);

	match($0,",[0-9a-fA-FxX][0-9a-fA-FxX][0-9a-fA-FxX][0-9a-fA-FxX]");
	element=substr($0,RSTART+1,4);

	match($0,"Keyword=\"[^\"]*\"")
	keyword=substr($0,RSTART+length("Keyword=\""),
		RLENGTH-length("Keyword=\"")-1);

	print "\tstatic public AttributeTag " keyword " = new AttributeTag(0x" group ",0x" element ");"
}

END {
	print "\t"
	print "\t// include old toolkit keywords prior to CP 850 establishment of official keywords, in case old code uses them ..."
	print "\t"
	print "\tstatic public AttributeTag FileSetCharacterSet = new AttributeTag(0x0004,0x1142);"
	print "\tstatic public AttributeTag RootDirectoryFirstRecord = new AttributeTag(0x0004,0x1200);"
	print "\tstatic public AttributeTag RootDirectoryLastRecord = new AttributeTag(0x0004,0x1202);"
	print "\tstatic public AttributeTag NextDirectoryRecordOffset = new AttributeTag(0x0004,0x1400);"
	print "\tstatic public AttributeTag LowerLevelDirectoryOffset = new AttributeTag(0x0004,0x1420);"
	print "\tstatic public AttributeTag OldDataSetType = new AttributeTag(0x0008,0x0040);"
	print "\tstatic public AttributeTag OldDataSetSubtype = new AttributeTag(0x0008,0x0041);"
	print "\tstatic public AttributeTag NuclearMedicineSeriesTypeRetired = new AttributeTag(0x0008,0x0042);"
	print "\tstatic public AttributeTag ReferringPhysicianTelephoneNumber = new AttributeTag(0x0008,0x0094);"
	print "\tstatic public AttributeTag PhysicianOfRecord = new AttributeTag(0x0008,0x1048);"
	print "\tstatic public AttributeTag PhysicianOfRecordIdentificationSequence = new AttributeTag(0x0008,0x1049);"
	print "\tstatic public AttributeTag PhysicianReadingStudy = new AttributeTag(0x0008,0x1060);"
	print "\tstatic public AttributeTag PhysicianReadingStudyIdentificationSequence = new AttributeTag(0x0008,0x1062);"
	print "\tstatic public AttributeTag OperatorName = new AttributeTag(0x0008,0x1070);"
	print "\tstatic public AttributeTag OldLossyImageCompression = new AttributeTag(0x0008,0x2110);"
	print "\tstatic public AttributeTag EventElapsedTime = new AttributeTag(0x0008,0x2130);"
	print "\tstatic public AttributeTag EventTimerName = new AttributeTag(0x0008,0x2132);"
	print "\tstatic public AttributeTag OtherPatientID = new AttributeTag(0x0010,0x1000);"
	print "\tstatic public AttributeTag OtherPatientName = new AttributeTag(0x0010,0x1001);"
	print "\tstatic public AttributeTag OtherPatientIDSequence = new AttributeTag(0x0010,0x1002);"
	print "\tstatic public AttributeTag PatientTelephoneNumber = new AttributeTag(0x0010,0x2154);"
	print "\tstatic public AttributeTag EchoNumber = new AttributeTag(0x0018,0x0086);"
	print "\tstatic public AttributeTag SecondaryCaptureDeviceSoftwareVersion = new AttributeTag(0x0018,0x1019);"
	print "\tstatic public AttributeTag SoftwareVersion = new AttributeTag(0x0018,0x1020);"
	print "\tstatic public AttributeTag FocalSpot = new AttributeTag(0x0018,0x1190);"
	print "\tstatic public AttributeTag VerticesOfPolygonalShutter = new AttributeTag(0x0018,0x1620);"
	print "\tstatic public AttributeTag VerticesOfPolygonalCollimator = new AttributeTag(0x0018,0x1720);"
	print "\tstatic public AttributeTag MultiplanarExcitation = new AttributeTag(0x0018,0x9012);"
	print "\tstatic public AttributeTag MetaboliteCodeSequence = new AttributeTag(0x0018,0x9083);"
	print "\tstatic public AttributeTag ChemicalShiftMinimumIntegrationLimitInPPM = new AttributeTag(0x0018,0x9295);"
	print "\tstatic public AttributeTag ChemicalShiftMaximumIntegrationLimitInPPM = new AttributeTag(0x0018,0x9296);"
	print "\tstatic public AttributeTag VerticesOfPolygonalExposureControlSensingRegion = new AttributeTag(0x0018,0x9442);"
	print "\tstatic public AttributeTag DiffusionBMatrixValueXX = new AttributeTag(0x0018,0x9602);"
	print "\tstatic public AttributeTag DiffusionBMatrixValueXY = new AttributeTag(0x0018,0x9603);"
	print "\tstatic public AttributeTag DiffusionBMatrixValueXZ = new AttributeTag(0x0018,0x9604);"
	print "\tstatic public AttributeTag DiffusionBMatrixValueYY = new AttributeTag(0x0018,0x9605);"
	print "\tstatic public AttributeTag DiffusionBMatrixValueYZ = new AttributeTag(0x0018,0x9606);"
	print "\tstatic public AttributeTag DiffusionBMatrixValueZZ = new AttributeTag(0x0018,0x9607);"
	print "\tstatic public AttributeTag ReportNumberTrial = new AttributeTag(0x0020,0x00AA);"
	print "\tstatic public AttributeTag PatientFrameofReferenceSource = new AttributeTag(0x0020,0x930C);"
	print "\tstatic public AttributeTag PatientEyeMovementCommandedCodeSequence = new AttributeTag(0x0022,0x0006);"
	print "\tstatic public AttributeTag AxialLengthOfEye = new AttributeTag(0x0022,0x0030);"
	print "\tstatic public AttributeTag SmallestPixelValueInPlane = new AttributeTag(0x0028,0x0110);"
	print "\tstatic public AttributeTag LargestPixelValueInPlane = new AttributeTag(0x0028,0x0111);"
	print "\tstatic public AttributeTag BiplaneAcquisitionSequence = new AttributeTag(0x0028,0x5000);"
	print "\tstatic public AttributeTag MaskPointer = new AttributeTag(0x0028,0x6030);"
	print "\tstatic public AttributeTag MultiframePresentationSequence = new AttributeTag(0x0028,0x9505);"
	print "\tstatic public AttributeTag CommentsOnPerformedProcedureStep = new AttributeTag(0x0040,0x0280);"
	print "\tstatic public AttributeTag CommentsOnScheduledProcedureStep = new AttributeTag(0x0040,0x0400);"
	print "\tstatic public AttributeTag ReasonForRequestedProcedure = new AttributeTag(0x0040,0x1002);"
	print "\tstatic public AttributeTag PlacerOrderNumberOfProcedure = new AttributeTag(0x0040,0x1006);"
	print "\tstatic public AttributeTag FillerOrderNumberOfProcedure = new AttributeTag(0x0040,0x1007);"
	print "\tstatic public AttributeTag ReasonForImagingServiceRequest = new AttributeTag(0x0040,0x2001);"
	print "\tstatic public AttributeTag PlacerOrderNumberOfImagingServiceRequestRetired = new AttributeTag(0x0040,0x2006);"
	print "\tstatic public AttributeTag FillerOrderNumberOfImagingServiceRequestRetired = new AttributeTag(0x0040,0x2007);"
	print "\tstatic public AttributeTag PlacerOrderNumberOfImagingServiceRequest = new AttributeTag(0x0040,0x2016);"
	print "\tstatic public AttributeTag FillerOrderNumberOfImagingServiceRequest = new AttributeTag(0x0040,0x2017);"
	print "\tstatic public AttributeTag ScheduledProcedureStepStartDateAndTime = new AttributeTag(0x0040,0x4005);"
	print "\tstatic public AttributeTag ScheduledProcedureStepModificationDateAndTime = new AttributeTag(0x0040,0x4010);"
	print "\tstatic public AttributeTag ExpectedCompletionDateAndTime = new AttributeTag(0x0040,0x4011);"
	print "\tstatic public AttributeTag HumanPerformersOrganization = new AttributeTag(0x0040,0x4036);"
	print "\tstatic public AttributeTag HumanPerformersName = new AttributeTag(0x0040,0x4037);"
	print "\tstatic public AttributeTag RelationshipTypeCodeSequenceModifierTrial = new AttributeTag(0x0040,0xDB73);"
	print "\tstatic public AttributeTag ImageRotationTrial = new AttributeTag(0x0070,0x0040);"
	print "\tstatic public AttributeTag ContentCreatorsName = new AttributeTag(0x0070,0x0084);"
	print "\tstatic public AttributeTag ContentCreatorsIdentificationCodeSequence = new AttributeTag(0x0070,0x0086);"
	print "\tstatic public AttributeTag PseudocolorType = new AttributeTag(0x0072,0x0704);"
	print "\tstatic public AttributeTag TopicKeyWords = new AttributeTag(0x0088,0x0912);"
	print "\tstatic public AttributeTag SOPAuthorizationDateAndTime = new AttributeTag(0x0100,0x0420);"
	print "\tstatic public AttributeTag ReferencedPrintJobSequencePull = new AttributeTag(0x2100,0x0500);"
	print "\tstatic public AttributeTag ReferencedPrintJobSequenceQueue = new AttributeTag(0x2120,0x0070);"
	print "}"
}

