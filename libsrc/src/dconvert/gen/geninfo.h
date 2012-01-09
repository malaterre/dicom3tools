#ifndef __Header_geninfo__
#define __Header_geninfo__

class GEN_FileStructureInformation  {
public:
	bool useExplicitOffsets;
	bool pixelDataLengthFieldPresent;
	Uint32 explicitFileHdrPtr;
	Uint32 explicitSuiteHdrPtr;
	Uint32 explicitExamHdrPtr;
	Uint32 explicitSeriesHdrPtr;
	Uint32 explicitImageHdrPtr;

	GEN_FileStructureInformation(void)
		{
			useExplicitOffsets=false;
			pixelDataLengthFieldPresent=false;
			explicitFileHdrPtr=0;
			explicitSuiteHdrPtr=0;
			explicitExamHdrPtr=0;
			explicitSeriesHdrPtr=0;
			explicitImageHdrPtr=0;
		}

	GEN_FileStructureInformation(
			bool explicitoff,
			bool lengthpresent,
			Uint32 fileptr,
			Uint32 suiteptr,
			Uint32 examptr,
			Uint32 seriesptr,
			Uint32 imageptr)
		{
			useExplicitOffsets=explicitoff;
			pixelDataLengthFieldPresent=lengthpresent;
			explicitFileHdrPtr=fileptr;
			explicitSuiteHdrPtr=suiteptr;
			explicitExamHdrPtr=examptr;
			explicitSeriesHdrPtr=seriesptr;
			explicitImageHdrPtr=imageptr;
		}
};

#endif // __Header_geninfo__
