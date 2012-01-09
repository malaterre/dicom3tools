#include <ctype.h>

#include "attrmxls.h"
#include "attrtype.h"
#include "attrval.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "dcopt.h"

static bool
isPrivateCreatorPresent(AttributeList &list,Uint16 group,Uint16 element,const char *creator)
{
//cerr << "isPrivateCreatorPresent: (" << hex << group << "," << element << dec << ") want <" << creator << ">" << endl;
	bool same=false;

	Assert(creator
	    && strlen(creator) > 0
	    && creator[strlen(creator)-1] != ' ');	// no trailing blanks

//cerr << "isPrivateCreatorPresent: looking for attribute" << endl;
	Attribute *a=list[Tag(group,element)];
	char *value=0;
//cerr << "isPrivateCreatorPresent: checking attribute" << endl;
	if (a && a->isString() && a->getValue(0,value) && value) {
//cerr << "isPrivateCreatorPresent: found value <" << value << ">" << endl;
		char *p = value+strlen(value);
		while (p-- > value && *p == ' ') *p=0;	// remove trailing spaces

//cerr << "isPrivateCreatorPresent: checking against <" << value << ">" << endl;
		if (strcmp(value,creator) == 0) {
//cerr << "isPrivateCreatorPresent: same" << endl;
			same=true;
		}
	}
//cerr << "isPrivateCreatorPresent: done" << endl;
	if (value) delete[] value;
//cerr << "isPrivateCreatorPresent: result " << (same ? "T" : "F") << endl;
	return same;
}

static Attribute *
findPrivateAttribute(AttributeList &list,Uint16 group,const char *creatorID,Uint16 element) {
	Attribute *a=NULL;
	int tryBlock;
	for (tryBlock=0; tryBlock < 0x0100; ++tryBlock) {
		if (isPrivateCreatorPresent(list,group,tryBlock,creatorID)) {
//cerr << "findPrivateAttribute: found " << creatorID << " " << hex << group << "," << ((tryBlock<<8)|(element&0x00ff)) << dec << endl;
			a=list[Tag(group,(tryBlock<<8)|(element&0x00ff))];
			break;
		}
	}
	return a;
}

static void
AcrNEMA_FixImagePlaneModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixImagePlaneModule:" << endl;

