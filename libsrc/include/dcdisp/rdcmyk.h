#ifndef __Header_rdcmyk__
#define __Header_rdcmyk__

class ReadableInterleaved32BitCMYKImage :
	public ReadableInterleaved32BitMultiplePlaneImage,
	public UseableTrueColorImage
{
public:
	ReadableInterleaved32BitCMYKImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit);
};

class ReadableNonInterleaved32BitCMYKImage :
	public ReadableNonInterleaved32BitMultiplePlaneImage,
	public UseableTrueColorImage
{
public:
	ReadableNonInterleaved32BitCMYKImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit);
};

#endif /* __Header_rdcmyk__ */
