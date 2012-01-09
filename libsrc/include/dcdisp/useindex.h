#ifndef __Header_useindex__
#define __Header_useindex__

class UseableIndexedColorImage
{
private:
	UseablePalette *palette;
	Uint16 *palettetocell;
	Uint32 npalette;
	Uint32 nused;

	unsigned short *palettetoindex;
	unsigned short *red;
	unsigned short *green;
	unsigned short *blue;
public:
	UseableIndexedColorImage(DicomLUT *RedLUT,
			DicomLUT *GreenLUT,
			DicomLUT *BlueLUT);
	virtual ~UseableIndexedColorImage();

	bool getColorCellsWanted(unsigned &nwanted,unsigned &nminimum);
	bool setColorCellsAvailable(unsigned n,unsigned long *table);
	bool getColorCellValues(unsigned n,
		unsigned short *&ared,
		unsigned short *&agreen,
		unsigned short *&ablue);

	Uint16 operator[](Uint16 index)
		{
			Assert(index<npalette);
			Assert(palettetocell);
			return palettetocell[index];
		}
};

#endif /* __Header_useindex__ */
