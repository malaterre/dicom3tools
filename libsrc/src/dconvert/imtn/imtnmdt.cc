#include "imtndc.h"
#include "elmconst.h"

void 
IMTN_Header_BothClass::ToDicom_ManualDates(AttributeList *list,unsigned imagenumber)
{
	Date thedate=Date(String_Use(IMTN_HeaderInstance_FILEHDR->
			IMTN_Method_ExtractTaggedString("IADATE")));

	Time thetime=Time(String_Use(IMTN_HeaderInstance_FILEHDR->
			IMTN_Method_ExtractTaggedString("IATIME")));

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

	// ContentTime (formerly Image) ... won't wrap around 24 hours :(

	(*list)+=new TimeStringAttribute(TagFromName(ContentTime),
		Time(Uint32(thetime)
		   + Uint32(IMTN_HeaderInstance_SLICEHDR->
			IMTN_Method_ExtractTaggedFloat(imagenumber,"ELAPSE")*1000)
		));

	// PatientBirthDate

	(*list)+=new DateStringAttribute(TagFromName(PatientBirthDate),
		Date(String_Use(IMTN_HeaderInstance_FILEHDR->
			IMTN_Method_ExtractTaggedString("IBDATE"))));
}