//cerr << "AcrNEMA_FixImagePlaneModule: SliceLocation" << endl;

	{
		Attribute *aSliceLocation=list[TagFromName(SliceLocation)];	// the DICOM attribute
		if (!aSliceLocation) {
			Attribute *aLocation=list[TagFromName(Location)];	// old ACR-NEMA - retired in DICOM
			if (aLocation) {
				char *vLocation=AttributeValue(aLocation);
				if (vLocation) {
					aSliceLocation=new DecimalStringAttribute(TagFromName(SliceLocation),vLocation);
					Assert(aSliceLocation);
					list+=aSliceLocation;
				}
			}
		}
	}

	// Try to make patient relative orientation and position ...

	bool headfirst = false;
	bool feetfirst = false;

	bool prone = false;
	bool supine = false;
	bool left = false;
	bool right = false;

	bool viewcranial = false;
	bool viewcaudal = false;
	bool viewleft = false;
	bool viewright = false;
	bool viewfront = false;
	bool viewback = false;
	
	bool patientOrientationClaimsColumnsPosterior = false;		// To check for and fix a Toshiba CT bug
	bool patientOrientationClaimsColumnsAnterior = false;		// To check for and fix a Toshiba CT bug

	{
		// Fix Toshiba bug ... trailing backslash on PatientOrientation

		Attribute *aPatientOrientation=list[TagFromName(PatientOrientation)];

		if (aPatientOrientation && aPatientOrientation->getVM() >= 3) {
			// don't have "removeValue" method so replace attribute:

			char *row_direction=0;	(void)aPatientOrientation->getValue(0,row_direction);
			char *col_direction=0;	(void)aPatientOrientation->getValue(1,col_direction);

			list-=aPatientOrientation;
			delete aPatientOrientation;

			aPatientOrientation=new CodeStringAttribute(TagFromName(PatientOrientation));
			Assert(aPatientOrientation);

			aPatientOrientation->addValue(row_direction);
			aPatientOrientation->addValue(col_direction);

			list+=aPatientOrientation;
//cerr << "PatientOrientation col_direction = " << col_direction << endl;
			if (col_direction && strcmp(col_direction,"P") == 0) {
				patientOrientationClaimsColumnsPosterior=true;		// To check for and fix a Toshiba CT bug
			}
//cerr << "patientOrientationClaimsColumnsPosterior = " << patientOrientationClaimsColumnsPosterior << endl;
			if (col_direction && strcmp(col_direction,"A") == 0) {
				patientOrientationClaimsColumnsAnterior=true;		// To check for and fix a Toshiba CT bug
			}
//cerr << "patientOrientationClaimsColumnsPosterior = " << patientOrientationClaimsColumnsPosterior << endl;
		}

		// Fix Toshiba bug ... trailing backslash on ImageOrientation

		Attribute *aImageOrientation=list[TagFromName(ImageOrientation)];

		if (aImageOrientation && aImageOrientation->getVM() >= 7) {
			// don't have "removeValue" method so replace attribute:

			Float32 orientation_row_X; (void)aImageOrientation->getValue(0,orientation_row_X);
			Float32 orientation_row_Y; (void)aImageOrientation->getValue(1,orientation_row_Y);
			Float32 orientation_row_Z; (void)aImageOrientation->getValue(2,orientation_row_Z);
			Float32 orientation_col_X; (void)aImageOrientation->getValue(3,orientation_col_X);
			Float32 orientation_col_Y; (void)aImageOrientation->getValue(4,orientation_col_Y);
			Float32 orientation_col_Z; (void)aImageOrientation->getValue(5,orientation_col_Z);
			
			list-=aImageOrientation;
			delete aImageOrientation;

			aImageOrientation=new DecimalStringAttribute(TagFromName(ImageOrientation));
			Assert(aImageOrientation);

			aImageOrientation->addValue(orientation_row_X);
			aImageOrientation->addValue(orientation_row_Y);
			aImageOrientation->addValue(orientation_row_Z);
			aImageOrientation->addValue(orientation_col_X);
			aImageOrientation->addValue(orientation_col_Y);
			aImageOrientation->addValue(orientation_col_Z);

			list+=aImageOrientation;
		}

		// Fix Toshiba bug ... trailing backslash on PixelSpacing

		Attribute *aPixelSpacing=list[TagFromName(PixelSpacing)];

		if (aPixelSpacing && aPixelSpacing->getVM() >= 3) {
			// don't have "removeValue" method so replace attribute:

			double vPixelSpacing_Row; 	(void)aPixelSpacing->getValue(0,vPixelSpacing_Row);
			double vPixelSpacing_Column;	(void)aPixelSpacing->getValue(1,vPixelSpacing_Column);

			list-=aPixelSpacing;
			delete aPixelSpacing;

			aPixelSpacing=new DecimalStringAttribute(TagFromName(PixelSpacing));
			Assert(aPixelSpacing);

			aPixelSpacing->addValue(vPixelSpacing_Row);
			aPixelSpacing->addValue(vPixelSpacing_Column);

			list+=aPixelSpacing;
		}
	}

	{
		Attribute *aPatientPosition=findPrivateAttribute(list,0x0013,"SIEMENS CM VA0  CMS",0x0044);	// Patient Position (Siemens SPI modified)

		if (!aPatientPosition) aPatientPosition=list[TagFromName(PatientPosition)];	// Patient Position (ACR/NEMA and DICOM)

		char *vPatientPosition=0;
		if (aPatientPosition) {
			aPatientPosition->getValue(0,vPatientPosition);
			list-=TagFromName(PatientPosition);			// will replace it later
		}

//cerr << "AcrNEMA_FixImagePlaneModule: PatientPosition = " << vPatientPosition << endl;

		// check for DICOM values ... (and check for Philips bug with space between values)

		if (!headfirst) headfirst = vPatientPosition && (strcmp(vPatientPosition,"HFS") == 0 || strcmp(vPatientPosition,"HF S") == 0);
		if (!headfirst) headfirst = vPatientPosition && (strcmp(vPatientPosition,"HFP") == 0 || strcmp(vPatientPosition,"HF P") == 0);
		if (!headfirst) headfirst = vPatientPosition && (strcmp(vPatientPosition,"HFDR") == 0 || strcmp(vPatientPosition,"HF DR") == 0);
		if (!headfirst) headfirst = vPatientPosition && (strcmp(vPatientPosition,"HFDL") == 0 || strcmp(vPatientPosition,"HF DL") == 0);
		if (!feetfirst) feetfirst = vPatientPosition && (strcmp(vPatientPosition,"FFS") == 0 || strcmp(vPatientPosition,"FF S") == 0);
		if (!feetfirst) feetfirst = vPatientPosition && (strcmp(vPatientPosition,"FFP") == 0 || strcmp(vPatientPosition,"FF P") == 0);
		if (!feetfirst) feetfirst = vPatientPosition && (strcmp(vPatientPosition,"FFDR") == 0 || strcmp(vPatientPosition,"FF DR") == 0);
		if (!feetfirst) feetfirst = vPatientPosition && (strcmp(vPatientPosition,"FFDL") == 0 || strcmp(vPatientPosition,"FF DL") == 0);

		if (!supine) supine = vPatientPosition && (strcmp(vPatientPosition,"HFS") == 0 || strcmp(vPatientPosition,"HF S") == 0);
		if (!supine) supine = vPatientPosition && (strcmp(vPatientPosition,"FFS") == 0 || strcmp(vPatientPosition,"FF S") == 0);
		if (!prone)  prone  = vPatientPosition && (strcmp(vPatientPosition,"HFP") == 0 || strcmp(vPatientPosition,"HF P") == 0);
		if (!prone)  prone  = vPatientPosition && (strcmp(vPatientPosition,"FFP") == 0 || strcmp(vPatientPosition,"FF P") == 0);
		if (!right)  right  = vPatientPosition && (strcmp(vPatientPosition,"HFDR") == 0 || strcmp(vPatientPosition,"HF DR") == 0);
		if (!right)  right  = vPatientPosition && (strcmp(vPatientPosition,"FFDR") == 0 || strcmp(vPatientPosition,"FF DR") == 0);
		if (!left)   left   = vPatientPosition && (strcmp(vPatientPosition,"HFDL") == 0 || strcmp(vPatientPosition,"HF DL") == 0);
		if (!left)   left   = vPatientPosition && (strcmp(vPatientPosition,"FFDL") == 0 || strcmp(vPatientPosition,"FF DL") == 0);

		if (!supine) supine = vPatientPosition && (strcmp(vPatientPosition,"SUPINE") == 0 || strcmp(vPatientPosition,"SUPI") == 0);	// Some Siemens SPI +/- truncated to 4 chars
		if (!prone)  prone  = vPatientPosition && (strcmp(vPatientPosition,"PRONE") == 0  || strcmp(vPatientPosition,"PRON") == 0);
		if (!right)  right  = vPatientPosition && (strcmp(vPatientPosition,"RIGHT") == 0  || strcmp(vPatientPosition,"RIGH") == 0);
		if (!left)   left   = vPatientPosition &&  strcmp(vPatientPosition,"LEFT") == 0;

		if (!vPatientPosition && isPrivateCreatorPresent(list,0x0019,0x0011,"TOSHIBA_MEC_CT_1.0")
		      && list[Tag(0x0019,0x1108)]	// OrientationHeadFeet (HF, ? what else)
		      && list[Tag(0x0019,0x110a)]	// OrientationSupineProne (SU, ? what else)
		) {
cerr << "Warning - Patient Position absent - reconstructing from private CT Toshiba attributes" << endl;
			Attribute *aOrientationHeadFeet=findPrivateAttribute(list,0x0019,"TOSHIBA_MEC_CT_1.0",0x0008);		// OrientationHeadFeet (HF, ? what else)
			char *vOrientationHeadFeet=0;
			if (aOrientationHeadFeet) aOrientationHeadFeet->getValue(0,vOrientationHeadFeet);
			if (vOrientationHeadFeet) {
//cerr << "Got vOrientationHeadFeet " << vOrientationHeadFeet << endl;
				if (strcmp(vOrientationHeadFeet,"HF") == 0) headfirst=true;
				else if (strcmp(vOrientationHeadFeet,"FF") == 0) feetfirst=true;		// guess at string value
			}
			
			Attribute *aOrientationSupineProne=findPrivateAttribute(list,0x0019,"TOSHIBA_MEC_CT_1.0",0x000a);		// OrientationHeadFeet (HF, ? what else)
			char *vOrientationSupineProne=0;
			if (aOrientationSupineProne) aOrientationSupineProne->getValue(0,vOrientationSupineProne);
			if (vOrientationSupineProne) {
//cerr << "Got vOrientationSupineProne " << vOrientationSupineProne << endl;
				if (strcmp(vOrientationSupineProne,"SU") == 0) supine=true;
				else if (strcmp(vOrientationSupineProne,"PR") == 0) prone=true;		// guess at string value
			}
			
		}

		if (!vPatientPosition && isPrivateCreatorPresent(list,0x0019,0x0010,"PHILIPS MR/PART")
		      && list[Tag(0x0019,0x1008)]	// PatientPosition
		      && list[Tag(0x0019,0x1009)]	// PatientOrientation
		) {
cerr << "Warning - Patient Position absent - reconstructing from private MR Philips attributes" << endl;

			Attribute *aPatientPosition=list[Tag(0x0019,0x1008)];
			Uint16 vPatientPosition=0;

			if (aPatientPosition && aPatientPosition->getVM() == 1) (void) aPatientPosition->getValue(0,vPatientPosition);

			switch (vPatientPosition) {
				case 1:		headfirst=true;
						break;
				case 2:		feetfirst=true;
						break;
			}

			Attribute *aPatientOrientation=list[Tag(0x0019,0x1009)];
			Uint16 vPatientOrientation=0;

			if (aPatientOrientation && aPatientOrientation->getVM() == 1) (void) aPatientOrientation->getValue(0,vPatientOrientation);

			switch (vPatientOrientation) {
				case 1:		supine=true;
						break;
				case 2:		prone=true;
						break;
				case 3:		left=true;
						break;
				case 4:		right=true;
						break;
			}
		}
	}
	{
		Attribute *aRestDirection=findPrivateAttribute(list,0x0013,"SIEMENS CM VA0  CMS",0x0042);		// Patient Rest Direction (Siemens SPI modified)
		if (!aRestDirection) aRestDirection=findPrivateAttribute(list,0x0021,"SIEMENS CM VA0  CMS",0x0032);	// Patient Rest Direction (Siemens SPI original)

		char *vRestDirection=0;
		if (aRestDirection) aRestDirection->getValue(0,vRestDirection);

		if (!headfirst) headfirst = vRestDirection && strcmp(vRestDirection,"HEAD") == 0;
		if (!feetfirst) feetfirst = vRestDirection && strcmp(vRestDirection,"FEET") == 0;
	}

	{
		// The view direction is the old "look from top down or bottom up"
		// issue that applied to EMI head vs. body scans

		// SPI generalizes it to cover sagittals ( "RtoL" or "LtoR") and
		// coronals ("AtoP" and "PtoA") as well ...

		Attribute *aViewDirection=findPrivateAttribute(list,0x0013,"SIEMENS CM VA0  CMS",0x0046);		// View Direction (Siemens SPI modified)
		if (!aViewDirection) aViewDirection=findPrivateAttribute(list,0x0021,"SIEMENS CM VA0  CMS",0x0030);	// View Direction (Siemens SPI original)
		if (!aViewDirection) aViewDirection=findPrivateAttribute(list,0x0019,"TOSHIBA_MEC_CT_1.0",0x0009);	// View Direction (Toshiba CT)

		char *vViewDirection=0;
		if (aViewDirection) aViewDirection->getValue(0,vViewDirection);
//cerr << "Got vViewDirection " << vViewDirection << endl;

		if (!viewcranial) viewcranial = vViewDirection && strcmp(vViewDirection,"HEAD") == 0;
		if (!viewcranial) viewcranial = vViewDirection && strcmp(vViewDirection,"HFF") == 0;		// Toshiba - guess
		if (!viewcaudal)  viewcaudal  = vViewDirection && strcmp(vViewDirection,"FEET") == 0;
		if (!viewcaudal)  viewcaudal  = vViewDirection && strcmp(vViewDirection,"VFF") == 0;		// Toshiba
		if (!viewleft)    viewleft    = vViewDirection && strcmp(vViewDirection,"LtoR") == 0;
		if (!viewright)   viewright   = vViewDirection && strcmp(vViewDirection,"RtoL") == 0;
		if (!viewfront)   viewfront   = vViewDirection && strcmp(vViewDirection,"AtoP") == 0;
		if (!viewback)    viewback    = vViewDirection && strcmp(vViewDirection,"PtoA") == 0;
	}


	Assert(!(supine && prone && right && left));
	Assert(!(headfirst && feetfirst));
	Assert(!(viewcranial && viewcaudal && viewleft && viewright && viewfront && viewback));

	Attribute *aImagePositionPatient=list[TagFromName(ImagePositionPatient)];		// DICOM patient relative
	Attribute *aImageOrientationPatient=list[TagFromName(ImageOrientationPatient)];		// DICOM patient relative

	if (aImagePositionPatient && aImageOrientationPatient) {

		// The DICOM attributes are present - read and rewrite them ...
		// (why ? converts scientific to float and makes GE AW voxtool happier)

		if (aImagePositionPatient    && aImagePositionPatient->getVM() == 3
		 && aImageOrientationPatient && aImageOrientationPatient->getVM() == 6) {

			Float32 position_X; (void)aImagePositionPatient->getValue(0,position_X);
			Float32 position_Y; (void)aImagePositionPatient->getValue(1,position_Y);
			Float32 position_Z; (void)aImagePositionPatient->getValue(2,position_Z);

			list-=aImagePositionPatient;
			aImagePositionPatient=new DecimalStringAttribute(TagFromName(ImagePositionPatient));
			Assert(aImagePositionPatient);
			aImagePositionPatient->addValue(position_X);
			aImagePositionPatient->addValue(position_Y);
			aImagePositionPatient->addValue(position_Z);
			list+=aImagePositionPatient;

			Float32 orientation_row_X; (void)aImageOrientationPatient->getValue(0,orientation_row_X);
			Float32 orientation_row_Y; (void)aImageOrientationPatient->getValue(1,orientation_row_Y);
			Float32 orientation_row_Z; (void)aImageOrientationPatient->getValue(2,orientation_row_Z);
			Float32 orientation_col_X; (void)aImageOrientationPatient->getValue(3,orientation_col_X);
			Float32 orientation_col_Y; (void)aImageOrientationPatient->getValue(4,orientation_col_Y);
			Float32 orientation_col_Z; (void)aImageOrientationPatient->getValue(5,orientation_col_Z);

			list-=aImageOrientationPatient;
			aImageOrientationPatient=new DecimalStringAttribute(TagFromName(ImageOrientationPatient));
			Assert(aImageOrientationPatient);
			aImageOrientationPatient->addValue(orientation_row_X);
			aImageOrientationPatient->addValue(orientation_row_Y);
			aImageOrientationPatient->addValue(orientation_row_Z);
			aImageOrientationPatient->addValue(orientation_col_X);
			aImageOrientationPatient->addValue(orientation_col_Y);
			aImageOrientationPatient->addValue(orientation_col_Z);
			list+=aImageOrientationPatient;
		}
	}
	else {
		// The DICOM attributes are not present - make them ...

		Attribute *aImagePosition=list[TagFromName(ImagePosition)];		// ACR-NEMA gantry relative
		Attribute *aImageOrientation=list[TagFromName(ImageOrientation)];	// ACR-NEMA gantry relative

		if (aImagePosition    && aImagePosition->getVM() == 3
		 && aImageOrientation && aImageOrientation->getVM() == 6) {

			Float32 position_X; (void)aImagePosition->getValue(0,position_X);
			Float32 position_Y; (void)aImagePosition->getValue(1,position_Y);
			Float32 position_Z; (void)aImagePosition->getValue(2,position_Z);

			Float32 orientation_row_X; (void)aImageOrientation->getValue(0,orientation_row_X);
			Float32 orientation_row_Y; (void)aImageOrientation->getValue(1,orientation_row_Y);
			Float32 orientation_row_Z; (void)aImageOrientation->getValue(2,orientation_row_Z);
			Float32 orientation_col_X; (void)aImageOrientation->getValue(3,orientation_col_X);
			Float32 orientation_col_Y; (void)aImageOrientation->getValue(4,orientation_col_Y);
			Float32 orientation_col_Z; (void)aImageOrientation->getValue(5,orientation_col_Z);
			
			if (prone) {		// swap L/R and A/P
cerr << "Prone so swapping L/R and A/P" << endl;
				position_X        = -position_X;
				orientation_row_X = -orientation_row_X;
				orientation_col_X = -orientation_col_X;

				position_Y        = -position_Y;
				orientation_row_Y = -orientation_row_Y;
				orientation_col_Y = -orientation_col_Y;
			}

			if (right) {		// swap L is really P, A is really L
cerr << "Decubitus Right so swapping X and Y and changing sign of new Y" << endl;
				Float32 temp;
				temp = position_X; position_X = position_Y; position_Y = -temp;
				temp = orientation_row_X; orientation_row_X = orientation_row_Y; orientation_row_Y = -temp;
				temp = orientation_col_X; orientation_col_X = orientation_col_Y; orientation_col_Y = -temp;
			}

			if (left) {		// swap L is really A, P is really L
cerr << "Decubitus Left so swapping X and Y and changing sign of new Y" << endl;
				Float32 temp;
				temp = position_X; position_X = position_Y; position_Y = -temp;
				temp = orientation_row_X; orientation_row_X = orientation_row_Y; orientation_row_Y = -temp;
				temp = orientation_col_X; orientation_col_X = orientation_col_Y; orientation_col_Y = -temp;
			}

			if (feetfirst) {	// swap L/R and H/F
cerr << "Feet first so swapping L/R and H/F" << endl;
				position_X        = -position_X;
				orientation_row_X = -orientation_row_X;
				orientation_col_X = -orientation_col_X;

				position_Z        = -position_Z;
				orientation_row_Z = -orientation_row_Z;
				orientation_col_Z = -orientation_col_Z;
			}

			if (orientation_col_Y  < 0 && patientOrientationClaimsColumnsPosterior
			 || orientation_col_Y  > 0 && patientOrientationClaimsColumnsAnterior) {
cerr << "Warning - Patient Orientation and generated Image Orientation (Patient) inconsistent in column Y direction - change sign (likely Toshiba CT bug)" << endl;
				orientation_row_Y=-orientation_row_Y;
				orientation_col_Y=-orientation_col_Y;
				position_Y       = -position_Y;
				
				// also fix up the original (incorrect) gantry-relative attributes
				{
					Attribute *aImageOrientation=list[TagFromName(ImageOrientation)];
					if (aImageOrientation && aImageOrientation->getVM() == 6) {
						Float32 orientation_row_X; (void)aImageOrientation->getValue(0,orientation_row_X);
						Float32 orientation_row_Y; (void)aImageOrientation->getValue(1,orientation_row_Y);
						Float32 orientation_row_Z; (void)aImageOrientation->getValue(2,orientation_row_Z);
						Float32 orientation_col_X; (void)aImageOrientation->getValue(3,orientation_col_X);
						Float32 orientation_col_Y; (void)aImageOrientation->getValue(4,orientation_col_Y);
						Float32 orientation_col_Z; (void)aImageOrientation->getValue(5,orientation_col_Z);
			
						list-=aImageOrientation;
						delete aImageOrientation;

						aImageOrientation=new DecimalStringAttribute(TagFromName(ImageOrientation));
						Assert(aImageOrientation);

						aImageOrientation->addValue(orientation_row_X);
						aImageOrientation->addValue(-orientation_row_Y);
						aImageOrientation->addValue(orientation_row_Z);
						aImageOrientation->addValue(orientation_col_X);
						aImageOrientation->addValue(-orientation_col_Y);
						aImageOrientation->addValue(orientation_col_Z);

						list+=aImageOrientation;
					}
					Attribute *aImagePosition=list[TagFromName(ImagePosition)];
					if (aImagePosition && aImagePosition->getVM() == 3) {
						Float32 position_X; (void)aImagePosition->getValue(0,position_X);
						Float32 position_Y; (void)aImagePosition->getValue(1,position_Y);
						Float32 position_Z; (void)aImagePosition->getValue(2,position_Z);
			
						list-=aImagePosition;
						delete aImagePosition;

						aImagePosition=new DecimalStringAttribute(TagFromName(ImagePosition));
						Assert(aImagePosition);

						aImagePosition->addValue(position_X);
						aImagePosition->addValue(-position_Y);
						aImagePosition->addValue(position_Z);

						list+=aImagePosition;
					}
				}
			}
			
			// For now assume that View Direction has already been accounted for in orientation attributes
			// ... not known if this assumption is valid or not :(

			Attribute *aImagePositionPatient=new DecimalStringAttribute(TagFromName(ImagePositionPatient));
			Assert(aImagePositionPatient);
			aImagePositionPatient->addValue(position_X);
			aImagePositionPatient->addValue(position_Y);
			aImagePositionPatient->addValue(position_Z);
			list+=aImagePositionPatient;

			Attribute *aImageOrientationPatient=new DecimalStringAttribute(TagFromName(ImageOrientationPatient));
			Assert(aImageOrientationPatient);
			aImageOrientationPatient->addValue(orientation_row_X);
			aImageOrientationPatient->addValue(orientation_row_Y);
			aImageOrientationPatient->addValue(orientation_row_Z);
			aImageOrientationPatient->addValue(orientation_col_X);
			aImageOrientationPatient->addValue(orientation_col_Y);
			aImageOrientationPatient->addValue(orientation_col_Z);
			list+=aImageOrientationPatient;
		}
		else if (isPrivateCreatorPresent(list,0x0019,0x0010,"PHILIPS MR/PART")
		      && list[Tag(0x0019,0x100A)]	// SliceOrientation
		      && isPrivateCreatorPresent(list,0x0019,0x0011,"PHILIPS MR/PART")
		      && list[Tag(0x0019,0x110B)]	// ImageOffcentreLR
		) {
cerr << "Warning - Image Position or Orientation absent or incomplete - reconstructing from private MR Philips attributes" << endl;

			// reconstruct ImageOrientationPatient from private SliceOrientation (which is already patient not gantry relative)

			Float32 orientation_row_X=0;
			Float32 orientation_row_Y=0;
			Float32 orientation_row_Z=0;

			Float32 orientation_col_X=0;
			Float32 orientation_col_Y=0;
			Float32 orientation_col_Z=0;

			Attribute *aSliceOrientation=list[Tag(0x0019,0x100A)];
			Uint16 vSliceOrientation=0;

			if (aSliceOrientation && aSliceOrientation->getVM() == 1) (void) aSliceOrientation->getValue(0,vSliceOrientation);

			// DICOM is +LPH

			switch (vSliceOrientation) {
				case 1:		// transverse
						orientation_row_X=1;	// +L
						orientation_col_Y=1;	// +P
						break;
				case 2:		// sagittal
						orientation_row_Y=1;	// +P
						orientation_col_Z=-1;	// -H
						break;
				case 3:		// coronal
						orientation_row_X=1;	// +L
						orientation_col_Z=-1;	// -H
						break;
				default:	// undefined or missing - assume transverse
						orientation_row_X=1;	// +L
						orientation_col_Y=1;	// +P
						break;
			}

			// should probably also account for private rotation attributes ... not yet implemented



			// reconstruct ImagePositionPatient from private Image Offcentre (assume is already patient not gantry relative)

			// DICOM is +LPH
			// Philips is +RAH

			Float32 position_X=0;
			Float32 position_Y=0;
			Float32 position_Z=0;

			Attribute *aImageOffcentreLR=list[Tag(0x0019,0x100B)];
			Float32 vImageOffcentreLR=0;

			if (aImageOffcentreLR && aImageOffcentreLR->getVM() == 1) {
				(void) aImageOffcentreLR->getValue(0,vImageOffcentreLR);
				position_X-=vImageOffcentreLR;
			}

			Attribute *aImageOffcentreCC=list[Tag(0x0019,0x100C)];
			Float32 vImageOffcentreCC=0;

			if (aImageOffcentreCC && aImageOffcentreCC->getVM() == 1) {
				(void) aImageOffcentreCC->getValue(0,vImageOffcentreCC);
				position_Z+=vImageOffcentreCC;
			}

			Attribute *aImageOffcentreAP=list[Tag(0x0019,0x100D)];
			Float32 vImageOffcentreAP=0;

			if (aImageOffcentreAP && aImageOffcentreAP->getVM() == 1) {
				(void) aImageOffcentreAP->getValue(0,vImageOffcentreAP);
				position_Y-=vImageOffcentreAP;
			}

			{
				Attribute *aImagePositionPatient=new DecimalStringAttribute(TagFromName(ImagePositionPatient));
				Assert(aImagePositionPatient);
				aImagePositionPatient->addValue(position_X);
				aImagePositionPatient->addValue(position_Y);
				aImagePositionPatient->addValue(position_Z);
				list+=aImagePositionPatient;

				Attribute *aImageOrientationPatient=new DecimalStringAttribute(TagFromName(ImageOrientationPatient));
				Assert(aImageOrientationPatient);
				aImageOrientationPatient->addValue(orientation_row_X);
				aImageOrientationPatient->addValue(orientation_row_Y);
				aImageOrientationPatient->addValue(orientation_row_Z);
				aImageOrientationPatient->addValue(orientation_col_X);
				aImageOrientationPatient->addValue(orientation_col_Y);
				aImageOrientationPatient->addValue(orientation_col_Z);
				list+=aImageOrientationPatient;
			}
		}
		else {
cerr << "Warning - Image Position or Orientation absent or incomplete while making patient relative" << endl;
cerr << "Warning - Orientation (such as left/right) may be wrong" << endl;

			// Let's be creative and make it up from Patient Orientation and Slice Location...
			// (which may have been derived from Location earlier in this routine)

			Attribute *aSliceLocation=list[TagFromName(SliceLocation)];

			Float32 vSliceLocation=0;
			if (aSliceLocation && aSliceLocation->getVM() == 1) aSliceLocation->getValue(0,vSliceLocation);

			Attribute *aPatientOrientation=list[TagFromName(PatientOrientation)];
			char *row_direction=0;
			char *col_direction=0;

			// Try this only if row and col directions are orthogonal
			// to major axes (e.g. P\F) and not obliques (that are
			// compound, e.g. AL\RF) since:
			// a) angle can't be determined and
			// b) sign of Slice Location becomes ambiguous

			if (aPatientOrientation && aPatientOrientation->getVM() == 2
			 && aPatientOrientation->getValue(0,row_direction) && (row_direction[1] == 0 || row_direction[1] == ' ')
			 && aPatientOrientation->getValue(1,col_direction) && (col_direction[1] == 0 || col_direction[1] == ' ') ) {

				Float32 orientation_row_X=0;
				Float32 orientation_row_Y=0;
				Float32 orientation_row_Z=0;

				Float32 orientation_col_X=0;
				Float32 orientation_col_Y=0;
				Float32 orientation_col_Z=0;

				// DICOM plane definitions are:
				// - Left Pos Superior +ve (aka +HPL)

				switch (row_direction[0]) {
					case 'R':	orientation_row_X=-1; break;
					case 'L':	orientation_row_X= 1; break;
					case 'A':	orientation_row_Y=-1; break;
					case 'P':	orientation_row_Y= 1; break;
					case 'F':	orientation_row_Z=-1; break;
					case 'H':	orientation_row_Z= 1; break;
				}

				switch (col_direction[0]) {
					case 'R':	orientation_col_X=-1; break;
					case 'L':	orientation_col_X= 1; break;
					case 'A':	orientation_col_Y=-1; break;
					case 'P':	orientation_col_Y= 1; break;
					case 'F':	orientation_col_Z=-1; break;
					case 'H':	orientation_col_Z= 1; break;
				}

				// compute normal to image plane ...
				// - cross product of row and column vectors
				// - points AWAY from viewer

				Float32 normal_X = orientation_row_Y*orientation_col_Z - orientation_row_Z*orientation_col_Y;
				Float32 normal_Y = orientation_row_Z*orientation_col_X - orientation_row_X*orientation_col_Z;
				Float32 normal_Z = orientation_row_X*orientation_col_Y - orientation_row_Y*orientation_col_X;

//cerr << "normal_X = " << normal_X << endl;
//cerr << "normal_Y = " << normal_Y << endl;
//cerr << "normal_Z = " << normal_Z << endl;

				// - change sign to point TOWARDS viewer

				normal_X = -normal_X;
				normal_Y = -normal_Y;
				normal_Z = -normal_Z;

				// Project Slice Location along normal making a lot of assumptions:
				// - which way the normal points - towards or away from the viewer
				// - is it consistent for all planes
				// - what is the sign of Slice Location for each plane for Siemens and Philips
				// - does the patient insertion in the gantry matter
				//
				// this needs serious work :)

				// assume patient relative and +FAL (typical late model Siemens)

//cerr << "vSliceLocation = " << vSliceLocation << endl;

				Float32 position_X=normal_X*vSliceLocation;		// sagittal viewed from L, DICOM is +L, location is +L
				Float32 position_Y=normal_Y*vSliceLocation * -1;	// coronal  viewed from A, DICOM is +P, location is +A
				Float32 position_Z=normal_Z*vSliceLocation * -1;	// axial    viewed from F, DICOM is +H, location is +F

				Attribute *aImagePositionPatient=new DecimalStringAttribute(TagFromName(ImagePositionPatient));
				Assert(aImagePositionPatient);
				aImagePositionPatient->addValue(position_X);
				aImagePositionPatient->addValue(position_Y);
				aImagePositionPatient->addValue(position_Z);
				list+=aImagePositionPatient;

				Attribute *aImageOrientationPatient=new DecimalStringAttribute(TagFromName(ImageOrientationPatient));
				Assert(aImageOrientationPatient);
				aImageOrientationPatient->addValue(orientation_row_X);
				aImageOrientationPatient->addValue(orientation_row_Y);
				aImageOrientationPatient->addValue(orientation_row_Z);
				aImageOrientationPatient->addValue(orientation_col_X);
				aImageOrientationPatient->addValue(orientation_col_Y);
				aImageOrientationPatient->addValue(orientation_col_Z);
				list+=aImageOrientationPatient;
			}

		}

	}

