#ifndef __Header_rdindex__
#define __Header_rdindex__

class Readable8BitIndexedColorImage :
	public Readable8BitSinglePlaneImage,
	protected UseableIndexedColorImage
{
protected:
	char mapPixel(const Uint16 src)
		{
			return UseableIndexedColorImage::operator[]((unsigned char)src);
		}

public:
	Readable8BitIndexedColorImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
		DicomLUT *RedLUT,DicomLUT *GreenLUT,DicomLUT *BlueLUT);

	virtual ~Readable8BitIndexedColorImage();

	bool getColorCellsWanted(unsigned &nwanted,unsigned &nminimum);
	bool setColorCellsAvailable(unsigned n,unsigned long *table);
	bool getColorCellValues(unsigned n,
		unsigned short *&red,
		unsigned short *&green,
		unsigned short *&blue);
};

class Readable16BitIndexedColorImage :
	public Readable16BitSinglePlaneImage,
	public UseableIndexedColorImage
{
public:
	Readable16BitIndexedColorImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
		DicomLUT *RedLUT,DicomLUT *GreenLUT,DicomLUT *BlueLUT);

	virtual ~Readable16BitIndexedColorImage();
};

#endif /* __Header_rdindex__ */
