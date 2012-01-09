#include "attrmxls.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "sopclu.h"
#include "elmconst.h"
#include "attrval.h"
#include "attrseq.h"

#include "iodcomp.h"

// Similar to TextOutputStream& Tag::write(TextOutputStream& stream,ElementDictionary *dict) in libsrc/src/dctool/attrtag.cc
// but without the VR

static void writeTagNumberAndNameToLog(Tag tag,ElementDictionary *dict,TextOutputStream &log) {
	log << "(";
	writeZeroPaddedHexNumber(log,tag.getGroup(),4);
	log << ",";
	writeZeroPaddedHexNumber(log,tag.getElement(),4);
	log << ")";
	const char *desc = NULL;
	if (dict) {
		desc = dict->getDescription(tag);
	}
	if (desc && strlen(desc) > 0 && strcmp(desc,"?") != 0) {
		log << " " << desc;
	}
}

static void writeTagNumberAndNameToLog(Attribute *a,ElementDictionary *dict,TextOutputStream &log) {
	writeTagNumberAndNameToLog(a->getTag(),dict,log);
}

// loopOverListsInSequencesWithLog is copied from libsrc/src/dctool/attrmxls.cc

static bool
loopOverListsInSequencesWithLog(Attribute *a,TextOutputStream &log,
		bool (*func)(AttributeList &,TextOutputStream &))
{
	bool succeeded=true;
	if (strcmp(a->getVR(),"SQ") == 0) {
		AttributeList **al;
		int n;
		if ((n=a->getLists(&al)) > 0) {
			int i;
			for (i=0; i<n; ++i) {
				if (!(*func)(*al[i],log)) succeeded=false;
			}
			delete [] al;
		}
	}
	return succeeded;
}

static bool
loopOverListsInSequencesWithRootListAndLog(AttributeList &rootlist,Attribute *a,TextOutputStream &log,
		bool (*func)(AttributeList &,AttributeList &,TextOutputStream &))
{
	bool succeeded=true;
	if (strcmp(a->getVR(),"SQ") == 0) {
		AttributeList **al;
		int n;
		if ((n=a->getLists(&al)) > 0) {
			int i;
			for (i=0; i<n; ++i) {
				if (!(*func)(rootlist,*al[i],log)) succeeded=false;
			}
			delete [] al;
		}
	}
	return succeeded;
}

static bool checkScaledNumericValues(Tag coarseTag,const char *coarseName,Tag fineTag,const char *fineName,double scaleFactor,AttributeList &list,TextOutputStream &log) {
	bool success=true;
	Attribute *aCoarse=list[coarseTag];
	Attribute *aFine=list[fineTag];
	if (aCoarse && aFine) {
		Uint16 coarseValue = 0;
		Float64 fineValue = 0;		// Uint16 would be OK if was always IS, but sometimes fine values are DS and not IS, and Uint16 may be too small
		if (aCoarse->getValue(0,coarseValue) && aFine->getValue(0,fineValue)) {
			if (coarseValue != Uint16(round(fineValue/scaleFactor)) && coarseValue != Uint16(fineValue/scaleFactor)) {		// NB. standard does not specify whether or not to round or truncate, so allow both
				log << EMsgDC(ScaledNumericValuesForSameConceptAreInconsistent) << " - " << coarseName << " = " << coarseValue << " and " << fineName << " = " << fineValue << endl;
				success=false;
			}
		}
	}
	return success;
}

static bool
checkScaledNumericValues(AttributeList &list,TextOutputStream &log) {
	bool success=true;
	if (!checkScaledNumericValues(TagFromName(Exposure),"Exposure",TagFromName(ExposureInuAs),"ExposureInuAs",1000.0,list,log)) success = false;
	if (!checkScaledNumericValues(TagFromName(ExposureTime),"ExposureTime",TagFromName(ExposureTimeInuS),"ExposureTimeInuS",1000.0,list,log)) success = false;
	if (!checkScaledNumericValues(TagFromName(XRayTubeCurrent),"XRayTubeCurrent",TagFromName(XRayTubeCurrentInuA),"XRayTubeCurrentInuA",1000.0,list,log)) success = false;

	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithLog(a,log,&::checkScaledNumericValues)) {
			success=false;
		}
		++listi;
	}
	return success;
}

static bool
checkPatientOrientationValuesForBiped(AttributeList &list,TextOutputStream &log) {
//log << "checkPatientOrientationValuesForBiped" << endl;
	bool success=true;
	Attribute *aPatientOrientation=list[TagFromName(PatientOrientation)];
	if (aPatientOrientation) {
		int nValues = aPatientOrientation->getVM();
		const char *firstValue = NULL;
		for (int i=0; i<nValues; ++i) {
			char *value;
			if (aPatientOrientation->getValue(i,value)) {
				if (value) {
					bool haveA = false;
					bool haveP = false;
					bool haveH = false;
					bool haveF = false;
					bool haveL = false;
					bool haveR = false;
					int length=strlen(value);
					for (int p=0; p<length; ++p) {
						char c = value[p];
						switch (c) {
							case 'A':	haveA = true; break;
							case 'P':	haveP = true; break;
							case 'H':	haveH = true; break;
							case 'F':	haveF = true; break;
							case 'L':	haveL = true; break;
							case 'R':	haveR = true; break;
							default:
										log << EMsgDC(IllegalCharacterInPatientOrientation) << " - '" << c << "' - only L, R, A, P, H or F permitted" << endl;
										success = false;
										break;
						}
					}
					if (haveA && haveP) {
						log << EMsgDC(ConflictingDirectionsInPatientOrientationCannotBePresentInSameValue) << " - A, P " << endl;
						success = false;
					}
					if (haveH && haveF) {
						log << EMsgDC(ConflictingDirectionsInPatientOrientationCannotBePresentInSameValue) << " - H, F " << endl;
						success = false;
					}
					if (haveL && haveR) {
						log << EMsgDC(ConflictingDirectionsInPatientOrientationCannotBePresentInSameValue) << " - L, R " << endl;
						success = false;
					}
					if (i == 0) {
						firstValue=value;
					}
					else if (firstValue && value && strlen(firstValue) > 0 && strcmp(firstValue,value) == 0) {
						log << EMsgDC(PatientOrientationRowAndColumnDirectionsCannotBeIdentical) << " - \"" << firstValue << "\" and \"" << value << "\"" << endl;
						success = false;
					}
				}
			}
		}
	}
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithLog(a,log,&::checkPatientOrientationValuesForBiped)) {
			success=false;
		}
		++listi;
	}
	return success;
}

static bool
checkPatientOrientationValuesForQuadruped(AttributeList &list,TextOutputStream &log) {
//log << "checkPatientOrientationValuesForQuadruped" << endl;
	bool success=true;
	Attribute *aPatientOrientation=list[TagFromName(PatientOrientation)];
	if (aPatientOrientation) {
		int nValues = aPatientOrientation->getVM();
		const char *firstValue = NULL;
		for (int i=0; i<nValues; ++i) {
			char *value;
			if (aPatientOrientation->getValue(i,value)) {
				if (value) {
					bool haveCD = false;	// (Cd or Caudal)
					bool haveCR = false;	// (Cr or Cranial)
					bool haveD  = false;	// (Dorsal)
					bool haveDI = false;	// (Di or Distal)
					bool haveL  = false;	// (Lateral)
					bool haveLE = false;	// (Le or Left)
					bool haveM  = false;	// (Medial)
					bool havePA = false;	// (Pa or Palmar)
					bool havePL = false;	// (Pl or Plantar
					bool havePR = false;	// (Pr or Proximal)
					bool haveR  = false;	// (Rostral)
					bool haveRT = false;	// (Rt or Right)
					bool haveV  = false;	// (Ventral)
					bool badValue = false;
					int length=strlen(value);
					char c;
					for (int p=0; p<length && !badValue; ++p) {
						c = value[p];
						switch (c) {
							case 'C':	if (p<length && value[p+1] == 'D') {
											++p;
											haveCD = true;
										}
										else if (p<length && value[p+1] == 'R') {
											++p;
											haveCR = true;
										}
										else {
											badValue = true;
										}
										break;
							case 'D':	if (p<length && value[p+1] == 'I') {
											++p;
											haveDI = true;
										}
										else {
											haveD = true;
										}
										break;
							case 'L':	if (p<length && value[p+1] == 'E') {
											++p;
											haveLE = true;
										}
										else {
											haveL = true;
										}
										break;
							case 'M':	haveM = true; break;
							case 'P':	if (p<length && value[p+1] == 'A') {
											++p;
											havePA = true;
										}
										else if (p<length && value[p+1] == 'L') {
											++p;
											havePL = true;
										}
										else if (p<length && value[p+1] == 'R') {
											++p;
											havePR = true;
										}
										else {
											badValue = true;
										}
										break;
							case 'R':	if (p<length && value[p+1] == 'T') {
											++p;
											haveRT = true;
										}
										else {
											haveR = true;
										}
										break;
							case 'V':	haveV = true; break;
							default:	badValue = true; break;
						}
					}
					if (badValue) {
						log << EMsgDC(IllegalCharacterInPatientOrientation) << " - '" << c << "' - only CD, CR, D, DI, L, LE, M, PA, PL, PR, R, RT or V permitted for quadruped" << endl;
						success = false;
					}
					else {
						if (haveLE && haveRT) {
							log << EMsgDC(ConflictingDirectionsInPatientOrientationCannotBePresentInSameValue) << " - LE, RT " << endl;
							success = false;
						}
						if (haveD && haveV) {
							log << EMsgDC(ConflictingDirectionsInPatientOrientationCannotBePresentInSameValue) << " - D, V " << endl;
							success = false;
						}
						if (haveCR && haveCD || haveR && haveCD || haveR && haveCR) {
							log << EMsgDC(ConflictingDirectionsInPatientOrientationCannotBePresentInSameValue) << " - CR, CD, R " << endl;
							success = false;
						}
						if (haveM && haveL) {
							log << EMsgDC(ConflictingDirectionsInPatientOrientationCannotBePresentInSameValue) << " - M, L " << endl;
							success = false;
						}
						if (havePR && haveDI) {
							log << EMsgDC(ConflictingDirectionsInPatientOrientationCannotBePresentInSameValue) << " - PR, DI " << endl;
							success = false;
						}
						if (havePA && havePL) {
							log << EMsgDC(ConflictingDirectionsInPatientOrientationCannotBePresentInSameValue) << " - PA, PL " << endl;
							success = false;
						}
						if (i == 0) {
							firstValue=value;
						}
						else if (firstValue && value && strlen(firstValue) > 0 && strcmp(firstValue,value) == 0) {
							log << EMsgDC(PatientOrientationRowAndColumnDirectionsCannotBeIdentical) << " - \"" << firstValue << "\" and \"" << value << "\"" << endl;
						success = false;
						}
					}
				}
			}
		}
	}
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithLog(a,log,&::checkPatientOrientationValuesForQuadruped)) {
			success=false;
		}
		++listi;
	}
	return success;
}

