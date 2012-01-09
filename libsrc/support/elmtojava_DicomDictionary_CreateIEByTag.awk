NR==1	{
	if (role != "java" && role != "csv") {
		print "Error - role " role " invalid" >"/dev/tty"
		exit 1
	}

	trace=0
	emitFrameLevel=0
	
	if (role == "java") {
		print "\tprotected void createInformationEntityByTag() {"
		print ""
		print "\t\tinformationEntityByTag = new HashMap(100);"
		print ""
	}
	else if (role = "csv") {
		#print "\"Element\",\"IE\""
	}
	
	predefinedIEByName["StudyInstanceUID"]="STUDY";
	predefinedIEByName["SeriesInstanceUID"]="SERIES";
	predefinedIEByName["SOPInstanceUID"]="INSTANCE";
	predefinedIEByName["SOPClassUID"]="INSTANCE";
	predefinedIEByName["TransferSyntaxUID"]="INSTANCE";
	predefinedIEByName["SourceApplicationEntityTitle"]="INSTANCE";
	predefinedIEByName["InstanceNumber"]="CONCATENATION";
	predefinedIEByName["ConcatenationUID"]="CONCATENATION";
	predefinedIEByName["InConcatenationTotalNumber"]="CONCATENATION";
	predefinedIEByName["NumberOfPatientRelatedStudies"]="PATIENT";
	predefinedIEByName["NumberOfPatientRelatedSeries"]="PATIENT";
	predefinedIEByName["NumberOfPatientRelatedInstances"]="PATIENT";
	predefinedIEByName["NumberOfStudyRelatedSeries"]="STUDY";
	predefinedIEByName["NumberOfStudyRelatedInstances"]="STUDY";
	predefinedIEByName["NumberOfSeriesRelatedInstances"]="SERIES";
	predefinedIEByName["ModalitiesInStudy"]="STUDY";
	predefinedIEByName["SOPClassesInStudy"]="STUDY";
	predefinedIEByName["OtherStudyNumbers"]="STUDY";		# for IHE Query Images transaction
	predefinedIEByName["InterpretationAuthor"]="STUDY";		# for IHE Query Images transaction
	predefinedIEByName["AccessionNumber"]="STUDY";			# is used within references at instance level in key objects
	predefinedIEByName["IssuerOfAccessionNumberSequence"]="STUDY";

	# supress various generic references ...
	
	predefinedIEByName["ContentItemModifierSequence"]="";
	predefinedIEByName["ProtocolContextSequence"]="";
	predefinedIEByName["PurposeOfReferenceCodeSequence"]="";
	predefinedIEByName["ReferencedSOPClassUID"]="";
	predefinedIEByName["ReferencedSOPInstanceUID"]="";
	predefinedIEByName["ReferencedStudySequence"]="";
	predefinedIEByName["ReferencedImageSequence"]="";

	# supress or declare (at lowest level) series versus instance or frame conflicts ...

	predefinedIEByName["ViewPosition"]="INSTANCE";
	predefinedIEByName["InstitutionAddress"]="SERIES";		
	predefinedIEByName["InstitutionName"]="SERIES";
	predefinedIEByName["InstitutionCodeSequence"]="SERIES";

	predefinedIEByName["AcquisitionStartCondition"]="INSTANCE";
	predefinedIEByName["AcquisitionTerminationCondition"]="INSTANCE";
	predefinedIEByName["CollimatorType"]="INSTANCE";
	predefinedIEByName["CoincidenceWindowWidth"]="INSTANCE";
	predefinedIEByName["CountsSource"]="INSTANCE";
	predefinedIEByName["EnergyWindowRangeSequence"]="INSTANCE";
	predefinedIEByName["RandomsCorrectionMethod"]="INSTANCE";
	predefinedIEByName["ScatterCorrectionMethod"]="INSTANCE";
	
	predefinedIEByName["BeatRejectionFlag"]="";
	predefinedIEByName["ConversionType"]="";
	predefinedIEByName["ConvolutionKernel"]="";
	predefinedIEByName["CorrectedImage"]="";
	predefinedIEByName["FieldOfViewDimensions"]="";
	predefinedIEByName["FieldOfViewShape"]="";
	predefinedIEByName["FilterType"]="";
	
	predefinedIEByName["FocalSpots"]="INSTANCE";
	
	predefinedIEByName["GantryDetectorTilt"]="";
	predefinedIEByName["HeartRate"]="";
	predefinedIEByName["InterventionDrugInformationSequence"]="";
	predefinedIEByName["NumberOfRRIntervals"]="";
	predefinedIEByName["NumberOfSlices"]="";
	predefinedIEByName["NumberOfTimeSlots"]="";
	predefinedIEByName["PVCRejection"]="";
	predefinedIEByName["PatientGantryRelationshipCodeSequence"]="";
	predefinedIEByName["PatientOrientationCodeSequence"]="";
	predefinedIEByName["PatientPosition"]="";
	predefinedIEByName["RadiopharmaceuticalInformationSequence"]="";
	predefinedIEByName["ReconstructionDiameter"]="";
	predefinedIEByName["SkipBeats"]="";
	predefinedIEByName["TriggerSourceOrType"]="";
	predefinedIEByName["TypeOfDetectorMotion"]="";
	predefinedIEByName["CardiacFramingType"]="";

	# do not need to actively supress frame versus instance conflicts caused by enhanced multi-frame family
	# since the refusal to descend into sequences mitigates this problem

	iodcomp=""
	profile=""
	ie=""
	module=""
	macro=""
	macroie=""
	
	withinSequence=0
}

