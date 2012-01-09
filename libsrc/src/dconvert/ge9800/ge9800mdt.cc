#include "ge9800dc.h"
#include "elmconst.h"

void 
GE9800_Header_BothClass::ToDicom_ManualDates(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// StudyDate

	(*list)+=new DateStringAttribute(TagFromName(StudyDate),
		Date(
			GE9800_HeaderInstance_EXAMHDR->EXAMSTARTDATEYY,
			GE9800_HeaderInstance_EXAMHDR->EXAMSTARTDATEMM,
			GE9800_HeaderInstance_EXAMHDR->EXAMSTARTDATEDD
		));

	// StudyTime

	(*list)+=new TimeStringAttribute(TagFromName(StudyTime),
		Time(
			GE9800_HeaderInstance_EXAMHDR->EXAMSTARTTIMEHH,
			GE9800_HeaderInstance_EXAMHDR->EXAMSTARTTIMEMM,
			GE9800_HeaderInstance_EXAMHDR->EXAMSTARTTIMESS
		));

	// ContentDate (formerly Image)

	(*list)+=new DateStringAttribute(TagFromName(ContentDate),
		Date(
			GE9800_HeaderInstance_IMAGE1HDR->RECONDATEYY,
			GE9800_HeaderInstance_IMAGE1HDR->RECONDATEMM,
			GE9800_HeaderInstance_IMAGE1HDR->RECONDATEDD
		));

	// ContentTime (formerly Image)

	(*list)+=new TimeStringAttribute(TagFromName(ContentTime),
		Time(
			GE9800_HeaderInstance_IMAGE1HDR->RECONTIMEHH,
			GE9800_HeaderInstance_IMAGE1HDR->RECONTIMEMM,
			GE9800_HeaderInstance_IMAGE1HDR->RECONTIMESS
		));

	// AcquisitionDate

	(*list)+=new DateStringAttribute(TagFromName(AcquisitionDate),
		Date(
			GE9800_HeaderInstance_IMAGE1HDR->STARTOFSCANDATEYY,
			GE9800_HeaderInstance_IMAGE1HDR->STARTOFSCANDATEMM,
			GE9800_HeaderInstance_IMAGE1HDR->STARTOFSCANDATEDD
		));

	// AcquisitionTime

	(*list)+=new TimeStringAttribute(TagFromName(AcquisitionTime),
		Time(
			GE9800_HeaderInstance_IMAGE1HDR->STARTOFSCANTIMEHH,
			GE9800_HeaderInstance_IMAGE1HDR->STARTOFSCANTIMEMM,
			GE9800_HeaderInstance_IMAGE1HDR->STARTOFSCANTIMESS
		));
}