//cerr << "AcrNEMA_FixImagePlaneModule: PatientPosition" << endl;

//cerr << "supine = " << (supine ? "true" : "false") << endl;
//cerr << "prone = "  << (prone  ? "true" : "false") << endl;
//cerr << "right = "  << (right  ? "true" : "false") << endl;
//cerr << "left = "   << (left   ? "true" : "false") << endl;

//cerr << "headfirst = "   << (headfirst   ? "true" : "false") << endl;
//cerr << "feetfirst = "   << (feetfirst   ? "true" : "false") << endl;

	if (!supine && !prone && !right && !left) {
		supine=true;
cerr << "Warning - Can't determine supine or prone - assuming supine" << endl;
	}

	if (!headfirst && !feetfirst) {
		headfirst=true;
cerr << "Warning - Can't determine head or feet first - assuming head first" << endl;
	}

	const char *vPatientPosition=0;
	if      (supine && headfirst) vPatientPosition="HFS";
	else if (supine && feetfirst) vPatientPosition="FFS";
	else if (prone  && headfirst) vPatientPosition="HFP";
	else if (prone  && feetfirst) vPatientPosition="FFP";

//cerr << "AcrNEMA_FixImagePlaneModule: new PatientPosition = " << vPatientPosition << endl;

	// add zero length even if no information - is a Type 2 attribute ...

	Attribute *aPatientPosition=new CodeStringAttribute(TagFromName(PatientPosition));
	Assert(aPatientPosition);
	if (vPatientPosition) aPatientPosition->addValue(vPatientPosition);
	list+=aPatientPosition;

