#include "sompdc.h"
#include "elmconst.h"

void 
SOMP_Header_BothClass::ToDicom_ManualDates(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// StudyDate

	(*list)+=new DateStringAttribute(TagFromName(StudyDate),
		Date(String_Use(SOMP_HeaderInstance_BINHDR->studydate)));

	// StudyTime

	(*list)+=new TimeStringAttribute(TagFromName(StudyTime),
		Time(String_Use(SOMP_HeaderInstance_BINHDR->studytime)));

	// ContentDate (formerly Image)

	(*list)+=new DateStringAttribute(TagFromName(ContentDate),
		Date(String_Use(SOMP_HeaderInstance_BINHDR->scandate)));

	// ContentTime (formerly Image)

	(*list)+=new TimeStringAttribute(TagFromName(ContentTime),
		Time(String_Use(SOMP_HeaderInstance_BINHDR->scantime)));

}