static bool
checkPatientOrientationValuesForBipedOrQuadruped(AttributeList &list,TextOutputStream &log) {
	bool quadruped = false;
	Attribute *aAnatomicalOrientationType=list[TagFromName(AnatomicalOrientationType)];
	if (aAnatomicalOrientationType) {
		const char *vAnatomicalOrientationType=AttributeValue(aAnatomicalOrientationType);
//log << "checkPatientOrientationValuesForBipedOrQuadruped(): vAnatomicalOrientationType = " << vAnatomicalOrientationType << endl;
		if (vAnatomicalOrientationType && strcmp(vAnatomicalOrientationType,"QUADRUPED") == 0) {
			quadruped = true;
		}
	}
	return quadruped ? checkPatientOrientationValuesForQuadruped(list,log) : checkPatientOrientationValuesForBiped(list,log);
}

static bool
checkUIDs(AttributeList &list,TextOutputStream &log) {
	bool success=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		const char *vr = a->getVR();
		if (vr && vr[0]=='U' &&  vr[1]=='I') {
			int nValues = a->getVM();
			for (int i=0; i<nValues; ++i) {
				char *value;
				if (a->getValue(i,value)) {
					if (value && strlen(value) >= 2) {
						if (strncmp(value,"1.",2) != 0 && strncmp(value,"2.",2) != 0) {
							log << EMsgDC(IllegalRootForUID) << " - \"" << value << "\" in ";
							writeTagNumberAndNameToLog(a,list.getDictionary(),log);
							log << endl;
							success = false;
						}
					}
				}
			}
		}
		if (!::loopOverListsInSequencesWithLog(a,log,&::checkUIDs)) {
			success=false;
		}
		++listi;
	}
	return success;
}

static bool
checkNoEmptyReferencedFileIDComponents(AttributeList &list,TextOutputStream &log) {
	bool success=true;
	Attribute *aReferencedFileID=list[TagFromName(ReferencedFileID)];
	if (aReferencedFileID && aReferencedFileID->isEmptyOrHasAnyEmptyValue()) {
		log << EMsgDC(ReferencedFileIDHasEmptyComponents) << endl;
		success=false;
	}
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithLog(a,log,&::checkNoEmptyReferencedFileIDComponents)) {
			success=false;
		}
		++listi;
	}
	return success;
}

static bool
checkNoIllegalOddNumberedGroups(AttributeList &list,TextOutputStream &log) {
	bool success=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithLog(a,log,&::checkNoIllegalOddNumberedGroups)) {
			success=false;
		}
		Tag tag=a->getTag();
		Uint16 group = tag.getGroup();
		if (group == 0x0001 || group == 0x0003 || group == 0x0005 || group == 0x0007) {
			log << EMsgDC(BadGroup) << " - (";
			writeZeroPaddedHexNumber(log,group,4);
			log << ",";
			writeZeroPaddedHexNumber(log,tag.getElement(),4);
			log << ") " << endl;
			success=false;
		}
		++listi;
	}
	return success;
}

static bool
checkHasStringValueElseWarning(AttributeList &list,Tag tag,const char *label,TextOutputStream &log)
{
	Attribute *a=list[tag];
	if (!a || a->getVM() == 0) {
		log << WMsgDC(MissingAttributeValueNeededForDirectory)
		    << " - " << label
		    << endl;
		return false;
	}
	else {
		return true;
	}
}

static bool
checkPresentationStateDisplayedAreaSelectionValuesAreValid(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	
	Attribute *aDisplayedAreaSelectionSequence=list[TagFromName(DisplayedAreaSelectionSequence)];
	if (aDisplayedAreaSelectionSequence) {
		if (aDisplayedAreaSelectionSequence->isSequence()) {
			SequenceAttribute *sDisplayedAreaSelectionSequence = (SequenceAttribute *)aDisplayedAreaSelectionSequence;
			AttributeList **displayedAreaSelectionSequenceItemLists;
			int nDisplayedAreaSelectionSequenceItems = sDisplayedAreaSelectionSequence->getLists(&displayedAreaSelectionSequenceItemLists);
			int i;
			for (int i=0; i<nDisplayedAreaSelectionSequenceItems; ++i) {
				AttributeList *displayedAreaSelectionSequenceItemList = displayedAreaSelectionSequenceItemLists[i];
				if (displayedAreaSelectionSequenceItemList) {
					Attribute *aDisplayedAreaTopLeftHandCorner=(*displayedAreaSelectionSequenceItemList)[TagFromName(DisplayedAreaTopLeftHandCorner)];
					Attribute *aDisplayedAreaBottomRightHandCorner=(*displayedAreaSelectionSequenceItemList)[TagFromName(DisplayedAreaBottomRightHandCorner)];
					if (aDisplayedAreaTopLeftHandCorner && aDisplayedAreaTopLeftHandCorner->getVM() == 2 && aDisplayedAreaBottomRightHandCorner && aDisplayedAreaBottomRightHandCorner->getVM() == 2) {
						Int32 tlhcX,tlhcY,brhcX,brhcY;
						if (aDisplayedAreaTopLeftHandCorner->getValue(0,tlhcX) && aDisplayedAreaTopLeftHandCorner->getValue(1,tlhcY)
						 && aDisplayedAreaBottomRightHandCorner->getValue(0,brhcX) && aDisplayedAreaBottomRightHandCorner->getValue(1,brhcY)) {
							if (tlhcX >= brhcX || tlhcY >= brhcY) {
								log << EMsgDC(DisplayedAreaSelectionSequenceInternallyInconsistent)
									<< " - DisplayedAreaTopLeftHandCorner = (" << tlhcX << "," << tlhcY << ") "
									<< " is not above and to the left of"
									<< " DisplayedAreaBottomRightHandCorner = (" << brhcX << "," << brhcY << ") "
									<< endl;
								success=false;
							}
						}
					}
				}
			}
		}
	}
	return success;
}

static bool
checkValuesNeededToBuildDicomDirectoryArePresentAndNotEmpty(AttributeList &list,TextOutputStream &log)
{
	bool success=true;

	Attribute *aSOPClassUID=list[TagFromName(SOPClassUID)];
	const char *vSOPClassUID=AttributeValue(aSOPClassUID);
	Attribute *aDirectoryRecordSequence=list[TagFromName(DirectoryRecordSequence)];
	if (!aDirectoryRecordSequence && (vSOPClassUID == NULL || (strcmp(vSOPClassUID,HangingProtocolStorageSOPClassUID) != 0 && strcmp(vSOPClassUID,ColorPaletteStorageSOPClassUID) != 0))) {
		success = checkHasStringValueElseWarning(list,TagFromName(PatientID),"Patient ID",log) && success;
		success = checkHasStringValueElseWarning(list,TagFromName(StudyDate),"Study Date",log) && success;
		success = checkHasStringValueElseWarning(list,TagFromName(StudyTime),"Study Time",log) && success;
		success = checkHasStringValueElseWarning(list,TagFromName(StudyID),"Study ID",log) && success;
		success = checkHasStringValueElseWarning(list,TagFromName(Modality),"Modality",log) && success;
		success = checkHasStringValueElseWarning(list,TagFromName(SeriesNumber),"Series Number",log) && success;
		if (list[TagFromName(PixelData)]) {
			success = checkHasStringValueElseWarning(list,TagFromName(InstanceNumber),"Instance Number",log) && success;
		}
		// should do RT, PS, Waveform, SR, Spectroscopy, etc. stuff here as well :(
	}
	return success;
}

