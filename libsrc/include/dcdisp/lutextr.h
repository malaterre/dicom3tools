#ifndef __Header_lutextr__
#define __Header_lutextr__

bool extractLookUpTables(TextOutputStream &log,AttributeList& list,
	DicomLUT *&RedLUT,
	DicomLUT *&GreenLUT,
	DicomLUT *&BlueLUT);

#endif /* __Header_lutextr__ */
