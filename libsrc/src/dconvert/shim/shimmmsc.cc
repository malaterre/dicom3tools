#include "shimdc.h"
#include "elmconst.h"

void 
SHIM_Header_BothClass::ToDicom_ManualMisc(AttributeList *list,unsigned imagenumber)
{
	(void)list; (void)imagenumber;
#ifdef CRAP
	ostrstream imagecommentstream;

	// ImageType

	const char *value1,*value2,*value3;

	int istudy=SHIM_HeaderInstance_FILEHDR->SHIM_Method_ExtractTaggedInteger("ISTUDY");
	imagecommentstream << "ISTUDY == " << dec << istudy << " is ";

	switch (istudy) {
		case  1:	// localization
			imagecommentstream << "LOCALIZATION";
			value1="ORIGINAL";
			value2="PRIMARY";
			value3="LOCALIZER";
			break;
		case  2:	// flow study
			imagecommentstream << "FLOW STUDY";
			value1="ORIGINAL";
			value2="PRIMARY";
			value3="AXIAL";
			break;
		case  3:	// movie study
			imagecommentstream << "MOVIE STUDY";
			value1="ORIGINAL";
			value2="PRIMARY";
			value3="AXIAL";
			break;
		case  4:	// average volume study
			imagecommentstream << "AVERAGE VOLUME STUDY";
			value1="ORIGINAL";
			value2="PRIMARY";
			value3="AXIAL";
			break;
		case  5:	// volume study
			imagecommentstream << "VOLUME STUDY";
			value1="ORIGINAL";
			value2="PRIMARY";
			value3="AXIAL";
			break;
		case  6:	// average flow study
			imagecommentstream << "AVERAGE FLOW STUDY";
			value1="ORIGINAL";
			value2="PRIMARY";
			value3="AXIAL";
			break;
		case  7:	// continuous volume study
			imagecommentstream << "CONTNUOUS VOLUME STUDY";
			value1="ORIGINAL";
			value2="PRIMARY";
			value3="AXIAL";
			break;
		case  51:	// image averaging
			imagecommentstream << "IMAGE AVERAGING";
			value1="DERIVED";
			value2="SECONDARY";
			value3="AXIAL";
			break;
		case  52:	// reformat
			imagecommentstream << "REFORMAT";
			value1="DERIVED";
			value2="SECONDARY";
			value3="REFORMAT";			// my own :(
			break;
		case  53:	// FIP Maximum Difference
			imagecommentstream << "FIP MAXIMUM DIFFERENCE";
			value1="DERIVED";
			value2="SECONDARY";
			value3="UNKNOWN";			// my own :(
			break;
		case  54:	// FIP Time to Peak
			imagecommentstream << "FIP TIME TO PEAK";
			value1="DERIVED";
			value2="SECONDARY";
			value3="UNKNOWN";			// my own :(
			break;
		case  55:	// FIP Area Under Curve
			imagecommentstream << "FIP AREA UNDER CURVE";
			value1="DERIVED";
			value2="SECONDARY";
			value3="UNKNOWN";			// my own :(
			break;
		case  56:	// FIP Center of Mass
			imagecommentstream << "FIP CENTER OF MASS";
			value1="DERIVED";
			value2="SECONDARY";
			value3="UNKNOWN";			// my own :(
			break;

		case  102:	// image subtraction flow
			imagecommentstream << "IMAGE SUBTRACTION FLOW";
			value1="DERIVED";
			value2="SECONDARY";
			value3="AXIAL";
			break;
		case  103:	// image subtraction movie
			imagecommentstream << "IMAGE SUBTRACTION MOVIE";
			value1="DERIVED";
			value2="SECONDARY";
			value3="AXIAL";
			break;
		case  105:	// image subtraction volume
			imagecommentstream << "IMAGE SUBTRACTION VOLUME";
			value1="DERIVED";
			value2="SECONDARY";
			value3="AXIAL";
			break;
		case  106:	// image subtraction average flow
			imagecommentstream << "IMAGE SUBTRACTION AVERAGE FLOW";
			value1="DERIVED";
			value2="SECONDARY";
			value3="AXIAL";
			break;

		case -1:	// screen save
			imagecommentstream << "SCREEN SAVE";
			value1="DERIVED";
			value2="SECONDARY";
			value3="UNKNOWN";			// my own :(
			break;
		case  0:	// special
			imagecommentstream << "SPECIAL CASE";
			value1="DERIVED";
			value2="SECONDARY";
			value3="UNKNOWN";			// my own :(
			break;
		default:
			imagecommentstream << "UNRECOGNIZED";
			value1="DERIVED";
			value2="SECONDARY";
			value3="UNKNOWN";			// my own :(
			break;
	}

	(*list)+=new CodeStringAttribute(TagFromName(ImageType),
		value1,value2,value3);

	imagecommentstream << ", ";

	// RotationDirection

	const char *rotationdirection;

	switch(SHIM_HeaderInstance_SLICEHDR->
			SHIM_Method_ExtractTaggedInteger(imagenumber,"IROTA")) {
		case -1:	rotationdirection="CC"; break;
		case  1:	rotationdirection="CW"; break;
		default:	rotationdirection="";   break;
	}

	(*list)+=new CodeStringAttribute(TagFromName(RotationDirection),rotationdirection);

	// ExposureTime

	// derive from number of DAS samples per detector per source fan
	// and number of averages per slice

	Float64 exposuretime;
	Float64 numberofaveragesperslice=SHIM_HeaderInstance_FILEHDR->SHIM_Method_ExtractTaggedInteger("NSLAVG");

	int irep=SHIM_HeaderInstance_FILEHDR->SHIM_Method_ExtractTaggedInteger("IREP");
	imagecommentstream << "IREP == " << dec << irep << " is ";

	switch (irep) {
		case 3:	// Multi-Slice Mode
			imagecommentstream << "MULTI-SLICE MODE";
			exposuretime=50*numberofaveragesperslice;
			break;
		case 6:	// Single-Slice Mode
			imagecommentstream << "SINGLE-SLICE MODE";
			exposuretime=100*numberofaveragesperslice;
			break;
		default:
			imagecommentstream << "UNRECOGNIZED MODE";
			exposuretime=0;
			break;
	}

	(*list)+=new DecimalStringAttribute(TagFromName(ExposureTime),exposuretime);

	char *imagecommentstring=imagecommentstream.str();
	(*list)+=new LongTextAttribute(TagFromName(ImageComments),imagecommentstring);
	if (imagecommentstring) delete[] imagecommentstring;
#endif /* CRAP */
}

