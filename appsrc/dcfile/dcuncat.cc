#include <fstream.h>

#include "attrmxls.h"
#include "attrtype.h"
#include "attrothr.h"
#include "attrval.h"
#include "attrseq.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "uidgen.h"
#include "sopclu.h"

// The following are copied unchanged from dcmulti.cc

// Define a funky derived OX class to be able to writeBase() and not activateSource() ...

class OtherUnspecifiedLargeAttributeDummy : public OtherUnspecifiedLargeAttributeBase {
public:
	OtherUnspecifiedLargeAttributeDummy(Tag t,
			Uint16 r,Uint16 c,Uint16 f,Uint16 sperp,
			TransferSyntax *transfersyntax,
			Uint16 bytesinword,
			Uint16 bitsallocated,
			Uint16 bitsstored,
			Uint16 highbit,
			Uint32 length=0xffffffff)
		: OtherUnspecifiedLargeAttributeBase(t,r,c,f,sperp)
		{
			srcpixeldata=0;

			srcbitsallocated=bitsallocated;
			srcbitsstored=bitsstored;
			srchighbit=highbit;

			Assert(transfersyntax);
			setOutputEncoding(
				transfersyntax,
				bytesinword,bitsallocated,
				bitsstored,highbit,length);
		}

	virtual ~OtherUnspecifiedLargeAttributeDummy()
		{
		}

	bool activateSource(void)
		{
			Assert(0);	// only here to make concrete ... pixel data is actually written manually
			return true;
		}

	bool activateSourceWithoutUnpacking(void)
		{
			Assert(0);	// only here to make concrete ... pixel data is actually written manually
			return true;
		}

	DicomOutputStream& writeBase(DicomOutputStream& dout)
		{
			return OtherUnspecifiedLargeAttributeBase::writeBase(dout);	//protected method
		}

	TextOutputStream& writeBase(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false)
		{
			return OtherUnspecifiedLargeAttributeBase::writeBase(stream,dict,verbose);	//protected method
		}
};

static Attribute *
isValuePresentElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Assert(label);
	Assert(filename);
	Attribute *a=list[tag];
	if (a && a->getVM())
		return a;
	else {
		log << filename << ": " 
		    << EMsgDC(MissingAttribute)
		    << " - " << label
		    << endl;
		return 0;
	}
}

static Uint16
getIntegerValueElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *a=isValuePresentElseError(list,tag,label,filename,log);
	Uint16 value=AttributeValue(a);
	return value;
}

// end of copy from dcmulti.cc

static void
removeAllPerFrameFunctionalGroupSequenceItemsExcept(AttributeList &list,SequenceAttribute *aOldPerFrameFunctionalGroupSequence,int firstFrame,int lastFrame) {
	list-=TagFromName(PerFrameFunctionalGroupsSequence);
	SequenceAttribute *aNewPerFrameFunctionalGroupSequence = new SequenceAttribute(TagFromName(PerFrameFunctionalGroupsSequence));
	list+=aNewPerFrameFunctionalGroupSequence;
	if (aOldPerFrameFunctionalGroupSequence) {
		AttributeList **oldItems;
		int nOldItems = aOldPerFrameFunctionalGroupSequence->getLists(&oldItems);
		if (nOldItems) {
			Assert(firstFrame >= 0);		// firstFrame,lastFrame begin at 0, not 1
			Assert(firstFrame <= lastFrame);
			Assert(lastFrame < nOldItems);
			Assert(oldItems);
			int i;
			for (i=firstFrame; i<=lastFrame; ++i) {
				Assert(oldItems[i]);
				(*aNewPerFrameFunctionalGroupSequence)+=oldItems[i];
			}
		}
	}
}

static void addFunctionalGroupContentsToTopLevel(AttributeList &list,AttributeList *functionalGroupAttributeList,bool noPrivateSequences,bool noPrivateAttributes) {
	if (functionalGroupAttributeList) {
		AttributeListIterator functionalGroupAttributeListIterator(*functionalGroupAttributeList);
		while (!functionalGroupAttributeListIterator) {
			Attribute *functionalGroupSequenceAttribute = functionalGroupAttributeListIterator();
			if (functionalGroupSequenceAttribute && strcmp(functionalGroupSequenceAttribute->getVR(),"SQ") == 0 && (!functionalGroupSequenceAttribute->getTag().isPrivateGroup() || !noPrivateSequences)) {
				AttributeList **functionalGroupSequenceAttributeItems;
				int nFunctionalGroupSequenceAttributeItems = functionalGroupSequenceAttribute->getLists(&functionalGroupSequenceAttributeItems);
				if (nFunctionalGroupSequenceAttributeItems > 0) {
					// we will only do the first item (there is normally only one), since can't twice to the top level dataset
					AttributeList *functionalGroupSequenceAttributeFirstItemContentList = functionalGroupSequenceAttributeItems[0];
					if (functionalGroupSequenceAttributeFirstItemContentList) {
						AttributeListIterator functionalGroupSequenceAttributeFirstItemContentListIterator(*functionalGroupSequenceAttributeFirstItemContentList);
						while (!functionalGroupSequenceAttributeFirstItemContentListIterator) {
							Attribute *a = functionalGroupSequenceAttributeFirstItemContentListIterator();
							if (a && (!a->getTag().isPrivateGroup() || !noPrivateAttributes)) {
								list-=a->getTag();	// remove any previous instance
								list+=a;
							}
							++functionalGroupSequenceAttributeFirstItemContentListIterator;
						}
					}
				}
			}
			++functionalGroupAttributeListIterator;
		}
	}
}

