#include "sgndc.h"
#include "elmconst.h"

void 
SGN_Header_BothClass::ToDicom_ManualDates(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	(*list)+=new DateStringAttribute(TagFromName(StudyDate),Date(
		SGN_HeaderInstance_STUDYHDR->StudyDateYear,
		SGN_HeaderInstance_STUDYHDR->StudyDateMonth,
		SGN_HeaderInstance_STUDYHDR->StudyDateDay
	));

	(*list)+=new TimeStringAttribute(TagFromName(StudyTime),Time(
		SGN_HeaderInstance_STUDYHDR->StudyTimeHour,
		SGN_HeaderInstance_STUDYHDR->StudyTimeMin,
		SGN_HeaderInstance_STUDYHDR->StudyTimeSec
	));

	(*list)+=new DateStringAttribute(TagFromName(SeriesDate),Date(
		SGN_HeaderInstance_SERIESHDR->SeriesDateYear,
		SGN_HeaderInstance_SERIESHDR->SeriesDateMonth,
		SGN_HeaderInstance_SERIESHDR->SeriesDateDay
	));

	(*list)+=new TimeStringAttribute(TagFromName(SeriesTime),Time(
		SGN_HeaderInstance_SERIESHDR->SeriesTimeHour,
		SGN_HeaderInstance_SERIESHDR->SeriesTimeMin,
		SGN_HeaderInstance_SERIESHDR->SeriesTimeSec
	));

	(*list)+=new DateStringAttribute(TagFromName(ContentDate),Date(
		SGN_HeaderInstance_IMAGEHDR->ImageDateYear,
		SGN_HeaderInstance_IMAGEHDR->ImageDateMonth,
		SGN_HeaderInstance_IMAGEHDR->ImageDateDay
	));

	(*list)+=new TimeStringAttribute(TagFromName(ContentTime),Time(
		SGN_HeaderInstance_IMAGEHDR->ImageTimeHour,
		SGN_HeaderInstance_IMAGEHDR->ImageTimeMin,
		SGN_HeaderInstance_IMAGEHDR->ImageTimeSec
	));

	(*list)+=new DateStringAttribute(TagFromName(AcquisitionDate),Date(
		SGN_HeaderInstance_IMAGEHDR->ImageDateYear,
		SGN_HeaderInstance_IMAGEHDR->ImageDateMonth,
		SGN_HeaderInstance_IMAGEHDR->ImageDateDay
	));

	(*list)+=new TimeStringAttribute(TagFromName(AcquisitionTime),Time(
		SGN_HeaderInstance_IMAGEHDR->ImageTimeHour,
		SGN_HeaderInstance_IMAGEHDR->ImageTimeMin,
		SGN_HeaderInstance_IMAGEHDR->ImageTimeSec
	));

	{
		// PatientAge

		char *str = SGN_HeaderInstance_STUDYHDR->PatientAge;
		ostrstream ost;
		ost << setfill('0') << setw(3)  << dec << atoi(str);
		if      (strchr(str,'d') || strchr(str,'D')) ost << "D";
		else if (strchr(str,'w') || strchr(str,'W')) ost << "W";
		else if (strchr(str,'m') || strchr(str,'M')) ost << "M";
		else ost << "Y";

		ost << ends;
		char *agestr=ost.str();
		if (agestr) {
			if (strlen(agestr))
				(*list)+=new AgeStringAttribute(
					TagFromName(PatientAge),agestr);
			delete[] agestr;
		}
	}

	// PatientWeight

	(*list)+=new DecimalStringAttribute(TagFromName(PatientWeight),Uint32(SGN_HeaderInstance_STUDYHDR->PatientWeightGrams)/1000.0);
}

