//#include <iostream.h>
//#include <iomanip.h>

#include "ge9800dc.h"
#include "elmconst.h"
#include "planea.h"

void 
GE9800_Header_BothClass::ToDicom_ManualPlane(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// Trying to derive Image Plane Module ...

	char *hfff,*ap;

	// GE 9800 plane definitions are unknown:
	// ? units of X,Y center
	// ? direction of table position
	// assume Left Ant Superior +ve (for no really good reason)

	// DICOM plane definitions are:
	// - Left Pos Superior +ve

	// Determine the plane of the image ...
	// There is really no valid way to do this for other
	// than Prospective & Scout views, so presume axial

	ImagePlanePlane plane;

	// Determine the "machine" plane of the image ...

		switch (GE9800_HeaderInstance_IMAGE1HDR->FileType) {
			case 1:	// Prospective
					plane=Axial;
					break;

			case 2:	// Scout - use tube azimuth later
					// 0 degrees is AP Scout
					plane=Coronal;
		// this is theoretically ok, but 9800 seems to rotate
		// the lateral scout to F/P rather than A/F :(
		// need to do some tests to see in which cases this happens
					break;

			case 3:		// Retrospective
			case 4:		// Segmented
			case 5:		// Screensave
			case 6:		// Plot
			default:	// :(
					plane=Axial;
					break;
		}

	ImagePlaneHeadFeet orientation;

		switch (GE9800_HeaderInstance_IMAGE1HDR->PatientOrientation) {
			case 1:
					hfff="HF";
					orientation=HeadFirst;
					break;
			case 2:
					hfff="FF";
					orientation=FeetFirst;
					break;
			default:	hfff="";		// :(
					orientation=HeadFirst;
					break;
		}

	ImagePlanePosition position;

		switch (GE9800_HeaderInstance_IMAGE1HDR->APPatientOrientation) {
			case 1:
					ap="P";
					position=Prone;
					break;
			case 2:
					ap="S";
					position=Supine;
					break;
			case 3:
					ap="DL";
					position=LeftLateralDecubitus;
					break;
			case 4:
					ap="DR";
					position=RightLateralDecubitus;
					break;
			default:	ap="";			// :(
					position=Supine;
					break;
		}

		double xscanfov;
		double yscanfov;

		if (GE9800_HeaderInstance_IMAGE1HDR->FileType == 2) {	// Scout
			// Could use CT98_ViewsPerScan.toUS() * pixsize ...

			yscanfov = GE9800_HeaderInstance_IMAGE1HDR->ScoutEndLocation - GE9800_HeaderInstance_IMAGE1HDR->SlicePosition;
			if (yscanfov < 0) yscanfov=-yscanfov;

			// Could use CT98_ReconPixelSize for pixsize ...
			// Could use CT98_ViewSpacing    for pixsize ...
			// but calculating it is more accurate

			double pixsize = yscanfov / GE9800_HeaderInstance_IMAGE1HDR->ViewsPerScan;

			xscanfov = GE9800_HeaderInstance_IMAGE1HDR->DetectorsPerView * pixsize;
		}
		else {
			// These may actually different, but the image
			// is always square, so select the maximum,
			// otherwise the mag/recon scaling will fail

			xscanfov = GE9800_HeaderInstance_IMAGE1HDR->XDiameter;
			yscanfov = GE9800_HeaderInstance_IMAGE1HDR->YDiameter;
			if (xscanfov > yscanfov) yscanfov=xscanfov;
			if (yscanfov > xscanfov) xscanfov=yscanfov;
		}

		double scanfov = (xscanfov > yscanfov) ? xscanfov : yscanfov;

		double xreconfov;
		double yreconfov;

		if (GE9800_HeaderInstance_IMAGE1HDR->FileType == 2) {	// Scout
			xreconfov = xscanfov;
			yreconfov = xscanfov;
		}
		else {
			xreconfov = GE9800_HeaderInstance_IMAGE1HDR->ReconXDiameter;
			yreconfov = GE9800_HeaderInstance_IMAGE1HDR->ReconYDiameter;
		}

		double reconfov = (xreconfov > yreconfov)
				 ? xreconfov : yreconfov;

	// Make a model of the image plane ...

//cerr << "ToDicom_ManualPlane::Creating ImagePlanet" << endl;

	ImagePlane imageplane(plane,xscanfov,yscanfov,orientation,position);

//cerr << "ToDicom_ManualPlane::Creating ImagePlane done" << endl;
//imageplane.put(cerr);

		// If scout, rotate into the correct plane ...

		// Rotation is CCW around Z
		// eg. Right Lateral is +90 degrees		... note sure this is correct ... have seen examples of the opposite ... need to check more samples :(

		if (GE9800_HeaderInstance_IMAGE1HDR->FileType == 2) {	// Scout 
			imageplane.MachinePlane::angle(
				HeadFeet,-(double)GE9800_HeaderInstance_IMAGE1HDR->TubeAzimuth);	// Note sign change ... not entirely sure this is correct ... need to check more samples :(
		}

		// While we are in the "display" or "gantry plane ...
		// (before angulation) 

		// Apply any transformations specified in the
		// display/gantry plane, such as magnification and
		// reconstruction target offsets ...

		// Assume that offsets are machine rather than body relative

		// Could use reconfov/scanfov ... same effect ...

		double reconfac	= GE9800_HeaderInstance_IMAGE1HDR->ReconMagFactor;
		double reconch	= GE9800_HeaderInstance_IMAGE1HDR->ReconXCenter;
		double reconcv	= GE9800_HeaderInstance_IMAGE1HDR->ReconYCenter;

		if (reconfac > .00001) {	// ie. not zero
			imageplane.MachinePlane::scale(1.0/reconfac);

			imageplane.MachinePlane::shift(
				Vector3D(reconch,-reconcv,0));
		}

		double magfac	= GE9800_HeaderInstance_IMAGE1HDR->MagFactor;
		double magch	= GE9800_HeaderInstance_IMAGE1HDR->XCenter;
		double magcv	= GE9800_HeaderInstance_IMAGE1HDR->YCenter;

		if (magfac > .00001) {	// ie. not zero
			imageplane.MachinePlane::scale(1.0/magfac);

			imageplane.MachinePlane::shift(
				Vector3D(magch,-magcv,0));
		}

		// CT9800 -ve is top towards table
		// Z is +ve into gantry, but X is +ve left, so leave sign alone

		double gantrydetectortilt=GE9800_HeaderInstance_IMAGE1HDR->GantryAngle;

		imageplane.MachinePlane::angle(LeftRight,gantrydetectortilt);

		// Account for physical offsets after angulation ...
		// CT9800 SlicePosition is positive into gantry

		// For scouts SlicePosition is start position not center

		{
			double zoffset;

			if (GE9800_HeaderInstance_IMAGE1HDR->FileType == 2) {	// Scout
				zoffset=((double)(GE9800_HeaderInstance_IMAGE1HDR->ScoutEndLocation)
				        +GE9800_HeaderInstance_IMAGE1HDR->SlicePosition)/2;
			}
			else {
				zoffset=GE9800_HeaderInstance_IMAGE1HDR->SlicePosition;
			}
			imageplane.MachinePlane::shift(Vector3D(0,0,zoffset));
		}

	// Now build the DICOM attributes ...

	(*list)+=new CodeStringAttribute(TagFromName(PatientPosition),
		String_Cat_Use(hfff,ap));

//cerr << "ToDicom_ManualPlane::addPlaneToList:" << endl;
	ImagePlaneLister ipl(imageplane);
	ipl.addPlaneToList(*list);
//cerr << "ToDicom_ManualPlane::addPlaneToList done:" << endl;

	// PixelSpacing

	(*list)+=new DecimalStringAttribute(TagFromName(PixelSpacing),
		GE9800_HeaderInstance_IMAGE1HDR->ReconPixelSize,
		GE9800_HeaderInstance_IMAGE1HDR->ReconPixelSize);

	// ReconstructionDiameter

	(*list)+=new DecimalStringAttribute(TagFromName(ReconstructionDiameter),reconfov);

	// DataCollectionDiameter

	(*list)+=new DecimalStringAttribute(TagFromName(DataCollectionDiameter),scanfov);
		
}