static void
moveRelevantPerFrameAndSharedFunctionalGroupSequenceItemsContentsToTopLevel(AttributeList &list,SequenceAttribute *aPerFrameFunctionalGroupSequence,SequenceAttribute *aSharedFunctionalGroupsSequence,int frameNumber,bool noPrivateSequences,bool noPrivateAttributes) {
	if (aPerFrameFunctionalGroupSequence) {
		list-=TagFromName(PerFrameFunctionalGroupsSequence);
		AttributeList **items;
		int nItems = aPerFrameFunctionalGroupSequence->getLists(&items);
		if (nItems) {
			Assert(frameNumber >= 0);		// frameNumber begins at 0, not 1
			Assert(frameNumber < nItems);
			Assert(items);
			addFunctionalGroupContentsToTopLevel(list,items[frameNumber],noPrivateSequences,noPrivateAttributes);
		}
	}

	if (aSharedFunctionalGroupsSequence) {
		list-=TagFromName(SharedFunctionalGroupsSequence);
		AttributeList **items;
		int nItems = aSharedFunctionalGroupsSequence->getLists(&items);
		if (nItems) {
			Assert(nItems == 1);
			Assert(items);
			addFunctionalGroupContentsToTopLevel(list,items[0],noPrivateSequences,noPrivateAttributes);
		}
	}
}

static void
changeSOPClassToUnenhanced(AttributeList &list) {
	char *sopClassUID = AttributeValue(list[TagFromName(SOPClassUID)]);
	const char *newSOPClassUID = sopClassUID;
	if (sopClassUID) {
		if (strcmp(sopClassUID,EnhancedCTImageStorageSOPClassUID) == 0) {
			newSOPClassUID = CTImageStorageSOPClassUID;
		}
		else if (strcmp(sopClassUID,EnhancedMRImageStorageSOPClassUID) == 0) {
			newSOPClassUID = MRImageStorageSOPClassUID;
		}
	}
	if (newSOPClassUID) {
		list-=TagFromName(SOPClassUID);
		list+=new UIStringAttribute(TagFromName(SOPClassUID),newSOPClassUID);

	}
}

static bool
isCT(AttributeList &list) {
	char *sopClassUID = AttributeValue(list[TagFromName(SOPClassUID)]);
	return sopClassUID && (strcmp(sopClassUID,CTImageStorageSOPClassUID) == 0 || strcmp(sopClassUID,EnhancedCTImageStorageSOPClassUID) == 0);
}

static bool
isMR(AttributeList &list) {
	char *sopClassUID = AttributeValue(list[TagFromName(SOPClassUID)]);
	return sopClassUID && (strcmp(sopClassUID,MRImageStorageSOPClassUID) == 0 || strcmp(sopClassUID,EnhancedMRImageStorageSOPClassUID) == 0);
}

static bool
isMultiFrameSOPClass(AttributeList &list) {
	char *sopClassUID = AttributeValue(list[TagFromName(SOPClassUID)]);
	return sopClassUID && (strcmp(sopClassUID,UltrasoundMultiframeImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,EnhancedUSVolumeStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,MultiframeSingleBitSecondaryCaptureImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,MultiframeGrayscaleByteSecondaryCaptureImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,MultiframeGrayscaleWordSecondaryCaptureImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,MultiframeTrueColorSecondaryCaptureImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,XRayAngiographicImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,EnhancedXAImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,XRayRadioFluoroscopicImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,EnhancedXRFImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,XRayAngiographicBiplaneImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,XRay3DAngiographicImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,XRay3DCraniofacialImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,BreastTomosynthesisImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,NuclearMedicineImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,VideoEndoscopicImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,VideoMicroscopicImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,VideoPhotographicImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,OphthalmicPhotography8BitImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,OphthalmicPhotography16BitImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,VLWholeSlideMicroscopyImageStorageSOPClassUID) == 0
						|| strcmp(sopClassUID,RTImageStorageSOPClassUID) == 0
			);
}

