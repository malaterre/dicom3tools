#include "shimdc.h"
#include "elmconst.h"

void 
SHIM_Header_BothClass::ToDicom_ManualDates(AttributeList *list,unsigned imagenumber)
{
	(void)imagenumber;

	Date thedate=Date(String_Use(SHIM_HeaderInstance_MAINHDR->ZADATW));

	Time thetime=Time(String_Use(SHIM_HeaderInstance_MAINHDR->ZATIME));

	// StudyDate

	(*list)+=new DateStringAttribute(TagFromName(StudyDate),thedate);

	// StudyTime

	(*list)+=new TimeStringAttribute(TagFromName(StudyTime),thetime);

	// SeriesDate

	(*list)+=new DateStringAttribute(TagFromName(SeriesDate),thedate);

	// SeriesTime

	(*list)+=new TimeStringAttribute(TagFromName(SeriesTime),thetime);

	// AcquisitionDate

	(*list)+=new DateStringAttribute(TagFromName(AcquisitionDate),thedate);

	// AcquisitionTime

	(*list)+=new TimeStringAttribute(TagFromName(AcquisitionTime),thetime);

	// ContentDate (formerly Image)

	(*list)+=new DateStringAttribute(TagFromName(ContentDate),thedate);

	// ContentTime (formerly Image)

	(*list)+=new TimeStringAttribute(TagFromName(ContentTime),thetime);

}

