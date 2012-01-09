#include "pqdc.h"
#include "elmconst.h"

void 
PQ_Header_BothClass::ToDicom_ManualMisc(AttributeList *list)
{
	// General Patient and Patient Study Modules

	{
		// PatientID

		char *pq_patient_id;
		int length;
		if (getAsciiAttribute("PATID",pq_patient_id,length)) {
			(*list)+=new LongStringAttribute(TagFromName(PatientID),pq_patient_id);
			if (pq_patient_id) delete[] pq_patient_id;
		}
		else {
			(*list)+=new LongStringAttribute(TagFromName(PatientID));	// Type 2
		}
	}
	{
		// PatientName

		char *pq_patient_last_name;
		int length;
		if (getAsciiAttribute("PATNAM",pq_patient_last_name,length)) {
			(*list)+=new PersonNameAttribute(TagFromName(PatientName),pq_patient_last_name);
			if (pq_patient_last_name) delete[] pq_patient_last_name;
		}
		else {
			(*list)+=new PersonNameAttribute(TagFromName(PatientName));	// Type 2
		}
	}
	{
		// PatientBirthDate

		(*list)+=new DateStringAttribute(TagFromName(PatientBirthDate));	// Type 2
	}
	{
		// PatientAge

		Int32 pq_patient_age;
		int length;
		ostrstream ost;
		ost << setfill('0') << setw(3) << dec;
		if (getIntegerAttribute("PATAGYR",pq_patient_age) || getIntegerAttribute("PATAGE",pq_patient_age)) {
			ost << pq_patient_age << "Y";
		}
		else if (getIntegerAttribute("PATAGMO",pq_patient_age)) {
			ost << pq_patient_age << "M";
		}
		else if (getIntegerAttribute("PATAGDA",pq_patient_age)) {
			ost << pq_patient_age << "D";
		}
		else if (getIntegerAttribute("PATAGHR",pq_patient_age)) {
			ost << unsigned(pq_patient_age/24) << "D";		// no hours in DICOM
		}
		ost << ends;
		char *agestr=ost.str();
		if (agestr) {
			if (strlen(agestr)) (*list)+=new AgeStringAttribute(TagFromName(PatientAge),agestr);
			delete[] agestr;
		}
	}
	{
/*
* Valid Values of: Patient_Sex == PATSEX
*=RNG PATSEX
Female
*=RNG PATSEX
Male
*=RNG PATSEX
Other
*/
		// PatientSex

		const char *patient_sex = 0;
		char *pq_patient_sex;
		int length;
		if (getAsciiAttribute("PATSEX",pq_patient_sex,length)) {
			if (length >= 1 && strncmp(pq_patient_sex,"M",1) == 0) {
				patient_sex="M";
			}
			else if (length >= 1 && strncmp(pq_patient_sex,"F",1) == 0) {
				patient_sex="F";
			}
			else if (length >= 1 && strncmp(pq_patient_sex,"O",1) == 0) {
				patient_sex="O";
			}
			if (pq_patient_sex) delete[] pq_patient_sex;
		}
		if (!patient_sex) patient_sex="";	// Type 2
		(*list)+=new CodeStringAttribute(TagFromName(PatientSex),patient_sex);
	}

	// General Study Module

	{
		// StudyID

		Int32 pq_study_number;
		int length;
		if (getIntegerAttribute("STNUM",pq_study_number)) {
			(*list)+=new ShortStringAttribute(TagFromName(StudyID),pq_study_number);
		}
		else {
			(*list)+=new ShortStringAttribute(TagFromName(StudyID));
		}
	}
	
	// AccessionNumber

	(*list)+=new ShortStringAttribute(TagFromName(AccessionNumber));	// Type 2

	{
		// ReferringPhysicianName

		char *pq_referring_physician;
		int length;
		if (getAsciiAttribute("REFPHY",pq_referring_physician,length)) {
			(*list)+=new PersonNameAttribute(TagFromName(ReferringPhysicianName),pq_referring_physician);
			if (pq_referring_physician) delete[] pq_referring_physician;
		}
		else {
			(*list)+=new PersonNameAttribute(TagFromName(ReferringPhysicianName));	// Type 2
		}
	}
	{
		// NameOfPhysiciansReadingStudy

		char *pq_referring_radiologist;
		int length;
		if (getAsciiAttribute("REFRAD",pq_referring_radiologist,length)) {
			(*list)+=new PersonNameAttribute(TagFromName(NameOfPhysiciansReadingStudy),pq_referring_radiologist);
			if (pq_referring_radiologist) delete[] pq_referring_radiologist;
		}
		// Type 3
	}

	// General Series Module

	{

		// SeriesNumber

		// PQ header doesn't have a series number ... put in different
		// series based on recon algorithm (since image numbers duplicated)
		
		Uint16 series_number=0;

		char *pq_recon_algorithm;
		int length;
		if (getAsciiAttribute("RCNALG",pq_recon_algorithm,length)) {
			if (length >= 4 && strncmp(pq_recon_algorithm,"????",4) == 0) series_number=1;	// localizer
			else if (length >= 4 && strncmp(pq_recon_algorithm,"STD",3) == 0) series_number=2;
			else if (length >= 4 && strncmp(pq_recon_algorithm,"BONE",4) == 0) series_number=3;
			else if (length >= 1) series_number=4;						// don't know
			if (pq_recon_algorithm) delete[] pq_recon_algorithm;
		}

		(*list)+=new IntegerStringAttribute(TagFromName(SeriesNumber),series_number);
	}

	// Laterality

	(*list)+=new CodeStringAttribute(TagFromName(Laterality));	// Type 2C

	// Modality

	{
/*
* Valid Values of: Modality == MODAL:
*=RNG MODAL
CT Scanner (includes CT Pilots?)
*=RNG MODAL
EKG 
*=RNG MODAL
*=RNG MODAL
NUCL
Nuclear Medicine
*=RNG MODAL
PET Scan
*=RNG MODAL
Projection Radiography
*/
		char *pq_modality;
		int length;
		const char *modality;
		if (getAsciiAttribute("MODAL",pq_modality,length)) {
			if (length >= 2 && strncmp(pq_modality,"CT",2) == 0) {
				modality="CT";
			}
			if (pq_modality) delete[] pq_modality;
		}
		if (!modality) modality="OT";
		(*list)+=new CodeStringAttribute(TagFromName(Modality),modality);
	}

	{
		// OperatorsName

		char *pq_operator_id;
		int length;
		if (getAsciiAttribute("OPERID",pq_operator_id,length)) {
			(*list)+=new PersonNameAttribute(TagFromName(OperatorsName),pq_operator_id);
			if (pq_operator_id) delete[] pq_operator_id;
		}
		// Type 3
	}

	{
		// ProtocolName

		char *pq_scan_protocol;
		int length;
		if (getAsciiAttribute("PRTSCAN",pq_scan_protocol,length)) {
			(*list)+=new LongStringAttribute(TagFromName(ProtocolName),pq_scan_protocol);
			if (pq_scan_protocol) delete[] pq_scan_protocol;
		}
		// Type 3
	}


	// General Equipment Module

	// Manufacturer

	(*list)+=new LongStringAttribute(TagFromName(Manufacturer),"Picker");

	{
		// InstitutionName

		char *pq_hospital_name;
		int length;
		if (getAsciiAttribute("SCNHOSP",pq_hospital_name,length)) {
			(*list)+=new LongStringAttribute(TagFromName(InstitutionName),pq_hospital_name);
			if (pq_hospital_name) delete[] pq_hospital_name;
		}
		// Type 3
	}

	{
		// ManufacturerModelName

		char *pq_gantry_type;
		int length;
		if (getAsciiAttribute("TYP_GNT",pq_gantry_type,length)) {
			(*list)+=new LongStringAttribute(TagFromName(ManufacturerModelName),pq_gantry_type);
			if (pq_gantry_type) delete[] pq_gantry_type;
		}
		// Type 3
	}

	{
		// DeviceSerialNumber

		Int32 pq_gantry_serial_number;
		if (getIntegerAttribute("S_N_GNT",pq_gantry_serial_number)) {
			(*list)+=new LongStringAttribute(TagFromName(DeviceSerialNumber),pq_gantry_serial_number);
		}
		// Type 3
	}

	{
		// SoftwareVersions

		char *pq_scanner_rev_level;
		int length;
		if (getAsciiAttribute("SCNREVL",pq_scanner_rev_level,length)) {
			(*list)+=new LongStringAttribute(TagFromName(SoftwareVersions),pq_scanner_rev_level);
			if (pq_scanner_rev_level) delete[] pq_scanner_rev_level;
		}
		// Type 3
	}

	// General Image Module

	{
		// InstanceNumber

		Int32 pq_image_number;
		if (getIntegerAttribute("IMNUM",pq_image_number)) {
			(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),pq_image_number);
		}
		else {
			(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber));	// Type 2
		}
	}

	// AcquisitionNumber

	(*list)+=new IntegerStringAttribute(TagFromName(AcquisitionNumber));	// Type 3, but 2 in CT Image

	// ImageType

	{
/*
* Valid Values of: Image Type == IMTYPE:
*=RNG IMTYPE
SLIC
Slice
*=RNG IMTYPE
PILT
Pilot
*=RNG IMTYPE
SYNT
Synthetic, look at IMTYPSY for details
*=RNG IMTYPE
DATA
Non-Image Data (Text, EKG, etc)
*/
		const char *value1="ORIGINAL";
		const char *value2="PRIMARY";
		const char *value3=0;

		char *pq_image_type;
		int length;
		if (getAsciiAttribute("IMTYPE",pq_image_type,length)) {
			if (length >= 4
			&& (strncmp(pq_image_type,"SLIC",4) == 0
			 || strncmp(pq_image_type,"SPIR",4) == 0) ) {
				value3="AXIAL";
			}
			else if (length >= 3 && strncmp(pq_image_type,"PIL",3) == 0) {	// db says PILT, files contain PILO
				value3="LOCALIZER";
			}
			if (pq_image_type) delete[] pq_image_type;
		}
		if (!value3) value3="";

		(*list)+=new CodeStringAttribute(TagFromName(ImageType),
			value1,value2,value3);
	}

	// ContrastBolus Module

	{
		// ContrastBolusAgent

		char *pq_contrast;
		int length;
		if (getAsciiAttribute("CONTR",pq_contrast,length)) {
			if (length >= 2 && strncmp(pq_contrast,"C+",2) == 0) {
				(*list)+=new LongStringAttribute(TagFromName(ContrastBolusAgent),"C+");
			}
			if (pq_contrast) delete[] pq_contrast;
		}
	}

	// CT Image Module

	{
		// KVP, XRayTubeCurrent, Exposure

		// ? need to check pilot values PILTKV, PILTMA, PILTMAS

		Int32 pq_kv;
		if (getIntegerAttribute("SCNKV",pq_kv))
			(*list)+=new DecimalStringAttribute(TagFromName(KVP),pq_kv);

		Int32 pq_ma;
		if (getIntegerAttribute("SCNMA",pq_ma))
			(*list)+=new IntegerStringAttribute(TagFromName(XRayTubeCurrent),pq_ma);

		Int32 pq_mas;
		if (getIntegerAttribute("SCNMAS",pq_mas))
			(*list)+=new IntegerStringAttribute(TagFromName(Exposure),pq_mas);

		// DistanceSourceToDetector

		Float64 pq_source_distance;
		if (getFloatAttribute("SCNSRCD",pq_source_distance))
			(*list)+=new DecimalStringAttribute(TagFromName(DistanceSourceToDetector),pq_source_distance);

		// ConvolutionKernel

		char *pq_recon_algorithm;
		int length;
		if (getAsciiAttribute("RCNALG",pq_recon_algorithm,length)) {
			(*list)+=new ShortStringAttribute(TagFromName(ConvolutionKernel),pq_recon_algorithm);
			if (pq_recon_algorithm) delete[] pq_recon_algorithm;
		}

		// DataCollectionDiameter
		// ReconstructionDiameter
		// GantryDetectorTilt
		// TableHeight

		// .. see pqmpln.h
	}

