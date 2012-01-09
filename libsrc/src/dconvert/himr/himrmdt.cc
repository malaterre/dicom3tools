#include "himrdc.h"
#include "elmconst.h"

void 
HIMR_Header_BothClass::ToDicom_ManualDates(AttributeList *list,unsigned imagenumber)
{
	{
		// PatientBirthDate

		unsigned yy=atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XIBYY));
		unsigned mm=atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XIBMM));
		unsigned dd=atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XIBDD));

		(*list)+=new DateStringAttribute(TagFromName(PatientBirthDate),Date(yy,mm,dd));
	}
	{
		// StudyDate
		// SeriesDate
		// ContentDate (formerly Image)

		unsigned yy=atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XIYY));
		unsigned mm=atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XIMM));
		unsigned dd=atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XIDD));

		(*list)+=new DateStringAttribute(TagFromName(StudyDate),Date(yy,mm,dd));
		(*list)+=new DateStringAttribute(TagFromName(SeriesDate),Date(yy,mm,dd));
		(*list)+=new DateStringAttribute(TagFromName(ContentDate),Date(yy,mm,dd));
	}
	{
		// StudyTime
		// SeriesTime
		// ContentTime (formerly Image)

		unsigned hh=atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XITH));
		unsigned mm=atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XITM));
		unsigned ss=atoi(String_Use(HIMR_HeaderInstance_IMGHDR->XITS));

		(*list)+=new TimeStringAttribute(TagFromName(StudyTime),Time(hh,mm,ss));
		(*list)+=new TimeStringAttribute(TagFromName(SeriesTime),Time(hh,mm,ss));
		(*list)+=new TimeStringAttribute(TagFromName(ContentTime),Time(hh,mm,ss));

		(*list)+=new IntegerStringAttribute(TagFromName(SeriesNumber),Uint32(hh*10000+mm*100+ss));
	}
}

