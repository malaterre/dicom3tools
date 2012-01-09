# create Java from element dictionary template 

NR==1	{
	print "\tprotected void createTagByName() {"
	print ""
	print "\t\ttagByName = new HashMap(100);"
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

	match($0,"Name=\"[^\"]*\"")
	name=substr($0,RSTART+length("Name=\""),
		RLENGTH-length("Name=\"")-1);

	gsub("\&","\\\&amp;",name);
	gsub("'","\\\&apos;",name);
	gsub("\"","\\\&quot;",name);
	gsub("<","\\\&lt;",name);
	gsub(">","\\\&gt;",name);

	match($0,"VR=\"[^\"]*\"");
	vr=substr($0,RSTART+length("VR=\""),RLENGTH-length("VR=\"")-1);

	match($0,"VM=\"[^\"]*\"");
	if (vr == "US or SS") vr= "XS";
	if (vr == "US or SS or OW") vr= "XO";
	if (vr == "US\\US or SS\\US") vr = "XS";
	if (vr == "OW/OB") vr = "OX";

	vm=substr($0,RSTART+length("VM=\""),RLENGTH-length("VM=\"")-1);
	if (vm == "") vm=0;
	vmmin=vmmax=vm;
	if (vm == "n") {
		vmmin=1;
		vmmax="n";
	}
	if (match(vm,"-")) {
		match(vm,"[0-9]*-");
		vmmin=substr(vm,RSTART,RLENGTH-1);
		match(vm,"-[0-9n]");
		vmmax=substr(vm,RSTART+1,RLENGTH-1);
		if (vmmax == "n") vmmax="n";
	}

	print "\t\ttagByName.put(\"" keyword "\",TagFromName." keyword ");"
	}

