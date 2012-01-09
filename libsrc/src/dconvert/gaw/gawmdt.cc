#include "gawdc.h"
#include "elmconst.h"

void 
GAW_Header_BothClass::ToDicom_ManualDates(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// ? should use EX_ex_lastmod ?

	// StudyDate

	(*list)+=new DateStringAttribute(TagFromName(StudyDate),
		Date(DateTime(GAW_HeaderInstance_EXAMHDR->EX_ex_datetime)));

	// StudyTime

	(*list)+=new TimeStringAttribute(TagFromName(StudyTime),
		Time(DateTime(GAW_HeaderInstance_EXAMHDR->EX_ex_datetime)));

	// ? should use SE_se_actual_dt ? SE_se_lastmod ?

	// SeriesDate

	(*list)+=new DateStringAttribute(TagFromName(SeriesDate),
		Date(DateTime(GAW_HeaderInstance_SERIESHDR->SE_se_datetime)));

	// SeriesTime

	(*list)+=new TimeStringAttribute(TagFromName(SeriesTime),
		Time(DateTime(GAW_HeaderInstance_SERIESHDR->SE_se_datetime)));

	// ? should use im_datetime ? im_lastmod ? im_actual_dt ?

	// ContentDate (formerly Image)

	if (GAW_isct)
		(*list)+=new DateStringAttribute(TagFromName(ContentDate),
			Date(DateTime(GAW_HeaderInstance_CTHDR->CT_im_lastmod)));
	else if (GAW_ismr)
		(*list)+=new DateStringAttribute(TagFromName(ContentDate),
			Date(DateTime(GAW_HeaderInstance_MRHDR->MR_im_lastmod)));

	// ContentTime (formerly Image)

	if (GAW_isct)
		(*list)+=new TimeStringAttribute(TagFromName(ContentTime),
			Time(DateTime(GAW_HeaderInstance_CTHDR->CT_im_lastmod)));
	else if (GAW_ismr)
		(*list)+=new TimeStringAttribute(TagFromName(ContentTime),
			Time(DateTime(GAW_HeaderInstance_MRHDR->MR_im_lastmod)));

	// AcquisitionDate

	if (GAW_isct)
		(*list)+=new DateStringAttribute(TagFromName(AcquisitionDate),
			Date(DateTime(GAW_HeaderInstance_CTHDR->CT_im_datetime)));
	else if (GAW_ismr)
		(*list)+=new DateStringAttribute(TagFromName(AcquisitionDate),
			Date(DateTime(GAW_HeaderInstance_MRHDR->MR_im_datetime)));

	// AcquisitionTime

	if (GAW_isct)
		(*list)+=new TimeStringAttribute(TagFromName(AcquisitionTime),
			Time(DateTime(GAW_HeaderInstance_CTHDR->CT_im_datetime)));
	else if (GAW_ismr)
		(*list)+=new TimeStringAttribute(TagFromName(AcquisitionTime),
			Time(DateTime(GAW_HeaderInstance_MRHDR->MR_im_datetime)));

	{
		// PatientSex

		const char *str;
		switch (GAW_HeaderInstance_EXAMHDR->EX_patsex) {
			case 1:		str="M"; break;
			case 2:		str="F"; break;
			default:	str="";  break;
		}

		(*list)+=new CodeStringAttribute(TagFromName(PatientSex),str);
	}

	{
		// PatientAge

		ostrstream ost;
		ost << setfill('0') << setw(3) << dec
		    << GAW_HeaderInstance_EXAMHDR->EX_patage;
		switch (GAW_HeaderInstance_EXAMHDR->EX_patian) {
			case 0:	ost << "Y"; break;
			case 1:	ost << "M"; break;
			case 2:	ost << "D"; break;
			case 3:	ost << "W"; break;
		}
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

	(*list)+=new DecimalStringAttribute(TagFromName(PatientWeight),
		Uint32(GAW_HeaderInstance_EXAMHDR->EX_patweight)/1000.0);
}

