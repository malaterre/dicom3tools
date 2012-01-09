#include "gawdc.h"
#include "elmconst.h"

void 
GAW_Header_BothClass::ToDicom_ManualMisc(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	{
		//    Rows = IH_img_height ? IH_img_height : CT_ or MR_imatrix_Y
		// Columns = IH_img_width  ? IH_img_width  : CT_ or MR_imatrix_X

//		Uint16    rows = GAW_HeaderInstance_FILEHDR->IH_img_height
//			? (Uint16)GAW_HeaderInstance_FILEHDR->IH_img_height
//			: (GAW_ismr

		Uint16 rows = (GAW_ismr
				? GAW_HeaderInstance_MRHDR->MR_imatrix_Y
				: GAW_HeaderInstance_CTHDR->CT_imatrix_Y);

//		Uint16 columns = GAW_HeaderInstance_FILEHDR->IH_img_width  
//			? (Uint16)GAW_HeaderInstance_FILEHDR->IH_img_width  
//			: (GAW_ismr

		Uint16 columns = (GAW_ismr
				? GAW_HeaderInstance_MRHDR->MR_imatrix_X
				: GAW_HeaderInstance_CTHDR->CT_imatrix_X);

		(*list)+=new UnsignedShortAttribute(TagFromName(Rows),rows);
		(*list)+=new UnsignedShortAttribute(TagFromName(Columns),columns);
	}

	{
		// BitsStored = depth
		// HighBit = depth - 1

		Uint16 depth;
#ifdef GENESISUSEDEPTHVALUE
		if (GAW_hashisto && Uint16(GAW_HeaderInstance_HISTOHDR->HS_hs_max) < 4096u)
			depth=12;
		else if (GAW_HeaderInstance_FILEHDR->IH_img_depth)
			depth=GAW_HeaderInstance_FILEHDR->IH_img_depth;
		else
#endif
			depth=16;

		(*list)+=new UnsignedShortAttribute(TagFromName(BitsStored),depth);
		(*list)+=new UnsignedShortAttribute(TagFromName(HighBit),depth-1);
	}

	// ContrastBolus Module

	{
		bool oral=false;
		const char *oralagent=0;
		bool iv=false;
		const char *ivagent=0;

		if (GAW_ismr) {
			if (GAW_HeaderInstance_MRHDR->MR_contmode & 0x1)
				oral=true;
			if (GAW_HeaderInstance_MRHDR->MR_contmode & 0x2)
				iv=true;
			oralagent=GAW_HeaderInstance_MRHDR->MR_contrastOral;
			ivagent=GAW_HeaderInstance_MRHDR->MR_contrastIV;
		}
		if (GAW_isct) {
			if (GAW_HeaderInstance_CTHDR->CT_contmode & 0x1)
				oral=true;
			if (GAW_HeaderInstance_CTHDR->CT_contmode & 0x2)
				iv=true;
			oralagent=GAW_HeaderInstance_CTHDR->CT_contrastOral;
			ivagent=GAW_HeaderInstance_CTHDR->CT_contrastIV;
		}

		// NB. VM of ContrastBolusAgent, etc. is 1 :(

		ostrstream oContrastBolusAgent;
		ostrstream oContrastBolusRoute;

		if (oral) {
			oContrastBolusAgent <<
			    (strlen(oralagent) ? oralagent : "ORAL AGENT UNKNOWN");
			oContrastBolusRoute << "ORAL";
		}
		if (oral && iv) {
			oContrastBolusAgent << " AND ";
			oContrastBolusRoute << " AND ";
		}
		if (iv) {
			oContrastBolusAgent <<
			    (strlen(ivagent) ? ivagent : "IV AGENT UNKNOWN");
			oContrastBolusRoute << "IV";
		}
		oContrastBolusAgent << ends;
		oContrastBolusRoute << ends;
		char *ContrastBolusAgent=oContrastBolusAgent.str();
		char *sContrastBolusRoute=oContrastBolusRoute.str();

		if (ContrastBolusAgent) {
			if (strlen(ContrastBolusAgent)) {
				(*list)+=new
					LongStringAttribute(
						TagFromName(ContrastBolusAgent),
						ContrastBolusAgent);
			}
			delete[] ContrastBolusAgent;
		}

		if (sContrastBolusRoute) {
			if (strlen(sContrastBolusRoute)) {
				(*list)+=new
					LongStringAttribute(
						TagFromName(ContrastBolusRoute),
						sContrastBolusRoute);
			}
			delete[] sContrastBolusRoute;
		}
	}

	{
		// ImageType

		const char *value1,*value2,*value3;

		switch (GAW_HeaderInstance_SERIESHDR->SE_se_typ) {
			case 1:		// Prospective
					value1="ORIGINAL";
					value2="PRIMARY";
					if (GAW_isct) value3="AXIAL";
					if (GAW_ismr) value3="OTHER";
					break;
			case 2:		// Retrospective
					value1="ORIGINAL";
					value2="SECONDARY";
					if (GAW_isct) value3="AXIAL";
					if (GAW_ismr) value3="OTHER";
					break;
			case 3:		// Scout
					value1="ORIGINAL";
					value2="PRIMARY";
					value3="LOCALIZER";	// CT IOD
					break;
			case 4:		// Reformatted
			case 5:		// Screensave
					value1="DERIVED";
					value2="SECONDARY";
					value3="OTHER";		// MR IOD
					break;
			case 6:		// Xenon
					value1="ORIGINAL";
					value2="PRIMARY";
					value3="AXIAL";		// CT IOD
			case 7:		// Service
					value1="DERIVED";
					value2="SECONDARY";
					value3="OTHER";		// MR IOD
					break;
			case 9:		// Projected
					value1="DERIVED";
					value2="SECONDARY";
					value3="PROJECTION IMAGE"; // MR IOD
					break;
			default:
					value1="UNKNOWN";
					value2="UNKNOWN";
					value3="UNKNOWN";
					break;
		}

		(*list)+=new CodeStringAttribute(TagFromName(ImageType),
				value1,value2,value3);
	}

	if (GAW_isct) {
		// ConvolutionKernel

		const char *str;

		// supposed to be a bitmap - we will assume enum :(

		switch (GAW_HeaderInstance_CTHDR->CT_reconalg) {
			case 1:		str="SMOOTH";
					break;
			case 2:		str="SOFT";
					break;
			case 4:		str="STANDARD";
					break;
			case 8:		str="DETAIL";
					break;
			case 16:	str="BONE";
					break;
			case 32:	str="EDGE";
					break;
			case 64:	str="SHARP";
					break;
			case 128:	str="EXP2";
					break;
			default:	str="UNKNOWN";
					break;
		}

		// could also utilize CT_perisflag & CT_iboneflag ??

		(*list)+=new ShortStringAttribute(TagFromName(ConvolutionKernel),str);

		// FilterType

		switch (GAW_HeaderInstance_CTHDR->CT_filttyp) {
			case 1:		str="AIR"; break;
			case 2:		str="BODY"; break;
			case 3:		str="BOWTIE:ADLT HEAD"; break;
			case 4:		str="FLAT"; break;
			case 5:		str="HI:ADULT HEAD/HF"; break;
			default:	str="UNKNOWN"; break;
		}
		(*list)+=new ShortStringAttribute(TagFromName(FilterType),str);

		// RotationDirection

		switch (GAW_HeaderInstance_CTHDR->CT_gandir) {
			case 1:		str="CW"; break;
			case 2:		str="CC"; break;
			default:	str="UNKNOWN"; break;
		}
		(*list)+=new
			ShortStringAttribute(TagFromName(RotationDirection),
				str);

		// ExposureTime
		// Exposure

		(*list)+=new
			IntegerStringAttribute(TagFromName(ExposureTime),
				Uint32(GAW_HeaderInstance_CTHDR->CT_sctime*1000.0));
		(*list)+=new
			IntegerStringAttribute(TagFromName(Exposure),
				Uint32(GAW_HeaderInstance_CTHDR->CT_sctime
					*GAW_HeaderInstance_CTHDR->CT_mamp));

		// DistanceSourceToDetector
		// DistanceSourceToPatient

		// (these are here rather than in template to avoid inclusion in MR)

		(*list)+=new
			DecimalStringAttribute(TagFromName(DistanceSourceToDetector),
				GAW_HeaderInstance_EXAMHDR->EX_srctodet);
		(*list)+=new
			DecimalStringAttribute(TagFromName(DistanceSourceToPatient),
				GAW_HeaderInstance_EXAMHDR->EX_srctoiso);
	}

	if (GAW_ismr) {
		// MagneticFieldStrength - Tesla

		(*list)+=new
			DecimalStringAttribute(TagFromName(MagneticFieldStrength),
				Uint32(GAW_HeaderInstance_EXAMHDR->EX_magstrength)/10000.0);

		// ImagingFrequency MHz (Genesis is in .1 Hz)

		(*list)+=new
			DecimalStringAttribute(TagFromName(ImagingFrequency),
				Uint32(GAW_HeaderInstance_MRHDR->MR_xmtfreq)/10000000.0);

		// RepetitionTime
		// EchoTime
		// InversionTime

		Uint32 tr_us=GAW_HeaderInstance_MRHDR->MR_tr;
		if (tr_us) (*list)+=new
			DecimalStringAttribute(TagFromName(RepetitionTime),
				tr_us/1000.0);


		Uint32 te_us=GAW_HeaderInstance_MRHDR->MR_te;
		if (te_us) (*list)+=new
			DecimalStringAttribute(TagFromName(EchoTime),
				te_us/1000.0);


		Uint32 ti_us=GAW_HeaderInstance_MRHDR->MR_ti;
		if (ti_us) (*list)+=new
			DecimalStringAttribute(TagFromName(InversionTime),
				ti_us/1000.0);

		double sctime_fd=GAW_HeaderInstance_MRHDR->MR_sctime;
		if (sctime_fd) (*list)+=new
			FloatDoubleAttribute(TagFromName(AcquisitionDuration),
				sctime_fd/1000000.0);

		// EchoTrainLength

		unsigned etl=GAW_HeaderInstance_MRHDR->MR_echo_trn_len;
		unsigned numecho=GAW_HeaderInstance_MRHDR->MR_numecho;
		unsigned useetl = etl ? etl : numecho;

		if (useetl) (*list)+=new
			IntegerStringAttribute(TagFromName(EchoTrainLength),
				Uint32(useetl));
	}

	if (GAW_ismr) {
		// ScanningSequence
		// SequenceVariant
		// ScanOptions

		CodeStringAttribute *ScanningSequence =
			new CodeStringAttribute(TagFromName(ScanningSequence));

		CodeStringAttribute *SequenceVariant =
			new CodeStringAttribute(TagFromName(SequenceVariant));

		CodeStringAttribute *ScanOptions =
			new CodeStringAttribute(TagFromName(ScanOptions));

		switch (GAW_HeaderInstance_MRHDR->MR_pseq) {
			case 0:			// SE
					ScanningSequence->addValue("SE");
					break;
			case 1:			// IR
					ScanningSequence->addValue("IR");
					break;
			case 2:			// RM:RM
					ScanningSequence->addValue("RM");
					break;
			case 3:			// RMGE:none
					ScanningSequence->addValue("RM");
					break;
			case 4:			// GRE:GR
					ScanningSequence->addValue("GR");
					break;
			case 5:			// MPGR
					ScanningSequence->addValue("GR");
					break;
			case 6:			// MPIRS:IR/s
					ScanningSequence->addValue("IR");
					break;
			case 7:			// MPIRI:IR
					ScanningSequence->addValue("IR");
					break;
			case 8:			// VOGRE:3D/GR
					ScanningSequence->addValue("GR");
					break;
			case 9:			// CINEGRE:Cine/GRE
					ScanningSequence->addValue("GR");
					break;
			case 10:		// SPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					break;
			case 11:		// SSFP
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("TRSS");
					break;
			case 12:		// TF:TOF
					ScanningSequence->addValue("GR");
					break;
			case 13:		// PC
					ScanningSequence->addValue("GR");
					break;
			case 14:		// CINSPGR:Cine/SPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					break;
			case 15:		// TOFGR:TOG/GR
					ScanningSequence->addValue("GR");
					break;
			case 16:		// TOFSPGR:TOF/SPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					break;
			case 17:		// PCGR:PC/GR
					ScanningSequence->addValue("GR");
					break;
			case 18:		// PCSPGR:PC/SPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					break;
			case 19:		// FSE
					ScanningSequence->addValue("SE");
					SequenceVariant->addValue("SK");
					break;
			case 20:		// FGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SK");
					break;
			case 21:		// FMPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SK");
					break;
			case 22:		// FSPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					SequenceVariant->addValue("SK");
					break;
			case 23:		// FMPSPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					SequenceVariant->addValue("SK");
					break;
			case 24:		// SPECT
					break;
			case 25:		// PSEQ_MIXED:MIXED
					break;
			default:	break;
		}

		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<0)) {	// EG - ECG Gated
			ScanOptions->addValue("CG");
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<1)) {	// RESP - Resp Gated
			ScanOptions->addValue("RG");
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<2)) {	// RC - Resp Compensated
			ScanOptions->addValue("PER");
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<3)) {	// FC - Flow Compensated
			ScanOptions->addValue("FC");
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<4)) {	// CL
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<5)		// ST - Sat parameters
		 || GAW_HeaderInstance_MRHDR->MR_satbits) {		// if any sat bits set
			ScanOptions->addValue("SP");			// spatial presaturation
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<6)) {	// PG - Periph Gated
			ScanOptions->addValue("PPG");
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<7)) {	// NP - No Phase Wrap
			SequenceVariant->addValue("OSP");
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<8)) {	// NF - ?
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<9)) {	// RT - Rect FOV
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<10)) {	// VB - Var Bandwidth
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<11)) {	// ED - Ext Dynamic Rng
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<12)) {	// PM - POMP
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<13)) {	// SQ - ?
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<14)) {	// CS - Contig Slice
		}
		if (GAW_HeaderInstance_MRHDR->MR_iopt & (1<<15)) {	// MP - Multiplanar
		}

		switch (GAW_HeaderInstance_MRHDR->MR_supp_tech) {
			case 1:
				ScanOptions->addValue("FS");		// fat saturation
				break;
			case 2:
				ScanOptions->addValue("WS");		// water saturation
				break;					// not DICOM def term
			default:
				break;
		}

		if (GAW_HeaderInstance_MRHDR->MR_scic) {
			ScanOptions->addValue("IIC");			// intensity correction
		}							// not DICOM def term

		if (GAW_HeaderInstance_MRHDR->MR_frac_echo & 1<<0) {
			ScanOptions->addValue("PFF");			// partial fourier freq
		}

		switch (GAW_HeaderInstance_MRHDR->MR_prep_pulse) {
			case 1:						// Inversion Recovery:IR
			case 2:						// Driven Equilibrium:DE
				SequenceVariant->addValue("MP");	// mag prepared
				break;
			default:
				break;
		}

		if (!SequenceVariant->getVM())
			SequenceVariant->addValue("NONE");

		(*list)+=ScanningSequence;
		(*list)+=SequenceVariant;
		(*list)+=ScanOptions;
	}

	if (GAW_ismr) {
		// MRAcquisitionType

		const char *str;
		switch(GAW_HeaderInstance_MRHDR->MR_imode) {
			case 1:			// Two D:2D
				str="2D";
				break;
			case 2:			// Three D Volume:3D
			case 3:			// Three D Fourier:
				str="3D";
				break;
			case 4:			// Cine:Cine
			case 5:			// Angiography:ANGIO
			case 6:			// Spectroscopy:SPECT
			case 7:			// Flouroscopy:FLOURO
			case 8:			// Research Mode:RM
			default:
				str="UNKNOWN";
				break;
		}
		(*list)+=new CodeStringAttribute(TagFromName(MRAcquisitionType),str);
	}

	if (GAW_ismr) {
		// InPlanePhaseEncodingDirection

		const char *str;

		//if (GAW_HeaderInstance_MRHDR->MR_swappf)	// Swap Phase/Frequency:SPF
		//	str="COL";
		//else
		//	str="ROW";
		//(*list)+=new CodeStringAttribute(TagFromName(InPlanePhaseEncodingDirection),str);

		if (GAW_HeaderInstance_MRHDR->MR_freq_dir == 1)
			str="COL";
		else if (GAW_HeaderInstance_MRHDR->MR_freq_dir == 2
		      || GAW_HeaderInstance_MRHDR->MR_freq_dir == 0)	// assume "unknown" is row
			str="ROW";
		else
			str=0;

		if (str)
			(*list)+=new CodeStringAttribute(TagFromName(InPlanePhaseEncodingDirection),str);
	}

	if (GAW_ismr) {
		// Acquisition Matrix

		Uint16 freq_rows=0;
		Uint16 freq_cols=0;
		Uint16 phase_rows=0;
		Uint16 phase_cols=0;

		if (GAW_HeaderInstance_MRHDR->MR_freq_dir == 1) {
			freq_rows=GAW_HeaderInstance_MRHDR->MR_dim_X;
			phase_cols=GAW_HeaderInstance_MRHDR->MR_dim_Y;
		}
		else if (GAW_HeaderInstance_MRHDR->MR_freq_dir == 2
		      || GAW_HeaderInstance_MRHDR->MR_freq_dir == 0) {	// assume "unknown" is row
			freq_cols=GAW_HeaderInstance_MRHDR->MR_dim_X;
			phase_rows=GAW_HeaderInstance_MRHDR->MR_dim_Y;
		}

		if (freq_rows || freq_cols || phase_rows || phase_cols) {
			Attribute *a=new UnsignedShortAttribute(TagFromName(AcquisitionMatrix));
			Assert(a);
			a->addValue(freq_rows);
			a->addValue(freq_cols);
			a->addValue(phase_rows);
			a->addValue(phase_cols);
			(*list)+=a;
		}
	}

	if (GAW_ismr) {
		// Number Of Phase Encoding Steps

		(*list)+=new IntegerStringAttribute(TagFromName(NumberOfPhaseEncodingSteps),(Uint32)GAW_HeaderInstance_MRHDR->MR_dim_Y);
	}
	
	if (GAW_ismr) {
		// MRAcquisitionPhaseEncodingStepsOutOfPlane (from new multiframe MR object)

		if (GAW_HeaderInstance_MRHDR->MR_imode == 2		// 3D mode
		 && GAW_HeaderInstance_MRHDR->MR_slquant > 0) {		// Number of slices in this scan group
			(*list)+=new UnsignedShortAttribute(TagFromName(MRAcquisitionPhaseEncodingStepsOutOfPlane),(Uint16)GAW_HeaderInstance_MRHDR->MR_slquant);
		}
	}
	
	if (GAW_ismr) {
		// AngioFlag

		bool angio;
		switch (GAW_HeaderInstance_MRHDR->MR_pseq) {
			case 12:		// TF:TOF
			case 13:		// PC
			case 15:		// TOFGR:TOG/GR
			case 16:		// TOFSPGR:TOF/SPGR
			case 17:		// PCGR:PC/GR
			case 18:		// PCSPGR:PC/SPGR
				angio=true;
				break;
			default:
				angio=false;
				break;
		}

		bool projected;
		switch (GAW_HeaderInstance_MRHDR->MR_proj_alg) {
			case 0:			// none
				projected=false;
				break;
			case 1:			// Prototype
			case 2:			// Minimum Pixel:Min
			case 3:			// Maximum Pixel:Max
			default:
				projected=true;
				break;
		}

		// if any bits set ...

		bool collapsed = GAW_HeaderInstance_MRHDR->MR_vas_collapse;

		if (angio) {
			const char *str;
			if (!collapsed && !projected)
				str="Y";
			else
				str="N";
			(*list)+=new CodeStringAttribute(TagFromName(AngioFlag),str);
		}
		// else leave it out
	}

	if (GAW_ismr && GAW_HeaderInstance_MRHDR->MR_vbw != 0) {
		// PixelBandwidth

		(*list)+=new DecimalStringAttribute(TagFromName(PixelBandwidth),(Float32)GAW_HeaderInstance_MRHDR->MR_vbw*2*1000);
	}
	
}

