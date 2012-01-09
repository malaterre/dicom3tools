#include "pqdc.h"
#include "elmconst.h"

void 
PQ_Header_BothClass::ToDicom_ManualDates(AttributeList *list)
{
	{
		// ContentDate (formerly Image)

		char *pq_recon_date;
		int length;
		if (getDateAttribute("RCNDATE",pq_recon_date,length) && length >= 6) {
			char *withdelim=new char[length+4];
			Assert(withdelim);
			strncpy(withdelim,pq_recon_date,2);
			withdelim[2]='/';
			strncpy(withdelim+3,pq_recon_date+2,2);
			withdelim[5]='/';
			strncpy(withdelim+6,pq_recon_date+4,length-4);
			withdelim[length+3]=0;

			(*list)+=new DateStringAttribute(TagFromName(ContentDate),Date(withdelim,DateOrderMonthMiddleYearFirst));

			if (pq_recon_date) delete[] pq_recon_date;
			if (withdelim) delete[] withdelim;
		}
		else {
			(*list)+=new DateStringAttribute(TagFromName(ContentDate));	// Type 2
		}
	}
	{
		// ContentTime (formerly Image)

		char *pq_recon_time;	// in hundredths of a second
		int length;
		if (getTimeAttribute("RCNTIME",pq_recon_time,length) && length >= 6) {
			char *withdelim=new char[length+4];
			Assert(withdelim);
			strncpy(withdelim,pq_recon_time,2);
			withdelim[2]=':';
			strncpy(withdelim+3,pq_recon_time+2,2);
			withdelim[5]=':';
			strncpy(withdelim+6,pq_recon_time+4,2);
			withdelim[8]='.';
			strncpy(withdelim+9,pq_recon_time+6,length-6);
			withdelim[length+3]=0;

			(*list)+=new TimeStringAttribute(TagFromName(ContentTime),Time(withdelim));
			if (pq_recon_time) delete[] pq_recon_time;
			if (withdelim) delete[] withdelim;
		}
		else {
			(*list)+=new TimeStringAttribute(TagFromName(ContentTime));	// Type 2
		}
	}
	{
		// AcquisitionDate, StudyDate

		char *pq_scan_acq_date;
		int length;
		if (getDateAttribute("SCNDATE",pq_scan_acq_date,length) && length >= 6) {

			char *withdelim=new char[length+4];
			Assert(withdelim);
			strncpy(withdelim,pq_scan_acq_date,2);
			withdelim[2]='/';
			strncpy(withdelim+3,pq_scan_acq_date+2,2);
			withdelim[5]='/';
			strncpy(withdelim+6,pq_scan_acq_date+4,length-4);
			withdelim[length+3]=0;

			(*list)+=new DateStringAttribute(TagFromName(AcquisitionDate),Date(withdelim,DateOrderMonthMiddleYearFirst));
			(*list)+=new DateStringAttribute(TagFromName(StudyDate),Date(withdelim,DateOrderMonthMiddleYearFirst));
			if (pq_scan_acq_date) delete[] pq_scan_acq_date;
			if (withdelim) delete[] withdelim;
		}
		else {
			(*list)+=new DateStringAttribute(TagFromName(StudyDate));	// Type 2
		}
	}
	{
		// AcquisitionTime, StudyTime

		char *pq_scan_acq_time;	// in hundredths of a second
		int length;
		if (getTimeAttribute("SCNTIME",pq_scan_acq_time,length) && length >= 6) {
			char *withdelim=new char[length+4];
			Assert(withdelim);
			strncpy(withdelim,pq_scan_acq_time,2);
			withdelim[2]=':';
			strncpy(withdelim+3,pq_scan_acq_time+2,2);
			withdelim[5]=':';
			strncpy(withdelim+6,pq_scan_acq_time+4,2);
			withdelim[8]='.';
			strncpy(withdelim+9,pq_scan_acq_time+6,length-6);
			withdelim[length+3]=0;

			(*list)+=new TimeStringAttribute(TagFromName(AcquisitionTime),Time(withdelim));
			(*list)+=new TimeStringAttribute(TagFromName(StudyTime),Time(withdelim));
			if (pq_scan_acq_time) delete[] pq_scan_acq_time;
			if (withdelim) delete[] withdelim;
		}
		else {
			(*list)+=new TimeStringAttribute(TagFromName(StudyTime));	// Type 2
		}
	}

	// forget about Series Date/Time
}

