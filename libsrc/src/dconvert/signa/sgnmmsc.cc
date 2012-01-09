#include "sgndc.h"
#include "elmconst.h"

void 
SGN_Header_BothClass::ToDicom_ManualMisc(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);

	{
		// ImageType

		const char *value1,*value2,*value3;

		switch (SGN_HeaderInstance_SERIESHDR->SeriesType) {
			case 0:		// Normal
					value1="ORIGINAL";
					value2="PRIMARY";
					value3="OTHER";
					break;
			case 1:		// Screensave
					value1="DERIVED";
					value2="PRIMARY";
					value3="OTHER";
					break;
			case 2:		// Composite
					value1="DERIVED";
					value2="PRIMARY";
					value3="OTHER";
					break;
			default:
					value1="UNKNOWN";
					value2="UNKNOWN";
					value3="UNKNOWN";
					break;
		}

		(*list)+=new CodeStringAttribute(TagFromName(ImageType),value1,value2,value3);
	}

	{
		// MagneticFieldStrength - Tesla

		(*list)+=new
			DecimalStringAttribute(TagFromName(MagneticFieldStrength),
				Uint32(SGN_HeaderInstance_SERIESHDR->MagneticFieldStrengthGauss)/10000.0);

		// ImagingFrequency MHz (Signa is in Hz)

		(*list)+=new
			DecimalStringAttribute(TagFromName(ImagingFrequency),
				Uint32(SGN_HeaderInstance_IMAGEHDR->TransmitFreq)/1000000.0);
	}

	{
		// RepetitionTime
		// EchoTime
		// InversionTime

		Uint32 tr_us=SGN_HeaderInstance_IMAGEHDR->RepetitionTimeUS;
		if (tr_us) {
			(*list)+=new DecimalStringAttribute(TagFromName(RepetitionTime),tr_us/1000.0);
		}

		Uint32 te_us=SGN_HeaderInstance_IMAGEHDR->EchoTimeUS;
		if (te_us) {
			(*list)+=new DecimalStringAttribute(TagFromName(EchoTime),te_us/1000.0);
		}

		Uint32 ti_us=SGN_HeaderInstance_IMAGEHDR->InversionTimeUS;
		if (ti_us) {
			(*list)+=new DecimalStringAttribute(TagFromName(InversionTime),ti_us/1000.0);
		}
		
		double sctime_fd=SGN_HeaderInstance_IMAGEHDR->ScanTimeUS;
		if (sctime_fd) {
			(*list)+=new FloatDoubleAttribute(TagFromName(AcquisitionDuration),sctime_fd/1000000.0);
		}
		
		// EchoTrainLength

		unsigned etl=SGN_HeaderInstance_IMAGEHDR->EchoTrainLength;
		unsigned numecho=SGN_HeaderInstance_IMAGEHDR->NumberOfEchoes;
		unsigned useetl = etl ? etl : numecho;

		if (useetl) {
			(*list)+=new IntegerStringAttribute(TagFromName(EchoTrainLength),Uint32(useetl));
		}
	}

	{
		// ScanningSequence
		// SequenceVariant
		// ScanOptions

		CodeStringAttribute *ScanningSequence =
			new CodeStringAttribute(TagFromName(ScanningSequence));

		CodeStringAttribute *SequenceVariant =
			new CodeStringAttribute(TagFromName(SequenceVariant));

		CodeStringAttribute *ScanOptions =
			new CodeStringAttribute(TagFromName(ScanOptions));

		switch (SGN_HeaderInstance_SERIESHDR->PulseSequence) {
			case 0:			// MEMP
					ScanningSequence->addValue("SE");
					break;
			case 1:			// IR
					ScanningSequence->addValue("IR");
					break;
			case 2:			// PS ? Partial saturation
						// -> not valid DICOM enum :(
					ScanningSequence->addValue("PS");
					break;
			case 3:			// RM
					ScanningSequence->addValue("RM");
					break;
			case 4:			// RMGE
					ScanningSequence->addValue("RM");
					break;
			case 5:			// GRE
					ScanningSequence->addValue("GR");
					break;
			case 6:			// VEMP
					ScanningSequence->addValue("SE");
					break;
			case 7:			// MPGR
					ScanningSequence->addValue("GR");
					break;
			case 8:			// MPGRV
					ScanningSequence->addValue("GR");
					break;
			case 9:			// MPIRS
					ScanningSequence->addValue("IR");
					break;
			case 10:		// MPIRI
					ScanningSequence->addValue("IR");
					break;
			case 11:		// 3D/GRE
					ScanningSequence->addValue("GR");
					break;
			case 12:		// CINE/GRE
					ScanningSequence->addValue("GR");
					break;
			case 13:		// SPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					break;
			case 14:		// SSFP
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("TRSS");
					break;
			case 15:		// CINE/SPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					break;
			case 16:		// 3D/SPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					break;
			case 17:		// FSE
					ScanningSequence->addValue("SE");
					SequenceVariant->addValue("SK");
					break;
			case 18:		// FVE
					ScanningSequence->addValue("SE");
					SequenceVariant->addValue("SK");
					break;
			case 19:		// FSPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					SequenceVariant->addValue("SK");
					break;
			case 20:		// FGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SK");
					break;
			case 21:		// FMPSPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SP");
					SequenceVariant->addValue("SK");
					break;
			case 22:		// FMPGR
					ScanningSequence->addValue("GR");
					SequenceVariant->addValue("SK");
					break;
			case 23:		// FMPIR
					ScanningSequence->addValue("IR");
					SequenceVariant->addValue("SK");
					break;
			case 24:		// PROBE.S
			case 25:		// PROBE.P
					break;
			default:	break;
		}

		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<0) {	// EG - ECG Gated
			ScanOptions->addValue("CG");
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<1) {	// RESP - Resp Gated
			ScanOptions->addValue("RG");
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<2) {	// RC - Resp Compensated
			ScanOptions->addValue("PER");
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<3) {	// FC - Flow Compensated
			ScanOptions->addValue("FC");
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<4) {	// CL
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<6) {	// PG - Periph Gated
			ScanOptions->addValue("PPG");
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<7) {	// NP - No Phase Wrap
			SequenceVariant->addValue("OSP");
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<8) {	// RF - ?
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<9) {	// RT - Rect FOV
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<10) {	// VB - Var Bandwidth
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<11) {	// ED - Ext Dynamic Rng
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<12) {	// PM - POMP
		}
		if (SGN_HeaderInstance_SERIESHDR->GatingType & 1<<15) {	// MP - Multiplanar
		}

		if ((SGN_HeaderInstance_SERIESHDR->GatingType & 1<<5)	// ST - Sat parameters
		 || SGN_HeaderInstance_SERIESHDR->Sat					// any sat bits set
		) {
			ScanOptions->addValue("SP");		// spatial presaturation
		}

		switch (SGN_HeaderInstance_SERIESHDR->SuppTech) {
			case 1:
				ScanOptions->addValue("FS");	// fat saturation
				break;
			case 2:
				ScanOptions->addValue("WS");	// water saturation
				break;			// not DICOM def term
			default:
				break;
		}

		if (SGN_HeaderInstance_SERIESHDR->SCIC) {
			ScanOptions->addValue("IIC");	// intensity correction
		}					// not DICOM def term

		if (SGN_HeaderInstance_IMAGEHDR->FracEcho & 1<<0) {
			ScanOptions->addValue("PFF");	// partial fourier freq
		}

		if (SGN_HeaderInstance_SERIESHDR->GatingType2 & 1<<0	// Inversion Recovery:IR
		 || SGN_HeaderInstance_SERIESHDR->GatingType2 & 1<<1) {	// Driven Equilibrium:DE
			SequenceVariant->addValue("MP");	// mag prepared
		}

		if (!SequenceVariant->getVM())
			SequenceVariant->addValue("NONE");

		(*list)+=ScanningSequence;
		(*list)+=SequenceVariant;
		(*list)+=ScanOptions;
	}

	{
		// MRAcquisitionType

		const char *str;
		switch(SGN_HeaderInstance_SERIESHDR->ImageMode) {
			case 0:			// 2D single
			case 1:			// 2D multiple
				str="2D";
				break;
			case 2:			// 3D volume
				str="3D";
				break;
			case 3:			// Cine:Cine
			case 4:			// Spectroscopy:SPECT
			default:
				str="";
				break;
		}
		(*list)+=new CodeStringAttribute(TagFromName(MRAcquisitionType),str);
	}

	{
		// InPlanePhaseEncodingDirection

		const char *str;

		if (SGN_HeaderInstance_IMAGEHDR->PFSwapped)	// Swap Phase/Frequency:SPF
			str="COL";
		else
			str="ROW";
		(*list)+=new CodeStringAttribute(TagFromName(InPlanePhaseEncodingDirection),str);
	}

	{
		// Acquisition Matrix

		Uint16 freq_rows=0;
		Uint16 freq_cols=0;
		Uint16 phase_rows=0;
		Uint16 phase_cols=0;

		if (SGN_HeaderInstance_IMAGEHDR->PFSwapped) {
			freq_rows=SGN_HeaderInstance_SERIESHDR->ScanMatrixX;
			phase_cols=SGN_HeaderInstance_SERIESHDR->ScanMatrixY;
		}
		else {
			freq_cols=SGN_HeaderInstance_SERIESHDR->ScanMatrixX;
			phase_rows=SGN_HeaderInstance_SERIESHDR->ScanMatrixY;
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

	{
		// Number Of Phase Encoding Steps

		(*list)+=new IntegerStringAttribute(TagFromName(NumberOfPhaseEncodingSteps),(Uint32)SGN_HeaderInstance_SERIESHDR->ScanMatrixY);
	}

	{
		// AngioFlag

		bool angio = SGN_HeaderInstance_SERIESHDR->VasMode != 0;	// 0=none,1=TOF,2=PC

		bool collapsed = SGN_HeaderInstance_IMAGEHDR->VascCollapse;

		if (angio) {
			const char *str;
			if (!collapsed)
				str="Y";
			else
				str="N";
			(*list)+=new CodeStringAttribute(TagFromName(AngioFlag),str);
		}
		// else leave it out
	}

	if (SGN_HeaderInstance_IMAGEHDR->VariableBandwidth != 0) {
		// PixelBandwidth

		(*list)+=new DecimalStringAttribute(TagFromName(PixelBandwidth),(Float32)SGN_HeaderInstance_IMAGEHDR->VariableBandwidth*2*1000);
	}
	
}

