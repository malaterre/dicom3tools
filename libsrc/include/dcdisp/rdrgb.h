#ifndef __Header_rdrgb__
#define __Header_rdrgb__

class ReadableInterleaved24BitRGBImage :
	public ReadableInterleaved24BitMultiplePlaneImage,
	public UseableTrueColorImage
{
protected:
	char mapPixel(const Uint16 red,const Uint16 green,const Uint16 blue)
		{
			return UseableTrueColorImage::mapPixel(red,green,blue);
		}
public:
	ReadableInterleaved24BitRGBImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit);

	virtual ~ReadableInterleaved24BitRGBImage();

	bool getColorCellsWanted(unsigned &nwanted,unsigned &nminimum);
	bool setColorCellsAvailable(unsigned n,unsigned long *table);
	bool getColorCellValues(unsigned n,
		unsigned short *&red,
		unsigned short *&green,
		unsigned short *&blue);
};

class ReadableNonInterleaved24BitRGBImage :
	public ReadableNonInterleaved24BitMultiplePlaneImage,
	public UseableTrueColorImage
{
protected:
	char mapPixel(const Uint16 red,const Uint16 green,const Uint16 blue)
		{
			return UseableTrueColorImage::mapPixel(red,green,blue);
		}
public:
	ReadableNonInterleaved24BitRGBImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit);

	virtual ~ReadableNonInterleaved24BitRGBImage();

	bool getColorCellsWanted(unsigned &nwanted,unsigned &nminimum);
	bool setColorCellsAvailable(unsigned n,unsigned long *table);
	bool getColorCellValues(unsigned n,
		unsigned short *&red,
		unsigned short *&green,
		unsigned short *&blue);
};

#endif /* __Header_rdrgb__ */
