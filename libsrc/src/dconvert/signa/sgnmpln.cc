//#include <iostream.h>
//#include <iomanip.h>

#include "sgndc.h"
#include "elmconst.h"
#include "planea.h"

void 
SGN_Header_BothClass::ToDicom_ManualPlane(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// Trying to derive Image Plane Module ...

	char *hfff,*ap;

	ImagePlaneHeadFeet headfeet;

	switch (SGN_HeaderInstance_SERIESHDR->PatientPosition) {
		case 0:
				hfff="HF";
				headfeet=HeadFirst;
				break;
		case 1:
				hfff="FF";
				headfeet=FeetFirst;
				break;
		default:	hfff="UNKNOWN";		// :(
				headfeet=HeadFirst;
				break;
	}

	ImagePlanePosition position;

	switch (SGN_HeaderInstance_SERIESHDR->PatientOrientation) {
		case 0:
				ap="S";
				position=Supine;
				break;
		case 1:
				ap="P";
				position=Prone;
				break;
		case 2:
				ap="DL";
				position=LeftLateralDecubitus;
				break;
		case 3:
				ap="DR";
				position=RightLateralDecubitus;
				break;
		default:	ap="UNKNOWN";		// :(
				position=Supine;
				break;
	}

	ImagePlane imageplane(headfeet,position);

	// get common definitions for CT or MR

	double tlhc_R;
	double tlhc_A;
	double tlhc_S;
	double trhc_R;
	double trhc_A;
	double trhc_S;
	double blhc_R;
	double blhc_A;
	double blhc_S;
	{
		tlhc_R=SGN_HeaderInstance_IMAGEHDR->TLHC_R;
		tlhc_A=SGN_HeaderInstance_IMAGEHDR->TLHC_A;
		tlhc_S=SGN_HeaderInstance_IMAGEHDR->TLHC_S;
		trhc_R=SGN_HeaderInstance_IMAGEHDR->TRHC_R;
		trhc_A=SGN_HeaderInstance_IMAGEHDR->TRHC_A;
		trhc_S=SGN_HeaderInstance_IMAGEHDR->TRHC_S;
		blhc_R=SGN_HeaderInstance_IMAGEHDR->BLHC_R;
		blhc_A=SGN_HeaderInstance_IMAGEHDR->BLHC_A;
		blhc_S=SGN_HeaderInstance_IMAGEHDR->BLHC_S;
	}

	// DICOM plane definitions are:
	// - Left Pos Superior +ve

	// The Genesis co-ordinates are:
	// - Right Ant Superior +ve
	// - Corners offset by center co-ordinates
	// - already account for offset from isocenter/reference

	// (NB. no effort is made to fix the sign of SliceLocation)

	// Change sign of AP and LR from Signa to DICOM...

	Point3D TLHC(-tlhc_R,-tlhc_A, tlhc_S);
	Point3D TRHC(-trhc_R,-trhc_A, trhc_S);
	Point3D BLHC(-blhc_R,-blhc_A, blhc_S);

	imageplane.PatientPlane::setCorners(TLHC,
					    TRHC-TLHC,	// Row Vector
					    BLHC-TLHC);	// Col Vector

	// Now build the DICOM attributes ...

	(*list)+=new CodeStringAttribute(TagFromName(PatientPosition),
		String_Cat_Use(hfff,ap));

	ImagePlaneLister ipl(imageplane);
	ipl.addPlaneToList(*list);


	// PixelSpacing

	double 	pixsize_X  = SGN_HeaderInstance_IMAGEHDR->PixelSize;
	double 	pixsize_Y  = SGN_HeaderInstance_IMAGEHDR->PixelSize;

	(*list)+=new DecimalStringAttribute(TagFromName(PixelSpacing),
					pixsize_X,pixsize_Y);

}