//cerr << "AcrNEMA_FixImagePlaneModule: Done" << endl;
}


static void
AcrNEMA_FixLaterality(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixLaterality:" << endl;
	bool right = false;
	bool left = false;

	{
		Attribute *aLaterality=list[TagFromName(Laterality)];		// ACR-NEMA and Siemens SPI
										// Value of "NO" is ignored

		char *vLaterality=0;
		if (aLaterality) {
			aLaterality->getValue(0,vLaterality);
			list-=TagFromName(Laterality);				// Replaced later
		}

		if (!right) right = vLaterality && strcmp(vLaterality,"RIGHT") == 0;
		if (!right) right = vLaterality && strcmp(vLaterality,"R") == 0;
		if (!left)  left  = vLaterality && strcmp(vLaterality,"LEFT") == 0;
		if (!left)  left  = vLaterality && strcmp(vLaterality,"L") == 0;
	}

	const char *vLaterality=0;
	if      (right) vLaterality="R";
	else if (left)  vLaterality="L";
	Attribute *aLaterality=new CodeStringAttribute(TagFromName(Laterality));
	Assert(aLaterality);
	if (vLaterality) aLaterality->addValue(vLaterality);
	list+=aLaterality;
}

static void
AcrNEMA_FixImagePixelModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixImagePixelModule:" << endl;
	{
		Attribute *aSamplesPerPixel=list[TagFromName(SamplesPerPixel)];
		if (!aSamplesPerPixel) {
			aSamplesPerPixel=new UnsignedShortAttribute(TagFromName(SamplesPerPixel),1);
			Assert(aSamplesPerPixel);
			list+=aSamplesPerPixel;
		}
	}
	{
		Attribute *aPhotometricInterpretation=list[TagFromName(PhotometricInterpretation)];
		if (!aPhotometricInterpretation) {
			aPhotometricInterpretation=new CodeStringAttribute(TagFromName(PhotometricInterpretation),"MONOCHROME2");
			Assert(aPhotometricInterpretation);
			list+=aPhotometricInterpretation;
		}
	}

	// If Bits Allocated is absent or has no value use next byte boundary >= Bits Stored
	// (eg. Siemens Somatom CT)

	{
		Attribute *aBitsAllocated=list[TagFromName(BitsAllocated)];

		if (!aBitsAllocated || aBitsAllocated->getVL() == 0) {
			Attribute *aBitsStored=list[TagFromName(BitsStored)];
			Uint16 vBitsStored;
			if (aBitsStored && aBitsStored->getValue(0,vBitsStored) && vBitsStored) {
				if (aBitsAllocated) {
					list-=TagFromName(BitsAllocated); 
				}

				aBitsAllocated=new UnsignedShortAttribute(TagFromName(BitsAllocated),Uint16(((vBitsStored-1)/8+1)*8));
				Assert(aBitsAllocated);
				list+=aBitsAllocated;
			}
		}
	}
	// If High Bit is absent or has no value use one less than Bits Stored
	// (eg. Toshiba Xpress CT)

	{
		Attribute *aHighBit=list[TagFromName(HighBit)];

		if (!aHighBit || aHighBit->getVL() == 0) {
			Attribute *aBitsStored=list[TagFromName(BitsStored)];
			Uint16 vBitsStored;
			if (aBitsStored && aBitsStored->getValue(0,vBitsStored) && vBitsStored) {
				if (aHighBit) {
					list-=TagFromName(HighBit); 
				}

				aHighBit=new UnsignedShortAttribute(TagFromName(HighBit),Uint16(vBitsStored-1));
				Assert(aHighBit);
				list+=aHighBit;
			}
		}
	}
}

static void
AcrNEMA_FixFrameOfReferenceModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixFrameOfReferenceModule:" << endl;
	// Frame of reference UID is taken care of in general dicom mx list stuff

	{
		Attribute *aPositionReferenceIndicator=list[TagFromName(PositionReferenceIndicator)];
		if (!aPositionReferenceIndicator) {
			aPositionReferenceIndicator=new LongStringAttribute(TagFromName(PositionReferenceIndicator));
			Assert(aPositionReferenceIndicator);
			list+=aPositionReferenceIndicator;
		}
	}
}