#ifdef CRAP
	// GeneratorPower

	// Loose fractional part ...

	(*list)+=new IntegerStringAttribute(TagFromName(GeneratorPower),
		(unsigned short)(atof(PQ_HeaderInstance_IMGHDR->IGENPOW)));

	{
		// SoftwareVersions

		ostrstream ost;
		ost << "System "    << PQ_HeaderInstance_STDHDR->SSYSVNO
		    << "."          << PQ_HeaderInstance_STDHDR->SSYSRNO
		    << " Database " << PQ_HeaderInstance_STDHDR->SDATVNO
		    << "."          << PQ_HeaderInstance_STDHDR->SDATRNO
		    << ends;

		char *str=ost.str();

		(*list)+=new LongStringAttribute(TagFromName(SoftwareVersions),str);
		if (str) delete[] str;
	}
	{
		// StationName 

		ostrstream ost;
		ost << PQ_HeaderInstance_STDHDR->SMODELNO_Mod << "."
		    << PQ_HeaderInstance_STDHDR->SMODELNO_Mac << "."
		    << PQ_HeaderInstance_STDHDR->SMACHINE
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
				char *lastspq=strrchr(buf,' ');
				if (lastspq && buf[maxlength-1] == ' ')
					*lastspq=0;
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
		desc.add(PQ_HeaderInstance_STDHDR->SDESC1,50);
		desc.add(PQ_HeaderInstance_STDHDR->SDESC2,50);
		desc.add(PQ_HeaderInstance_STDHDR->SDESC3,50);
		desc.add(PQ_HeaderInstance_STDHDR->SDESC4,50);
		desc.add(PQ_HeaderInstance_STDHDR->SDESC5,50);

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
		desc.add(PQ_HeaderInstance_IMGHDR2->ISRDESC1,16);
		desc.add(PQ_HeaderInstance_IMGHDR2->ISRDESC2,14);
		desc.add(PQ_HeaderInstance_IMGHDR2->ISRDESC3,12);

		char *str=desc.get();

		(*list)+=new LongStringAttribute(TagFromName(SeriesDescription),str);
		if (str) delete[] str;
	}
	{
		// ImageComments

		Description desc(' ');
		desc.add(PQ_HeaderInstance_IMGHDR2->IIMDESC1,10);
		desc.add(PQ_HeaderInstance_IMGHDR2->IIMDESC2,8);
		desc.add(PQ_HeaderInstance_IMGHDR2->IIMDESC3,7);

		char *str=desc.get();

		(*list)+=new LongTextAttribute(TagFromName(ImageComments),str);
		if (str) delete[] str;
	}
	{
		// AdditionalPatientHistory

		Description desc(' ');
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC1,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC2,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC3,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC4,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC5,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC6,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC7,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC8,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC9,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC10,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC11,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC12,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC13,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC14,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC15,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC16,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC17,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC18,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC19,17);
		desc.add(PQ_HeaderInstance_IMGHDR2->IEXDESC20,17);

		char *str=desc.get();

		(*list)+=new LongTextAttribute(TagFromName(AdditionalPatientHistory),str);
		if (str) delete[] str;
	}
