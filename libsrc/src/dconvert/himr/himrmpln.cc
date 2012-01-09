//#include <iostream.h>
//#include <iomanip.h>

#include "himrdc.h"
#include "elmconst.h"
#include "planea.h"

void 
HIMR_Header_BothClass::ToDicom_ManualPlane(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// Trying to derive Image Plane Module ...

	char *hfff,*ap;

	ImagePlanePlane plane;

	// Determine the "machine" plane of the image ...

	switch (((char *)(HIMR_HeaderInstance_IMGHDR->MISLPL))[0]) {
		case 'T':
			plane=Axial;
			break;
		case 'S':
			plane=Sagittal;
			break;
		case 'C':
			plane=Coronal;
			break;
		default:	// Yuck - just in case :(
			plane=Axial;
			break;
	}

	ImagePlaneHeadFeet orientation;

	switch (((char *)(HIMR_HeaderInstance_IMGHDR->XIPSD))[0]) {
		case 'H':
			hfff="HF";
			orientation=HeadFirst;
			break;
		case 'F':
			hfff="FF";
			orientation=FeetFirst;
			break;
		default:
			hfff="";		// :(
			orientation=HeadFirst;
			break;
	}

	ImagePlanePosition position;

	switch (((char *)(HIMR_HeaderInstance_IMGHDR->XIPSP))[0]) {
		case 'P':
			ap="P";
			position=Prone;
			break;
		case 'S':
			ap="S";
			position=Supine;
			break;
		case 'L':
			ap="DL";
			position=LeftLateralDecubitus;
			break;
		case 'R':
			ap="DR";
			position=RightLateralDecubitus;
			break;
		default:
			ap="";			// :(
			position=Supine;
			break;
	}

	(*list)+=new CodeStringAttribute(TagFromName(PatientPosition),
		String_Cat_Use(hfff,ap));


	double scanfov = atof(HIMR_HeaderInstance_IMGHDR->XIDFOV);	// mm

	// Make a model of the image plane ...

	ImagePlane imageplane(plane,scanfov,orientation,position);

//cerr << "ToDicom_ManualPlane::Creating ImagePlane done" << endl;
//imageplane.put(cerr);

	double location=(double)atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XIPOS))/10.0;	// header value in 0.1 mm

	// Have no idea of the signs and whether in patient or machine space

	double xcenter=0;
	double ycenter=0;
	double zcenter=0;

	switch (plane) {
		case Axial:	zcenter=location;		// should account for left/right flip view top or bottom
				break;
		case Coronal:	ycenter=location;
				break;
		case Sagittal:	xcenter=location;
				break;
	}

	Vector3D offset(xcenter,ycenter,zcenter);
	imageplane.MachinePlane::shift(offset);

	// Haven't accounted for angulation

	ImagePlaneLister ipl(imageplane);
	ipl.addPlaneToList(*list);

	// SliceLocation

	(*list)+=new DecimalStringAttribute(TagFromName(SliceLocation),location);

	// PixelSpacing

	double original_xsize=double(HIMR_HeaderInstance_IMGHDR->XIPSX)/1000.0;
	double original_ysize=double(HIMR_HeaderInstance_IMGHDR->XIPSY)/1000.0;
	double mag_factor=double(HIMR_HeaderInstance_IMGHDR->XIMAG)/1000.0;

	(*list)+=new DecimalStringAttribute(TagFromName(PixelSpacing),
		original_ysize*mag_factor,
		original_xsize*mag_factor);

	// PositionReferenceIndicator

	(*list)+=new LongStringAttribute(TagFromName(PositionReferenceIndicator));

}