static void
AcrNEMA_FixMRImageModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixMRImageModule:" << endl;
	// Send dummy values for Type 1s and 2s ...

	{
		// Type 1
		Attribute *aScanningSequence=list[TagFromName(ScanningSequence)];
		if (!aScanningSequence) {
			aScanningSequence=new CodeStringAttribute(TagFromName(ScanningSequence),"NONE");	// invalid enum :(
			Assert(aScanningSequence);
			list+=aScanningSequence;
		}
	}

	{
		// Type 1
		Attribute *aSequenceVariant=list[TagFromName(SequenceVariant)];
		if (!aSequenceVariant) {
			aSequenceVariant=new CodeStringAttribute(TagFromName(SequenceVariant),"NONE");
			Assert(aSequenceVariant);
			list+=aSequenceVariant;
		}
	}

	{
		// Type 2
		Attribute *aScanOptions=list[TagFromName(ScanOptions)];
		if (!aScanOptions) {
			aScanOptions=new CodeStringAttribute(TagFromName(ScanOptions));
			Assert(aScanOptions);
			list+=aScanOptions;
		}
	}

	{
		// Type 2
		Attribute *aMRAcquisitionType=list[TagFromName(MRAcquisitionType)];
		if (!aMRAcquisitionType) {
			Attribute *aNumberOf3DRawPartitionsNominal=findPrivateAttribute(list,0x0021,"SIEMENS MR VA0  GEN",0x0030);

			aMRAcquisitionType=new CodeStringAttribute(TagFromName(MRAcquisitionType));
			Assert(aMRAcquisitionType);

			if (aNumberOf3DRawPartitionsNominal) {
				Uint16 vNumberOf3DRawPartitionsNominal = AttributeValue(aNumberOf3DRawPartitionsNominal);
				aMRAcquisitionType->addValue(vNumberOf3DRawPartitionsNominal ? "3D" : "2D");
			}
			list+=aMRAcquisitionType;
		}
	}

	{
		// Type 2
		Attribute *aEchoTrainLength=list[TagFromName(EchoTrainLength)];
		if (!aEchoTrainLength) {
			Attribute *aNumberOfEchoes=findPrivateAttribute(list,0x0021,"SIEMENS MR VA0  GEN",0x0070);
			Uint16 vNumberOfEchoes = aNumberOfEchoes ? Uint16(AttributeValue(aNumberOfEchoes)) : 0;

			// send 0 rather than zero length else AW gets upset

			aEchoTrainLength=new IntegerStringAttribute(TagFromName(EchoTrainLength),vNumberOfEchoes);
			Assert(aEchoTrainLength);
			list+=aEchoTrainLength;
		}
	}

	// Map some key optional values ...

	{
		Attribute *aFlipAngle=list[TagFromName(FlipAngle)];		// the DICOM attribute
		if (aFlipAngle) {
			list-=aFlipAngle;	// replace it to get rid of exponential notation (screws up AW)
		}
		else {
			if (isPrivateCreatorPresent(list,0x0019,0x0010,"PHILIPS MR/PART"))
				aFlipAngle=list[Tag(0x0019,0x101a)];			// Philips SPI MR - FFE Flip Angle
			else
				aFlipAngle=findPrivateAttribute(list,0x0019,"SIEMENS MR VA0  GEN",0x0060);
		}

		if (aFlipAngle) {
			double vFlipAngle=AttributeValue(aFlipAngle);
			aFlipAngle=new DecimalStringAttribute(TagFromName(FlipAngle),vFlipAngle);
			Assert(aFlipAngle);
			list+=aFlipAngle;
		}
	}

	{
		Attribute *aEchoTime=list[TagFromName(EchoTime)];		// the DICOM attribute
		double vEchoTime;
		if (aEchoTime) {
			list-=aEchoTime;	// replace it to get rid of exponential notation (screws up AW)
			double vEchoTime=AttributeValue(aEchoTime);
			aEchoTime=new DecimalStringAttribute(TagFromName(EchoTime),vEchoTime);
			Assert(aEchoTime);
			list+=aEchoTime;
		}
	}

	{
		Attribute *aRepetitionTime=list[TagFromName(RepetitionTime)];		// the DICOM attribute
		double vRepetitionTime;
		if (aRepetitionTime) {
			list-=aRepetitionTime;	// replace it to get rid of exponential notation (screws up AW)
			double vRepetitionTime=AttributeValue(aRepetitionTime);
			aRepetitionTime=new DecimalStringAttribute(TagFromName(RepetitionTime),vRepetitionTime);
			Assert(aRepetitionTime);
			list+=aRepetitionTime;
		}
	}

	{
		Attribute *aInversionTime=list[TagFromName(InversionTime)];		// the DICOM attribute
		double vInversionTime;
		if (aInversionTime) {
			list-=aInversionTime;	// replace it to get rid of exponential notation (screws up AW)
			double vInversionTime=AttributeValue(aInversionTime);
			aInversionTime=new DecimalStringAttribute(TagFromName(InversionTime),vInversionTime);
			Assert(aInversionTime);
			list+=aInversionTime;
		}
	}

	{
		Attribute *aMagneticFieldStrength=list[TagFromName(MagneticFieldStrength)];		// the DICOM attribute
		if (!aMagneticFieldStrength) {
			aMagneticFieldStrength=findPrivateAttribute(list,0x0019,"SIEMENS MR VA0  COAD",0x0012);	// Siemens SPI MR is in Tesla
			if (aMagneticFieldStrength) {
				double vMagneticFieldStrength=AttributeValue(aMagneticFieldStrength);
				aMagneticFieldStrength=new DecimalStringAttribute(TagFromName(MagneticFieldStrength),vMagneticFieldStrength);
				Assert(aMagneticFieldStrength);
				list+=aMagneticFieldStrength;
			}
		}
	}

	{
		Attribute *aReconstructionDiameter=list[TagFromName(ReconstructionDiameter)];		// the DICOM attribute
		if (!aReconstructionDiameter) {

			Attribute *aPixelSpacing=list[TagFromName(PixelSpacing)];
			Attribute *aRows=list[TagFromName(Rows)];
			Attribute *aColumns=list[TagFromName(Columns)];

			Uint16 vRows;
			Uint16 vColumns;
			unsigned vmPixelSpacing;
			double vPixelSpacing_Row;
			double vPixelSpacing_Column;

			bool havespacingattributes = false;

			if (aRows && aRows->getValue(0,vRows)
			 && aColumns && aColumns->getValue(0,vColumns)
			 && aPixelSpacing && (vmPixelSpacing=aPixelSpacing->getVM()) >= 1 && vmPixelSpacing <= 2) {
				if (vmPixelSpacing == 1) {	// potentially common error
cerr << "Warning - using single PixelSpacing value since two values not present" << endl;
					(void)aPixelSpacing->getValue(0,vPixelSpacing_Row);
					vPixelSpacing_Column=vPixelSpacing_Row;
				}
				else {
					(void)aPixelSpacing->getValue(0,vPixelSpacing_Row);
					(void)aPixelSpacing->getValue(1,vPixelSpacing_Column);
				}
				havespacingattributes=true;
			}

			// try to derive Reconstruction Diameter from Philips MR Private ...

			Attribute *aFieldOfView=
				isPrivateCreatorPresent(list,0x0019,0x0010,"PHILIPS MR/PART")
				? list[Tag(0x0019,0x1000)] : 0;

			if (aFieldOfView && aFieldOfView->getVM() == 1) {
				//
				// NB. Do not use when two values specifying
				// rectangular FoV presumably.
				//
				double vReconstructionDiameter=AttributeValue(aFieldOfView);
				aReconstructionDiameter=new DecimalStringAttribute(TagFromName(ReconstructionDiameter),vReconstructionDiameter);
				Assert(aReconstructionDiameter);
				list+=aReconstructionDiameter;

				// check for T5 Pixel Spacing bug (different values when should be the same) ...

				if (havespacingattributes && vmPixelSpacing == 2
				    && fabs(vPixelSpacing_Row-vPixelSpacing_Column) > .001		// two values different
				    && fabs(vPixelSpacing_Row-vReconstructionDiameter/vColumns) < .001	// first value ==  FoV/columns
				) {
cerr << "Warning - PixelSpacing values different - replacing incorrect column dimension" << endl;
					vPixelSpacing_Column=vPixelSpacing_Row;
					list-=aPixelSpacing;
					aPixelSpacing=new DecimalStringAttribute(TagFromName(PixelSpacing),vPixelSpacing_Row,vPixelSpacing_Column);
					Assert(aPixelSpacing);
					list+=aPixelSpacing;
				}
			}
			else {
				// derive Reconstruction Diameter from the Type 1 attributes, if they were present ...

				if (havespacingattributes) {

					// use average. though probably both row and col values identical ...

					double vReconstructionDiameter=(vRows*vPixelSpacing_Row+vColumns*vPixelSpacing_Column)/2;

					aReconstructionDiameter=new DecimalStringAttribute(TagFromName(ReconstructionDiameter),vReconstructionDiameter);
					Assert(aReconstructionDiameter);
					list+=aReconstructionDiameter;
				}
			}
		}
	}
	{
		Attribute *aAcquisitionMatrix=list[TagFromName(AcquisitionMatrix)];
		Attribute *aNumberOfPhaseEncodingSteps=list[TagFromName(NumberOfPhaseEncodingSteps)];
		Attribute *aInPlanePhaseEncodingDirection=list[TagFromName(InPlanePhaseEncodingDirection)];
		if (!aAcquisitionMatrix || !aNumberOfPhaseEncodingSteps || !aInPlanePhaseEncodingDirection) {
			// derive from SPI Siemens MR Private ...

			Attribute *aNumberOfFourierLinesNominal=
				findPrivateAttribute(list,0x0019,"SIEMENS MR VA0  GEN",0x0020);
			//Attribute *aAcquisitionColumns=
			//	findPrivateAttribute(list,0x0019,"SIEMENS MR VA0  GEN",0x0030);
			Attribute *aBaseRawMatrixSize=
				findPrivateAttribute(list,0x0019,"SIEMENS MR VA0  COAD",0x00d4);
			Attribute *aPhaseCodingDirection=
				findPrivateAttribute(list,0x0019,"SIEMENS MR VA0  COAD",0x00da);
			/* if (aNumberOfFourierLinesNominal && aAcquisitionColumns && aPhaseCodingDirection) { */
			if (aNumberOfFourierLinesNominal && aBaseRawMatrixSize && aPhaseCodingDirection) {
				char *vPhaseCodingDirection=AttributeValue(aPhaseCodingDirection);
				Uint16 vNumberOfFourierLinesNominal=AttributeValue(aNumberOfFourierLinesNominal);
				/* Uint16 vAcquisitionColumns=AttributeValue(aAcquisitionColumns); */
				Uint16 vBaseRawMatrixSize=AttributeValue(aBaseRawMatrixSize);

				Uint16 frequency_rows=0;					
				Uint16 frequency_columns=0;
				Uint16 phase_rows=0;
				Uint16 phase_columns=0;
				const char *phase_encoding_direction=0;

				if (strcmp(vPhaseCodingDirection,"plusX") == 0 || strcmp(vPhaseCodingDirection,"minusX") == 0
				 || strcmp(vPhaseCodingDirection,"PLUSX") == 0 || strcmp(vPhaseCodingDirection,"MINUSX") == 0) {
					phase_encoding_direction="ROW";
					frequency_columns=vNumberOfFourierLinesNominal;
					/* phase_rows=vAcquisitionColumns; */
					phase_rows=vBaseRawMatrixSize;
				}
				else if (strcmp(vPhaseCodingDirection,"plusY") == 0 || strcmp(vPhaseCodingDirection,"minusY") == 0
				      || strcmp(vPhaseCodingDirection,"PLUSY") == 0 || strcmp(vPhaseCodingDirection,"MINUSY") == 0) {
					phase_encoding_direction="COL";
					frequency_rows=vNumberOfFourierLinesNominal;					
					/* phase_columns=vAcquisitionColumns; */
					phase_columns=vBaseRawMatrixSize;
				}

				if (!aAcquisitionMatrix && (frequency_rows || frequency_columns || phase_rows || phase_columns)) {
					aAcquisitionMatrix=new UnsignedShortAttribute(TagFromName(AcquisitionMatrix));
					Assert(aAcquisitionMatrix);
					aAcquisitionMatrix->addValue(frequency_rows);
					aAcquisitionMatrix->addValue(frequency_columns);
					aAcquisitionMatrix->addValue(phase_rows);
					aAcquisitionMatrix->addValue(phase_columns);
					list+=aAcquisitionMatrix;
				}

				if (!aNumberOfPhaseEncodingSteps && vBaseRawMatrixSize)
					list+=new IntegerStringAttribute(TagFromName(NumberOfPhaseEncodingSteps),vBaseRawMatrixSize);

				if (!aInPlanePhaseEncodingDirection && phase_encoding_direction)
					list+=new CodeStringAttribute(TagFromName(InPlanePhaseEncodingDirection),phase_encoding_direction);
			}
		}
	}

	{
		Attribute *aTriggerTime=list[TagFromName(TriggerTime)];
		if (!aTriggerTime && isPrivateCreatorPresent(list,0x0019,0x0010,"PHILIPS MR/PART")) {
			aTriggerTime=list[Tag(0x0019,0x106e)];			// Philips SPI MR - Trigger Delay Time
			if (aTriggerTime) {
				double vTriggerTime=AttributeValue(aTriggerTime);
				aTriggerTime=new DecimalStringAttribute(TagFromName(TriggerTime),vTriggerTime);
				Assert(aTriggerTime);
				list+=aTriggerTime;
			}
		}
	}

	{
		Attribute *aAcquisitionDuration=list[TagFromName(AcquisitionDuration)];
		if (!aAcquisitionDuration) {
			if (isPrivateCreatorPresent(list,0x2005,0x0010,"Philips MR Imaging DD 001")) {
				aAcquisitionDuration=list[Tag(0x2005,0x1033)];
				if (aAcquisitionDuration) {
					double vAcquisitionDuration=AttributeValue(aAcquisitionDuration);
					aAcquisitionDuration=new FloatDoubleAttribute(TagFromName(AcquisitionDuration),vAcquisitionDuration);
					Assert(aAcquisitionDuration);
					list+=aAcquisitionDuration;
				}
			}
			else if (isPrivateCreatorPresent(list,0x0019,0x0010,"GEMS_ACQU_01")) {
				aAcquisitionDuration=list[Tag(0x0019,0x105a)];
				if (aAcquisitionDuration) {
					double vAcquisitionDuration=double(AttributeValue(aAcquisitionDuration))/1000000.0;		// seems to be in microseconds and we need seconds
					if (vAcquisitionDuration >= 0) {		// sometimes observed to be -ve and hence presumably meaningless
						aAcquisitionDuration=new FloatDoubleAttribute(TagFromName(AcquisitionDuration),vAcquisitionDuration);
						Assert(aAcquisitionDuration);
						list+=aAcquisitionDuration;
					}
				}
			}
		}
	}