static void
fixupUnenhanced(AttributeList &list) {
	if (isMR(list)) {
		{
			Attribute *aScanningSequence = list[TagFromName(ScanningSequence)];
			if (!aScanningSequence) {
				aScanningSequence=new CodeStringAttribute(TagFromName(ScanningSequence));
				Assert(aScanningSequence);
			
				char *echoPulseSequence = AttributeValue(list[TagFromName(EchoPulseSequence)]);
				if (echoPulseSequence) {
					if (strcmp(echoPulseSequence,"SPIN") == 0) {
						aScanningSequence->addValue("SE");
					}
					else if (strcmp(echoPulseSequence,"GRADIENT") == 0) {
						aScanningSequence->addValue("GR");
					}
					else if (strcmp(echoPulseSequence,"BOTH") == 0) {
						aScanningSequence->addValue("SE");
						aScanningSequence->addValue("GR");
					}
				}
			
				char *echoPlanarPulseSequence = AttributeValue(list[TagFromName(EchoPlanarPulseSequence)]);
				if (echoPlanarPulseSequence) {
					if (strcmp(echoPlanarPulseSequence,"YES") == 0) {
						aScanningSequence->addValue("EP");
					}
				}
			
				char *inversionRecovery = AttributeValue(list[TagFromName(InversionRecovery)]);
				if (inversionRecovery) {
					if (strcmp(inversionRecovery,"YES") == 0) {
						aScanningSequence->addValue("IR");
					}
				}
			
				char *contentQualification = AttributeValue(list[TagFromName(ContentQualification)]);
				if (contentQualification) {
					if (strcmp(contentQualification,"RESEARCH") == 0) {
						aScanningSequence->addValue("RM");
					}
				}
			
				list+=aScanningSequence;
			}
		}
		{
			Attribute *aSequenceVariant = list[TagFromName(SequenceVariant)];
			if (!aSequenceVariant) {
				aSequenceVariant=new CodeStringAttribute(TagFromName(SequenceVariant));
				Assert(aSequenceVariant);

				char *steadyStatePulseSequence = AttributeValue(list[TagFromName(SteadyStatePulseSequence)]);
				if (steadyStatePulseSequence) {
					if (strcmp(steadyStatePulseSequence,"TIME_REVERSED") == 0) {
						aSequenceVariant->addValue("TRSS");
					}
					else if (strcmp(steadyStatePulseSequence,"NONE") != 0) {
						aSequenceVariant->addValue("SS");
					}
				}
			
				char *segmentedKSpaceTraversal = AttributeValue(list[TagFromName(SegmentedKSpaceTraversal)]);
				if (segmentedKSpaceTraversal) {
					if (strcmp(segmentedKSpaceTraversal,"PARTIAL") == 0) {
						aSequenceVariant->addValue("SK");
					}
				}
			
				char *magnetizationTransfer = AttributeValue(list[TagFromName(MagnetizationTransfer)]);
				if (magnetizationTransfer) {
					if (strcmp(magnetizationTransfer,"NONE") != 0) {
						aSequenceVariant->addValue("MTC");
					}
				}
			
				char *spoiling = AttributeValue(list[TagFromName(Spoiling)]);
				if (spoiling) {
					if (strcmp(spoiling,"NONE") != 0) {
						aSequenceVariant->addValue("SP");
					}
				}
			
				char *t2Preparation = AttributeValue(list[TagFromName(T2Preparation)]);
				if (t2Preparation) {
					if (strcmp(t2Preparation,"NO") != 0) {
						aSequenceVariant->addValue("MP");
					}
				}
			
				char *oversamplingPhase = AttributeValue(list[TagFromName(OversamplingPhase)]);
				if (oversamplingPhase) {
					if (strcmp(oversamplingPhase,"NONE") != 0) {
						aSequenceVariant->addValue("OSP");
					}
				}
			
				if (aSequenceVariant->getVM() == 0) {
					aSequenceVariant->addValue("NONE");
				}

				list+=aSequenceVariant;
			}
		}
		{
			Attribute *aScanOptions = list[TagFromName(ScanOptions)];
			if (!aScanOptions) {
				aScanOptions=new CodeStringAttribute(TagFromName(ScanOptions));
			
				char *rectilinearPhaseEncodeReordering = AttributeValue(list[TagFromName(RectilinearPhaseEncodeReordering)]);
//cerr << "rectilinearPhaseEncodeReordering = " << rectilinearPhaseEncodeReordering << endl;
				if (rectilinearPhaseEncodeReordering) {
					if (!(strcmp(rectilinearPhaseEncodeReordering,"UNKNOWN") == 0 || strcmp(rectilinearPhaseEncodeReordering,"NONE") == 0)) {	// neither of these are defined terms in the standard, but type 1 :(
						aScanOptions->addValue("PER");
					}
				}
				
				char *respiratoryMotionCompensationTechnique = AttributeValue(list[TagFromName(RespiratoryMotionCompensationTechnique)]);
				if (respiratoryMotionCompensationTechnique) {
					if (strcmp(respiratoryMotionCompensationTechnique,"NONE") != 0) {
						aScanOptions->addValue("RG");
					}
				}
				
				char *cardiacSynchronizationTechnique = AttributeValue(list[TagFromName(CardiacSynchronizationTechnique)]);
				if (cardiacSynchronizationTechnique) {
					if (strcmp(cardiacSynchronizationTechnique,"NONE") != 0) {
						char *cardiacSignalSource = AttributeValue(list[TagFromName(CardiacSignalSource)]);
						if (cardiacSignalSource && strcmp(cardiacSignalSource,"PP") == 0) {
							aScanOptions->addValue("PPG");
						}
						else {
							aScanOptions->addValue("CG");
						}
					}
				}
	
				char *flowCompensation = AttributeValue(list[TagFromName(FlowCompensation)]);
				if (flowCompensation) {
					if (strcmp(flowCompensation,"NONE") != 0) {
						aScanOptions->addValue("FC");
					}
				}
				
				char *partialFourier = AttributeValue(list[TagFromName(PartialFourier)]);
				if (partialFourier && strcmp(partialFourier,"YES") == 0) {
					char *partialFourierDirection = AttributeValue(list[TagFromName(PartialFourierDirection)]);		// may be present even if PartialFourier == NO, so check first
					if (partialFourierDirection) {
						if (strcmp(partialFourierDirection,"FREQUENCY") == 0) {
							aScanOptions->addValue("PFF");
						}
						else if (strcmp(partialFourierDirection,"PHASE") == 0) {
							aScanOptions->addValue("PFP");
						}
						else if (strcmp(partialFourierDirection,"COMBINATION") == 0) {
							aScanOptions->addValue("PFF");
							aScanOptions->addValue("PFP");
						}
					}
				}
				
				char *spatialPresaturation = AttributeValue(list[TagFromName(SpatialPresaturation)]);
				if (spatialPresaturation) {
					if (strcmp(spatialPresaturation,"NONE") != 0) {
						aScanOptions->addValue("SP");
					}
				}
	
				char *spectrallySelectedSuppression = AttributeValue(list[TagFromName(SpectrallySelectedSuppression)]);
				if (spectrallySelectedSuppression) {
					if (strcmp(spectrallySelectedSuppression,"FAT") == 0) {		// there are many other defined terms for other substances and combinations :(
						aScanOptions->addValue("FS");
					}
				}
	
				list+=aScanOptions;
			}
		}
		{
			char *inPlanePhaseEncodingDirection = AttributeValue(list[TagFromName(InPlanePhaseEncodingDirection)]);
			if (inPlanePhaseEncodingDirection && strcmp(inPlanePhaseEncodingDirection,"COLUMN") == 0) {
				list-=TagFromName(InPlanePhaseEncodingDirection);
				list+=new CodeStringAttribute(TagFromName(InPlanePhaseEncodingDirection),"COL");	// gratuitious difference between old and new MR objects that is too late to fix with a CP. since enumerated values :(
			}
		}
		{
			Attribute *aEchoTime = list[TagFromName(EchoTime)];
			if (!aEchoTime) {
				Float32 effectiveEchoTime = AttributeValue(list[TagFromName(EffectiveEchoTime)]);
				list+=new DecimalStringAttribute(TagFromName(EchoTime),effectiveEchoTime);
			}
		}
		{
			Attribute *aInversionTime = list[TagFromName(InversionTime)];
			if (!aInversionTime) {
				Attribute *aInversionTimes = list[TagFromName(InversionTimes)];
				if (aInversionTimes && aInversionTimes->getVM() > 0) {
					Float32 inversionTime = AttributeValue(aInversionTimes);		// use 1st value only, even if multiple
					list+=new DecimalStringAttribute(TagFromName(InversionTime),inversionTime);
				}
			}
		}
		{
			Attribute *aTriggerTime = list[TagFromName(TriggerTime)];
			if (!aTriggerTime) {
				Attribute *aNominalCardiacTriggerDelayTime = list[TagFromName(NominalCardiacTriggerDelayTime)];
				if (aNominalCardiacTriggerDelayTime && aNominalCardiacTriggerDelayTime->getVM() > 0) {
					Float32 nominalCardiacTriggerDelayTime = AttributeValue(aNominalCardiacTriggerDelayTime);
					list+=new DecimalStringAttribute(TagFromName(TriggerTime),nominalCardiacTriggerDelayTime);
				}
			}
		}
	}
	else if (isCT(list)) {
	}
	
	if (isCT(list) || isMR(list)) {
		{
			Attribute *aSliceThickness = list[TagFromName(SliceThickness)];
			if (!aSliceThickness) {
				list+=new DecimalStringAttribute(TagFromName(SliceThickness));		// Type 2
			}
		}
		{
			Attribute *aLaterality = list[TagFromName(Laterality)];
			if (!aLaterality || aLaterality->getVM() == 0) {
				Attribute *aFrameLaterality = list[TagFromName(FrameLaterality)];
				if (aFrameLaterality && aFrameLaterality->getVM() > 0) {
					char *frameLaterality = AttributeValue(aFrameLaterality);
					const char *laterality = frameLaterality;
					if (frameLaterality) {
						if (strcmp(frameLaterality,"U") == 0 || strcmp(frameLaterality,"B") == 0) {
							laterality="";
						}
					}
					list-=TagFromName(Laterality);
					list+=new CodeStringAttribute(TagFromName(Laterality),laterality);
				}
			}
		}
	}
	
	if (isMultiFrameSOPClass) {
		list+=new IntegerStringAttribute(TagFromName(NumberOfFrames),Uint16(1));
	}
}

