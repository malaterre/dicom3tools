#ifndef __Header_rdargb__
#define __Header_rdargb__

class ReadableInterleaved32BitARGBImage :
	public ReadableInterleaved32BitMultiplePlaneImage,
	public UseableTrueColorImage,
	public UseableIndexedColorImage
{
public:
	ReadableInterleaved32BitARGBImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
		DicomLUT *RedLUT,DicomLUT *GreenLUT,DicomLUT *BlueLUT);
};

class ReadableNonInterleaved32BitARGBImage :
	public ReadableNonInterleaved32BitMultiplePlaneImage,
	public UseableTrueColorImage,
	public UseableIndexedColorImage
{
public:
	ReadableNonInterleaved32BitARGBImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
		DicomLUT *RedLUT,DicomLUT *GreenLUT,DicomLUT *BlueLUT);
};

#endif /* __Header_rdargb__ */
