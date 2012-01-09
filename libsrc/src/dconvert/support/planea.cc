#include "attrtype.h"
#include "attrlist.h"
#include "elmconst.h"
#include "planea.h"
#include "strtype.h"

void
ImagePlaneLister::addPlaneToList(AttributeList& list)
{
	// PatientPosition
	// ImagePositionPatient
	// ImageOrientationPatient
	// ImagePosition
	// ImageOrientation
	// PatientOrientation

	Assert(plane);

	list+=new DecimalStringAttribute(TagFromName(ImagePositionPatient),
		plane->PatientPlane::getTLHC().getX(),
		plane->PatientPlane::getTLHC().getY(),
		plane->PatientPlane::getTLHC().getZ());

	list+=new DecimalStringAttribute(TagFromName(ImageOrientationPatient),
		plane->PatientPlane::getRowVector().getX(),
		plane->PatientPlane::getRowVector().getY(),
		plane->PatientPlane::getRowVector().getZ(),
		plane->PatientPlane::getColVector().getX(),
		plane->PatientPlane::getColVector().getY(),
		plane->PatientPlane::getColVector().getZ());

	list+=new DecimalStringAttribute(TagFromName(ImagePosition),
		plane->MachinePlane::getTLHC().getX(),
		plane->MachinePlane::getTLHC().getY(),
		plane->MachinePlane::getTLHC().getZ());

	list+=new DecimalStringAttribute(TagFromName(ImageOrientation),
		plane->MachinePlane::getRowVector().getX(),
		plane->MachinePlane::getRowVector().getY(),
		plane->MachinePlane::getRowVector().getZ(),
		plane->MachinePlane::getColVector().getX(),
		plane->MachinePlane::getColVector().getY(),
		plane->MachinePlane::getColVector().getZ());

	list+=new CodeStringAttribute(TagFromName(PatientOrientation),
		String_Use(plane->PatientPlane::getRowOrientation()),
		String_Use(plane->PatientPlane::getColOrientation()));
}

