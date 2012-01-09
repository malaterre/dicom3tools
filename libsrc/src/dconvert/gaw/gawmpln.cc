//#include <iostream.h>
//#include <iomanip.h>

#include "gawdc.h"
#include "elmconst.h"
#include "planea.h"

void 
GAW_Header_BothClass::ToDicom_ManualPlane(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// Trying to derive Image Plane Module ...

	char *hfff,*ap;

	ImagePlaneHeadFeet headfeet;

	switch (GAW_HeaderInstance_SERIESHDR->SE_entry) {
		case 1:
				hfff="HF";
				headfeet=HeadFirst;
				break;
		case 2:
				hfff="FF";
				headfeet=FeetFirst;
				break;
		default:	hfff="UNKNOWN";		// :(
				headfeet=HeadFirst;
				break;
	}

	ImagePlanePosition position;

	switch (GAW_HeaderInstance_SERIESHDR->SE_position) {
		case 1:
				ap="S";
				position=Supine;
				break;
		case 2:
				ap="P";
				position=Prone;
				break;
		case 4:
				ap="DL";
				position=LeftLateralDecubitus;
				break;
		case 8:
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
	double brhc_R;
	double brhc_A;
	double brhc_S;
	if (GAW_isct) {
		tlhc_R=GAW_HeaderInstance_CTHDR->CT_tlhc_R;
		tlhc_A=GAW_HeaderInstance_CTHDR->CT_tlhc_A;
		tlhc_S=GAW_HeaderInstance_CTHDR->CT_tlhc_S;
		trhc_R=GAW_HeaderInstance_CTHDR->CT_trhc_R;
		trhc_A=GAW_HeaderInstance_CTHDR->CT_trhc_A;
		trhc_S=GAW_HeaderInstance_CTHDR->CT_trhc_S;
		brhc_R=GAW_HeaderInstance_CTHDR->CT_brhc_R;
		brhc_A=GAW_HeaderInstance_CTHDR->CT_brhc_A;
		brhc_S=GAW_HeaderInstance_CTHDR->CT_brhc_S;
	}
	if (GAW_ismr) {
		tlhc_R=GAW_HeaderInstance_MRHDR->MR_tlhc_R;
		tlhc_A=GAW_HeaderInstance_MRHDR->MR_tlhc_A;
		tlhc_S=GAW_HeaderInstance_MRHDR->MR_tlhc_S;
		trhc_R=GAW_HeaderInstance_MRHDR->MR_trhc_R;
		trhc_A=GAW_HeaderInstance_MRHDR->MR_trhc_A;
		trhc_S=GAW_HeaderInstance_MRHDR->MR_trhc_S;
		brhc_R=GAW_HeaderInstance_MRHDR->MR_brhc_R;
		brhc_A=GAW_HeaderInstance_MRHDR->MR_brhc_A;
		brhc_S=GAW_HeaderInstance_MRHDR->MR_brhc_S;
	}

	// DICOM plane definitions are:
	// - Left Pos Superior +ve

	// The Genesis co-ordinates are:
	// - Right Ant Superior +ve
	// - Corners offset by center co-ordinates
	// - already account for offset from isocenter/reference

	// (NB. no effort is made to fix the sign of SliceLocation)

	// Change sign of AP and LR from Genesis to DICOM...

	Point3D TLHC(-tlhc_R,-tlhc_A, tlhc_S);
	Point3D TRHC(-trhc_R,-trhc_A, trhc_S);
	Point3D BRHC(-brhc_R,-brhc_A, brhc_S);

	imageplane.PatientPlane::setCorners(TLHC,
					    TRHC-TLHC,	// Row Vector
					    BRHC-TRHC);	// Col Vector

	// Now build the DICOM attributes ...

	(*list)+=new CodeStringAttribute(TagFromName(PatientPosition),
		String_Cat_Use(hfff,ap));

	ImagePlaneLister ipl(imageplane);
	ipl.addPlaneToList(*list);


	// PixelSpacing
	// ReconstructionDiameter

	double 	dfov       = 0;
	double 	dfov_rect  = 0;
	double 	pixsize_X  = 0;
	double 	pixsize_Y  = 0;
	if (GAW_isct) {
		dfov     =GAW_HeaderInstance_CTHDR->CT_dfov;
		dfov_rect=GAW_HeaderInstance_CTHDR->CT_dfov_rect;
		pixsize_X=GAW_HeaderInstance_CTHDR->CT_pixsize_X;
		pixsize_Y=GAW_HeaderInstance_CTHDR->CT_pixsize_Y;
	}
	if (GAW_ismr) {
		dfov     =GAW_HeaderInstance_MRHDR->MR_dfov;
		dfov_rect=GAW_HeaderInstance_MRHDR->MR_dfov_rect;
		pixsize_X=GAW_HeaderInstance_MRHDR->MR_pixsize_X;
		pixsize_Y=GAW_HeaderInstance_MRHDR->MR_pixsize_Y;
	}

	(*list)+=new DecimalStringAttribute(TagFromName(PixelSpacing),
					pixsize_X,pixsize_Y);

	// which Gawesis tags to use for ReconstructionDiameter ?

	// sample gawesis mr sagittal:

	// Image matrix size - X  <256>
	// Image matrix size - Y  <256>
	// Display field of view - X (mm) (MR_dfov)       <240.000000>
	// Display field of view - Y (if different) (MR_dfov_rect)        <240.000000>
	// Image dimension - X    <256.000000>
	// Image dimension - Y    <192.000000>
	// Image pixel size - X   <0.937500>
	// Image pixel size - Y   <0.937500>

	// sample gawesis ct scout:

	// Image matrix size - X  <512>
	// Image matrix size - Y  <197>
	// Display field of view - X (mm) (CT_dfov)       <496.375183>
	// Display field of view - Y (if different) (CT_dfov_rect)        <199.890442>
	// Image dimension - X    <496.375183>
	// Image dimension - Y    <199.890442>
	// Image pixel size - X   <0.969483>
	// Image pixel size - Y   <1.014672>
	// Scan field of view (mm)        <496.369995>

	// it seems that imatrix_X*pixsize_X=dfov
	// and           imatrix_Y*pixsize_Y=dfov_rect
	//
	// furthermore this matches the image co-ordinate values
	//
	// so use average or max of dfov and dfov_rect
	//
	// what then are dim_X and dim_Y ? (they are acquisition matrix)

	// NB. the DataCollectionDiameter is done for CT in the
	// template using CT_sfovmm

	// NB. this highlights the fact that DICOM is wrong to
	// use just a simple diameter and not a multivalued
	// dimension ... therefore to be useful one has to use
	// the pixel spacing times matrix size to be safe !

	// should this be average or max  ... use max ?

	(*list)+=new
		DecimalStringAttribute(TagFromName(ReconstructionDiameter),
			(dfov > dfov_rect) ? dfov : dfov_rect);
}