#endif
}

#ifdef CRAP
<COMNT_D>	 (Display_Brief_Commnt)	 = 1 [24] [0] asc	 = <HEAD                    >
<COMNT_1>	 (Comments_1)	 = 530 [60] [0] asc	 = <HEAD                                                        >
<COMNT_2>	 (Comments_2)	 = 590 [60] [0] asc	 = <                                                            >
<COMNT_3>	 (Comments_3)	 = 650 [60] [0] asc	 = <                                                            >
<COMNT_4>	 (Comments_4)	 = 710 [60] [0] asc	 = <                                                            >


<COUCHAQ>	 (Init_Couch_Pos_1234.12)	 = 37 [8] [1] int	 = <   22900>
<COUCHHT>	 (Couch_Height)	 = 45 [4] [1] int	 = < 343>
<COUCHIM>	 (Couch_Index_Multiplier)	 = 49 [3] [1] int	 = < 10>
<COUCHIX>	 (Couch_Index)	 = 52 [6] [1] int	 = <   -50>
<COUCH>	 (Couch_Posit_1234.12)	 = 29 [8] [1] int	 = <   22900>

<GTILT>	 (Gantry_Tilt_-12.1)	 = 98 [4] [1] int	 = <-240>

<POSHF>	 (Position_Head/Feet)	 = 300 [4] [0] asc	 = <HEAD>
<POSPRN>	 (Position_Prone/Supine)	 = 304 [4] [0] asc	 = <SUPI>