#ifdef CRAP
		Element <SequenceName> not present
		Element <AngioFlag> not present
		Element <MagneticFieldStrength> not present
		Element <SpacingBetweenSlices> not present
		Element <NumberOfPhaseEncodingSteps> not present
		Element <PercentSampling> not present
		Element <PercentPhaseFieldOfView> not present
		Element <PixelBandwidth> not present
		Element <NominalInterval> not present
		Element <BeatRejectionFlag> not present
		Element <LowRRValue> not present
		Element <HighRRValue> not present
		Element <IntervalsAcquired> not present
		Element <IntervalsRejected> not present
		Element <PVCRejection> not present
		Element <SkipBeats> not present
		Element <HeartRate> not present
		Element <CardiacNumberOfImages> not present
		Element <TriggerWindow> not present
		Element <ReconstructionDiameter> not present
		Element <TransmittingCoil> not present
		Element <AcquisitionMatrix> not present
		Element <InPlanePhaseEncodingDirection> not present
		Element <SAR> not present
		Element <VariableFlipAngleFlag> not present
		Element <dBdt> not present
		Element <TemporalPositionIdentifier> not present
		Element <NumberOfTemporalPositions> not present
		Element <TemporalResolution> not present
#endif
}

static void
AcrNEMA_FixCTImageModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixCTImageModule:" << endl;

	{
		Attribute *aExposure=list[TagFromName(Exposure)];
		Uint32 vExposure=AttributeValue(aExposure);
		Attribute *aExposureInuAs=list[TagFromName(ExposureInuAs)];
		Uint32 vExposureInuAs=AttributeValue(aExposureInuAs);
		Attribute *aExposureTime=list[TagFromName(ExposureTime)];
		Uint32 vExposureTime=AttributeValue(aExposureTime);
		Attribute *aXRayTubeCurrent=list[TagFromName(XRayTubeCurrent)];
		Uint32 vXRayTubeCurrent=AttributeValue(aXRayTubeCurrent);

		if (aExposure && !aExposureInuAs) {
			vExposureInuAs=vExposure*1000;
			aExposureInuAs=new IntegerStringAttribute(TagFromName(ExposureInuAs),vExposureInuAs);
			Assert(aExposureInuAs);
			list+=aExposureInuAs;
		}
		else if (aExposureInuAs && !aExposure) {
			vExposure=vExposureInuAs/1000;
			aExposure=new IntegerStringAttribute(TagFromName(Exposure),vExposure);
			Assert(aExposure);
			list+=aExposure;
		}

		if (aExposureInuAs && aExposureTime && !aXRayTubeCurrent) {
			vXRayTubeCurrent=vExposureInuAs/vExposureTime;
			aXRayTubeCurrent=new IntegerStringAttribute(TagFromName(XRayTubeCurrent),vXRayTubeCurrent);
			Assert(aXRayTubeCurrent);
			list+=aXRayTubeCurrent;
		}
		else if (aExposureInuAs && !aExposureTime && aXRayTubeCurrent) {
			vExposureTime=vExposureInuAs/vXRayTubeCurrent;
			aExposureTime=new IntegerStringAttribute(TagFromName(ExposureTime),vExposureTime);
			Assert(aExposureTime);
			list+=aExposureTime;
		}
		else if (!aExposureInuAs && aExposureTime && aXRayTubeCurrent) {
			vExposureInuAs=vExposureTime*vXRayTubeCurrent;
			aExposureInuAs=new IntegerStringAttribute(TagFromName(ExposureInuAs),vExposureInuAs);
			Assert(aExposureInuAs);
			list+=aExposureInuAs;

			vExposure=vExposureInuAs/1000;
			aExposure=new IntegerStringAttribute(TagFromName(Exposure),vExposure);
			Assert(aExposure);
			list+=aExposure;
		}
	}

	// fix some Somatom files that have rescale slope/intercept
	// values padded to more than allowed 16 characters for VR ...

	{
		Attribute *aRescaleSlope=list[TagFromName(RescaleSlope)];
		if (aRescaleSlope && aRescaleSlope->getVM() > 0) {
			double vRescaleSlope=AttributeValue(aRescaleSlope);
			list-=aRescaleSlope;
			aRescaleSlope=new DecimalStringAttribute(TagFromName(RescaleSlope),vRescaleSlope);
			Assert(aRescaleSlope);
			list+=aRescaleSlope;
		}
	}
	{
		Attribute *aRescaleIntercept=list[TagFromName(RescaleIntercept)];
		if (aRescaleIntercept && aRescaleIntercept->getVM() > 0) {
			double vRescaleIntercept=AttributeValue(aRescaleIntercept);
			list-=aRescaleIntercept;
			aRescaleIntercept=new DecimalStringAttribute(TagFromName(RescaleIntercept),vRescaleIntercept);
			Assert(aRescaleIntercept);
			list+=aRescaleIntercept;
		}
	}
	
	// Extract additional attributes for helical scanners
	
	{

		Attribute *aRevolutionTime=list[TagFromName(RevolutionTime)];
		if (!aRevolutionTime) {
			aRevolutionTime=findPrivateAttribute(list,0x0019,"GEMS_ACQU_01",0x1027);	// GEMS - RotationSpeed
			if (aRevolutionTime) {
				double vRevolutionTime=AttributeValue(aRevolutionTime);
				if (vRevolutionTime > 10 ) {						// DICOM value is seconds, GE's is (supposedly) msec
					vRevolutionTime/=1000.0;					// but GE seems to actually encode as seconds
				}
//cerr << "vRevolutionTime=" << vRevolutionTime << endl;
				aRevolutionTime=new FloatDoubleAttribute(TagFromName(RevolutionTime),vRevolutionTime);
				Assert(aRevolutionTime);
				list+=aRevolutionTime;
			}
			else {
				aRevolutionTime=findPrivateAttribute(list,0x01f1,"ELSCINT1",0x1027);	// Elscint - Rotation Time
				double vRevolutionTime=AttributeValue(aRevolutionTime);
				aRevolutionTime=new FloatDoubleAttribute(TagFromName(RevolutionTime),vRevolutionTime);
				Assert(aRevolutionTime);
				list+=aRevolutionTime;
			}
		}
	
		Attribute *aTableFeedPerRotation=list[TagFromName(TableFeedPerRotation)];
		if (!aTableFeedPerRotation) {
			aTableFeedPerRotation=findPrivateAttribute(list,0x0019,"GEMS_ACQU_01",0x1023);	// GEMS - Table Speed [mm/rotation]
			if (!aTableFeedPerRotation) {
				aTableFeedPerRotation=findPrivateAttribute(list,0x0019,"SIEMENS CT VA0  COAD",0x10b0);	// Siemens - Feed per Rotation [mm/rotation]
			}
			if (aTableFeedPerRotation) {
				double vTableFeedPerRotation=AttributeValue(aTableFeedPerRotation);
//cerr << "vTableFeedPerRotation=" << vTableFeedPerRotation << endl;
				aTableFeedPerRotation=new FloatDoubleAttribute(TagFromName(TableFeedPerRotation),vTableFeedPerRotation);
				Assert(aTableFeedPerRotation);
				list+=aTableFeedPerRotation;
			}
		}
		
		Attribute *aTableSpeed=list[TagFromName(TableSpeed)];
		if (!aTableSpeed && aRevolutionTime && aTableFeedPerRotation) {
			double vRevolutionTime=AttributeValue(aRevolutionTime);				// seconds/rotation
			if (vRevolutionTime > 0) {
				double vTableFeedPerRotation=AttributeValue(aTableFeedPerRotation);	// mm/rotation
				double vTableSpeed = vTableFeedPerRotation/vRevolutionTime;		// mm/second
//cerr << "vTableSpeed=" << vTableSpeed << endl;
				aTableSpeed=new FloatDoubleAttribute(TagFromName(TableSpeed),vTableSpeed);
				Assert(aTableSpeed);
				list+=aTableSpeed;
			}
		}
		
		Attribute *aSpiralPitchFactor=list[TagFromName(SpiralPitchFactor)];
		if (!aSpiralPitchFactor) {
			aSpiralPitchFactor=findPrivateAttribute(list,0x0043,"GEMS_PARM_01",0x1027);	// GEMS - Scan Pitch Ratio, e.g. "0.562:1"
			char *sSpiralPitchFactor=AttributeValue(aSpiralPitchFactor);
			if (sSpiralPitchFactor && strlen(sSpiralPitchFactor) > 0) {
//cerr << "sSpiralPitchFactor=" << sSpiralPitchFactor << endl;
				double vSpiralPitchFactor=0;
				if (strcmp(sSpiralPitchFactor,"HQ") == 0) {
					vSpiralPitchFactor=3;				// High quality
				}
				else if (strcmp(sSpiralPitchFactor,"HS") == 0) {
					vSpiralPitchFactor=6;				// High speed
				}
				else {
					if (*sSpiralPitchFactor == '/') {
						++sSpiralPitchFactor;			// e.g. skip the slash in "/1.4:1"
					}
					char *colonIndex = strchr(sSpiralPitchFactor,':');
					if (colonIndex) {
						*colonIndex=0;
					}
					vSpiralPitchFactor=strtod(sSpiralPitchFactor,NULL);
				}
//cerr << "vSpiralPitchFactor=" << vSpiralPitchFactor << endl;
				aSpiralPitchFactor=new FloatDoubleAttribute(TagFromName(SpiralPitchFactor),vSpiralPitchFactor);
				Assert(aSpiralPitchFactor);
				list+=aSpiralPitchFactor;
			}
			else {
				aSpiralPitchFactor=NULL;
			}
		}
		
		Attribute *aTotalCollimationWidth=list[TagFromName(TotalCollimationWidth)];
		if (!aTotalCollimationWidth && aTableFeedPerRotation && aSpiralPitchFactor) {
			double vTableFeedPerRotation=AttributeValue(aTableFeedPerRotation);		// mm/rotation
			double vSpiralPitchFactor=AttributeValue(aSpiralPitchFactor);			// 1/rotation
			double vTotalCollimationWidth = vTableFeedPerRotation/vSpiralPitchFactor;	// mm
//cerr << "vTotalCollimationWidth=" << vTotalCollimationWidth << endl;
			aTotalCollimationWidth=new FloatDoubleAttribute(TagFromName(TotalCollimationWidth),vTotalCollimationWidth);
			Assert(aTotalCollimationWidth);
			list+=aTotalCollimationWidth;
		}
		
		Attribute *aSingleCollimationWidth=list[TagFromName(SingleCollimationWidth)];
		if (!aSingleCollimationWidth && aTotalCollimationWidth) {
			Attribute *aNumberOfDetectors=findPrivateAttribute(list,0x0045,"GEMS_HELIOS_01",0x1001);	// GEMS - Number of Macro Rows in Detector
			if (aNumberOfDetectors) {
				double vNumberOfDetectors=AttributeValue(aNumberOfDetectors);
//cerr << "vNumberOfDetectors=" << vNumberOfDetectors << endl;
				if (vNumberOfDetectors > 0) {
					double vTotalCollimationWidth = AttributeValue(aTotalCollimationWidth);		// mm
					double vSingleCollimationWidth = vTotalCollimationWidth/vNumberOfDetectors;	// mm
//cerr << "vSingleCollimationWidth=" << vSingleCollimationWidth << endl;
					aSingleCollimationWidth=new FloatDoubleAttribute(TagFromName(SingleCollimationWidth),vSingleCollimationWidth);
					Assert(aSingleCollimationWidth);
					list+=aSingleCollimationWidth;
				}
			}
		}
		
		Attribute *aReconstructionAngle=list[TagFromName(ReconstructionAngle)];
		if (!aReconstructionAngle) {
			Attribute *aScanArc=list[TagFromName(ScanArc)];			// a nuclear medicine tomo attribute used by Elscint CT
			if (aScanArc) {
				double vScanArc=AttributeValue(aScanArc);
				aReconstructionAngle=new FloatDoubleAttribute(TagFromName(ReconstructionAngle),vScanArc);
				Assert(aReconstructionAngle);
				list+=aReconstructionAngle;
			}
		}
		
	}
}

