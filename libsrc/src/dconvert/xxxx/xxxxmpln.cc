//#include <iostream.h>
//#include <iomanip.h>

#include "xxxxdc.h"
#include "elmconst.h"
#include "planea.h"

void 
XXXX_Header_BothClass::ToDicom_ManualPlane(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// Trying to derive Image Plane Module ...

	char *hfff,*ap;

	ImagePlanePlane plane;

	// Determine the "machine" plane of the image ...

	// (Doesn't handle scout azimuth yet)
	// Same for CT and MR (CT seems to set IMTYPE "AX  ")
	switch (((char *)(XXXX_HeaderInstance_IMGHDR->IPLANE))[0]) {
		case 'A':	// "AX  "
			plane=Axial;
			break;
		case 'S':	// "SAG "
			plane=Sagittal;
			break;
		case 'C':	// "COR "
			plane=Coronal;
			break;
		default:	// Yuck - just in case :(
			plane=Axial;
			break;
	}

	ImagePlaneHeadFeet orientation;

	switch (((char *)(XXXX_HeaderInstance_IMGHDR->IHFFIRST))[0]) {
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

	switch (((char *)(XXXX_HeaderInstance_IMGHDR->IPATORI))[0]) {
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

	double scanfov = atof(XXXX_HeaderInstance_IMGHDR->ISFOV)*10;	// cm -> mm

	// Make a model of the image plane ...

//cerr << "ToDicom_ManualPlane::Creating ImagePlanet" << endl;

	ImagePlane imageplane(plane,scanfov,orientation,position);

//cerr << "ToDicom_ManualPlane::Creating ImagePlane done" << endl;
//imageplane.put(cerr);

	// While we are in the "display" or "gantry plane ...
	// (before angulation) 

	// Apply any transformations specified in the
	// display/gantry plane, such as magnification and
	// reconstruction target offsets ...

	// Assume that these are machine rather than body relative

	double reconfov	= atof(XXXX_HeaderInstance_IMGHDR2->IRFOV)*10;	// cm -> mm
	double reconch	= XXXX_HeaderInstance_IMGHDR2->IRCENTX;
	double reconcv	= XXXX_HeaderInstance_IMGHDR2->IRCENTY;

	double magfact 	= XXXX_HeaderInstance_IMGHDR2->IMAGFACT;
	double magch	= XXXX_HeaderInstance_IMGHDR2->IMAGCX;
	double magcv	= XXXX_HeaderInstance_IMGHDR2->IMAGCY;

	Vector3D reconoffset;
	Vector3D magoffset;

	// Determine which axes are "horizontal" & "vertical" ...
	// Don't forget sign changes ...

	switch (plane) {
		case Axial:
			reconoffset=Vector3D(reconch,-reconcv,0);
			magoffset  =Vector3D(magch,    -magcv,0);
			break;
		case Sagittal:
			reconoffset=Vector3D(0,-reconch,reconcv);
			magoffset  =Vector3D(0,  -magch,  magcv);
			break;
		case Coronal:
			reconoffset=Vector3D(reconch,0,reconcv);
			magoffset  =Vector3D(magch  ,0,  magcv);
			break;
	}

//cerr << "ToDicom_ManualPlane::reconoffset is:" << endl;
//reconoffset.put(cerr);
//cerr << "ToDicom_ManualPlane::magoffset is:" << endl;
//magoffset.put(cerr);

//cerr << "ToDicom_ManualPlane::scale:" << endl;
	imageplane.MachinePlane::scale(magfact*reconfov/scanfov);
//cerr << "ToDicom_ManualPlane::scale result:" << endl;
//imageplane.put(cerr);
//cerr << "ToDicom_ManualPlane::shift by reconoffset:" << endl;
	imageplane.MachinePlane::shift(reconoffset);
//cerr << "ToDicom_ManualPlane::shift by reconoffset result:" << endl;
//imageplane.put(cerr);
//cerr << "ToDicom_ManualPlane::shift by magoffset:" << endl;
	imageplane.MachinePlane::shift(magoffset);
//cerr << "ToDicom_ManualPlane::shift by magoffset result:" << endl;
//imageplane.put(cerr);

	// Account for angulation ...

	if (XXXX_isct) {
//cerr << "ToDicom_ManualPlane::angling:" << endl;
#ifdef XXXXTILTCTUSINGMACHINECORDS
		// +ve is top towards table
		// whereas Z is positive into gantry so change sign
		imageplane.MachinePlane::angle(
			LeftRight,-XXXX_HeaderInstance_CTHDR->IGTILT);
#else // XXXXTILTCTUSINGMACHINECORDS
		// +ve is always towards front of head
		imageplane.PatientPlane::angle(
			LeftRight,atof(XXXX_HeaderInstance_CTHDR->IGTILTB));
#endif // XXXXTILTCTUSINGMACHINECORDS
//cerr << "ToDicom_ManualPlane::angling result:" << endl;
//imageplane.put(cerr);
	}

	if (XXXX_ismr) {
		// Not yet tested ... no samples available ...

		// Tilt is specified as none or relative to axis
		// Axis+/-Angle [xx+/-xx]
		// NB. in body not machine coordinates **********

		char *str=XXXX_HeaderInstance_MRHDR->IMRTILT;
		char axis[3]; strncpy(axis,str,2); axis[2]=0;
		int angle=str[2] ? atoi(str+2) : 0;

		// in Japanese docs, text contradicts drawing so
		// go with drawing ...

		ImagePlaneAxis planeaxis;

		if (angle) {
			switch (axis[0]) {
				case 'A':
				case 'P':
					planeaxis=AnteriorPosterior;
					// Xxxx CW +ve facing posterior
					// angle=angle;
					break;
				case 'R':
				case 'L':
					planeaxis=LeftRight;
					// Xxxx CW +ve facing right lat
					angle=-angle;
					break;
				case 'H':
				case 'F':
					planeaxis=HeadFeet;
					// Xxxx CW +ve facing head
					// angle=angle;
					break;
			}
		}
		imageplane.PatientPlane::angle(planeaxis,angle);
	}

	// Account for physical offsets in the body plane ...
	// (after angulation)

	// For Xxxx the only difference between slices seems to be
	// in the "Slice position by body coords" (ISLPOSxx) fields
	// These do NOT include recon center offset and gantry tilt

	// NB. The "Image center machine coords" (IIMPOSx) already
	// account for both the recon center offset and the gantry
	// tilt. They do NOT however handle the change between
	// slices in the Z direction (what I would expect to be
	// in ITBLPOS but is not ... it is zero in every slice :()
	// so we can't use them.

	double xcenter=XXXX_HeaderInstance_IMGHDR->ISLPOSLR;
	double ycenter=XXXX_HeaderInstance_IMGHDR->ISLPOSAP;
	double zcenter=XXXX_HeaderInstance_IMGHDR->ISLPOSHF;

	// Consider sign difference Xxxx vs. Dicom for Y

//cerr << "ToDicom_ManualPlane::physical offset is" << endl;
	Vector3D offset(xcenter,-ycenter,zcenter);
//offset.put(cerr);
//cerr << "ToDicom_ManualPlane::shift by physical offset" << endl;
	imageplane.PatientPlane::shift(offset);
//cerr << "ToDicom_ManualPlane::shift by physical offset result:" << endl;
//imageplane.put(cerr);

	// Now build the DICOM attributes ...

	(*list)+=new CodeStringAttribute(TagFromName(PatientPosition),
		String_Cat_Use(hfff,ap));

//cerr << "ToDicom_ManualPlane::addPlaneToList:" << endl;
	ImagePlaneLister ipl(imageplane);
	ipl.addPlaneToList(*list);
//cerr << "ToDicom_ManualPlane::addPlaneToList done:" << endl;

	// SliceLocation

	// ITBLPOS always seems to be zero, so use IPLANE to
	// decide which slice position value to use

	// Xxxx slice position values are in body coordinates.
	// Neither DICOM PS3 nor ACR/NEMA 2 define sign of
	// SliceLocation, but seeing as it is an old element
	// it seems appropriate to use machine coordinates

	// Don't want to use the angled, magnified, targeted
	// imageplane because it is in the axis of the
	// image center rather than the axis at isocenter
	// and is slightly different from the conventional
	// interpretation of table position if an offset
	// in the display/gantry plane has been applied.

	// So set up a new imageplane just to convert from
	// body to machine cordinates, and apply physical
	// offset without angulation or recon or zoom ...

//cerr << "ToDicom_ManualPlane::Use a new image plane for rest:" << endl;

	ImagePlane locationimageplane(plane,scanfov,orientation,position);

	locationimageplane.PatientPlane::shift(offset);

	Point3D center=locationimageplane.MachinePlane::getCenter();

	double location;

	switch (plane) {
		case Axial:
			location=center.getZ();
			break;
		case Sagittal:
			location=center.getX();
			break;
		case Coronal:
			location=center.getY();
			break;
	}
	(*list)+=new DecimalStringAttribute(TagFromName(SliceLocation),location);

	// TableHeight

	// DICOM ...+ve is below center
	// Xxxx is in machine co-ordinates ... +ve is up

	if (XXXX_isct) (*list)+=new
		DecimalStringAttribute(TagFromName(TableHeight),
			double(-double(XXXX_HeaderInstance_CTHDR->ITBLHGT)));

	// PixelSpacing

	(*list)+=new DecimalStringAttribute(TagFromName(PixelSpacing),
		XXXX_HeaderInstance_IMGHDR2->IPIXSIZE,
		XXXX_HeaderInstance_IMGHDR2->IPIXSIZE);

	// ZoomFactor

	(*list)+=new DecimalStringAttribute(TagFromName(ZoomFactor),
		XXXX_HeaderInstance_IMGHDR2->IMAGFACT,
		XXXX_HeaderInstance_IMGHDR2->IMAGFACT);

	// ZoomCenter

	(*list)+=new DecimalStringAttribute(TagFromName(ZoomCenter),
			XXXX_HeaderInstance_IMGHDR2->IMAGCX,
			XXXX_HeaderInstance_IMGHDR2->IMAGCY);

	// ReconstructionDiameter

	(*list)+=new DecimalStringAttribute(TagFromName(ReconstructionDiameter),
		atof(XXXX_HeaderInstance_IMGHDR2->IRFOV)*10);

	// DataCollectionDiameter

	(*list)+=new DecimalStringAttribute(TagFromName(DataCollectionDiameter),
		atof(XXXX_HeaderInstance_IMGHDR->ISFOV)*10);
}

