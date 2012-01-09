#ifndef __Header_rdgray__
#define __Header_rdgray__

class ReadableWindowed8BitGrayImage :
	public Readable8BitSinglePlaneImage,
	public UseableWindowedGrayImage
{
protected:
	char mapPixel(const Uint16 src)
		{
			return UseableWindowedGrayImage::operator[]((unsigned char)src);
		}

	const char *getPixelMap(Uint32 &mapsize) const
		{
			return UseableWindowedGrayImage::getPixelMap(mapsize);
		}
public:
	ReadableWindowed8BitGrayImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
		Uint16 vPixelRepresentation,bool invertedgrayscale);

	virtual ~ReadableWindowed8BitGrayImage();

	bool getColorCellsWanted(unsigned &nwanted,unsigned &nminimum);
	bool setColorCellsAvailable(unsigned n,unsigned long *table);
	bool getColorCellValues(unsigned n,
		unsigned short *&red,
		unsigned short *&green,
		unsigned short *&blue);
};

class ReadableWindowed16BitGrayImage :
	public Readable16BitSinglePlaneImage,
	public UseableWindowedGrayImage
{
private:
	bool usepadvalue;
	Uint16 padvalue;
protected:
	char mapPixel(const Uint16 src)
		{
			return UseableWindowedGrayImage::operator[](src);
		}

	const char *getPixelMap(Uint32 &mapsize) const
		{
			return UseableWindowedGrayImage::getPixelMap(mapsize);
		}
public:
	ReadableWindowed16BitGrayImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
		Uint16 vPixelRepresentation,bool invertedgrayscale,
		bool padvaluepresent,Uint16 vPixelPaddingValue);

	virtual ~ReadableWindowed16BitGrayImage();

	bool getColorCellsWanted(unsigned &nwanted,unsigned &nminimum);
	bool setColorCellsAvailable(unsigned n,unsigned long *table);
	bool getColorCellValues(unsigned n,
		unsigned short *&red,
		unsigned short *&green,
		unsigned short *&blue);

	bool hasWindowLevelWidth(void)	{ return true; }
	bool setWindowLevelWidth(Uint16 level,Uint16 width);
	bool getWindowLevelWidth(Uint16 &level,Uint16 &width);
	bool needToResetColorCells(void);
	bool needToResetIndexedPixels(void);

	bool setVOILUT(Uint16 first,Uint16 number,Uint16 depth,const Uint16 *table);
};

#endif /* __Header_rdgray__ */