static void
AcrNEMA_FixPatientModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixPatientModule:" << endl;
	{
		Attribute *aPatientBirthDate=list[TagFromName(PatientBirthDate)];	// Type 2, may be old date format

		char *vPatientBirthDate=0;
		if (aPatientBirthDate) {
			aPatientBirthDate->getValue(0,vPatientBirthDate);
			list-=TagFromName(PatientBirthDate);				// Replaced later
		}

		if (vPatientBirthDate)
			aPatientBirthDate=new DateStringAttribute(TagFromName(PatientBirthDate),Date(vPatientBirthDate,DateOrderMonthMiddleYearFirst));
		else
			aPatientBirthDate=new DateStringAttribute(TagFromName(PatientBirthDate));
		Assert(aPatientBirthDate);
		list+=aPatientBirthDate;
	}
}

static void
AcrNEMA_FixGeneralStudyModuleAfterGeneralSeriesModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixGeneralStudyModuleAfterGeneralSeriesModule:" << endl;

	// If study id is absent, there is a risk that two studies
	// that are actually different will collide (and get the
	// same UID if using the same -stamp in the same translation
	// session). Use date/time to try to avoid this.

	{
		Attribute *aStudyID=list[TagFromName(StudyID)];

		if (!aStudyID || aStudyID->getVL() == 0) {

			if (aStudyID) {
				list-=TagFromName(StudyID); 
			}

			char *vStudyDate=0;
			Attribute *aStudyDate=list[TagFromName(StudyDate)];
			if (aStudyDate && aStudyDate->getVL() != 0) {
				aStudyDate->getValue(0,vStudyDate);
			}
			char *vStudyTime=0;
			Attribute *aStudyTime=list[TagFromName(StudyTime)];
			if (aStudyTime && aStudyTime->getVL() != 0) {
				aStudyTime->getValue(0,vStudyTime);
			}
			ostrstream oStudyID;
			oStudyID << (vStudyDate ? vStudyDate : "") << (vStudyTime ? vStudyTime : "") << ends;
			char *vStudyID = oStudyID.str();

			// clean out any punctuation ...

			char *src,*dst;
			for (src=vStudyID,dst=vStudyID; *src; ++src) if (isalnum(*src)) *dst++=*src;
			*dst=0;

			// limit to 16 characters max ... (not including trailing \0)

			if (strlen(vStudyID) > 16) vStudyID[16]=0;

			aStudyID=new ShortStringAttribute(TagFromName(StudyID),vStudyID);
			Assert(aStudyID);
			list+=aStudyID;
		}
	}
}

static void
AcrNEMA_FixGeneralSeriesModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixGeneralSeriesModule:" << endl;

	// If series number is absent, use:
	// - the leading numeric part of the study ID, for Siemens images of all modalities
	// - the acquisition number, for Philips MR images (also in the case of Series Number == 1)
	// - else zero

	{
		bool useStudyID=false;
		bool useAcquisitionNumber=false;

		Attribute *aSeriesNumber=list[TagFromName(SeriesNumber)];
		Uint32 vSeriesNumber=0;					// 0 is flag that none was found or was zero length
		if (aSeriesNumber) {
			if (aSeriesNumber->getVL() > 0)
				aSeriesNumber->getValue(0,vSeriesNumber);
			list-=TagFromName(SeriesNumber); 
		}

		Attribute *aModality=list[TagFromName(Modality)];
		char *vModality=0;
		if (aModality && aModality->getVL() > 0)
			(void)aModality->getValue(0,vModality);

		Attribute *aManufacturer=list[TagFromName(Manufacturer)];
		char *vManufacturer=0;
		if (aManufacturer && aManufacturer->getVL() > 0)
			(void)aManufacturer->getValue(0,vManufacturer);

		Attribute *aAcquisitionNumber=list[TagFromName(AcquisitionNumber)];
		char *vAcquisitionNumber=0;
		Uint32 nAcquisitionNumber=0;
		if (aAcquisitionNumber && aAcquisitionNumber->getVL() > 0) {
			(void)aAcquisitionNumber->getValue(0,vAcquisitionNumber);
			if (vAcquisitionNumber && *vAcquisitionNumber && isdigit(*vAcquisitionNumber)) {
				//istrstream istrAcquisitionNumber(vAcquisitionNumber);
				//istrAcquisitionNumber >> nAcquisitionNumber;
				nAcquisitionNumber=atol(vAcquisitionNumber);
//cerr << "AcquisitionNumber leading numeric part=" << nAcquisitionNumber << endl;
			}
		}

		Attribute *aStudyID=list[TagFromName(StudyID)];
		char *vStudyID=0;
		Uint32 nStudyID=0;
		if (aStudyID && aStudyID->getVL() > 0) {
			(void)aStudyID->getValue(0,vStudyID);
			if (vStudyID && *vStudyID && isdigit(*vStudyID)) {
				//istrstream istrStudyID(vStudyID);
				//istrStudyID >> nStudyID;
				nStudyID=atol(vStudyID);
//cerr << "StudyID leading numeric part=" << nStudyID << endl;
			}
		}

		// use Study ID for all Siemens MR if no Series Number

		if (vStudyID && *vStudyID && isdigit(*vStudyID)
		 && !vSeriesNumber
		 && vManufacturer && *vManufacturer && strcmp(vManufacturer,"SIEMENS") == 0
		 && vModality && *vModality && strcmp(vModality,"MR") == 0) {
			useStudyID=true;
		}

		// use Acquisition Number for all Siemens CT if no Series Number,
		// or is 1 and Acquisition Number is "reasonably" low

		if (vAcquisitionNumber && *vAcquisitionNumber
		 && (!vSeriesNumber || (vSeriesNumber == 1 && nAcquisitionNumber < 100))
		 && vManufacturer && *vManufacturer && strcmp(vManufacturer,"SIEMENS") == 0
		 && vModality && *vModality && strcmp(vModality,"CT") == 0) {
			useAcquisitionNumber=true;
		}

		// use Acquisition Number for all Philips MR if no Series Number or is 1

		if (vAcquisitionNumber && *vAcquisitionNumber
		 && (!vSeriesNumber || vSeriesNumber == 1)
		 && vManufacturer && *vManufacturer && strcmp(vManufacturer,"Philips Medical Systems") == 0
		 && vModality && *vModality && strcmp(vModality,"MR") == 0) {
			useAcquisitionNumber=true;
		}

		// getting desperate ... using anything we can find ...

		if (!useStudyID && !useAcquisitionNumber && !vSeriesNumber) {
			if (vAcquisitionNumber && *vAcquisitionNumber) {
				useAcquisitionNumber=true;
			}
			else if (vStudyID && *vStudyID && isdigit(*vStudyID)) {
				useStudyID=true;
			}
		}
				
		Assert(!(useStudyID && useAcquisitionNumber));

		if (useStudyID) {
			vSeriesNumber=nStudyID;
//cerr << "Using StudyID vSeriesNumber" << vSeriesNumber << endl;
		}
		else if (useAcquisitionNumber) {
			vSeriesNumber=nAcquisitionNumber;
//cerr << "Using AcquisitionNumber vSeriesNumber" << vSeriesNumber << endl;
		}

		{
			// Philips MR mixes multiple dynamic scans with same Slice Number
			// in single Acquisition Number ... make into separate series 

			Attribute *aDynamicScanNumber=
				isPrivateCreatorPresent(list,0x0021,0x0010,"PHILIPS MR/PART")
				? list[Tag(0x0021,0x1050)] : 0;

			Uint16 nDynamicScanNumber;

			Attribute *aNumberOfScans=
				isPrivateCreatorPresent(list,0x0019,0x0010,"PHILIPS MR/PART")
				? list[Tag(0x0019,0x101b)] : 0;
			
			Uint16 nNumberOfScans;

			if (aDynamicScanNumber && aDynamicScanNumber->getValue(0,nDynamicScanNumber) && nDynamicScanNumber
			 && aNumberOfScans && aNumberOfScans->getValue(0,nNumberOfScans) && nNumberOfScans > 1
			) {
				vSeriesNumber=vSeriesNumber*1000+nDynamicScanNumber;
			}
		}

		aSeriesNumber=new IntegerStringAttribute(TagFromName(SeriesNumber),vSeriesNumber);
		Assert(aSeriesNumber);
		list+=aSeriesNumber;
	}

	// copy study into series date/time/description if series level absent

	{
		Attribute *aSeriesDate=list[TagFromName(SeriesDate)];

		if (!aSeriesDate || aSeriesDate->getVL() == 0) {
			
			Attribute *aStudyDate=list[TagFromName(StudyDate)];

			if (aStudyDate && aStudyDate->getVL() != 0) {
				char *vStudyDate=0;
				aStudyDate->getValue(0,vStudyDate);

				if (aSeriesDate) {
					list-=TagFromName(SeriesDate); 
				}

				aSeriesDate=new DateStringAttribute(TagFromName(SeriesDate),vStudyDate);
				Assert(aSeriesDate);
				list+=aSeriesDate;
			}

		}
	}

	{
		Attribute *aSeriesTime=list[TagFromName(SeriesTime)];

		if (!aSeriesTime || aSeriesTime->getVL() == 0) {
			
			Attribute *aStudyTime=list[TagFromName(StudyTime)];

			if (aStudyTime && aStudyTime->getVL() != 0) {
				char *vStudyTime=0;
				aStudyTime->getValue(0,vStudyTime);

				if (aSeriesTime) {
					list-=TagFromName(SeriesTime); 
				}

				aSeriesTime=new TimeStringAttribute(TagFromName(SeriesTime),vStudyTime);
				Assert(aSeriesTime);
				list+=aSeriesTime;
			}

		}
	}

	{
		Attribute *aSeriesDescription=list[TagFromName(SeriesDescription)];

		if (!aSeriesDescription || aSeriesDescription->getVL() == 0) {
			
			Attribute *aStudyDescription=list[TagFromName(StudyDescription)];

			if (aStudyDescription && aStudyDescription->getVL() != 0) {
				char *vStudyDescription=0;
				aStudyDescription->getValue(0,vStudyDescription);

				if (aSeriesDescription) {
					list-=TagFromName(SeriesDescription); 
				}

				aSeriesDescription=new LongStringAttribute(TagFromName(SeriesDescription),vStudyDescription);
				Assert(aSeriesDescription);
				list+=aSeriesDescription;
			}

		}
	}

}