int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);
	
	if (!dicom_output_options.filename) {
		cerr << "Must explicitly specify output file prefix using -of option" << endl;
		bad=true;
	}
	
	bool verbose=options.get("verbose") || options.get("v");
	bool ignorereaderrors=options.get("ignorereaderrors");
	bool replaceSeriesNumberAndUID=!options.get("sameseries");
	bool insertConcatenationAttributes=!options.get("noconcat");
	
	int instanceNumberOffset = 0;
	bool replaceInstanceNumber=options.get("instancenumber");
	if (options.get("instancenumberstart",instanceNumberOffset)) {
		--instanceNumberOffset;		// if want to actually start from 1 as specified, offset needs to be 0
		replaceInstanceNumber=true;
	}
	
	bool replaceFrameIncrementPointerWithInstanceNumber=options.get("frameincrementpointerinstancenumber");
	bool replaceVectors=options.get("vectorupdate");
	bool unenhance=options.get("unenhance");
	bool noPrivateSequences=options.get("noprivateseq");
	bool noPrivateAttributes=options.get("noprivateattr");
	
	if ((noPrivateSequences || noPrivateAttributes) && !unenhance) {
		cerr << "Cannot specify \"noprivateseq\" or \"noprivateattr\" without \"unenhance\"" << endl;
		bad=true;
	}
	
	int framesPerEachInstanceOfConcatenation = 1;
	if (!options.get("framesper",framesPerEachInstanceOfConcatenation) && !unenhance) {
		cerr << "Must specify \"framesper\" - number of frames per each instance of the concatentation, if not unenhancing" << endl;
		bad=true;
	}
	else {
		if (unenhance && framesPerEachInstanceOfConcatenation != 1) {
			cerr << "Must specify \"framesper\" of exactly one frame when unenhance option specified " << dec << framesPerEachInstanceOfConcatenation << endl;
			bad=true;
		}
		else if (framesPerEachInstanceOfConcatenation < 0) {
			cerr << "Must specify \"framesper\" with more than one frame - got " << dec << framesPerEachInstanceOfConcatenation << endl;
			bad=true;
		}
	}

	if (replaceFrameIncrementPointerWithInstanceNumber && !replaceInstanceNumber) {
		cerr << "Cannot specify \"frameincrementpointerinstancenumber\" without \"instancenumber\"" << endl;
		bad=true;
	}
	
	if (replaceFrameIncrementPointerWithInstanceNumber && replaceVectors) {
		cerr << "Cannot specify \"frameincrementpointerinstancenumber\" and \"vectorupdate\"" << endl;
		bad=true;
	}
	
	dicom_input_options.done();
	dicom_output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,dicom_input_options.filename,cin);

	cerr << dicom_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();

	if (!dicom_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " -framesper n"
			<< " [-unenhance [-noprivateseq] [-noprivateattr]]"
			<< " [-instancenumber|[-instancenumberstart n]]"
			<< " [-frameincrementpointerinstancenumber|vectorupdate]"
			<< " [-sameseries]"
			<< " [-noconcat]"
			<< " [-v|-verbose]"
			<< " [-ignorereaderrors]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}
	
	bool success=true;
	TextOutputStream log(cerr);

	// Strategy is to keep reading the same file over again, writing it each time
	// such that only the frames relevant to that part of the concatenation are
	// written on each occasion
	
	DicomInputStream din(*(istream *)input_opener,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);

	const char *inputFilename = dicom_input_options.filename ? dicom_input_options.filename : "";
	ManagedAttributeList list;
	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);
	if (!list.good()) {
		if (!ignorereaderrors) {
			log << list.errors();
			return 1;
		}
		log << EMsgDC(DatasetReadFailed) << endl;
	}
	// Get pixel data attribute for later use, then remove it from the list ...
	Attribute *apixeldata=list[TagFromName(PixelData)];
	if (!apixeldata) {
		log << EMsgDC(MissingAttribute) << " - \"PixelData\"" << endl;
		return 1;
	}
	else {
		list-=TagFromName(PixelData);
	}

	int numberOfFrames = getIntegerValueElseError(list,TagFromName(NumberOfFrames),"Number of Frames",inputFilename,log);
	if (numberOfFrames < 1) {
		log << "Number of frames in input must be > 0" << endl;
		return 1;
	}
	Uint16  inConcatenationTotalNumber = (numberOfFrames-1)/framesPerEachInstanceOfConcatenation+1;

	Uint16 vSeriesNumber = Uint16(AttributeValue(list[TagFromName(SeriesNumber)],1)) + (replaceSeriesNumberAndUID ? 7000 : 0);
	char *studystring = AttributeValue(list[TagFromName(StudyID)]);
	Uint16 studyNumber = studystring ? atoi(studystring) : 0;
	char *vSeriesInstanceUID = replaceSeriesNumberAndUID ? StrDup(GeneratedSeriesInstanceUID(dicom_output_options.stamp,studyNumber,vSeriesNumber)) : AttributeValue(list[TagFromName(SeriesInstanceUID)]);
