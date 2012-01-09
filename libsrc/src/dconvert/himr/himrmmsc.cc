#include "himrdc.h"
#include "elmconst.h"

void 
HIMR_Header_BothClass::ToDicom_ManualMisc(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// Modality

	(*list)+=new CodeStringAttribute(TagFromName(Modality),"MR");

	// MagneticFieldStrength - Tesla

	(*list)+=new
		DecimalStringAttribute(TagFromName(MagneticFieldStrength),
			Uint32(HIMR_HeaderInstance_IMGHDR->MIBO)/100.0);

	// ImagingFrequency MHz

	(*list)+=new
		DecimalStringAttribute(TagFromName(ImagingFrequency),
			Uint32(HIMR_HeaderInstance_IMGHDR->MIFREQO)/1000000.0);

	{
		// ImageType

		char *value1,*value2,*value3;

		value1="ORIGINAL";

		value2="PRIMARY";

		value3="MPR";

		//value3=strncmp(HIMR_HeaderInstance_IMGHDR->XISCAN,"C",1) == 0
		//	? "AXIAL" : "LOCALIZER";

		(*list)+=new CodeStringAttribute(TagFromName(ImageType),
			value1,value2,value3);
	}

	// ScanOptions

	(*list)+=new CodeStringAttribute(TagFromName(ScanOptions));

	// SequenceVariant

	(*list)+=new CodeStringAttribute(TagFromName(SequenceVariant),"NONE");

	{
		// ScanningSequence

		const char *str=0;

		if      (strncmp(HIMR_HeaderInstance_IMGHDR->MISQ,"SR",2) == 0) str="SE";
		else if (strncmp(HIMR_HeaderInstance_IMGHDR->MISQ,"SE",2) == 0) str="SE";
		else if (strncmp(HIMR_HeaderInstance_IMGHDR->MISQ,"IR",2) == 0) str="IR";
		else if (strncmp(HIMR_HeaderInstance_IMGHDR->MISQ,"GE",2) == 0) str="GR";
		else if (strncmp(HIMR_HeaderInstance_IMGHDR->MISQ,"GR",2) == 0) str="GR";

		(*list)+=new CodeStringAttribute(TagFromName(ScanningSequence),str);
	}

	{
		// MRAcquisitionType

		const char *str=strncmp(HIMR_HeaderInstance_IMGHDR->MI3D,"1",1) == 0 ? "3D" : "2D";

		(*list)+=new CodeStringAttribute(TagFromName(MRAcquisitionType),str);
	}

	// ContrastBolus Module

	if (strncmp(HIMR_HeaderInstance_IMGHDR->XICM,"C",1) == 0) {

		// ContrastBolusAgent ="Contrast"

		(*list)+=new LongStringAttribute(TagFromName(ContrastBolusAgent),
			"Contrast");
	}

	// Do the descriptions ...

	class Description {
		ostrstream ost;
		int first;
		char delim;
	public:
		Description(char d=0)	{ first=1; delim=d; }

		void add(const char *desc,unsigned maxlength)
			{
				char *buf = new char[maxlength+1];
				strncpy(buf,desc,maxlength); buf[maxlength]=0;
				char *lastshimr=strrchr(buf,' ');
				if (lastshimr && buf[maxlength-1] == ' ')
					*lastshimr=0;
				char *start = buf;
				while (*start && *start == ' ') ++start;
				if (*start) {
					if (!first && delim) ost << delim;
					ost << start;
				}
				first=0;
				if (buf) delete[] buf;
			}

		char *get(void)		{ ost << ends; return ost.str(); }
	};

	{
		// ImageComments

		Description desc(' ');
		desc.add(HIMR_HeaderInstance_IMGHDR->XICMT1,26);
		desc.add(HIMR_HeaderInstance_IMGHDR->XICMT2,26);
		desc.add(HIMR_HeaderInstance_IMGHDR->XICMT3,26);
		desc.add(HIMR_HeaderInstance_IMGHDR->XICMT4,26);
		desc.add(HIMR_HeaderInstance_IMGHDR->XICMT5,26);

		char *str=desc.get();

		(*list)+=new LongTextAttribute(TagFromName(ImageComments),str);
		if (str) delete[] str;
	}

#ifdef CRAP
	// GeneratorPower

	// Loose fractional part ...

	(*list)+=new IntegerStringAttribute(TagFromName(GeneratorPower),
		(unsigned short)(atof(HIMR_HeaderInstance_IMGHDR->IGENPOW)));

	// Do the descriptions ...

	class Description {
		ostrstream ost;
		int first;
		char delim;
	public:
		Description(char d=0)	{ first=1; delim=d; }

		void add(const char *desc,unsigned maxlength)
			{
				char *buf = new char[maxlength+1];
				strncpy(buf,desc,maxlength); buf[maxlength]=0;
				char *lastshimr=strrchr(buf,' ');
				if (lastshimr && buf[maxlength-1] == ' ')
					*lastshimr=0;
				char *start = buf;
				while (*start && *start == ' ') ++start;
				if (*start) {
					if (!first && delim) ost << delim;
					ost << start;
				}
				first=0;
				if (buf) delete[] buf;
			}

		char *get(void)		{ ost << ends; return ost.str(); }
	};

	{
		// StudyDescription

		Description desc(' ');
		desc.add(HIMR_HeaderInstance_STDHDR->SDESC1,50);
		desc.add(HIMR_HeaderInstance_STDHDR->SDESC2,50);
		desc.add(HIMR_HeaderInstance_STDHDR->SDESC3,50);
		desc.add(HIMR_HeaderInstance_STDHDR->SDESC4,50);
		desc.add(HIMR_HeaderInstance_STDHDR->SDESC5,50);

		char *str=desc.get();
		unsigned length=strlen(str);
		if (length > 64) {
			str[64]=0;
			length=64;
		}

		(*list)+=new LongStringAttribute(TagFromName(StudyDescription),str);
		if (str) delete[] str;
	}
	{
		// SeriesDescription

		Description desc(' ');
		desc.add(HIMR_HeaderInstance_IMGHDR2->ISRDESC1,16);
		desc.add(HIMR_HeaderInstance_IMGHDR2->ISRDESC2,14);
		desc.add(HIMR_HeaderInstance_IMGHDR2->ISRDESC3,12);

		char *str=desc.get();

		(*list)+=new LongStringAttribute(TagFromName(SeriesDescription),str);
		if (str) delete[] str;
	}
	{
		// ImageComments

		Description desc(' ');
		desc.add(HIMR_HeaderInstance_IMGHDR2->IIMDESC1,10);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IIMDESC2,8);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IIMDESC3,7);

		char *str=desc.get();

		(*list)+=new LongTextAttribute(TagFromName(ImageComments),str);
		if (str) delete[] str;
	}
	{
		// AdditionalPatientHistory

		Description desc(' ');
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC1,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC2,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC3,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC4,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC5,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC6,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC7,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC8,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC9,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC10,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC11,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC12,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC13,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC14,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC15,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC16,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC17,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC18,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC19,17);
		desc.add(HIMR_HeaderInstance_IMGHDR2->IEXDESC20,17);

		char *str=desc.get();

		(*list)+=new LongTextAttribute(TagFromName(AdditionalPatientHistory),str);
		if (str) delete[] str;
	}
#endif


}