/^[ 	]*CompositeIOD=/ {
	iodcomp=""
	if (match($0,"CompositeIOD=\"[^\"]*\""))
		iodcomp=substr($0,RSTART+length("CompositeIOD=\""),
			RLENGTH-length("CompositeIOD=\"")-1);

	profile=""
	if (match($0,"Profile=\"[^\"]*\""))
		profile=substr($0,RSTART+length("Profile=\""),
			RLENGTH-length("Profile=\"")-1);

	if (trace) print "(CompositeIOD): iodcomp=" iodcomp " profile=" profile >"/dev/stderr"
}

/^[ 	]*CompositeIODEnd/ {
	if (trace) print "(CompositeIODEnd):" >"/dev/stderr"
	
	iodcomp=""
	profile=""
	module=""		# since no corresponding ModuleEnd in IODs ... else last module in IE in IOD carries into first module definition
}

/^[ 	]*InformationEntity=/ {
	ie=""
	if (match($0,"InformationEntity=\"[^\"]*\""))
		ie=substr($0,RSTART+length("InformationEntity=\""),
			RLENGTH-length("InformationEntity=\"")-1);

	if (trace) print "(InformationEntity): ie=" ie " profile=" profile >"/dev/stderr"

	if (profile != "") {
		if (trace) print "Supressing ie " ie " in " iodcomp " because of profile " profile >"/dev/stderr"
		ie=""	# supress information that occurs in profiles, since for convenience it may be at wrong level (e.g., always instance), e.g., IHEMammoProfile, DentalImageOnMediaProfile
	}
}

/^[ 	]*InformationEntityEnd/ {
	if (trace) print "(InformationEntityEnd):" >"/dev/stderr"
	
	ie=""
	module=""		# since no corresponding ModuleEnd in IODs ... else last module in IE in IOD carries into first module definition
}

/^[ 	]*DefineMacro=/ {
	macro=""
	if (match($0,"DefineMacro=\"[^\"]*\""))
		macroie=substr($0,RSTART+length("DefineMacro=\""),
			RLENGTH-length("DefineMacro=\"")-1);

	macroie=""
	if (match($0,"InformationEntity=\"[^\"]*\""))
		macroie=substr($0,RSTART+length("InformationEntity=\""),
			RLENGTH-length("InformationEntity=\"")-1);

	if (trace) print "(DefineMacro): macro=" macro " macroie=" macroie >"/dev/stderr"
}

/^[ 	]*MacroEnd/ {
	if (trace) print "(MacroEnd):" >"/dev/stderr"
	
	if (withinSequence != 0) {
		print "Error - unterminated sequence " sequence " within macro " macro >"/dev/stderr"
	}
	macro=""
	macroie=""
}