static void
AcrNEMA_FixGeneralImageModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixGeneralImageModule:" << endl;
	// If image (instance) number is absent, try and find a private attribute
	// to use

	{
		Attribute *aInstanceNumber=list[TagFromName(InstanceNumber)];

		// Always read Instance Number and rewrite ...
		// (in case non-numeric trailing in some old Philips MR)

		Uint32 vInstanceNumber = 0;						// Defaults to zero rather than absent or zero length

		if (aInstanceNumber) {
			if (aInstanceNumber->getVL() > 0) {
				char  *sInstanceNumber = 0;
				if (aInstanceNumber->getValue(0,sInstanceNumber)) {
					Assert(sInstanceNumber);
					vInstanceNumber=atoi(sInstanceNumber);		// extracts leading numeric part
					if (sInstanceNumber) delete[] sInstanceNumber;	// was a copy
				}
			}
			list-=TagFromName(InstanceNumber);
		}

		// Always replace Instance Number with Philips MR Slice Number if present ...

		Attribute *aSliceNumber=
				isPrivateCreatorPresent(list,0x0021,0x0010,"PHILIPS MR/PART")
				? list[Tag(0x0021,0x1020)] : 0;

		Uint32 vSliceNumber;
			
		if (aSliceNumber && aSliceNumber->getValue(0,vSliceNumber)) {
			vInstanceNumber=vSliceNumber;
		}

		aInstanceNumber=new IntegerStringAttribute(TagFromName(InstanceNumber),vInstanceNumber);	// may be zero
		Assert(aInstanceNumber);
		list+=aInstanceNumber;
	}

	{
		Attribute *aImageType=list[TagFromName(ImageType)];
		if (!aImageType) {
			aImageType=new CodeStringAttribute(TagFromName(ImageType));
			Assert(aImageType);
			aImageType->addValue("ORIGINAL");
			aImageType->addValue("PRIMARY");
			aImageType->addValue("UNKNOWN");
			list+=aImageType;
		}
	}
	
	{
		// if GE CT Delta Start Time is present, add it to Acquisition Time (which is constant for all images in acquisition)
		
		Attribute *aDeltaStartTime=
			isPrivateCreatorPresent(list,0x0043,0x0010,"GEMS_PARM_01")
			? list[Tag(0x0043,0x101e)] : 0;
		double vDeltaStartTime=0;
		if (aDeltaStartTime && aDeltaStartTime->getValue(0,vDeltaStartTime)) {
			vDeltaStartTime*=1000;		// delta start time is in seconds (confirmed by GE, though some conformance statements erroneously claim milliseconds)
			Attribute *aAcquisitionTime=list[TagFromName(AcquisitionTime)];
			char *vAcquisitionTime=0;
			if (aAcquisitionTime && aAcquisitionTime->getVL() != 0 && aAcquisitionTime->getValue(0,vAcquisitionTime) && vAcquisitionTime && strlen(vAcquisitionTime)) {
				Uint32 uAcquisitionTime=Uint32(Time(vAcquisitionTime)) + Uint32(vDeltaStartTime);	// all in milliseconds
				list-=TagFromName(AcquisitionTime);
				list+=new TimeStringAttribute(TagFromName(AcquisitionTime),Time(uAcquisitionTime));
			}
		}
	}
}

static void
AcrNEMA_FixVOILUTModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixVOILUTModule:" << endl;

	// If Window Center or Width are zero length, remove them

	Attribute *aWindowCenter=list[TagFromName(WindowCenter)];
	if (aWindowCenter && aWindowCenter->getVL() == 0) list-=aWindowCenter;

	Attribute *aWindowWidth=list[TagFromName(WindowWidth)];
	if (aWindowWidth && aWindowWidth->getVL() == 0) list-=aWindowWidth;
}

static void
AcrNEMA_FixOverlayModule(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixOverlayModule:" << endl;

	Uint16 group;
	for (group=0x6000; group <= 0x601e; group+=2) {
		Tag tOverlayRows=Tag(group,0x0010);
		Tag tOverlayColumns=Tag(group,0x0011);
		Tag tOverlayType=Tag(group,0x0040);
		Tag tOverlayOrigin=Tag(group,0x0050);
		Tag tOverlayBitsAllocated=Tag(group,0x0100);
		Tag tOverlayBitPosition=Tag(group,0x0102);
		Tag tOverlayLocation=Tag(group,0x0200);
		Tag tOverlayData=Tag(group,0x3000);

		Attribute *aOverlayRows=list[tOverlayRows];
		Attribute *aOverlayColumns=list[tOverlayColumns];
		Attribute *aOverlayType=list[tOverlayType];
		Attribute *aOverlayOrigin=list[tOverlayOrigin];
		Attribute *aOverlayBitsAllocated=list[tOverlayBitsAllocated];
		Attribute *aOverlayBitPosition=list[tOverlayBitPosition];
		Attribute *aOverlayLocation=list[tOverlayLocation];		// not in DICOM
		Attribute *aOverlayData=list[tOverlayData];

		if (aOverlayRows || aOverlayColumns || aOverlayType || aOverlayOrigin
		 || aOverlayBitsAllocated || aOverlayBitPosition || aOverlayLocation
		 || aOverlayData) {

			// we have an overlay

			if (!aOverlayRows) {
				Attribute *aRows=list[TagFromName(Rows)];
				Uint16 vRows;
				if (aRows && aRows->getValue(0,vRows)) {
					aOverlayRows=new UnsignedShortAttribute(tOverlayRows,vRows);
					list+=aOverlayRows;
				}
			}
			if (!aOverlayColumns) {
				Attribute *aColumns=list[TagFromName(Columns)];
				Uint16 vColumns;
				if (aColumns && aColumns->getValue(0,vColumns)) {
					aOverlayColumns=new UnsignedShortAttribute(tOverlayColumns,vColumns);
					list+=aOverlayColumns;
				}
			}
			if (!aOverlayBitsAllocated) {
				if (!aOverlayData) {
					Attribute *aBitsAllocated=list[TagFromName(BitsAllocated)];
					Uint16 vBitsAllocated;
					if (aBitsAllocated && aBitsAllocated->getValue(0,vBitsAllocated)) {
						aOverlayBitsAllocated=new UnsignedShortAttribute(tOverlayBitsAllocated,vBitsAllocated);
						list+=aOverlayBitsAllocated;
					}
				}
				// else don't really know what to do ... use 1 ?
			}
			if (!aOverlayOrigin) {
				aOverlayOrigin=new SignedShortAttribute(tOverlayOrigin);
				aOverlayOrigin->addValue(Uint16(1));
				aOverlayOrigin->addValue(Uint16(1));
				list+=aOverlayOrigin;
			}
		}
	}
}

static void
AcrNEMA_FixAllTimes(ManagedAttributeList &list)
{
//cerr << "AcrNEMA_FixAllTimes:" << endl;

	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);

		Tag tag=a->getTag();

		const char *vr=a->getVR();
		Assert(vr);

		if (strcmp(vr,"TM") == 0) {
			char *v=0;
			if (a && a->getValue(0,v) && v) {

//cerr << "AcrNEMA_FixAllTimes: old string " << v << endl;

				Time t(v);

//cerr << "AcrNEMA_FixAllTimes: time isgood " << (t.isgood() ? "t" : "f") << endl;

				// following is from attrtypd.cc (should really be method in Time class :( )

				ostrstream ost;
				ost << dec << setfill('0');
				if (t.isgood()) {
					ost << setw(2) << t.getHour()
					    << setw(2) << t.getMinute()
					    << setw(2) << t.getSecond();
					if (t.getMilliSecond())
						ost << '.' << setw(3) << t.getMilliSecond();
				}
				ost << ends;
				char *s = ost.str();

//cerr << "AcrNEMA_FixAllTimes: new string " << s << endl;

				a->setValue(0,s);
				if (s) delete[] s;
			}
			if (v) delete[] v;
		}

		++listi;
	}
}



static void
AcrNEMAorSPItoDICOM(ManagedAttributeList &list)
{
//cerr << "AcrNEMAorSPItoDICOM:" << endl;
	AcrNEMA_FixPatientModule(list);
	AcrNEMA_FixGeneralSeriesModule(list);
	AcrNEMA_FixGeneralStudyModuleAfterGeneralSeriesModule(list);
	AcrNEMA_FixGeneralImageModule(list);
	AcrNEMA_FixImagePixelModule(list);

	char *vModality=AttributeValue(list[TagFromName(Modality)]);

	if (vModality && (strcmp(vModality,"MR") == 0 || strcmp(vModality,"CT") == 0)) {
		AcrNEMA_FixImagePlaneModule(list);
		AcrNEMA_FixFrameOfReferenceModule(list);
	}
	if (vModality && strcmp(vModality,"MR") == 0) {
		AcrNEMA_FixMRImageModule(list);
	}
	if (vModality && strcmp(vModality,"CT") == 0) {
		AcrNEMA_FixCTImageModule(list);
	}

	AcrNEMA_FixLaterality(list);
	AcrNEMA_FixVOILUTModule(list);
	AcrNEMA_FixOverlayModule(list);

	AcrNEMA_FixAllTimes(list);
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

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
		log << list.errors()
		    << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	else {
		// Don't write list warnings yet ...
		// done in usualManagedAttributeListWrite ...

		AcrNEMAorSPItoDICOM(list);

		if (!usualManagedAttributeListWrite(list,dout,
			dicom_output_options,log,verbose)) success=false;
	}

	return success ? 0 : 1;
}

	
