#ifndef __Header_rdhsv__
#define __Header_rdhsv__

class ReadableInterleaved24BitHSVImage :
	public ReadableInterleaved24BitMultiplePlaneImage,
	public UseableTrueColorImage
{
public:
	ReadableInterleaved24BitHSVImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit);
};

class ReadableNonInterleaved24BitHSVImage :
	public ReadableNonInterleaved24BitMultiplePlaneImage,
	public UseableTrueColorImage
{
public:
	ReadableNonInterleaved24BitHSVImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit);
};

#endif /* __Header_rdhsv__ */
