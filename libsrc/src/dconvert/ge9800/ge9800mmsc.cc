#include "ge9800dc.h"
#include "elmconst.h"

void 
GE9800_Header_BothClass::ToDicom_ManualMisc(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);
	
	{
		// BitsStored
		// HighBit

		Int16 bitsstored = GE9800_HeaderInstance_IMAGE1HDR->DataRange;
		
		if (bitsstored <= 0) {	// seen as -1 for YL files
			bitsstored = 12;
		}

		(*list)+=new UnsignedShortAttribute(TagFromName(BitsStored),bitsstored);
		(*list)+=new UnsignedShortAttribute(TagFromName(HighBit),bitsstored-1);
	}
	{
		// Rows

		Uint16 rows =
			(GE9800_HeaderInstance_IMAGE1HDR->FileType == 2)	// Scout
			? GE9800_HeaderInstance_IMAGE1HDR->ViewsPerScan
			: GE9800_HeaderInstance_IMAGE1HDR->ImageSize;
		
		(*list)+=new UnsignedShortAttribute(TagFromName(Rows),rows);
	}
	{
		// Columns

		Uint16 columns =
			(GE9800_HeaderInstance_IMAGE1HDR->FileType == 2)	// Scout
			? GE9800_HeaderInstance_IMAGE1HDR->DetectorsPerView
			: GE9800_HeaderInstance_IMAGE1HDR->ImageSize;

		(*list)+=new UnsignedShortAttribute(TagFromName(Columns),columns);
	}
	{
		// WindowCenter
		// WindowWidth

		Int16 center = GE9800_HeaderInstance_IMAGE1HDR->LevelSetting;
		Int16 width = GE9800_HeaderInstance_IMAGE1HDR->WindowSetting;
		
		if (width > 0) {
			(*list)+=new DecimalStringAttribute(TagFromName(WindowCenter),center);
			(*list)+=new DecimalStringAttribute(TagFromName(WindowWidth),width);
		}
	}

	{
		// ImageType

		char *value1,*value2,*value3;

		switch (GE9800_HeaderInstance_IMAGE1HDR->FileType) {
			case 1:		// Prospective
					value1="ORIGINAL";
					value2="PRIMARY";
					value3="AXIAL";
					break;
			case 2:		// Scout
					value1="ORIGINAL";
					value2="PRIMARY";
					value3="LOCALIZER";
					break;
			case 3:		// Retrospective
					value1="DERIVED";
					value2="SECONDARY";
					value3="AXIAL";
					break;
			case 4:		// Segmented
					value1="DERIVED";
					value2="SECONDARY";
					value3="AXIAL";
					break;
			case 5:		// Screensave
					value1="DERIVED";
					value2="SECONDARY";
					value3="AXIAL";
					break;
			case 6:		// Plot
					value1="DERIVED";
					value2="SECONDARY";
					value3="OTHER";
					break;
			default:
					value1="";
					value2="";
					value3="";
					break;
		}

		(*list)+=new CodeStringAttribute(TagFromName(ImageType),
			value1,value2,value3);
	}
	
	{
		// ConvolutionKernel

		char *str;

		switch (GE9800_HeaderInstance_IMAGE1HDR->PreprocessReconType) {
			case 0:		str="UNDEFINED";
					break;
			case 1:		str="BONE";
					break;
			case 2:		str="SOFTTISSUE";
					break;
			case 3:		str="QUICKSOFTTISSUE";
					break;
			default:	str="";
					break;
		}

		(*list)+=new ShortStringAttribute(TagFromName(ConvolutionKernel),str);
	}

	{
		// FilterType

		char *str;
		switch (GE9800_HeaderInstance_IMAGE1HDR->BowtieFilter) {
			case 1:		str="BODY"; break;
			case 2:		str="HEAD"; break;
			case 3:		str="A"; break;
			default:	str=""; break;
		}
		(*list)+=new ShortStringAttribute(TagFromName(FilterType),str);
	}
	{
		// RotationDirection

		char *str;
		switch (GE9800_HeaderInstance_IMAGE1HDR->RotationDirection) {
			case 1:		str="CW"; break;
			case 2:		str="CC"; break;
			default:	str=""; break;
		}
		(*list)+=new CodeStringAttribute(TagFromName(RotationDirection),str);
	}
	{
		// ExposureTime

		Uint16 exposuretime = (Uint16)(GE9800_HeaderInstance_IMAGE1HDR->ScanTime * 1000.0);
		(*list)+=new IntegerStringAttribute(TagFromName(ExposureTime),exposuretime);
	}

	{
		// BodyPartExamined
		
		// guess from filter ...

		char *str;
		switch (GE9800_HeaderInstance_IMAGE1HDR->BowtieFilter) {
			case 1:		str="WHOLEBODY"; break;
			case 2:		str="HEAD"; break;
			default:	str=NULL; break;
		}
		if (str) {
			(*list)+=new CodeStringAttribute(TagFromName(BodyPartExamined),str);
		}
		else {
			(*list)+=new CodeStringAttribute(TagFromName(Laterality));		// laterality is Type 2 if can't tell if unpaired body part
		}
	}
	
}

