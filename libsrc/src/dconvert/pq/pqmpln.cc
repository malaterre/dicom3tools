//#include <iostream.h>
//#include <iomanip.h>

#include "pqdc.h"
#include "elmconst.h"
#include "planea.h"

void 
PQ_Header_BothClass::ToDicom_ManualPlane(AttributeList *list)
{
	// Derive Image Plane Module ...

	char *hfff,*ap;

	ImagePlaneHeadFeet orientation;

	{
		hfff="";		// default :(
		orientation=HeadFirst;

		char *pq_poshf;
		int length;
		if (getAsciiAttribute("POSHF",pq_poshf,length)) {
			if (length >= 4 && strncmp(pq_poshf,"HEAD",4) == 0) {
				hfff="HF";
				orientation=HeadFirst;
			}
			else if (length >= 4 && strncmp(pq_poshf,"FEET",4) == 0) {
				hfff="FF";
				orientation=FeetFirst;
			}
			if (pq_poshf) delete[] pq_poshf;
		}
	}

	ImagePlanePosition position;

	{
		ap="";			// default :(
		position=Supine;

		char *pq_posprn;
		int length;
		if (getAsciiAttribute("POSPRN",pq_posprn,length)) {
			if (length >= 4 && strncmp(pq_posprn,"SUPI",4) == 0) {
				ap="S";
				position=Supine;
			}
			else if (length >= 4 && strncmp(pq_posprn,"PRON",4) == 0) {	// Need to check this term
				ap="P";
				position=Prone;
			}
			//ap="DL";
			//position=LeftLateralDecubitus;
			//ap="DR";
			//position=RightLateralDecubitus;

			if (pq_posprn) delete[] pq_posprn;
		}
	}

	ImagePlanePlane plane;		// Determine the "machine" plane of the image ...
					// CT slices are axial, pilots are coronal or sagittal

	bool ispilot=false;
	bool isslice=false;
	Int32 pq_pilot_angle;
	Int32 pq_pilot_length;
	Int32 pq_pilot_origin_pixels;
	Int32 pq_pilot_pixels_per_mm;
	Int32 pq_pilot_field_size=0;
	char *pilot_type=0;		// HORZ,VERT,OBL	// need to delete[] later if not zero
	int pilot_type_length=0;

	Int32 pq_recon_field_size=0;
	Int32 pq_recon_matrix_size;
	{
		plane=Axial;	// default :(

		char *pq_image_type;
		int length;
		if (getAsciiAttribute("IMTYPE",pq_image_type,length)) {
			if (length >= 4 && (strncmp(pq_image_type,"SLIC",4) == 0
					 || strncmp(pq_image_type,"SPIR",4) == 0) ) {
				isslice=true;
				plane=Axial;

				if (!getIntegerAttribute("RCNFSIZ",pq_recon_field_size)) Assert(0);
				if (!getIntegerAttribute("RCNMTRX",pq_recon_matrix_size)) Assert(0);
			}
			else if (length >= 3 && strncmp(pq_image_type,"PIL",3) == 0) {	// db says PILT, files contain PILO
				ispilot=true;

				// this needs serious work when more examples are available ...

				if (getIntegerAttribute("PILTANG",pq_pilot_angle)
				 && getAsciiAttribute("PILTYPE",pilot_type,pilot_type_length)) {
					if (pq_pilot_angle == 90 && strncmp(pilot_type,"VERT",4) == 0)
						plane=Coronal;
					else if (pq_pilot_angle == 0 && strncmp(pilot_type,"HORZ",4) == 0)
						plane=SagittalIP;
				}
				if (!getIntegerAttribute("PILTLNG",pq_pilot_length)) Assert(0);
				if (!getIntegerAttribute("PILTORG",pq_pilot_origin_pixels)) Assert(0);
				if (!getIntegerAttribute("PILTPPM",pq_pilot_pixels_per_mm)) Assert(0);
				if (!getIntegerAttribute("PILTFSZ",pq_pilot_field_size)) Assert(0);
			}
			else {
				Assert(0);	// other types not supported
			}
			if (pq_image_type) delete[] pq_image_type;
		}
	}

	// if (pilot_type) delete[] pilot_type;		// put this later when no longer needed

	// Make a model of the image plane ...

	ImagePlane imageplane(plane,ispilot ? pq_pilot_field_size : pq_recon_field_size,orientation,position);

//cerr << "ToDicom_ManualPlane::Creating ImagePlane done" << endl;
//imageplane.put(cerr);

	// While we are in the "display" or "gantry plane ...
	// (before angulation) 

	// Account for angulation ...

	double gantry_tilt;
	{
		Int32 pq_gantry_tilt_times_ten;
		if (!getIntegerAttribute("GTILT",pq_gantry_tilt_times_ten)) pq_gantry_tilt_times_ten=0;
		gantry_tilt=double(pq_gantry_tilt_times_ten)/10.0;

		// assume +ve tilt is always top of gantry towards the foot of the table
		// DICOM Z is positive into gantry (towards head)
		imageplane.MachinePlane::angle(LeftRight,-gantry_tilt);

//cerr << "ToDicom_ManualPlane::angling result:" << endl;
//imageplane.put(cerr);
	}

	// Account for physical offsets in the gantry plane ...
	// (after angulation)

	double couch_position;
	Int32 pq_couch_height;
	{
		Int32 pq_couch_position_times_100;
		if (!getIntegerAttribute("COUCH",pq_couch_position_times_100)) pq_couch_position_times_100=0;
		couch_position=double(pq_couch_position_times_100)/100.0;

		if (!getIntegerAttribute("COUCHHT",pq_couch_height)) pq_couch_height=0;

		// assume couch position is +ve out of gantry (towards feet in supine/hf)
		// assume couch height is +ve above table (towards anterior in supine/hf)
		// make DICOM Y coordinates around an arbitrary height of 300mm (rather than the floor !)
		// DICOM is +LPH

		// Note that the pilot correction needs to be calculated for the center of the image

		double couch_position_correction = ispilot
			? double(pq_pilot_origin_pixels)/double(pq_pilot_pixels_per_mm) - pq_pilot_field_size/2.0
			: 0.0;

//cerr << "couch_position_correction=" << couch_position_correction << endl;

		Vector3D offset(0,300-pq_couch_height,-(couch_position-couch_position_correction));

		imageplane.MachinePlane::shift(offset);
	}

	// Now build the DICOM attributes ...

	// PatientPosition

	(*list)+=new CodeStringAttribute(TagFromName(PatientPosition),String_Cat_Use(hfff,ap));

	// ImageOrientationPatient and ImagePositionPatient

	ImagePlaneLister ipl(imageplane);
	ipl.addPlaneToList(*list);

	// SliceLocation

	(*list)+=new DecimalStringAttribute(TagFromName(SliceLocation),couch_position);

	// TableHeight

	(*list)+=new DecimalStringAttribute(TagFromName(TableHeight),double(pq_couch_height));

	// GantryDetectorTilt

	(*list)+=new DecimalStringAttribute(TagFromName(GantryDetectorTilt),gantry_tilt);

	// PixelSpacing

	double pixel_spacing = ispilot
			? 1.0/double(pq_pilot_pixels_per_mm)
			: double(pq_recon_field_size)/double(pq_recon_matrix_size);

	(*list)+=new DecimalStringAttribute(TagFromName(PixelSpacing),pixel_spacing,pixel_spacing);

	// ReconstructionDiameter

	(*list)+=new DecimalStringAttribute(TagFromName(ReconstructionDiameter),
		double(ispilot ? pq_pilot_field_size : pq_recon_field_size));

	// DataCollectionDiameter

	Int32 pq_scan_field_size;
	if (getIntegerAttribute("SCNFSIZ",pq_scan_field_size)) {
		(*list)+=new DecimalStringAttribute(TagFromName(DataCollectionDiameter),
			double(pq_scan_field_size));
	}

	// SliceThickness

	Int32 pq_scan_thickness_multiplier;
	Int32 pq_scan_thickness_value;
	if (getIntegerAttribute("SCNTHKM",pq_scan_thickness_multiplier)
	 && getIntegerAttribute("SCNTHKN",pq_scan_thickness_value)) {
		(*list)+=new DecimalStringAttribute(TagFromName(SliceThickness),
			double(pq_scan_thickness_value)/double(pq_scan_thickness_multiplier));
	}

	// PositionReferenceIndicator

	(*list)+=new LongStringAttribute(TagFromName(PositionReferenceIndicator));
}