static bool
checkWaveformSequenceIsInternallyConsistent(AttributeList &list,TextOutputStream &log) {
	bool success=true;
	Attribute *aWaveformSequence=list[TagFromName(WaveformSequence)];
	if (aWaveformSequence) {
		if (aWaveformSequence->isSequence()) {
			SequenceAttribute *sWaveformSequence = (SequenceAttribute *)aWaveformSequence;
			AttributeList **multiplexGroupItemLists;
			int nMultiplexGroups = sWaveformSequence->getLists(&multiplexGroupItemLists);
			int i;
			for (int i=0; i<nMultiplexGroups; ++i) {
				AttributeList *multiplexGroupList = multiplexGroupItemLists[i];
				if (multiplexGroupList) {
					Attribute *aNumberOfWaveformChannels = (*multiplexGroupList)[TagFromName(NumberOfWaveformChannels)];
					Attribute *aChannelDefinitionSequence = (*multiplexGroupList)[TagFromName(ChannelDefinitionSequence)];
					if (aNumberOfWaveformChannels && aChannelDefinitionSequence && aChannelDefinitionSequence->isSequence()) {
						Uint16 numberOfWaveformChannels = AttributeValue(aNumberOfWaveformChannels);
						SequenceAttribute *sChannelDefinitionSequence = (SequenceAttribute *)aChannelDefinitionSequence;
						AttributeList **channelDefinitionSequenceLists;
						int numberOfChannelDefinitionSequenceItems = sChannelDefinitionSequence->getLists(&channelDefinitionSequenceLists);
						if (numberOfWaveformChannels != numberOfChannelDefinitionSequenceItems) {
							log << EMsgDC(WaveformSequenceInternallyInconsistent)
							    << " - NumberOfWaveformChannels = " << numberOfWaveformChannels
							    << " but number of ChannelDefinitionSequence Items = " << numberOfChannelDefinitionSequenceItems
							    << endl;
							success=false;
						}
					}
				}
			}
		}
	}
	return success;
}

static bool
checkPixelDataIsTheCorrectLength(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	Attribute *aPixelData=list[TagFromName(PixelData)];
	if (aPixelData) {
		Uint32 vl = aPixelData->getVL();
		if (vl != 0xffffffff) {
//log << "vl=" << dec << vl << endl;
			Uint16 vRows=AttributeValue(list[TagFromName(Rows)]);
			Uint16 vColumns=AttributeValue(list[TagFromName(Columns)]);
			Uint16 vBitsAllocated=AttributeValue(list[TagFromName(BitsAllocated)]);
			Uint16 vBitsStored=AttributeValue(list[TagFromName(BitsStored)]);
			Uint16 vNumberOfFrames=AttributeValue(list[TagFromName(NumberOfFrames)],1);
			if (vNumberOfFrames == 0) {	// cannot be -ve since unsigned
				// do not send an error message, since will always be picked up as unexpected enumerated value of 0 in CheckSingleFramePseudo Module or NotZeroError in Multiframe Module etc.
				vNumberOfFrames = 1;	// so that pixel data length expected will not be 0 (000393)
			}
			Uint16 vSamplesPerPixel=AttributeValue(list[TagFromName(SamplesPerPixel)],1);
//log << "vRows=" << dec << vRows << endl;
//log << "vColumns=" << dec << vColumns << endl;
//log << "vBitsAllocated=" << dec << vBitsAllocated << endl;
//log << "vBitsStored=" << dec << vBitsStored << endl;
//log << "vNumberOfFrames=" << dec << vNumberOfFrames << endl;
//log << "vSamplesPerPixel=" << dec << vSamplesPerPixel << endl;
			// suppress the error when BitsAllocated is not sent, assuming byte aligned based on BitsStored
			Uint16 useBits = vBitsAllocated == 0 ? (vBitsStored == 0 ? 16 : ((vBitsStored-1)/8+1)*8 ) : vBitsAllocated;
//log << "useBits=" << dec << useBits << endl;
			Uint32 expectedLength;
			if (useBits % 8 == 0) {
				Uint16 useBytes = useBits/8;
//log << "useBytes=" << dec << useBytes << endl;
				expectedLength = ((Uint32)vRows)*vColumns*useBytes*vNumberOfFrames*vSamplesPerPixel;
			}
			else {
				// account for packed data, as in old ACR-NEMA objects, instead of assuming whole number of bytes
				// but this may overflow 32 bits if too long :(
				expectedLength = (((Uint32)vRows)*vColumns*useBits*vNumberOfFrames*vSamplesPerPixel - 1)/8 + 1;
			}
//log << "expectedLength=" << dec << expectedLength << endl;
			if (expectedLength%2 == 1) {
				++expectedLength;	// odd lengths are always padded by one byte to even ([bugs.dicom3tools] (000113))
			}
			if (vl != expectedLength) {
				log << EMsgDC(PixelDataIncorrectVL)
					<< " - " << MMsgDC(Expected) << " " << dec << expectedLength << " dec"
					<< " - " << MMsgDC(Got) << " " << dec << vl << " dec"
				    << endl;
				success=false;
			}
		}
		// else do not check if encapsulated
	}
	
	return success;
}


static bool
checkPixelAspectRatioValidIfPresent(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	Attribute *aPixelAspectRatio=list[TagFromName(PixelAspectRatio)];
	if (aPixelAspectRatio) {
		int length = aPixelAspectRatio->getVL();
		Uint32 value1,value2;
		if (aPixelAspectRatio->getValue(0,value1) && aPixelAspectRatio->getValue(1,value2)) {
			if (value1 == value2) {
				log << EMsgDC(PixelAspectRatioNotPermittedWhenOneToOne) << " - values are " << value1 << "\\" << value2 << endl;
			}
		}
	}

	return success;
}

static bool
checkEstimatedRadiographicMagnificationFactorIfPresent(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	Attribute *aEstimatedRadiographicMagnificationFactor=list[TagFromName(EstimatedRadiographicMagnificationFactor)];
	Attribute *aDistanceSourceToDetector=list[TagFromName(DistanceSourceToDetector)];
	Attribute *aDistanceSourceToPatient=list[TagFromName(DistanceSourceToPatient)];
	if (aEstimatedRadiographicMagnificationFactor && aDistanceSourceToDetector && aDistanceSourceToPatient) {
		Float32 vEstimatedRadiographicMagnificationFactor = 0;
		Float32 vDistanceSourceToDetector = 0;
		Float32 vDistanceSourceToPatient = 0;
		if (aEstimatedRadiographicMagnificationFactor->getValue(0,vEstimatedRadiographicMagnificationFactor)
		 && aDistanceSourceToDetector->getValue(0,vDistanceSourceToDetector)
		 && aDistanceSourceToPatient->getValue(0,vDistanceSourceToPatient)) {
			if (vDistanceSourceToPatient > 0
			 && fabs(vDistanceSourceToDetector/vDistanceSourceToPatient - vEstimatedRadiographicMagnificationFactor) > 0.0001) {
				log << WMsgDC(EstimatedRadiographicMagnificationFactorDoesNotMatchRatioOfDistanceSourceToDetectorAndDistanceSourceToPatient)
					<< " - values are " << vEstimatedRadiographicMagnificationFactor << ", " << vDistanceSourceToDetector << ", " << vDistanceSourceToPatient << endl;
			}
		}
	}

	return success;
}

static bool checkUnitVector(Attribute *a,int length,int valueOffset,const char *whichVector,const char *tagName,TextOutputStream &log) {
//cerr << "checkUnitVector(): checking " << whichVector << " of " << tagName << endl;
	bool success=true;
	Float32 x,y,z;
	if (valueOffset+2 < length && a->getValue(valueOffset,x) && a->getValue(valueOffset+1,y) && a->getValue(valueOffset+2,z)) {
		Float32 sumOfSquares = x*x + y*y + z*z;
//cerr << "checkUnitVector(): x = " << x << endl;
//cerr << "checkUnitVector(): y = " << y << endl;
//cerr << "checkUnitVector(): z = " << z << endl;
//cerr << "checkUnitVector(): sumOfSquares = " << sumOfSquares << endl;
		if (fabs(sumOfSquares - 1) > 0.0001) {
			success=false;
			log << EMsgDC(OrientationVectorIsNotUnitVector) << " for " << whichVector << " vector of " << tagName << " - values are " << x << "\\" << y << "\\" << z << endl;
		}
	}
	return success;
}

static bool checkOrthogonal(
		Attribute *a1,int length1,int valueOffset1,const char *whichVector1,const char *tagName1,
		Attribute *a2,int length2,int valueOffset2,const char *whichVector2,const char *tagName2,
		TextOutputStream &log) {
//cerr << "checkOrthogonal(): checking " << whichVector1 << " of " << tagName1 << " and " << whichVector2 << " of " << tagName2 << endl;
	bool success=true;
	Float32 x1,y1,z1;
	Float32 x2,y2,z2;
	if (valueOffset1+2 < length1 && a1->getValue(valueOffset1,x1) && a1->getValue(valueOffset1+1,y1) && a1->getValue(valueOffset1+2,z1)
	 && valueOffset2+2 < length2 && a2->getValue(valueOffset2,x2) && a2->getValue(valueOffset2+1,y2) && a2->getValue(valueOffset2+2,z2)) {
//cerr << "checkUnitVector(): x1 = " << x1 << endl;
//cerr << "checkUnitVector(): y1 = " << y1 << endl;
//cerr << "checkUnitVector(): z1 = " << z1 << endl;
//cerr << "checkUnitVector(): x2 = " << x2 << endl;
//cerr << "checkUnitVector(): y2 = " << y2 << endl;
//cerr << "checkUnitVector(): z2 = " << z2 << endl;
		Float32 dotProduct = x1*x2 + y1*y2 + z1*z2;
//cerr << "checkUnitVector(): dotProduct = " << dotProduct << endl;
		if (fabs(dotProduct) > 0.0001) {
			success=false;
			log << EMsgDC(OrientationVectorsAreNotOrthogonal)
				<< " for " << whichVector1 << " vector of " << tagName1 << " (" << x1 << "\\" << y1 << "\\" << z1 << ")"
				<< " and " << whichVector2 << " vector of " << tagName2 << " (" << x2 << "\\" << y2 << "\\" << z2 << ")"
				<< endl;
		}
	}
	return success;
}

