#include "sompdc.h"
#include "elmconst.h"

void 
SOMP_Header_BothClass::ToDicom_ManualMisc(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	// Rows,Columns

	(*list)+=new UnsignedShortAttribute(TagFromName(Rows),
		SOMP_HeaderInstance_BINHDR->matrix);
	(*list)+=new UnsignedShortAttribute(TagFromName(Columns),
		SOMP_HeaderInstance_BINHDR->matrix);

	// ExposureTime

	// Loose fractional part ...

	{
		// Not in line because SC 4.0 has ambiguity :(
		Int32 i = Int32(SOMP_HeaderInstance_BINHDR->acqtimef);
		(*list)+=new IntegerStringAttribute(TagFromName(ExposureTime),i);
	}

#ifdef CRAP
	// Modality

	switch (SOMP_HeaderInstance_STDHDR->SMODELNO_Mod) {
		case 1:
			(*list)+=new CodeStringAttribute(TagFromName(Modality),"CT");
			break;
		case 2:
			(*list)+=new CodeStringAttribute(TagFromName(Modality),"MR");
			break;
		default:
			(*list)+=new CodeStringAttribute(TagFromName(Modality),"SC");
			break;
	}

	// ImageType

	char *value1,*value2,*value3;

	value1="ORIGINAL";

	// P is prospective, R is retrospective ...

	value2=strncmp(SOMP_HeaderInstance_IMGHDR2->ITCTYPE,"P",1) == 0
		? "PRIMARY" : "SECONDARY";

	value3=strncmp(SOMP_HeaderInstance_IMGHDR->IMTYPE,"AX",2) == 0
		? "AXIAL" : "LOCALIZER";

	(*list)+=new CodeStringAttribute(TagFromName(ImageType),
		value1,value2,value3);

	// ContrastBolus Module

	if (strncmp(SOMP_HeaderInstance_IMGHDR->IENHANCE,"+C",2) == 0) {

		// ContrastBolusAgent ="Contrast"
		// ContrastBolusStartTime

		(*list)+=new LongStringAttribute(TagFromName(ContrastBolusAgent),
			"Contrast");

		(*list)+=new TimeStringAttribute(TagFromName(ContrastBolusStartTime),
			Time(String_Use(SOMP_HeaderInstance_IMGHDR->ICETIME)));
	}

	// GeneratorPower

	// Loose fractional part ...

	(*list)+=new IntegerStringAttribute(TagFromName(GeneratorPower),
		(unsigned short)(atof(SOMP_HeaderInstance_IMGHDR->IGENPOW)));

	{
		// SoftwareVersion 

		ostrstream ost;
		ost << "System "    << SOMP_HeaderInstance_STDHDR->SSYSVNO
		    << "."          << SOMP_HeaderInstance_STDHDR->SSYSRNO
		    << " Database " << SOMP_HeaderInstance_STDHDR->SDATVNO
		    << "."          << SOMP_HeaderInstance_STDHDR->SDATRNO
		    << ends;

		char *str=ost.str();

		(*list)+=new LongStringAttribute(TagFromName(SoftwareVersion),str);
		if (str) delete[] str;
	}
	{
		// StationName 

		ostrstream ost;
		ost << SOMP_HeaderInstance_STDHDR->SMODELNO_Mod << "."
		    << SOMP_HeaderInstance_STDHDR->SMODELNO_Mac << "."
		    << SOMP_HeaderInstance_STDHDR->SMACHINE
		    << ends;

		char *str=ost.str();

		(*list)+=new ShortStringAttribute(TagFromName(StationName),str);
		if (str) delete[] str;
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
				char *lastssomp=strrchr(buf,' ');
				if (lastssomp && buf[maxlength-1] == ' ')
					*lastssomp=0;
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
		desc.add(SOMP_HeaderInstance_STDHDR->SDESC1,50);
		desc.add(SOMP_HeaderInstance_STDHDR->SDESC2,50);
		desc.add(SOMP_HeaderInstance_STDHDR->SDESC3,50);
		desc.add(SOMP_HeaderInstance_STDHDR->SDESC4,50);
		desc.add(SOMP_HeaderInstance_STDHDR->SDESC5,50);

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
		desc.add(SOMP_HeaderInstance_IMGHDR2->ISRDESC1,16);
		desc.add(SOMP_HeaderInstance_IMGHDR2->ISRDESC2,14);
		desc.add(SOMP_HeaderInstance_IMGHDR2->ISRDESC3,12);

		char *str=desc.get();

		(*list)+=new LongStringAttribute(TagFromName(SeriesDescription),str);
		if (str) delete[] str;
	}
	{
		// ImageComments

		Description desc(' ');
		desc.add(SOMP_HeaderInstance_IMGHDR2->IIMDESC1,10);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IIMDESC2,8);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IIMDESC3,7);

		char *str=desc.get();

		(*list)+=new LongTextAttribute(TagFromName(ImageComments),str);
		if (str) delete[] str;
	}
	{
		// AdditionalPatientHistory

		Description desc(' ');
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC1,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC2,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC3,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC4,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC5,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC6,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC7,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC8,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC9,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC10,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC11,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC12,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC13,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC14,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC15,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC16,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC17,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC18,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC19,17);
		desc.add(SOMP_HeaderInstance_IMGHDR2->IEXDESC20,17);

		char *str=desc.get();

		(*list)+=new LongTextAttribute(TagFromName(AdditionalPatientHistory),str);
		if (str) delete[] str;
	}
#endif
}