END {
	print "\t\t"
	print "\t\t// include old toolkit keywords prior to CP 850 establishment of official keywords, in case old code uses them ..."
	print "\t\t"
	print "\t\ttagByName.put(\"FileSetCharacterSet\",TagFromName.SpecificCharacterSetOfFileSetDescriptorFile);"
	print "\t\ttagByName.put(\"RootDirectoryFirstRecord\",TagFromName.OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity);"
	print "\t\ttagByName.put(\"RootDirectoryLastRecord\",TagFromName.OffsetOfTheLastDirectoryRecordOfTheRootDirectoryEntity);"
	print "\t\ttagByName.put(\"NextDirectoryRecordOffset\",TagFromName.OffsetOfTheNextDirectoryRecord);"
	print "\t\ttagByName.put(\"LowerLevelDirectoryOffset\",TagFromName.OffsetOfReferencedLowerLevelDirectoryEntity);"
	print "\t\ttagByName.put(\"ReferringPhysicianTelephoneNumber\",TagFromName.ReferringPhysicianTelephoneNumbers);"
	print "\t\ttagByName.put(\"PhysicianOfRecord\",TagFromName.PhysiciansOfRecord);"
	print "\t\ttagByName.put(\"PhysicianOfRecordIdentificationSequence\",TagFromName.PhysiciansOfRecordIdentificationSequence);"
	print "\t\ttagByName.put(\"PhysicianReadingStudy\",TagFromName.NameOfPhysiciansReadingStudy);"
	print "\t\ttagByName.put(\"PhysicianReadingStudyIdentificationSequence\",TagFromName.PhysiciansReadingStudyIdentificationSequence);"
	print "\t\ttagByName.put(\"OperatorName\",TagFromName.OperatorsName);"
	print "\t\ttagByName.put(\"OldLossyImageCompression\",TagFromName.LossyImageCompressionRetired);"
	print "\t\ttagByName.put(\"EventElapsedTime\",TagFromName.EventElapsedTimes);"
	print "\t\ttagByName.put(\"EventTimerName\",TagFromName.EventTimerNames);"
	print "\t\ttagByName.put(\"OtherPatientID\",TagFromName.OtherPatientIDs);"
	print "\t\ttagByName.put(\"OtherPatientName\",TagFromName.OtherPatientNames);"
	print "\t\ttagByName.put(\"OtherPatientIDSequence\",TagFromName.OtherPatientIDsSequence);"
	print "\t\ttagByName.put(\"PatientTelephoneNumber\",TagFromName.PatientTelephoneNumbers);"
	print "\t\ttagByName.put(\"EchoNumber\",TagFromName.EchoNumbers);"
	print "\t\ttagByName.put(\"SecondaryCaptureDeviceSoftwareVersion\",TagFromName.SecondaryCaptureDeviceSoftwareVersions);"
	print "\t\ttagByName.put(\"SoftwareVersion\",TagFromName.SoftwareVersions);"
	print "\t\ttagByName.put(\"FocalSpot\",TagFromName.FocalSpots);"
	print "\t\ttagByName.put(\"VerticesOfPolygonalShutter\",TagFromName.VerticesOfThePolygonalShutter);"
	print "\t\ttagByName.put(\"VerticesOfPolygonalCollimator\",TagFromName.VerticesOfThePolygonalCollimator);"
	print "\t\ttagByName.put(\"MultiplanarExcitation\",TagFromName.MultiPlanarExcitation);"
	print "\t\ttagByName.put(\"MetaboliteCodeSequence\",TagFromName.MetaboliteMapCodeSequence);"
	print "\t\ttagByName.put(\"ChemicalShiftMinimumIntegrationLimitInPPM\",TagFromName.ChemicalShiftMinimumIntegrationLimitInppm);"
	print "\t\ttagByName.put(\"ChemicalShiftMaximumIntegrationLimitInPPM\",TagFromName.ChemicalShiftMaximumIntegrationLimitInppm);"
	print "\t\ttagByName.put(\"VerticesOfPolygonalExposureControlSensingRegion\",TagFromName.VerticesOfThePolygonalExposureControlSensingRegion);"
	print "\t\ttagByName.put(\"DiffusionBMatrixValueXX\",TagFromName.DiffusionBValueXX);"
	print "\t\ttagByName.put(\"DiffusionBMatrixValueXY\",TagFromName.DiffusionBValueXY);"
	print "\t\ttagByName.put(\"DiffusionBMatrixValueXZ\",TagFromName.DiffusionBValueXZ);"
	print "\t\ttagByName.put(\"DiffusionBMatrixValueYY\",TagFromName.DiffusionBValueYY);"
	print "\t\ttagByName.put(\"DiffusionBMatrixValueYZ\",TagFromName.DiffusionBValueYZ);"
	print "\t\ttagByName.put(\"DiffusionBMatrixValueZZ\",TagFromName.DiffusionBValueZZ);"
	print "\t\ttagByName.put(\"ReportNumberTrial\",TagFromName.ReportNumber);"
	print "\t\ttagByName.put(\"PatientFrameofReferenceSource\",TagFromName.PatientFrameOfReferenceSource);"
	print "\t\ttagByName.put(\"PatientEyeMovementCommandedCodeSequence\",TagFromName.PatientEyeMovementCommandCodeSequence);"
	print "\t\ttagByName.put(\"AxialLengthOfEye\",TagFromName.AxialLengthOfTheEye);"
	print "\t\ttagByName.put(\"SmallestPixelValueInPlane\",TagFromName.SmallestImagePixelValueInPlane);"
	print "\t\ttagByName.put(\"LargestPixelValueInPlane\",TagFromName.LargestImagePixelValueInPlane);"
	print "\t\ttagByName.put(\"BiplaneAcquisitionSequence\",TagFromName.BiPlaneAcquisitionSequence);"
	print "\t\ttagByName.put(\"MaskPointer\",TagFromName.MaskPointers);"
	print "\t\ttagByName.put(\"MultiframePresentationSequence\",TagFromName.MultiFramePresentationSequence);"
	print "\t\ttagByName.put(\"CommentsOnPerformedProcedureStep\",TagFromName.CommentsOnThePerformedProcedureStep);"
	print "\t\ttagByName.put(\"CommentsOnScheduledProcedureStep\",TagFromName.CommentsOnTheScheduledProcedureStep);"
	print "\t\ttagByName.put(\"ReasonForRequestedProcedure\",TagFromName.ReasonForTheRequestedProcedure);"
	print "\t\ttagByName.put(\"PlacerOrderNumberOfProcedure\",TagFromName.PlacerOrderNumberProcedure);"
	print "\t\ttagByName.put(\"FillerOrderNumberOfProcedure\",TagFromName.FillerOrderNumberProcedure);"
	print "\t\ttagByName.put(\"ReasonForImagingServiceRequest\",TagFromName.ReasonForTheImagingServiceRequest);"
	print "\t\ttagByName.put(\"PlacerOrderNumberOfImagingServiceRequestRetired\",TagFromName.PlacerOrderNumberImagingServiceRequestRetired);"
	print "\t\ttagByName.put(\"FillerOrderNumberOfImagingServiceRequestRetired\",TagFromName.FillerOrderNumberImagingServiceRequestRetired);"
	print "\t\ttagByName.put(\"PlacerOrderNumberOfImagingServiceRequest\",TagFromName.PlacerOrderNumberImagingServiceRequest);"
	print "\t\ttagByName.put(\"FillerOrderNumberOfImagingServiceRequest\",TagFromName.FillerOrderNumberImagingServiceRequest);"
	print "\t\ttagByName.put(\"ScheduledProcedureStepStartDateAndTime\",TagFromName.ScheduledProcedureStepStartDateTime);"
	print "\t\ttagByName.put(\"ScheduledProcedureStepModificationDateAndTime\",TagFromName.ScheduledProcedureStepModificationDateTime);"
	print "\t\ttagByName.put(\"ExpectedCompletionDateAndTime\",TagFromName.ExpectedCompletionDateTime);"
	print "\t\ttagByName.put(\"HumanPerformersOrganization\",TagFromName.HumanPerformerOrganization);"
	print "\t\ttagByName.put(\"HumanPerformersName\",TagFromName.HumanPerformerName);"
	print "\t\ttagByName.put(\"ImageRotationTrial\",TagFromName.ImageRotationRetired);"
	print "\t\ttagByName.put(\"ContentCreatorsName\",TagFromName.ContentCreatorName);"
	print "\t\ttagByName.put(\"ContentCreatorsIdentificationCodeSequence\",TagFromName.ContentCreatorIdentificationCodeSequence);"
	print "\t\ttagByName.put(\"PseudocolorType\",TagFromName.PseudoColorType);"
	print "\t\ttagByName.put(\"TopicKeyWords\",TagFromName.TopicKeywords);"
	print "\t\ttagByName.put(\"SOPAuthorizationDateAndTime\",TagFromName.SOPAuthorizationDateTime);"
	print "\t\ttagByName.put(\"ReferencedPrintJobSequencePull\",TagFromName.ReferencedPrintJobSequencePullStoredPrint);"
	print "\t\ttagByName.put(\"ReferencedPrintJobSequenceQueue\",TagFromName.ReferencedPrintJobSequence);"
	print "\t\t"
	print "\t}"
	print ""
}