static bool
checkOrientationsAreOrthogonal(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	{
		Attribute *aImageOrientationPatient=list[TagFromName(ImageOrientationPatient)];
		if (aImageOrientationPatient) {
			int length = aImageOrientationPatient->getVL();
			if (!checkOrthogonal(aImageOrientationPatient,length,0,"row","ImageOrientationPatient",aImageOrientationPatient,length,3,"column","ImageOrientationPatient",log)) success = false;
		}
	}
	{
		Attribute *aImageOrientation=list[TagFromName(ImageOrientation)];
		if (aImageOrientation) {
			int length = aImageOrientation->getVL();
			if (!checkOrthogonal(aImageOrientation,length,0,"row","ImageOrientation",aImageOrientation,length,3,"column","ImageOrientation",log)) success = false;
		}
	}
	
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithLog(a,log,&::checkOrientationsAreOrthogonal)) {
			success=false;
		}
		++listi;
	}
	return success;
}

static bool
checkOrientationsAreUnitVectors(AttributeList &list,TextOutputStream &log)
{
	// call for ImageOrientationPatient and ImageOrientation
	bool success=true;
	{
		Attribute *aImageOrientationPatient=list[TagFromName(ImageOrientationPatient)];
		if (aImageOrientationPatient) {
			int length = aImageOrientationPatient->getVL();
			if (!checkUnitVector(aImageOrientationPatient,length,0,"row","ImageOrientationPatient",log)) success = false;
			if (!checkUnitVector(aImageOrientationPatient,length,3,"column","ImageOrientationPatient",log)) success = false;
		}
	}
	{
		Attribute *aImageOrientation=list[TagFromName(ImageOrientation)];
		if (aImageOrientation) {
			int length = aImageOrientation->getVL();
			if (!checkUnitVector(aImageOrientation,length,0,"row","ImageOrientation",log)) success = false;
			if (!checkUnitVector(aImageOrientation,length,3,"column","ImageOrientation",log)) success = false;
		}
	}
	{
		Attribute *aControlPointOrientation=list[TagFromName(ControlPointOrientation)];
		if (aControlPointOrientation) {
			int length = aControlPointOrientation->getVL();
			if (!checkUnitVector(aControlPointOrientation,length,0,"","ControlPointOrientation",log)) success = false;
		}
	}
	{
		Attribute *aSlabOrientation=list[TagFromName(SlabOrientation)];
		if (aSlabOrientation) {
			int length = aSlabOrientation->getVL();
			if (!checkUnitVector(aSlabOrientation,length,0,"","SlabOrientation",log)) success = false;
		}
	}
	{
		Attribute *aVelocityEncodingDirection=list[TagFromName(VelocityEncodingDirection)];
		if (aVelocityEncodingDirection) {
			int length = aVelocityEncodingDirection->getVL();
			if (!checkUnitVector(aVelocityEncodingDirection,length,0,"","VelocityEncodingDirection",log)) success = false;
		}
	}
	
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithLog(a,log,&::checkOrientationsAreUnitVectors)) {
			success=false;
		}
		++listi;
	}
	return success;
}

static bool
checkPixelSpacingCalibration(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	Attribute *aPixelSpacing=list[TagFromName(PixelSpacing)];
	Attribute *aImagerPixelSpacing=list[TagFromName(ImagerPixelSpacing)];
	Attribute *aNominalScannedPixelSpacing=list[TagFromName(NominalScannedPixelSpacing)];
	Attribute *aPixelSpacingCalibrationType=list[TagFromName(PixelSpacingCalibrationType)];

	if (aPixelSpacing && !aPixelSpacingCalibrationType) {
		Float32 vPixelSpacingRow = 0;
		Float32 vPixelSpacingCol = 0;
		if (aPixelSpacing->getValue(0,vPixelSpacingRow) && aPixelSpacing->getValue(1,vPixelSpacingCol)) {
			if (aImagerPixelSpacing) {
				Float32 vImagerPixelSpacingRow = 0;
				Float32 vImagerPixelSpacingCol = 0;
				if (aImagerPixelSpacing->getValue(0,vImagerPixelSpacingRow) && aImagerPixelSpacing->getValue(1,vImagerPixelSpacingCol)
				&& (vPixelSpacingRow != vImagerPixelSpacingRow || vPixelSpacingCol != vImagerPixelSpacingCol)) {
					log << WMsgDC(PixelSpacingDoesNotMatchImagerPixelSpacingButPixelSpacingCalibrationTypeNotPresent)
					    << " - PixelSpacing = " << vPixelSpacingRow << "\\" << vPixelSpacingCol << " versus ImagerPixelSpacing = "
						<< vImagerPixelSpacingRow << "\\" << vImagerPixelSpacingCol << endl;
				}
			}
			if (aNominalScannedPixelSpacing) {
				Float32 vNominalScannedPixelSpacingRow = 0;
				Float32 vNominalScannedPixelSpacingCol = 0;
				if (aNominalScannedPixelSpacing->getValue(0,vNominalScannedPixelSpacingRow) && aNominalScannedPixelSpacing->getValue(1,vNominalScannedPixelSpacingCol)
				&& (vPixelSpacingRow != vNominalScannedPixelSpacingRow || vPixelSpacingCol != vNominalScannedPixelSpacingCol)) {
					log << WMsgDC(PixelSpacingDoesNotMatchNominalScannedPixelSpacingButPixelSpacingCalibrationTypeNotPresent)
					    << " - PixelSpacing = " << vPixelSpacingRow << "\\" << vPixelSpacingCol << " versus NominalScannedPixelSpacing = "
						<< vNominalScannedPixelSpacingRow << "\\" << vNominalScannedPixelSpacingCol << endl;
				}
			}
		}
	}

	return success;
}

static bool
checkSpacingBetweenSlicesIsNotNegative(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	Attribute *aSpacingBetweenSlices=list[TagFromName(SpacingBetweenSlices)];

	if (aSpacingBetweenSlices) {
		Float32 vSpacingBetweenSlices = 0;
		if (aSpacingBetweenSlices->getValue(0,vSpacingBetweenSlices)) {
			if (vSpacingBetweenSlices < 0) {
				Attribute *aSOPClassUID=list[TagFromName(SOPClassUID)];
				char *vSOPClassUID;
				if (!aSOPClassUID || !aSOPClassUID->getValue(0,vSOPClassUID) || strcmp(vSOPClassUID,NuclearMedicineImageStorageSOPClassUID) != 0) {
					log << EMsgDC(IllegalNegativeValue) << " - SpacingBetweenSlices = " << vSpacingBetweenSlices << endl;
					success = false;
				}
			}
		}
	}
	return success;
}

static bool
checkFrameIncrementPointerValuesValid(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	Attribute *aFrameIncrementPointer=list[TagFromName(FrameIncrementPointer)];
	if (aFrameIncrementPointer) {
		int length = aFrameIncrementPointer->getVL();
		for (int i=0; i<length; ++i) {
			Uint32 value;
			if (aFrameIncrementPointer->getValue(i,value)) {
				Tag *tag = new Tag(value);
				Attribute *a = list[*tag];
				if (!a) {
					log << EMsgDC(FrameIncrementPointerValueNotPresentInDataset)
					    << " for value " << i << ", which is ";
					writeTagNumberAndNameToLog(*tag,list.getDictionary(),log);
					log << endl;
				}
				delete tag;
			}
		}
	}

	return success;
}

static bool
checkFrameVectorCountsValid(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	Attribute *aNumberOfFrames=list[TagFromName(NumberOfFrames)];
	Uint32 vNumberOfFrames = 1;
	if (aNumberOfFrames) {
		(void)aNumberOfFrames->getValue(0,vNumberOfFrames);
	}
	
	const Tag vectorTags[] = {
		TagFromName(EnergyWindowVector),
		TagFromName(DetectorVector),
		TagFromName(PhaseVector),
		TagFromName(RotationVector),
		TagFromName(RRIntervalVector),
		TagFromName(TimeSlotVector),
		TagFromName(SliceVector),
		TagFromName(AngularViewVector)
	};
	const Tag numberTags[] = {
		TagFromName(NumberOfEnergyWindows),
		TagFromName(NumberOfDetectors),
		TagFromName(NumberOfPhases),
		TagFromName(NumberOfRotations),
		TagFromName(NumberOfRRIntervals),
		TagFromName(NumberOfTimeSlots),
		TagFromName(NumberOfSlices),
		TagFromName(TimeSliceVector)
	};
	int nTags = 8;

	for (int i=0; i<nTags; ++i) {
		Attribute *aVector=list[vectorTags[i]];
		if (aVector) {
			int nValues = aVector->getVM();
			if (nValues != vNumberOfFrames) {
				log << EMsgDC(NumberOfValuesInVectorDoesNotMatchNumberOfFrames)
					<< " for Attribute ";
				writeTagNumberAndNameToLog(aVector,list.getDictionary(),log);
				log << ", which has " << dec << nValues << " values"
					<< " though Number of Frames is " << vNumberOfFrames
					<< endl;
				success = false;
			}
			Attribute *aNumber=list[numberTags[i]];
			Uint32 vNumber = 0;
			if (aNumber && aNumber->getValue(0,vNumber) && nValues > 0) {
				Uint32 lowestValueFound = 0xffffffff;
				Uint32 highestValueFound = 0;
				for (int j=0; j<nValues; ++j) {
					Uint32 value;
					if (aVector->getValue(j,value)) {
						if (value > highestValueFound) {
							highestValueFound = value;
						}
						if (value < lowestValueFound) {
							lowestValueFound = value;
						}
					}
				}
				if (highestValueFound != vNumber) {
					log << EMsgDC(SpecifiedNumberOfVectorValuesDoesNotMatchActualValuesInVector)
						<< " for Attribute ";
					writeTagNumberAndNameToLog(aNumber,list.getDictionary(),log);
					log << ", which has value " << dec << vNumber
						<< ", whereas the highest value found in ";
					writeTagNumberAndNameToLog(aVector,list.getDictionary(),log);
					log << " is " << dec << highestValueFound
						<< endl;
					success = false;
				}
				if (lowestValueFound != 1) {
					log << EMsgDC(LowestValueInVectorIsNotOne)
						<< " for Attribute ";
					writeTagNumberAndNameToLog(aVector,list.getDictionary(),log);
					log << ", but rather is " << dec << lowestValueFound
						<< endl;
					success = false;
				}
			}
		}
	}
	return success;
}