//cerr << "vSeriesInstanceUID=" << vSeriesInstanceUID << endl;
	Uint16 vInstanceNumber = Uint16(AttributeValue(list[TagFromName(InstanceNumber)],double(0)));
	char *vConcatenationUID = StrDup(GeneratedConcatenationUID(dicom_output_options.stamp,studyNumber,vSeriesNumber,vInstanceNumber));
	//cerr << "vConcatenationUID=" << vConcatenationUID << endl;
	char *vSOPInstanceUIDOfConcatenationSource = StrDup(AttributeValue(list[TagFromName(SOPInstanceUID)]));
	//cerr << "vSOPInstanceUIDOfConcatenationSource=" << vSOPInstanceUIDOfConcatenationSource << endl;

	SequenceAttribute *aPerFrameFunctionalGroupSequence = (SequenceAttribute *)list[TagFromName(PerFrameFunctionalGroupsSequence)];
	list-=TagFromName(PerFrameFunctionalGroupsSequence);
	SequenceAttribute *aSharedFunctionalGroupsSequence = (SequenceAttribute *)list[TagFromName(SharedFunctionalGroupsSequence)];
	list-=TagFromName(PerFrameFunctionalGroupsSequence);

	//if (unenhance) {										// why not leave them ? If not, should also remove DimensionIndexValues :(
	//	list-=TagFromName(DimensionOrganizationSequence);
	//	list-=TagFromName(DimensionIndexSequence);
	//}
	
	Uint16            rows=getIntegerValueElseError(list,TagFromName(Rows),"Rows",inputFilename,log);
	Uint16         columns=getIntegerValueElseError(list,TagFromName(Columns),"Columns",inputFilename,log);
	Uint16 samplesPerPixel=getIntegerValueElseError(list,TagFromName(SamplesPerPixel),"Samples per Pixel",inputFilename,log);
	Uint16   bitsAllocated=getIntegerValueElseError(list,TagFromName(BitsAllocated),"Bits Allocated",inputFilename,log);
	Uint16      bitsStored=getIntegerValueElseError(list,TagFromName(BitsStored),"Bits Stored",inputFilename,log);
	Uint16         highBit=getIntegerValueElseError(list,TagFromName(HighBit),"High Bit",inputFilename,log);
	
	AttributeList cacheOfFrameIncrementPointerVectors;

	int concatenationFrameOffsetNumber;
	int inConcatenationNumber;
	for (concatenationFrameOffsetNumber=0,inConcatenationNumber=1;
			concatenationFrameOffsetNumber < numberOfFrames;
			concatenationFrameOffsetNumber+=framesPerEachInstanceOfConcatenation,++inConcatenationNumber) {
		
		// Figure out how many frames to copy this time around ...
		int firstFrame = concatenationFrameOffsetNumber;
		int lastFrame = firstFrame + framesPerEachInstanceOfConcatenation - 1;
		if (lastFrame >= numberOfFrames) lastFrame=numberOfFrames-1;
		int frameCountThisInstance = lastFrame-firstFrame+1;
//cerr << "concatenationFrameOffsetNumber=" << dec << concatenationFrameOffsetNumber << endl;
//cerr << "firstFrame=" << dec << firstFrame << endl;
//cerr << "lastFrame=" << dec << lastFrame << endl;
//cerr << "frameCountThisInstance=" << dec << frameCountThisInstance << endl;
//cerr << "numberOfFrames=" << dec << numberOfFrames << endl;

		Uint16 inConcatenationNumber = firstFrame/framesPerEachInstanceOfConcatenation+1;
//cerr << "adding InConcatenationNumber=" << dec << inConcatenationNumber << endl;
		{
			list-=TagFromName(InConcatenationNumber);
			list-=TagFromName(InConcatenationTotalNumber);
			list-=TagFromName(ConcatenationFrameOffsetNumber);
			list-=TagFromName(ConcatenationUID);
			list-=TagFromName(SOPInstanceUIDOfConcatenationSource);
		}
		if (insertConcatenationAttributes) {
			list+=new UnsignedShortAttribute(TagFromName(InConcatenationNumber),inConcatenationNumber);
			list+=new UnsignedShortAttribute(TagFromName(InConcatenationTotalNumber),inConcatenationTotalNumber);
			list+=new UnsignedLongAttribute(TagFromName(ConcatenationFrameOffsetNumber),firstFrame);
			list+=new UIStringAttribute(TagFromName(ConcatenationUID),vConcatenationUID);
			list+=new UIStringAttribute(TagFromName(SOPInstanceUIDOfConcatenationSource),vSOPInstanceUIDOfConcatenationSource);
		}
		
		if (replaceSeriesNumberAndUID) {
			list-=TagFromName(SeriesNumber);
			list+=new IntegerStringAttribute(TagFromName(SeriesNumber),vSeriesNumber);
			list-=TagFromName(SeriesInstanceUID);
			list+=new UIStringAttribute(TagFromName(SeriesInstanceUID),vSeriesInstanceUID);
		}
		
		if (replaceInstanceNumber) {
			list-=TagFromName(InstanceNumber);
			list+=new IntegerStringAttribute(TagFromName(InstanceNumber),Uint32(instanceNumberOffset + inConcatenationNumber));	// will increment by from 1 for degenerate case of framesPerEachInstanceOfConcatenation == 1
		}
		
		Attribute *aFrameIncrementPointer = list[TagFromName(FrameIncrementPointer)];
		if (replaceFrameIncrementPointerWithInstanceNumber) {
			list-=TagFromName(FrameIncrementPointer);
			list+=new AttributeTagAttribute(TagFromName(FrameIncrementPointer),TagFromName(InstanceNumber));
			
			// remove vectors that might previously have been referenced and now do not match frames ...
			list-=TagFromName(EnergyWindowVector);
			list-=TagFromName(DetectorVector);
			list-=TagFromName(PhaseVector);
			list-=TagFromName(RotationVector);
			list-=TagFromName(RRIntervalVector);
			list-=TagFromName(TimeSlotVector);
			list-=TagFromName(SliceVector);
			list-=TagFromName(AngularViewVector);
			list-=TagFromName(TimeSliceVector);
			
			list-=TagFromName(NumberOfPhases);
			list-=TagFromName(NumberOfRotations);
			list-=TagFromName(NumberOfRRIntervals);
			list-=TagFromName(NumberOfTimeSlots);
			list-=TagFromName(NumberOfSlices);

			list-=TagFromName(FrameTimeVector);
			
			list-=TagFromName(PageNumberVector);
			list-=TagFromName(FrameLabelVector);
			list-=TagFromName(FramePrimaryAngleVector);
			list-=TagFromName(FrameSecondaryAngleVector);
			list-=TagFromName(SliceLocationVector);
			list-=TagFromName(DisplayWindowLabelVector);
			
			list-=TagFromName(GridFrameOffsetVector);
		}
		else if (replaceVectors && frameCountThisInstance == 1 && aFrameIncrementPointer && aFrameIncrementPointer->isTag() && aFrameIncrementPointer->getVM() > 0) {
			int n = aFrameIncrementPointer->getVM();
			for (unsigned i=0; i<n; ++i) {
				Tag tag;
				if (aFrameIncrementPointer->getValue(i,tag)) {
					Attribute *aVector = cacheOfFrameIncrementPointerVectors[tag];
					if (aVector == NULL) {
//cerr << "lazy instantiation of copy of vector (" << hex << tag.getGroup() << "," << tag.getElement() << ")" << dec << endl;
						aVector = list[tag];
						if (aVector != NULL) {
							cacheOfFrameIncrementPointerVectors+=aVector;
						}
					}
					list-=tag;	// remove from  output regardless
					if (aVector && aVector->getVM() > 0) {
//cerr << "doing vector (" << hex << tag.getGroup() << "," << tag.getElement() << ")" << dec << endl;
						if (aVector->getVM() >= inConcatenationNumber) {
							int vectorIndex = inConcatenationNumber-1;
							const char *vr = aVector->getVR();
							if (strcmp(vr,"US") == 0) {
								Uint16 valueForFrame;
								if (aVector->getValue(vectorIndex,valueForFrame)) {
//cerr << "doing vector (" << hex << tag.getGroup() << "," << tag.getElement() << ") frame[" << dec << inConcatenationNumber << "] vr = " <<  vr << " value = " << valueForFrame << endl;
									list+=new UnsignedShortAttribute(tag,valueForFrame);
								}
							}
							else if (strcmp(vr,"IS") == 0) {
								Int32 valueForFrame;
								if (aVector->getValue(vectorIndex,valueForFrame)) {
//cerr << "doing vector (" << hex << tag.getGroup() << "," << tag.getElement() << ") frame[" << dec << inConcatenationNumber << "] vr = " <<  vr << " value = " << valueForFrame << endl;
									list+=new IntegerStringAttribute(tag,valueForFrame);
								}
							}
							else if (strcmp(vr,"DS") == 0) {
								Float64 valueForFrame;
								if (aVector->getValue(vectorIndex,valueForFrame)) {
//cerr << "doing vector (" << hex << tag.getGroup() << "," << tag.getElement() << ") frame[" << dec << inConcatenationNumber << "] vr = " <<  vr << " value = " << valueForFrame << endl;
									list+=new DecimalStringAttribute(tag,valueForFrame);
								}
							}
							else if (strcmp(vr,"SH") == 0) {
								char *valueForFrame;
								if (aVector->getValue(vectorIndex,valueForFrame)) {
//cerr << "doing vector (" << hex << tag.getGroup() << "," << tag.getElement() << ") frame[" << dec << inConcatenationNumber << "] vr = " <<  vr << " value = " << valueForFrame << endl;
									list+=new ShortStringAttribute(tag,valueForFrame);
								}
							}

						}
					}
				}
			}
		}
		
		// For the following, new ones are added automatically during list write ...
//cerr << "removing UIDs" << endl;
		list-=TagFromName(SOPInstanceUID);
		list-=TagFromName(InstanceCreationDate);
		list-=TagFromName(InstanceCreationTime);
		
		// Select out only the per-frame items we want
		
		list-=TagFromName(NumberOfFrames);
		if (unenhance) {
			Assert(firstFrame == lastFrame);
			moveRelevantPerFrameAndSharedFunctionalGroupSequenceItemsContentsToTopLevel(list,aPerFrameFunctionalGroupSequence,aSharedFunctionalGroupsSequence,firstFrame,noPrivateSequences,noPrivateAttributes);
			changeSOPClassToUnenhanced(list);
			fixupUnenhanced(list);		// will add value of 1 for NumberOfFrames only if required for SOP Class
		}
		else {
			list+=new IntegerStringAttribute(TagFromName(NumberOfFrames),Uint16(frameCountThisInstance));
			removeAllPerFrameFunctionalGroupSequenceItemsExcept(list,aPerFrameFunctionalGroupSequence,firstFrame,lastFrame);
		}
		
		// Start writing, first the list, then the pixel data base, then the selected pixel data frames
		
		const char *outputFileName;
		{
			ostrstream ostr;
			if (dicom_output_options.filename) ostr << dicom_output_options.filename;
			ostr << (instanceNumberOffset + inConcatenationNumber) << ends;				// include offset to match instance number
			outputFileName=ostr.str();
		}
#ifdef USEBINARYFLAGFOROUTPUTOPENMODE
		ofstream out(outputFileName,ios::out|ios::trunc|ios::binary);
#else
		ofstream out(outputFileName,ios::out|ios::trunc);
#endif
		if (!out || !out.rdbuf()->is_open()) {
			log << EMsgDC(FileWriteOpenFailed)
				<< " - \"" << outputFileName << "\"" << endl;
			return false;
		}
		DicomOutputStream dout(out,
			dicom_output_options.transfersyntaxuid,
			dicom_output_options.usemetaheader,
			dicom_output_options.useimplicitmetaheader,
			dicom_output_options.addtiff);

//cerr << "Calling usualManagedAttributeListWrite for file " << outputFileName << endl;
		if (!usualManagedAttributeListWrite(list,dout,dicom_output_options,log,verbose)) {
			success=false;
			break;
		}
//cerr << "Finished usualManagedAttributeListWrite" << endl;
		
		OtherUnspecifiedLargeAttributeDummy *dummyPixelData = new
			OtherUnspecifiedLargeAttributeDummy(TagFromName(PixelData),
				rows,columns,frameCountThisInstance,samplesPerPixel,
				dout.getTransferSyntaxToWriteDataSet(),
				0 /* let encoding rules calculate bytesinword */,
				bitsAllocated,bitsStored,highBit,
				0xffffffff /* let encoding rules calculate length */);
		Assert(dummyPixelData);
		dummyPixelData->writeBase(dout);

		Assert(apixeldata);
		if (!apixeldata->isOtherData()) {
			log << EMsgDC(PixelDataIncorrectVR) << endl;
			success=false;
			break;
		}
		else {
			// Check for compatible transfer syntax and append to pixel data ...
			TransferSyntax *its=din.getTransferSyntaxToReadDataSet();
			Assert(its);
			TransferSyntax *ots=dout.getTransferSyntaxToWriteDataSet();
			Assert(ots);

			if (its->isEncapsulated()) {
				log << "Can't read encapsulated transfer syntax from file <" << inputFilename << ">" << endl;
				success=false;
				break;
			}
			else if (its->getEndian() != ots->getEndian()) {
				log << "Input byte order in input file <" << inputFilename << "> differs from output - not supported" << endl;
				success=false;
				break;
			}
			else {
				OtherUnspecifiedLargeAttributeBase *opixeldata = apixeldata->castToOtherData();
				Assert(opixeldata);
				Uint32 bytesPerFrame = opixeldata->getDestinationBytesPerFrame();
				Uint32 offset = bytesPerFrame*firstFrame;
				Uint32 length = bytesPerFrame*frameCountThisInstance;
//cerr << "offset=" << dec << offset << " length=" << length << endl;
				opixeldata->writeRaw(dout,offset,length);
			}
		}

	}

	return success ? 0 : 1;
}

	
