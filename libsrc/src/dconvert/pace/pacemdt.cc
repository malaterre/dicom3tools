#include "pacedc.h"
#include "elmconst.h"

void 
PACE_Header_BothClass::ToDicom_ManualDates(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	{
		// PatientAge

		// Pace is yyy;mm
		unsigned yyy=atoi(String_Use(PACE_HeaderInstance_STDHDR->SAGE));
		unsigned mm =atoi((const char *)(String_Use(PACE_HeaderInstance_STDHDR->SAGE))+4);

		ostrstream ost;
		if (yyy)
			ost << setw(3) << setfill('0')
			    << yyy << 'Y' << ends;	// blow off months
		else
			ost << setw(3) << setfill('0')
			    << mm  << 'M' << ends;	// a baby presumably

		char *agestr=ost.str();
		(*list)+=new AgeStringAttribute(TagFromName(PatientAge),agestr);
		if (agestr) delete[] agestr;
	}

	// StudyDate

	(*list)+=new DateStringAttribute(TagFromName(StudyDate),
		Date(String_Use(PACE_HeaderInstance_STDHDR->SDATE)));

	// StudyTime

	(*list)+=new TimeStringAttribute(TagFromName(StudyTime),
		Time(String_Use(PACE_HeaderInstance_STDHDR->STIME)));

	// ContentDate (formerly Image)

	(*list)+=new DateStringAttribute(TagFromName(ContentDate),
		Date(String_Use(PACE_HeaderInstance_IMGHDR->IDATE)));

	// ContentTime (formerly Image)

	(*list)+=new TimeStringAttribute(TagFromName(ContentTime),
		Time(String_Use(PACE_HeaderInstance_IMGHDR->ITIME)));

}