static bool
checkMetaInformationMatchesSOPInstance(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	Attribute *aMediaStorageSOPInstanceUID=list[TagFromName(MediaStorageSOPInstanceUID)];
	Attribute *aMediaStorageSOPClassUID=list[TagFromName(MediaStorageSOPClassUID)];
	Attribute *aSOPInstanceUID=list[TagFromName(SOPInstanceUID)];
	Attribute *aSOPClassUID=list[TagFromName(SOPClassUID)];
	Attribute *aDirectoryRecordSequence=list[TagFromName(DirectoryRecordSequence)];
		
	if (aMediaStorageSOPInstanceUID) {
		if (aSOPInstanceUID) {
			const char *vMediaStorageSOPInstanceUID=AttributeValue(aMediaStorageSOPInstanceUID);
			const char *vSOPInstanceUID=AttributeValue(aSOPInstanceUID);
			if (!vMediaStorageSOPInstanceUID || !vSOPInstanceUID || strcmp(vMediaStorageSOPInstanceUID,vSOPInstanceUID) != 0) {
				success=false;
				log << EMsgDC(MediaStorageSOPInstanceUIDDifferentFromSOPInstanceUID)
				    << endl;
			}
		}
		else if (!aDirectoryRecordSequence) {
			success=false;
			log << EMsgDC(MediaStorageSOPInstanceUIDButMissingSOPInstanceUIDAndNotADirectory)
			    << endl;
		}
	}

	if (aMediaStorageSOPClassUID) {
		const char *vMediaStorageSOPClassUID=AttributeValue(aMediaStorageSOPClassUID);
		if (aSOPClassUID) {
			const char *vSOPClassUID=AttributeValue(aSOPClassUID);
			if (!vMediaStorageSOPClassUID || !vSOPClassUID || strcmp(vMediaStorageSOPClassUID,vSOPClassUID) != 0) {
				success=false;
				log << EMsgDC(MediaStorageSOPClassUIDDifferentFromSOPClassUID)
				    << endl;
			}
		}
		else if (aDirectoryRecordSequence) {
			if (!vMediaStorageSOPClassUID || strcmp(vMediaStorageSOPClassUID,MediaStorageDirectoryStorageSOPClassUID) != 0) {
				success=false;
				log << EMsgDC(MediaStorageSOPClassUIDNotMediaStorageDirectoryStorageSOPClassUID)
				    << endl;
			}
		}
		else {
			success=false;
			log << EMsgDC(MediaStorageSOPClassUIDButMissingSOPClassUIDAndNotADirectory)
			    << endl;
		}
	}

	return success;
}

static bool
checkLUTDataValuesMatchSpecifiedRange(AttributeList &list,Tag descriptorTag,Tag dataTag,const char *message,TextOutputStream &log)
{
	bool success=true;
	{
		Attribute *aLUTDescriptor = list[descriptorTag];
		Attribute *aLUTData = list[dataTag];
		if (aLUTDescriptor && aLUTDescriptor->getVM() == 3) {
			Uint16 numberOfBits = 0;
			Uint16 numberOfEntries = 0;
			if (aLUTDescriptor->getValue(0,numberOfEntries)
			 && aLUTDescriptor->getValue(2,numberOfBits)) {
				Uint32 actualNumberOfEntries = (numberOfEntries == 0) ? 0x10000 : ((Uint32)numberOfEntries & 0xffff);
				Uint16 wantMaxValueMax = (Uint16)( ( ((Uint32)1) <<  numberOfBits    ) - 1 );
				Uint16 wantMaxValueMin = (Uint16)( ( ((Uint32)1) << (numberOfBits-1) )     );
//cerr << "checkLUTDataValuesMatchSpecifiedRange(): wantMaxValueMax = 0x" << hex << wantMaxValueMax << dec << endl;
//cerr << "checkLUTDataValuesMatchSpecifiedRange(): wantMaxValueMin = 0x" << hex << wantMaxValueMin << dec << endl;
				if (aLUTData) {
					bool foundValuesToCheck = false;
					Uint16 maxValue = 0;
					Uint32 nLUTData = 0;
					if (numberOfBits > 8) {
						if (aLUTData->isOtherWordNonPixel()) {
//cerr << "checkLUTDataValuesMatchSpecifiedRange(): checking > 8 bit OW" << endl;
							const Uint16 *vLUTData = NULL;
							if (aLUTData->getValue(vLUTData,nLUTData)) {
								foundValuesToCheck=true;
								for (int i=0; i<nLUTData; ++i) {
									Uint16 value = vLUTData[i];
									if (value > maxValue) maxValue = value;
								}
							}
						}
						else if (aLUTData->isNumericBinary()) {		// e.g. US VR
//cerr << "checkLUTDataValuesMatchSpecifiedRange(): checking > 8 bit " << aLUTData->getVR() << endl;
							foundValuesToCheck=true;
							nLUTData = aLUTData->getVM();
							for (int i=0; i<nLUTData; ++i) {	// very slow :(
								Uint16 value;
								if (aLUTData->getValue(i,value)) {
									if (value > maxValue) maxValue = value;
								}
							}
						}
						else {
							// what have we missed ?
							log << WMsgDC(NotCheckingLUTDataMaximum)
							    << " - " << message
							    << " - 16 bit LUT for VR " << aLUTData->getVR()
							    << endl;
						}
						if (foundValuesToCheck) {
							if (nLUTData != actualNumberOfEntries) {
								log << EMsgDC(LUTDataWrongLength)
								    << " - " << message
								    << " - LUT Descriptor number of entries = " << actualNumberOfEntries
								    << " but number of 16 bit values = " << nLUTData
								    << endl;
								success=false;
							}
						}
					}
					else {
						if (aLUTData->isOtherWordNonPixel()) {
//cerr << "checkLUTDataValuesMatchSpecifiedRange(): checking <= 8 bit OW" << endl;
							const Uint16 *vLUTData = NULL;
							if (aLUTData->getValue(vLUTData,nLUTData)) {
								foundValuesToCheck=true;
								for (int i=0; i<nLUTData; ++i) {
									Uint16 value = vLUTData[i];
									Uint16 lowValue = (value >> 8) & 0xff;
									if (lowValue > maxValue) maxValue = lowValue;
									Uint16 highValue = value & 0xff;
									if (highValue > maxValue) maxValue = highValue;
								}
							}
						}
						else if (aLUTData->isNumericBinary()) {		// e.g. US VR
//cerr << "checkLUTDataValuesMatchSpecifiedRange(): checking <= 8 bit " << aLUTData->getVR() << endl;
							foundValuesToCheck=true;
							nLUTData = aLUTData->getVM();
							for (int i=0; i<nLUTData; ++i) {	// very slow :(
								Uint16 value;
								if (aLUTData->getValue(i,value)) {
									Uint16 lowValue = (value >> 8) & 0xff;
									if (lowValue > maxValue) maxValue = lowValue;
									Uint16 highValue = value & 0xff;
									if (highValue > maxValue) maxValue = highValue;
								}
							}
						}
						else {
							// what have we missed ?
							log << WMsgDC(NotCheckingLUTDataMaximum)
							    << " - " << message
							    << " - 8 bit LUT for VR " << aLUTData->getVR()
							    << endl;
						}
						if (foundValuesToCheck) {
							if (nLUTData*2 != actualNumberOfEntries) {
								log << EMsgDC(LUTDataWrongLength)
								    << " - " << message
								    << " - LUT Descriptor number of entries = " << actualNumberOfEntries
								    << " but number of 8 bit values = " << nLUTData*2
								    << " packed into " << nLUTData << " 16 bit words"
								    << endl;
								success=false;
							}
						}
					}
					if (foundValuesToCheck) {
//cerr << "checkLUTDataValuesMatchSpecifiedRange(): maxValue = 0x" << hex << maxValue << dec << endl;
						if (maxValue < wantMaxValueMin || maxValue > wantMaxValueMax) {
							log << EMsgDC(LUTDataBad)
							    << " - " << message
							    << " - LUT Descriptor number of bits = " << numberOfBits
							    << " but maximum LUT Data value is " << hex << maxValue << dec
							    << endl;
							success=false;
						}
					}
				}
			}
		}
	}
	return success;
}

static bool
checkLUTDataValuesMatchSpecifiedRange(AttributeList &list,Tag sequenceTag,Tag descriptorTag,Tag dataTag,const char *message,TextOutputStream &log)
{
	bool success=true;
	Attribute* a = list[sequenceTag];
	if (a && a->isSequence()) {
		SequenceAttribute *sa = (SequenceAttribute *)a;
		AttributeList **itemLists;
		int nItems = sa->getLists(&itemLists);
		int i;
		for (int i=0; i<nItems; ++i) {
			AttributeList *itemList = itemLists[i];
			if (itemList) {
				if (!checkLUTDataValuesMatchSpecifiedRange(*itemList,descriptorTag,dataTag,message,log)) success = false;
			}
		}
	}
	return success;
}