<SCNFSIZ>	 (Scan_Fld_Siz_mm)	 = 400 [4] [1] int	 = < 240>
<SCNTHKM>	 (Scan_Thickness_Multplr)	 = 446 [3] [1] int	 = < 10>
<SCNTHKN>	 (Scan_Thickness)	 = 449 [4] [1] int	 = <  50>

<RCNFSIZ>	 (Recon_Fld_Size_mm)	 = 343 [4] [1] int	 = < 240>
<RCNMTRX>	 (Recon_Matrix_Pixels)	 = 347 [4] [1] int	 = < 512>

<RCNALG>	 (Recon_Algorithm)	 = 1138 [4] [0] asc	 = <BONE>



<SCNSPED>	 (Scan_Speed)	 = 1535 [4] [1] int	 = <  15>
<SCNSPOT>	 (Scan_Focal_Spot)	 = 1539 [4] [1] int	 = <   1>
<SCNSRCD>	 (Source_Distance)	 = 1543 [16] [6] flt	 = <635.359924      >



<AAA_DEL>	 (Deleted_Flag)	 = 0 [1] [5] flg	 = <?>
<DICONTX>	 (Display_Context)	 = 58 [2] [0] asc	 = < A>
<FA_HLMN>	 (HL_Manual_Xmit_Flg[])	 = 60 [16] [5] flg	 = <????????????????>
<FOLDER>	 (Folder_Name)	 = 76 [8] [0] asc	 = <TAPE    >
<F_A_EXB>	 (Exabyte_Auto_Arch_Fl)	 = 84 [1] [5] flg	 = <X>
<F_A_OPD>	 (O-Disk_Auto_Arch_Fl)	 = 85 [1] [5] flg	 = < >
<F_CMPRS>	 (Image_Compressed_Flg)	 = 86 [1] [5] flg	 = <0>
<F_COMPL>	 (Image_Complete_Flag)	 = 87 [1] [5] flg	 = <C>
<F_DELOK>	 (Image_Delete_OK_Flag)	 = 88 [1] [5] flg	 = < >
<F_FILMD>	 (Image_Filmed_Flag)	 = 89 [1] [5] flg	 = <?>
<F_FILOK>	 (Image_Film_OK_Flag)	 = 90 [1] [5] flg	 = < >
<F_HLANY>	 (Any_Archive_Flags_Set)	 = 91 [1] [5] flg	 = <?>
<F_HLAUT>	 (Auto_Xmit_Requested)	 = 92 [1] [5] flg	 = < >
<F_M_EXB>	 (Exabyte_Manual_Arch_Fl)	 = 93 [1] [5] flg	 = < >
<F_M_OPD>	 (Disk_Manual_Arch_Fl)	 = 94 [1] [5] flg	 = < >
<F_N_DEL>	 (Non-Deletable)	 = 95 [1] [5] flg	 = < >
<F_RPROK>	 (Image_Reproc_OK_Flag)	 = 96 [1] [5] flg	 = < >
<F_SHAPE>	 (Image_Shaped_Flag)	 = 97 [1] [5] flg	 = <S>
<IMFCLAS>	 (Img_File_Class_Flg)	 = 102 [1] [5] flg	 = <I>
<IMFICON>	 (Img_File_Contg_Flg)	 = 103 [1] [5] flg	 = <C>
<IMFILE>	 (Image_File_Name)	 = 104 [12] [4] fil	 = <IM.000012070>
<IMFIOFS>	 (Img_File_Hdr_Ofs_Byt)	 = 116 [5] [1] int	 = < 8192>
<IMFISIZ>	 (Img_File_Size_Bytes)	 = 121 [10] [1] int	 = <    532480>
<IMFPATH>	 (Image_File_Path)	 = 131 [20] [0] asc	 = </usr/IQ/datad/      >
<LABBOTT>	 (Bottom_Label)	 = 163 [12] [0] asc	 = <POSTERIOR   >
<LABLEFT>	 (Left_Side_Label)	 = 175 [12] [0] asc	 = <RIGHT       >
<LABRGHT>	 (Right_Side_Label)	 = 187 [12] [0] asc	 = <LEFT        >
<LABTOP>	 (Top_Label)	 = 199 [12] [0] asc	 = <ANTERIOR    >
<PILTANG>	 (Pilot_Angle)	 = 251 [3] [1] int	 = <???>
<PILTFLD>	 (Pilot_Half/Full)	 = 254 [4] [0] asc	 = <HALF>
<PILTFSZ>	 (Pilot_Field_Size)	 = 258 [4] [1] int	 = < 256>
<PILTHKN>	 (Pilot_Scan_Thickn)	 = 262 [4] [1] int	 = <  50>
<PILTLNG>	 (Pilot_Length_mm)	 = 270 [6] [1] int	 = <     0>
<PILTORG>	 (Pilot_Org_Pixels)	 = 284 [4] [1] int	 = <   0>
<PILTPPM>	 (Pilot_Pixel/mm)	 = 288 [8] [1] int	 = <       2>
<PILTYPE>	 (Pilot_horz/vert/obl)	 = 296 [4] [0] asc	 = <OFF >
<PVRFLAG>	 (Pvr_flag)	 = 332 [3] [0] asc	 = <OFF>
<RCNCX>	 (Recon_X_Centr_-123mm)	 = 335 [4] [1] int	 = <   0>
<RCNCY>	 (Recon_Y_Centr_-123mm)	 = 339 [4] [1] int	 = <   0>
<RPRFLAG>	 (Reprocessed_Flag)	 = 375 [1] [5] flg	 = <R>
<RPRTAG>	 (Reprocess_Seq_Tag)	 = 376 [2] [0] asc	 = <A >
<SCATFLG>	 (Scatter_Correct_flag)	 = 378 [1] [5] flg	 = <H>
<SCNCOLL>	 (Scan_collimation)	 = 379 [4] [1] int	 = <  50>
<SCNFLD>	 (Scan_Fld_[HALF|FULL])	 = 389 [4] [0] asc	 = <HALF>
<SCNFLDX>	 (Scan_Fld_[FULLCTR])	 = 393 [7] [0] asc	 = <HALF   >
<SCNSOF>	 (Start_of_Field)	 = 440 [6] [1] int	 = < 36968>
<SRCE>	 (Source_SCAN/TAPE/NET)	 = 461 [4] [0] asc	 = <tape>
<ST_UNIQ>	 (Study_Collis_Resolvr)	 = 481 [2] [0] asc	 = <  >
<VOLUSED>	 (Volume_Datafile_Used)	 = 483 [4] [0] asc	 = <    >
<AAA_FNU>	 (File_Seq_in_Study)	 = 488 [6] [1] int	 = <??????>
<AAA_NXT>	 (O-Disk_Study_Next_Pos)	 = 494 [10] [1] int	 = <??????????>
<AAA_POS>	 (O-Disk_Study_Pos_Sect)	 = 504 [10] [1] int	 = <??????????>
<AAA_SEQ>	 (Study_Seq_on_Tape)	 = 514 [6] [1] int	 = <??????>
<ACQTAG>	 (Acquisition_Tag)	 = 520 [2] [0] asc	 = < A>
<ACQ_BAT>	 (Batch_Acquisition_Set)	 = 522 [2] [0] asc	 = <??>
<AIRINTP>	 (Aircal_Interpolated)	 = 524 [1] [0] asc	 = <?>
<BEAMLIM>	 (Beam_Limiter_On_Off)	 = 525 [4] [0] asc	 = <????>
<CALSRC>	 (Cal Source)	 = 529 [1] [0] asc	 = <S>
<DICOM3>	 (DICOM3)	 = 770 [16] [0] asc	 = <                >
<DRGCODE>	 (DRG_Code)	 = 786 [12] [0] asc	 = <            >
<FLATSRC>	 (FLATSS_Source)	 = 798 [1] [0] asc	 = <?>
<F_ARCOK>	 (Manual_Archive_OK_Fl)	 = 799 [1] [5] flg	 = < >
<F_M_FLP>	 (Floppy_Manual_Arch_Fl)	 = 800 [1] [5] flg	 = <?>
<F_REVU>	 (Image_Reviewed_Flag)	 = 801 [1] [5] flg	 = <?>
<GNTNDET>	 (#_Dets_on_Gantry)	 = 802 [4] [1] int	 = <4800>
<IMFISHP>	 (Img_File_Shape_Table)	 = 806 [8] [0] asc	 = <????????>
<IMFI_X>	 (Img_File_X_Width)	 = 814 [4] [1] int	 = <????>
<IMFI_Y>	 (Img_File_Y_Height)	 = 818 [4] [1] int	 = <????>
<IMFPSIZ>	 (Img_Fil_PhySize_Bytes)	 = 822 [10] [1] int	 = <    532480>
<IMGFLIP>	 (Image_Flipped_Yes_No)	 = 832 [3] [0] asc	 = <NO >
<LEVEL>	 (Level)	 = 835 [6] [1] int	 = <   400>
<L_ALG>	 (Lng_Algorithm)	 = 841 [8] [1] int	 = <????????>
<L_BADET>	 (Lng_Bad_Detectors)	 = 849 [8] [1] int	 = <????????>
<L_CAL>	 (Lng_Calib_Overall)	 = 857 [8] [1] int	 = <????????>
<L_CENTR>	 (Lng_Centering)	 = 865 [8] [1] int	 = <????????>
<L_COEFS>	 (Lng_nth_Ord_Coef)	 = 873 [8] [1] int	 = <????????>
<L_GEO_C>	 (Lng_Geom_Correct)	 = 881 [8] [1] int	 = <????????>
<L_LINZT>	 (Lng_Linearization)	 = 889 [8] [1] int	 = <????????>
<L_LMPRF>	 (Lng_Lump_Profile)	 = 897 [8] [1] int	 = <????????>
<L_LMSFT>	 (Lng_Lump_Shift_Pos)	 = 905 [8] [1] int	 = <????????>
<L_OFFS>	 (Lng_AirCal_Offs)	 = 913 [8] [1] int	 = <????????>
<L_PICOL>	 (Pilot_Col_Alg_Length)	 = 921 [8] [1] int	 = <????????>
<L_PIROW>	 (Pilot_Row_Alg_Length)	 = 929 [8] [1] int	 = <????????>
<L_PROFL>	 (Lng_AirCal_Profile)	 = 937 [8] [1] int	 = <????????>
<L_RIPPL>	 (Lng_Ripple_Vector)	 = 945 [8] [1] int	 = <????????>
<N_IMDIS>	 (#_Images_Displayable)	 = 953 [3] [1] int	 = <???>
<N_IMRAW>	 (#_Img_Raw_Data)	 = 956 [3] [1] int	 = <???>
<N_IMTOT>	 (#_Images_Total)	 = 959 [3] [1] int	 = <???>
<O_1ORDC>	 (Ofs_1st_Ord)	 = 974 [10] [1] int	 = <??????????>
<O_ALG>	 (Ofs_Algorithm)	 = 984 [10] [1] int	 = <??????????>
<O_BADET>	 (Ofs_Bad_Detectors)	 = 994 [10] [1] int	 = <??????????>
<O_CAL>	 (Ofs_Calib_Overall)	 = 1004 [10] [1] int	 = <??????????>
<O_CENTR>	 (Ofs_Centering)	 = 1014 [10] [1] int	 = <??????????>
<O_GEO_C>	 (Ofs_Geom_Correct)	 = 1024 [10] [1] int	 = <??????????>
<O_LINZT>	 (Ofs_Linearization)	 = 1034 [10] [1] int	 = <??????????>
<O_LMPRF>	 (Ofs_Lump_Profile)	 = 1044 [10] [1] int	 = <??????????>
<O_LMSFT>	 (Ofs_Lump_Shift_Pos)	 = 1054 [10] [1] int	 = <??????????>
<O_OFFS>	 (Ofs_AirCal_Offs)	 = 1064 [10] [1] int	 = <??????????>
<O_PICOL>	 (Pilot_Col_Alg_Offset)	 = 1074 [10] [1] int	 = <??????????>
<O_PIROW>	 (Pilot_Row_Alg_Offset)	 = 1084 [10] [1] int	 = <??????????>
<O_PROFL>	 (Ofs_AirCal_Profile)	 = 1094 [10] [1] int	 = <??????????>
<O_RIPPL>	 (Ofs_Ripple_Vector)	 = 1104 [10] [1] int	 = <??????????>
<O_VIEWS>	 (Ofs_Views)	 = 1114 [10] [1] int	 = <??????????>
<PILTENH>	 (Pilot_Edge_Enhanced)	 = 1133 [4] [0] asc	 = <ON  >
<RCNAVGN>	 (Recon_Averag_N_Views)	 = 1254 [4] [1] int	 = <   4>
<RCNIMRT>	 (Recon_Image_Rotation)	 = 1264 [4] [1] int	 = <   0>
<RCNNVUE>	 (Number_Of_Views)	 = 1268 [4] [1] int	 = <1200>
<RCNNXVU>	 (Recon_Next_View_Offset)	 = 1272 [8] [1] int	 = <     960>
<RCNSQNA>	 (Recon_Sequence_Name)	 = 1288 [12] [0] asc	 = <2STD        >
<RCNWGTF>	 (Vol_Weighting_Filename)	 = 1308 [80] [0] asc	 = <????????????????????????????????????????????????????????????????????????????????>
<SCNANSP>	 (Scan_Anode_Speed)	 = 1388 [4] [0] asc	 = <????>
<SCNCOMP>	 (Scan_Compensator)	 = 1400 [4] [0] asc	 = <HALF>
<SCNCSPD>	 (Scan_Couch_Speed)	 = 1404 [10] [6] flt	 = <??????????>
<SCNDELT>	 (Scan_Delta_Mode)	 = 1414 [4] [0] asc	 = <ON  >
<SCNDETN>	 (Scan_Offs_to_Det#)	 = 1418 [4] [1] int	 = <   0>
<SCNDTYP>	 (Scan_Data_Type/= float, int, long,...)	 = 1422 [1] [5] flg	 = <I>
<SCNEXRV>	 (Scan_Extra_Revs)	 = 1423 [6] [6] flt	 = <0.7500>
<SCNFAN>	 (Scan_Fan_#_Dets)	 = 1429 [4] [1] int	 = < 680>
<SCNFILT>	 (Scan_Filter)	 = 1433 [8] [0] asc	 = <       0>
<SCNGAP>	 (Scan_Gap_#_Dets)	 = 1441 [4] [1] int	 = <  64>
<SCNGBCN>	 (Scan_Global_Centrng)	 = 1445 [16] [6] flt	 = <0.250000        >
<SCNINTM>	 (Scan_Integ_Period)	 = 1461 [4] [1] int	 = <5243>
<SCNLPTS>	 (Scan_#_Byt_Preceedg)	 = 1465 [4] [1] int	 = <   8>
<SCNMAOF>	 (Scan_Ma_Offset)	 = 1469 [16] [6] flt	 = <-2705.729980    >
<SCNNREC>	 (Scan_#_Records/File)	 = 1485 [8] [1] int	 = <     512>
<SCNNREF>	 (Scan_#_Ref_Dets)	 = 1493 [4] [1] int	 = <  20>
<SCNNREV>	 (Scan_#_Revs)	 = 1497 [4] [1] int	 = <   1>
<SCNOVER>	 (Scan_#_Overscan_Dets)	 = 1501 [4] [1] int	 = < 256>
<SCNPACK>	 (Scan_Data_Packing)	 = 1505 [4] [0] asc	 = <ON  >
<SCNPART>	 (#_of_PAR_Detectors)	 = 1509 [4] [0] asc	 = <   0>
<SCNPITC>	 (Spiral_Pitch_Factor)	 = 1513 [6] [6] flt	 = <??????>
<SCNRCSZ>	 (Scan_Record_Size_Byte)	 = 1519 [4] [1] int	 = <1024>
<SCNSAMP>	 (Scan_Sample)	 = 1531 [4] [1] int	 = <1024>
<SCNTPTS>	 (Scan_#_Byt_Trailing)	 = 1559 [4] [1] int	 = <????>
<S_N_CPU>	 (CPU_Serial_#)	 = 1563 [4] [1] int	 = <????>
<TYP_CPU>	 (CPU_Type)	 = 1571 [4] [0] asc	 = <????>
<TYP_TUB>	 (X-Tube_Type)	 = 1587 [4] [1] int	 = <   6>
<WINDOW>	 (Window)	 = 1591 [6] [1] int	 = <  4000>
<ZSCNINX>	 (Spiral_Acq_Lngth)	 = 1597 [8] [6] flt	 = <0.000000>






* Valid Values of: Image File Type == IMFTYPE:
* --- Values for CT Scan Image Data:
*=RNG IMFTYPE
SRCF
Source Fan Data
*=RNG IMFTYPE
DETF
Detector Fan Data
*=RNG IMFTYPE
RIPV
Ripple Vector
*=RNG IMFTYPE
AIRC
Air Wing Corrected Data
*=RNG IMFTYPE
RIPC
Ripple Corrected Data
*=RNG IMFTYPE
CENC
Fan Centered Data
*=RNG IMFTYPE
BDDC
Bad Detector Replacement
*=RNG IMFTYPE
CAIR
Compressed Air Cal Data
*=RNG IMFTYPE
SPEC
Spectrum Corrected Data
*=RNG IMFTYPE
RFAN
Refanned Data (Linearization Vector)
*=RNG IMFTYPE
GEOC
Geometric Corrected Data
*=RNG IMFTYPE
PCOM
Projection Completion or Zero Fill
*=RNG IMFTYPE
FDAT
Filtered Data
*=RNG IMFTYPE
BPVW
Views for Backprojection
*=RNG IMFTYPE
IMAG
Image Data
*=RNG IMFTYPE
PRAW
Pilot Raw Data
* --- Other Image File Types:
*=RNG IMFTYPE
EKG Data File
*=RNG IMFTYPE
TEXT
Text File
* Valid Values of: Image Type == IMTYPE:
*=RNG IMTYPE
SLIC
Slice
*=RNG IMTYPE
PILT
Pilot
*=RNG IMTYPE
SYNT
Synthetic, look at IMTYPSY for details
*=RNG IMTYPE
DATA
Non-Image Data (Text, EKG, etc)
* Valid Values of: Modality == MODAL:
*=RNG MODAL
CT Scanner (includes CT Pilots?)
*=RNG MODAL
EKG 
*=RNG MODAL
*=RNG MODAL
NUCL
Nuclear Medicine
*=RNG MODAL
PET Scan
*=RNG MODAL
Projection Radiography
* Valid Values of: Patient_Sex == PATSEX
*=RNG PATSEX
Female
*=RNG PATSEX
Male
*=RNG PATSEX
Other
* Valid Values of: Position_Head/Feet == POSHF:
*=RNG POSHF
FEET
Feet Toward Gantry
*=RNG POSHF
HEAD
Head Toward Gantry
* Valid Values of: Source_SCAN/TAPE/NET == SRCE    
*=RNG SRCE
SCAN
Scanned on this machine
*=RNG SRCE
TAPE
Retrieved from tape
*=RNG SRCE
ODSK
Retrieved from optical disk
*=RNG SRCE
NETW
Recieved from network

#endif
