//#include <iostream.h>
//#include <iomanip.h>

#include "shimdc.h"
#include "elmconst.h"
#include "planea.h"

void 
SHIM_Header_BothClass::ToDicom_ManualPlane(AttributeList *list,unsigned imagenumber)
{
	// Not yet implemented
	(void)list; (void)imagenumber;
#ifdef CRAP
	// PixelSpacing

	(*list)+=new DecimalStringAttribute(TagFromName(PixelSpacing),
		SHIM_HeaderInstance_FILEHDR->
			SHIM_Method_ExtractTaggedFloat("PIXLEN"),
		SHIM_HeaderInstance_FILEHDR->
			SHIM_Method_ExtractTaggedFloat("PIXLEN"));

	// ZoomFactor

	double zoomfactor=SHIM_HeaderInstance_SLICEHDR->SHIM_Method_ExtractTaggedFloat(imagenumber,"ZOOM");

	if (fabs(zoomfactor) < .0001) zoomfactor=1;	// ie. zero

	(*list)+=new DecimalStringAttribute(TagFromName(ZoomFactor),zoomfactor,zoomfactor);

	// ZoomCenter

	double reconcenterhorizontal=SHIM_HeaderInstance_SLICEHDR->SHIM_Method_ExtractTaggedFloat(imagenumber,"XORG");
	double reconcentervertical  =SHIM_HeaderInstance_SLICEHDR->SHIM_Method_ExtractTaggedFloat(imagenumber,"YORG");

	(*list)+=new DecimalStringAttribute(TagFromName(ZoomCenter),reconcenterhorizontal,reconcentervertical);

	// SliceLocation

	double slicelocation=SHIM_HeaderInstance_SLICEHDR->SHIM_Method_ExtractTaggedInteger(imagenumber,"INOUT")/10.0;

	int irep=SHIM_HeaderInstance_FILEHDR->SHIM_Method_ExtractTaggedInteger("IREP");
	if (irep == 3) {
		// Multi-Slice mode
		double targetoffset;
		int target=SHIM_HeaderInstance_SLICEHDR->SHIM_Method_ExtractTaggedInteger(imagenumber,"ITARGET");
		switch (target) {
			case 'A':	targetoffset=40;
					break;
			case 'B':	targetoffset=20;
					break;
			case 'C':	targetoffset=0;
					break;
			case 'D':	targetoffset=-20;
					break;
			default:	// blech :(
					targetoffset=0;
					break;
		}
		int ring=SHIM_HeaderInstance_SLICEHDR->SHIM_Method_ExtractTaggedInteger(imagenumber,"IRING");
		double detectoroffset=(ring%2)*8;

		slicelocation+=targetoffset+detectoroffset;
	}
	// assume IREP == 6, Single-Slice mode, which just uses INOUT and is relative to Target C, Detector Ring 2

	(*list)+=new DecimalStringAttribute(TagFromName(SliceLocation),slicelocation);

	// Trying to derive Image Plane Module ...

	ImagePlanePlane plane=Axial;

	char *hfff,*ap;
	ImagePlaneHeadFeet orientation;
	ImagePlanePosition position;

	// The Imatron may already have flipped the image pixel data,
	// so record the position and orientation in the descriptive DICOM tags
	// but tell the ImagePlane module generator the flipped not the physical position

	// Is this going to mess up the relationship between machine plane and patient plane ?

	switch (SHIM_HeaderInstance_SLICEHDR->
			SHIM_Method_ExtractTaggedInteger(imagenumber,"IPATOR")) {

		case -5:	// Supine feet first, flipped to match supine head first
			ap="S";
			position=Supine;
			hfff="FF";
			orientation=HeadFirst;	// Not FeetFirst ... already flipped
			break;

		case -6:	// Prone feet first, flipped to match prone head first
			ap="P";
			position=Prone;
			hfff="FF";
			orientation=HeadFirst;	// Not FeetFirst ... already flipped
			break;

		case -7:	// Decubitus right feet first, flipped to match decubitus right head first
			ap="DR";
			position=RightLateralDecubitus;
			hfff="FF";
			orientation=HeadFirst;	// Not FeetFirst ... already flipped
			break;

		case -8:	// Decubitus left feet first, flipped to match decubitus left head first
			ap="DL";
			position=LeftLateralDecubitus;
			hfff="FF";
			orientation=HeadFirst;	// Not FeetFirst ... already flipped
			break;

		case  -1:	// Supine feet first
			ap="S";
			position=Supine;
			hfff="FF";
			orientation=FeetFirst;
			break;

		case  -2:	// Prone feet first
			ap="P";
			position=Prone;
			hfff="FF";
			orientation=FeetFirst;
			break;

		case  -3:	// Decubitus right feet first
			ap="DR";
			position=RightLateralDecubitus;
			hfff="FF";
			orientation=FeetFirst;
			break;

		case  -4:	// Decubitus left feet first
			ap="DL";
			position=LeftLateralDecubitus;
			hfff="FF";
			orientation=FeetFirst;
			break;
		case  1:	// Supine head first
			ap="S";
			position=Supine;
			hfff="HF";
			orientation=HeadFirst;
			break;

		case  2:	// Prone head first
			ap="P";
			position=Prone;
			hfff="HF";
			orientation=HeadFirst;
			break;

		case  3:	// Decubitus right head first
			ap="DR";
			position=RightLateralDecubitus;
			hfff="HF";
			orientation=HeadFirst;
			break;

		case  4:	// Decubitus left head first
			ap="DL";
			position=LeftLateralDecubitus;
			hfff="HF";
			orientation=HeadFirst;
			break;

		case  5:	// Prone head first flipped (assume to match supine ? not specified in format)
			ap="P";
			position=Supine;	// Not Prone ... already flipped
			hfff="HF";
			orientation=HeadFirst;

		case  0:	// Special case :(
			Assert(0);
			ap="S";
			position=Supine;
			hfff="HF";
			orientation=HeadFirst;
			break;
	}

	double scanfov=SHIM_HeaderInstance_SLICEHDR->SHIM_Method_ExtractTaggedFloat(imagenumber,"PICRAD");

	// Make a model of the image plane ...

	ImagePlane imageplane(plane,scanfov,orientation,position);

	// While we are in the "display" or "gantry plane ...
	// (before angulation) 

	// Apply any transformations specified in the
	// display/gantry plane, such as magnification and
	// reconstruction target offsets ...

	// Assume that these are machine rather than body relative	DON'T KNOW IF THIS IS CORRECT :(

//	imageplane.MachinePlane::scale(zoomfactor);			// Leave out - assume for now that PICRAD is DFOV

	// Always axial so X and Y are really X and Y
	// Don't forget sign changes ...				DON'T KNOW IF THIS IS CORRECT :(

	Vector3D reconoffset(reconcenterhorizontal,reconcentervertical,0);

	imageplane.MachinePlane::shift(reconoffset);

	// Account for angulation ...

	double gantrytilt=SHIM_HeaderInstance_SLICEHDR->SHIM_Method_ExtractTaggedInteger(imagenumber,"ITILT");

	// +ve is top towards table					DON'T KNOW IF THIS IS CORRECT :(
	imageplane.MachinePlane::angle(LeftRight,gantrytilt);

	// Account for physical offsets after angulation

	// Assume no sign difference Shim vs. Dicom for Z		DON'T KNOW IF THIS IS CORRECT :(
	// Assume that these are machine rather than body relative	DON'T KNOW IF THIS IS CORRECT :(

	Vector3D offset(0,0,slicelocation);
	imageplane.MachinePlane::shift(offset);

	// Now build the DICOM attributes ...

	(*list)+=new CodeStringAttribute(TagFromName(PatientPosition),String_Cat_Use(hfff,ap));

	ImagePlaneLister ipl(imageplane);
	ipl.addPlaneToList(*list);

#endif /* CRAP */
}