static bool
checkLUTDataValuesInIconImageSequenceMatchSpecifiedRange(AttributeList &list,TextOutputStream &log)
{
	bool success=true;
	Attribute* aIconImageSequence = list[TagFromName(IconImageSequence)];
	if (aIconImageSequence && aIconImageSequence->isSequence()) {
		SequenceAttribute *sa = (SequenceAttribute *)aIconImageSequence;
		AttributeList **itemLists;
		int nItems = sa->getLists(&itemLists);
		int i;
		for (int i=0; i<nItems; ++i) {
			AttributeList *itemList = itemLists[i];
			if (itemList) {
				if (!checkLUTDataValuesMatchSpecifiedRange(*itemList,TagFromName(RedPaletteColorLookupTableDescriptor),TagFromName(RedPaletteColorLookupTableData),
					"Icon Image Sequence - Red Palette Color LUT",log)) success = false;
				if (!checkLUTDataValuesMatchSpecifiedRange(*itemList,TagFromName(GreenPaletteColorLookupTableDescriptor),TagFromName(GreenPaletteColorLookupTableData),
					"Icon Image Sequence - Green Palette Color LUT",log)) success = false;
				if (!checkLUTDataValuesMatchSpecifiedRange(*itemList,TagFromName(BluePaletteColorLookupTableDescriptor),TagFromName(BluePaletteColorLookupTableData),
					"Icon Image Sequence - Blue Palette Color LUT",log)) success = false;
			}
		}
	}
	return success;
}

static bool
checkLUTDataValuesMatchSpecifiedRange(AttributeList &list,TextOutputStream &log)
{
	bool success = true;
	if (!checkLUTDataValuesMatchSpecifiedRange(list,TagFromName(ModalityLUTSequence),TagFromName(LUTDescriptor),TagFromName(LUTData),"Modality LUT",log)) success = false;
	if (!checkLUTDataValuesMatchSpecifiedRange(list,TagFromName(VOILUTSequence),TagFromName(LUTDescriptor),TagFromName(LUTData),"VOI LUT",log)) success = false;
	if (!checkLUTDataValuesMatchSpecifiedRange(list,TagFromName(PresentationLUTSequence),TagFromName(LUTDescriptor),TagFromName(LUTData),"Presentation LUT",log)) success = false;
	
	Attribute* aSoftcopyVOILUTSequence = list[TagFromName(SoftcopyVOILUTSequence)];
	if (aSoftcopyVOILUTSequence && aSoftcopyVOILUTSequence->isSequence()) {
		SequenceAttribute *sa = (SequenceAttribute *)aSoftcopyVOILUTSequence;
		AttributeList **itemLists;
		int nItems = sa->getLists(&itemLists);
		int i;
		for (int i=0; i<nItems; ++i) {
			AttributeList *itemList = itemLists[i];
			if (itemList) {
				if (!checkLUTDataValuesMatchSpecifiedRange(*itemList,TagFromName(VOILUTSequence),TagFromName(LUTDescriptor),TagFromName(LUTData),
					"Softcopy VOI LUT",log)) success = false;
			}
		}
	}

	if (!checkLUTDataValuesMatchSpecifiedRange(list,TagFromName(RedPaletteColorLookupTableDescriptor),TagFromName(RedPaletteColorLookupTableData),"Red Palette Color LUT",log)) success = false;
	if (!checkLUTDataValuesMatchSpecifiedRange(list,TagFromName(GreenPaletteColorLookupTableDescriptor),TagFromName(GreenPaletteColorLookupTableData),"Green Palette Color LUT",log)) success = false;
	if (!checkLUTDataValuesMatchSpecifiedRange(list,TagFromName(BluePaletteColorLookupTableDescriptor),TagFromName(BluePaletteColorLookupTableData),"Blue Palette Color LUT",log)) success = false;

	if (!checkLUTDataValuesInIconImageSequenceMatchSpecifiedRange(list,log)) success = false;

	Attribute* aDirectoryRecordSequence = list[TagFromName(DirectoryRecordSequence)];
	if (aDirectoryRecordSequence && aDirectoryRecordSequence->isSequence()) {
		SequenceAttribute *sa = (SequenceAttribute *)aDirectoryRecordSequence;
		AttributeList **itemLists;
		int nItems = sa->getLists(&itemLists);
		int i;
		for (int i=0; i<nItems; ++i) {
			AttributeList *itemList = itemLists[i];
			if (itemList) {
				if (!checkLUTDataValuesInIconImageSequenceMatchSpecifiedRange(*itemList,log)) success = false;
			}
		}
	}
	
	return success;
}

static bool
checkCodeMeaningsForMeasurementUnitsDoNotContainInappropriateQuotes(AttributeList &list,TextOutputStream &log) {
//cerr << "checkCodeMeaningsForMeasurementUnitsDoNotContainInappropriateQuotes():" << endl;
	bool success=true;
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (a->getTag() == TagFromName(MeasurementUnitsCodeSequence)) {
			if (a && a->isSequence() && !a->isEmpty()) {
//cerr << "checkCodeMeaningsForMeasurementUnitsDoNotContainInappropriateQuotes(): have MeasurementUnitsCodeSequence" << endl;
				AttributeList **al;
				int n;
				if ((n=a->getLists(&al)) > 0) {
//cerr << "checkCodeMeaningsForMeasurementUnitsDoNotContainInappropriateQuotes(): have " << n << " items" << endl;
					int i;
					for (i=0; i<n; ++i) {
						AttributeList *itemList = al[i];
						Attribute *aCodeMeaning=(*itemList)[TagFromName(CodeMeaning)];
//cerr << "checkCodeMeaningsForMeasurementUnitsDoNotContainInappropriateQuotes(): have CodeMeaning" << endl;
						if (aCodeMeaning) {
							Uint16 nValues= aCodeMeaning->getVM();
							if (nValues > 0) {
//cerr << "checkCodeMeaningsForMeasurementUnitsDoNotContainInappropriateQuotes(): have CodeMeaning with " << nValues << " values" << endl;
								int j;
								for (j=0; j<nValues; ++j) {
									char *value;
									if (aCodeMeaning->getValue(j,value)) {
//cerr << "checkCodeMeaningsForMeasurementUnitsDoNotContainInappropriateQuotes(): have CodeMeaning value [" <<j << "] " << value << endl;
										Uint32 length=0;
										const char *ptr=value;
										while (*ptr++) ++length;
										if (value[0] == '\'' || value[length-1] == '\''
										 || value[0] == '\"' || value[length-1] == '\"') {
											log << WMsgDC(CodeMeaningForMeasurementUnitsBeginsOrEndsWithQuotationCharacters) << " - meaning is <" << value << ">" << endl;
											//success=false;
										}
									}
								}
							}
						}
					}
					delete [] al;
				}
			}
		}
		else {
			if (!::loopOverListsInSequencesWithLog(a,log,&::checkCodeMeaningsForMeasurementUnitsDoNotContainInappropriateQuotes)) {
				success=false;
			}
		}
		++listi;
	}
	return success;
}


static bool
checkCoordinateContentItemsHaveAppropriateChildren(AttributeList &list,TextOutputStream &log) {
//cerr << "checkCoordinateContentItemsHaveAppropriateChildren():" << endl;
	bool success=true;
	{
		Attribute *aValueType = list[TagFromName(ValueType)];
		if (aValueType) {
			const char *vValueType=AttributeValue(aValueType);
			if (vValueType && (strcmp(vValueType,"SCOORD") == 0 || strcmp(vValueType,"TCOORD") == 0)) {
//cerr << "checkCoordinateContentItemsHaveAppropriateChildren(): have SCOORD" << endl;
				bool foundAppropriateChildOrIsByReferenceRelationship = false;
				const char *vChildValueType = NULL;
				Attribute *aContentSequence = list[TagFromName(ContentSequence)];
				if (aContentSequence && aContentSequence->isSequence() && !aContentSequence->isEmpty()) {
//cerr << "checkCoordinateContentItemsHaveAppropriateChildren(): have ContentSequence for children" << endl;
					AttributeList **al;
					int n;
					if ((n=aContentSequence->getLists(&al)) > 0) {
//cerr << "checkCoordinateContentItemsHaveAppropriateChildren(): have " << n << " items" << endl;
						int i;
						for (i=0; i<n; ++i) {
							AttributeList *itemList = al[i];
							Attribute *aChildValueType=(*itemList)[TagFromName(ValueType)];
							if (aChildValueType) {
								vChildValueType=AttributeValue(aChildValueType);
								if (vChildValueType &&
										(strcmp(vValueType,"SCOORD") == 0 && strcmp(vChildValueType,"IMAGE") == 0)
									 || (strcmp(vValueType,"TCOORD") == 0 && (strcmp(vChildValueType,"SCOORD") == 0 || strcmp(vChildValueType,"IMAGE") == 0 || strcmp(vChildValueType,"WAVEFORM") == 0))
									) {
//cerr << "checkCoordinateContentItemsHaveAppropriateChildren(): foundAppropriateChild " << vChildValueType << endl;
									foundAppropriateChildOrIsByReferenceRelationship = true;
								}
							}
							else {
//cerr << "checkCoordinateContentItemsHaveAppropriateChildren(): no child ValueType" << endl;
								Attribute *aReferencedContentItemIdentifier=(*itemList)[TagFromName(ReferencedContentItemIdentifier)];
								if (aReferencedContentItemIdentifier) {
//cerr << "checkCoordinateContentItemsHaveAppropriateChildren(): have ReferencedContentItemIdentifier" << endl;
									if (aReferencedContentItemIdentifier->getVM() > 0) {
//cerr << "checkCoordinateContentItemsHaveAppropriateChildren(): found by-Reference Relationship" << endl;
										foundAppropriateChildOrIsByReferenceRelationship = true;
										// too hard to actually check if target of reference is legitimate :(
									}
								}
							}
							
							bool foundAppropriateRelationship = false;
							const char *vRelationshipType = NULL;
							Attribute *aRelationshipType=(*itemList)[TagFromName(RelationshipType)];
							if (aRelationshipType) {
								vRelationshipType=AttributeValue(aRelationshipType);
//cerr << "checkCoordinateContentItemsHaveAppropriateChildren(): foundAppropriateRelationship <" << vRelationshipType << ">" << endl;
								if (vRelationshipType && strcmp(vRelationshipType,"SELECTED FROM") == 0) {
									foundAppropriateRelationship = true;
								}
							}
							if (!foundAppropriateRelationship) {
								log << EMsgDC(CoordinatesContentItemHasIncorrectRelationshipChildContentItem) << " for " << vValueType
									<< " got " << (vRelationshipType ? vRelationshipType : "no relationship") << " - expected SELECTED FROM" << endl;
								success=false;
							}
						}
						delete [] al;
					}
				}
				if (!foundAppropriateChildOrIsByReferenceRelationship) {
					log << EMsgDC(CoordinatesContentItemMissingOrIncorrectRequiredChildContentItem) << " for " << vValueType
						<< " got " << (vChildValueType ? vChildValueType : "no child")
						<< " expected " << (strcmp(vValueType,"SCOORD") == 0 ? "IMAGE" : "SCOORD, IMAGE or WAVEFORM")
						<< endl;
					success=false;
				}
			}
		}
	}
	
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithLog(a,log,&::checkCoordinateContentItemsHaveAppropriateChildren)) {
			success=false;
		}
		++listi;
	}
	return success;
}