/^[ 	]*Module/ {

	module=""
	if (match($0,"Module=\"[^\"]*\""))
		module=substr($0,RSTART+length("Module=\""),
			RLENGTH-length("Module=\"")-1);

	if (trace) print "(Module): module=" module " ie=" ie >"/dev/stderr"
                        
	if (ie != "") {
		ieByModule[module]=ie;
	}
}

/^[ 	]*ModuleEnd/ {
	if (trace) print "(ModuleEnd):" >"/dev/stderr"
	
	if (withinSequence != 0) {
		print "Error - unterminated sequence " sequence " within module " module >"/dev/stderr"
	}
	module=""
}

/^[ 	]*SequenceEnd/ {
	if (trace) print "(SequenceEnd):" >"/dev/stderr"
	
	--withinSequence
}

/^[ 	]*(Name|Sequence)=/ {
	
	name=""
	if (match($0,"Name=\"[^\"]*\""))
		name=substr($0,RSTART+length("Name=\""),
			RLENGTH-length("Name=\"")-1);

	sequence=""
	if (match($0,"Sequence=\"[^\"]*\"")) {
		sequence=substr($0,RSTART+length("Sequence=\""),
			RLENGTH-length("Sequence=\"")-1);
	}
	
	if (trace) print "(Name|Sequence): name=" name " sequence=" sequence " in module=" module " macro=" macro " macroie=" macroie >"/dev/stderr"

	if (name == "" && sequence != "" && sequence != "BiplaneSequence") {
		name=sequence
	}

	if (withinSequence > 0) {
		if (trace) print "Skipping " name " within sequence in " module macro >"/dev/stderr"
		if (sequence  != "") {
			++withinSequence
		}
		next
	}
	else {
		if (trace) print "Not skipping " name " since not within sequence in " module macro >"/dev/stderr"
	}
	
	if (sequence  != "") {
		++withinSequence
	}
	
	if (name != "" && (module != "" || macroie != "")) {
		if (trace) print "Doing " name " in " module macro >"/dev/stderr"
		if (!(name in predefinedIEByName)) {		# else will be written out in bulk at the end
			if (macroie != "") {
				ourie=macroie
			}
			else {
				ourie=ieByModule[module];
			}
			useie="";
			if (ourie == "Patient") useie="PATIENT"
			else if (ourie == "Study") useie="STUDY"
			else if (ourie == "Series") useie="SERIES"
			else if (ourie == "Equipment") useie="SERIES"
			else if (ourie == "FrameOfReference") useie="SERIES"
			else if (ourie == "Image") useie="INSTANCE"
			else if (ourie == "Document") useie="INSTANCE"
			else if (ourie == "Waveform") useie="INSTANCE"
			else if (ourie == "Presentation") useie="INSTANCE"
			else if (ourie == "Instance") useie="INSTANCE"
			else if (ourie == "Frame") {
				if (emitFrameLevel) {
					useie="FRAME"
				}
				else {
					useie="INSTANCE"
				}
			}
			else if (ourie == "FunctionalGroup" && emitFrameLevel) useie="FRAME"
			
			#else ignore Curve, Directory, File, ModalityLUT, Overlay, Plan, StructureSet, TreatmentRecord, VOILUT, etc.

			if (useie != "") {
				if (ieByName[name] != "") {
					if (useie != ieByName[name]) {
						print "Error - different IE specified for " name " - " useie " (" ourie " in " module macro "), conflicts with first encountered " ieByName[name] " - using the latter"  >"/dev/stderr"
						#print name " " useie " " ieByName[name] >"/dev/stderr"
					}
					next;
				}
				ieByName[name]=useie;
				if (role == "java") {
					print "\t\tinformationEntityByTag.put(TagFromName." name ",InformationEntity." useie ");"
				}
				else if (role = "csv") {
					print name "," useie
				}
			}
		}
	}
}

END {
	for (name in predefinedIEByName) {
		useie=predefinedIEByName[name]
		if (useie != "") {
			if (role == "java") {
				print "\t\tinformationEntityByTag.put(TagFromName." name ",InformationEntity." useie ");"
			}
			else if (role = "csv") {
				print name "," useie
			}
		}
	}
	if (role == "java") {
		print "\t}"
		print ""
	}
}