static AttributeList *
findInstanceInHierarchicalEvidenceSequence(Attribute *aEvidenceSequence,const char *vSOPInstanceUID) {
//cerr << "findInstanceInHierarchicalEvidenceSequences():" << endl;
	if (vSOPInstanceUID && aEvidenceSequence && aEvidenceSequence->isSequence() && !aEvidenceSequence->isEmpty()) {
		AttributeList **alstudy;
		int nstudy;
		if ((nstudy=aEvidenceSequence->getLists(&alstudy)) > 0) {
//cerr << "findInstanceInHierarchicalEvidenceSequences(): have " << nstudy << " EvidenceSequence items" << endl;
			int istudy;
			for (istudy=0; istudy<nstudy; ++istudy) {
				AttributeList *itemListStudy = alstudy[istudy];
				Attribute *aReferencedSeriesSequence=(*itemListStudy)[TagFromName(ReferencedSeriesSequence)];
				if (aReferencedSeriesSequence && aReferencedSeriesSequence->isSequence() && !aReferencedSeriesSequence->isEmpty()) {
					AttributeList **alseries;
					int nseries;
					if ((nseries=aReferencedSeriesSequence->getLists(&alseries)) > 0) {
//cerr << "findInstanceInHierarchicalEvidenceSequences(): have " << nseries << " ReferencedSeriesSequence items" << endl;
						int iseries;
						for (iseries=0; iseries<nseries; ++iseries) {
							AttributeList *itemListSeries = alseries[iseries];
							Attribute *aReferencedSOPSequence=(*itemListSeries)[TagFromName(ReferencedSOPSequence)];
							if (aReferencedSOPSequence && aReferencedSOPSequence->isSequence() && !aReferencedSOPSequence->isEmpty()) {
								AttributeList **alinstance;
								int ninstance;
								if ((ninstance=aReferencedSOPSequence->getLists(&alinstance)) > 0) {
//cerr << "findInstanceInHierarchicalEvidenceSequences(): have " << ninstance << " ReferencedSOPSequence items" << endl;
									int iinstance;
									for (iinstance=0; iinstance<ninstance; ++iinstance) {
										AttributeList *itemListInstance = alinstance[iinstance];
										Attribute *aReferencedSOPInstanceUID=(*itemListInstance)[TagFromName(ReferencedSOPInstanceUID)];
										if (aReferencedSOPInstanceUID) {
											const char *vReferencedSOPInstanceUID=AttributeValue(aReferencedSOPInstanceUID);
//cerr << "findInstanceInHierarchicalEvidenceSequences(): have ReferencedSOPInstanceUID " << vReferencedSOPInstanceUID << endl;
											if (vReferencedSOPInstanceUID && strcmp(vSOPInstanceUID,vReferencedSOPInstanceUID) == 0) {
//cerr << "findInstanceInHierarchicalEvidenceSequences(): found match ReferencedSOPInstanceUID " << vReferencedSOPInstanceUID << endl;
												return itemListInstance;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}

static bool
findInstanceInHierarchicalEvidenceSequences(AttributeList &list,const char *vSOPInstanceUID,const char *vSOPClassUID,const char *vValueType,const char *subType,TextOutputStream &log) {
//cerr << "findInstanceInHierarchicalEvidenceSequences():" << endl;
	bool success = false;
	AttributeList *foundList = NULL;
	Attribute *aCurrentRequestedProcedureEvidenceSequence = list[TagFromName(CurrentRequestedProcedureEvidenceSequence)];
	Attribute *aPertinentOtherEvidenceSequence = list[TagFromName(PertinentOtherEvidenceSequence)];
	if (aCurrentRequestedProcedureEvidenceSequence) {
//cerr << "findInstanceInHierarchicalEvidenceSequences(): have CurrentRequestedProcedureEvidenceSequence" << endl;
		foundList = findInstanceInHierarchicalEvidenceSequence(aCurrentRequestedProcedureEvidenceSequence,vSOPInstanceUID);
	}
	if (!foundList && aPertinentOtherEvidenceSequence) {
//cerr << "findInstanceInHierarchicalEvidenceSequences(): have PertinentOtherEvidenceSequence" << endl;
		foundList = findInstanceInHierarchicalEvidenceSequence(aPertinentOtherEvidenceSequence,vSOPInstanceUID);
	}
	if (foundList) {
		success	= true;
		Attribute *aReferencedSOPClassUID=(*foundList)[TagFromName(ReferencedSOPClassUID)];
		if (aReferencedSOPClassUID) {
			const char *vReferencedSOPClassUID=AttributeValue(aReferencedSOPClassUID);
//cerr << "findInstanceInHierarchicalEvidenceSequences(): have vReferencedSOPClassUID " << vReferencedSOPClassUID << endl;
			if (vReferencedSOPClassUID && strcmp(vSOPClassUID,vReferencedSOPClassUID) == 0) {
//cerr << "findInstanceInHierarchicalEvidenceSequences(): found match ReferencedSOPClassUID " << vSOPClassUID << endl;
			}
			else {
				log << EMsgDC(SOPClassInCurrentRequestedProcedureOrPertinentOtherEvidenceSequenceDoesNotMatchReference)
					<< " for " << vValueType << " " << (subType ? " " : "") << (subType ? subType : "") << " ReferencedSOPInstanceUID " << (vSOPInstanceUID ? vSOPInstanceUID : "missing or empty SOPInstanceUID")
					<< " " << vValueType << " ReferencedSOPClassUID is " << vSOPClassUID << " but evidence ReferencedSOPClassUID is " << vReferencedSOPClassUID
					<< endl;
				success	= false;
			}
		}
	}
	else {
		log << EMsgDC(NotListedInCurrentRequestedProcedureOrPertinentOtherEvidenceSequence)
			<< " but have " << vValueType << (subType ? " " : "") << (subType ? subType : "") << " ReferencedSOPInstanceUID " << (vSOPInstanceUID ? vSOPInstanceUID : "missing or empty SOPInstanceUID") << endl;
	}
	return success;
}

static bool
checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences(AttributeList &rootlist,AttributeList &list,TextOutputStream &log) {
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences():" << endl;
	bool success=true;

	{
		Attribute *aValueType = list[TagFromName(ValueType)];
		if (aValueType) {
			const char *vValueType=AttributeValue(aValueType);
			if (vValueType && (strcmp(vValueType,"IMAGE") == 0 || strcmp(vValueType,"COMPOSITE") == 0 || strcmp(vValueType,"WAVEFORM") == 0)) {
				const char *vReferencedSOPInstanceUID = NULL;
				const char *vReferencedSOPClassUID = NULL;
				const char *vPresentationStateReferencedSOPInstanceUID = NULL;
				const char *vPresentationStateReferencedSOPClassUID = NULL;
				const char *vRealWorldValueReferencedSOPInstanceUID = NULL;
				const char *vRealWorldValueReferencedSOPClassUID = NULL;
				Attribute *aReferencedSOPSequence=list[TagFromName(ReferencedSOPSequence)];
				if (aReferencedSOPSequence && aReferencedSOPSequence->isSequence() && !aReferencedSOPSequence->isEmpty()) {
					AttributeList **alinstance;
					int ninstance;
					if ((ninstance=aReferencedSOPSequence->getLists(&alinstance)) > 0) {
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences(): in " <<vValueType  << " have " << ninstance << " ReferencedSOPSequence items" << endl;
						int iinstance;
						for (iinstance=0; iinstance<ninstance; ++iinstance) {
							AttributeList *itemListInstance = alinstance[iinstance];
							Attribute *aReferencedSOPInstanceUID=(*itemListInstance)[TagFromName(ReferencedSOPInstanceUID)];
							if (aReferencedSOPInstanceUID) {
								vReferencedSOPInstanceUID=AttributeValue(aReferencedSOPInstanceUID);
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences():  in " << vValueType  << " have ReferencedSOPInstanceUID " << vReferencedSOPInstanceUID << endl;
							}
							Attribute *aReferencedSOPClassUID=(*itemListInstance)[TagFromName(ReferencedSOPClassUID)];
							if (aReferencedSOPClassUID) {
								vReferencedSOPClassUID=AttributeValue(aReferencedSOPClassUID);
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences():  in " << vValueType  << " have ReferencedSOPClassUID " << vReferencedSOPClassUID << endl;
							}
							
							Attribute *aPresentationStateReferencedSOPSequence=(*itemListInstance)[TagFromName(ReferencedSOPSequence)];
							if (aPresentationStateReferencedSOPSequence && aPresentationStateReferencedSOPSequence->isSequence() && !aPresentationStateReferencedSOPSequence->isEmpty()) {
								AttributeList **alpresentationstate;
								int npresentationstate;
								if ((npresentationstate=aPresentationStateReferencedSOPSequence->getLists(&alpresentationstate)) > 0) {
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences(): in " << vValueType  << " have " << npresentationstate << " ReferencedSOPSequence items" << endl;
									int ipresentationstate;
									for (ipresentationstate=0; ipresentationstate<npresentationstate; ++ipresentationstate) {
										AttributeList *itemListPresentationStateInstance = alpresentationstate[ipresentationstate];
										Attribute *aPresentationStateReferencedSOPInstanceUID=(*itemListPresentationStateInstance)[TagFromName(ReferencedSOPInstanceUID)];
										if (aPresentationStateReferencedSOPInstanceUID) {
											vPresentationStateReferencedSOPInstanceUID=AttributeValue(aPresentationStateReferencedSOPInstanceUID);
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences():  in " << vValueType  << " have Presentation State ReferencedSOPInstanceUID " << vPresentationStateReferencedSOPInstanceUID << endl;
										}
										Attribute *aPresentationStateReferencedSOPClassUID=(*itemListPresentationStateInstance)[TagFromName(ReferencedSOPClassUID)];
										if (aPresentationStateReferencedSOPClassUID) {
											vPresentationStateReferencedSOPClassUID=AttributeValue(aPresentationStateReferencedSOPClassUID);
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences():  in " <<vValueType  << " have Presentation State ReferencedSOPClassUID " << vPresentationStateReferencedSOPClassUID << endl;
										}
									}
								}
							}
							
							Attribute *aReferencedRealWorldValueMappingInstanceSequence=(*itemListInstance)[TagFromName(ReferencedRealWorldValueMappingInstanceSequence)];
							if (aReferencedRealWorldValueMappingInstanceSequence && aReferencedRealWorldValueMappingInstanceSequence->isSequence() && !aReferencedRealWorldValueMappingInstanceSequence->isEmpty()) {
								AttributeList **alRealWorldValue;
								int nRealWorldValue;
								if ((nRealWorldValue=aReferencedRealWorldValueMappingInstanceSequence->getLists(&alRealWorldValue)) > 0) {
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences(): in " << vValueType  << " have " << nRealWorldValue << " ReferencedRealWorldValueMappingInstanceSequence items" << endl;
									int iRealWorldValue;
									for (iRealWorldValue=0; iRealWorldValue<nRealWorldValue; ++iRealWorldValue) {
										AttributeList *itemListRealWorldValue = alRealWorldValue[iRealWorldValue];
										Attribute *aRealWorldValueReferencedSOPInstanceUID=(*itemListRealWorldValue)[TagFromName(ReferencedSOPInstanceUID)];
										if (aRealWorldValueReferencedSOPInstanceUID) {
											vRealWorldValueReferencedSOPInstanceUID=AttributeValue(aRealWorldValueReferencedSOPInstanceUID);
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences():  in " << vValueType  << " have Real World Value Mapping ReferencedSOPInstanceUID " << vRealWorldValueReferencedSOPInstanceUID << endl;
										}
										Attribute *aRealWorldValueReferencedSOPClassUID=(*itemListRealWorldValue)[TagFromName(ReferencedSOPClassUID)];
										if (aRealWorldValueReferencedSOPClassUID) {
											vRealWorldValueReferencedSOPClassUID=AttributeValue(aRealWorldValueReferencedSOPClassUID);
//cerr << "checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences():  in " <<vValueType  << " have Real World Value Mapping ReferencedSOPClassUID " << vRealWorldValueReferencedSOPClassUID << endl;
										}
									}
								}
							}
							
						}
					}
				}
				
				if (!findInstanceInHierarchicalEvidenceSequences(rootlist,vReferencedSOPInstanceUID,vReferencedSOPClassUID,vValueType,NULL,log)) {
					success=false;
				}
				// presentation state is optional, so do not check if no referenced
				if (vPresentationStateReferencedSOPInstanceUID && !findInstanceInHierarchicalEvidenceSequences(rootlist,vPresentationStateReferencedSOPInstanceUID,vPresentationStateReferencedSOPClassUID,vValueType,"PresentationState",log)) {
					success=false;
				}
				// real world value mapping is optional, so do not check if no referenced
				if (vRealWorldValueReferencedSOPInstanceUID && !findInstanceInHierarchicalEvidenceSequences(rootlist,vRealWorldValueReferencedSOPInstanceUID,vRealWorldValueReferencedSOPClassUID,vValueType,"RealWorldValueMapping",log)) {
					success=false;
				}
			}
		}
	}

	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequencesWithRootListAndLog(rootlist,a,log,&::checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences)) {
			success=false;
		}
		++listi;
	}
	return success;
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool describe=options.get("describe");
	bool dump=options.get("dump");
	bool showfilename=options.get("filename");
	
	const char *profile = NULL;
	(void)(options.get("profile",profile));

	bool bad=false;

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
			<< " [-profile profilename]"
			<< " [-describe]"
			<< " [-dump]"
			<< " [-filename]"
			<< " [-v|-verbose]"
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
	TextOutputStream log(cerr);

	if (showfilename) {
		const char *filenameused = input_opener.getFilename();
		log << "Filename: \"" << (filenameused && strlen(filenameused) > 0 ? filenameused : "-") << "\"" << endl;
	}
	
	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	const char *errors=list.errors();
	if (errors) log << errors << flush;
	if (!list.good()) {
		log << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	
	// do not && each of these with success, else no function gets called after first failure !
	
	if (!checkMetaInformationMatchesSOPInstance(list,log)) success = false;
	
	if (!checkPixelDataIsTheCorrectLength(list,log)) success = false;
	
	if (!checkWaveformSequenceIsInternallyConsistent(list,log)) success = false;
	
	if (!checkNoIllegalOddNumberedGroups(list,log)) success = false;
	
	if (!checkLUTDataValuesMatchSpecifiedRange(list,log)) success = false;
	
	if (!checkNoEmptyReferencedFileIDComponents(list,log)) success = false;
	
	if (!checkFrameIncrementPointerValuesValid(list,log)) success = false;
	
	if (!checkFrameVectorCountsValid(list,log)) success = false;
	
	if (!checkPixelAspectRatioValidIfPresent(list,log)) success = false;
	
	if (!checkEstimatedRadiographicMagnificationFactorIfPresent(list,log)) success = false;
	
	if (!checkPixelSpacingCalibration(list,log)) success = false;
	
	if (!checkOrientationsAreUnitVectors(list,log)) success = false;
	
	if (!checkOrientationsAreOrthogonal(list,log)) success = false;
	
	if (!checkPatientOrientationValuesForBipedOrQuadruped(list,log)) success = false;
	
	if (!checkScaledNumericValues(list,log)) success = false;
	
	if (!checkSpacingBetweenSlicesIsNotNegative(list,log)) success = false;
	
	if (!checkUIDs(list,log)) success = false;

	if (!checkPresentationStateDisplayedAreaSelectionValuesAreValid(list,log)) success = false;
	
	if (!checkCodeMeaningsForMeasurementUnitsDoNotContainInappropriateQuotes(list,log)) success = false;
	
	if (!checkCoordinateContentItemsHaveAppropriateChildren(list,log)) success = false;
	
	if (!checkInstanceReferencesAreIncludedInHierarchicalEvidenceSequences(list,list,log)) success = false;
	
	checkValuesNeededToBuildDicomDirectoryArePresentAndNotEmpty(list,log);	// always only warnings ... do not affect success
	
	if (verbose) log << "success after manual checks " << (success ? "success" : "failure") << endl;

	if (!list.validateVR(log)) {
		log << EMsgDC(DataSetContainsInvalidValuesForVR)
		    << endl;
		success=false;
	}
	if (verbose) log << "success after validateVR " << (success ? "success" : "failure") << endl;

	if (!list.validateRetired(log)) {
		log << WMsgDC(DataSetContainsRetiredAttributes)
		    << endl;
	}

	CompositeIOD *iod = selectCompositeIOD(&list,profile);
	if (iod) {
		log << iod->identify() << endl;
		if (iod->retired()) {
		log << WMsgDC(InformationObject)
		    << " "
		    << MMsgDC(Retired)
		    << endl;
		}
		if (!iod->verify(&list,verbose,log,list.getDictionary())) success=false;
		if (describe || verbose) iod->write(log,&list,list.getDictionary());
	}
	else {
		log << EMsgDC(InformationObject)
		    << " "
		    << MMsgDC(NotFound)
		    << endl;
		success=false;
	}
	if (verbose) log << "success after iod verify " << (success ? "success" : "failure") << endl;

	if (iod && !list.validateUsed(log)) {
		log << WMsgDC(DataSetContainsAttributesNotUsedInIOD)
		    << endl;
	}
	
	if (dump) {
		list.write(log,verbose,true/*showUsedAndIE*/);
	}
	if (verbose) log << "success at end " << (success ? "success" : "failure") << endl;
	return success ? 0 : 1;
}

