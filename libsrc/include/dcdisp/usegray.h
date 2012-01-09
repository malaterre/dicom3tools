#ifndef __Header_usegray__
#define __Header_usegray__

class UseableWindowedGrayImage
{
private:
	bool inverted;
	bool needdefaultwindowsetting;
	Uint16 mingray;
	Uint16 maxgray;
	Uint16 bottom;
	Uint16 top;
	Uint16 signxor;
	char  *graytocell;
	Uint32 ngrays;
	char  *indextocell;
	Uint32 ncells;

	unsigned short *red;
	unsigned short *green;
	unsigned short *blue;
public:
	UseableWindowedGrayImage(
		bool invertedgrayscale,Uint16 bits,bool issigned);
	virtual ~UseableWindowedGrayImage();

	bool getColorCellsWanted(unsigned &nwanted,unsigned &nminimum);
	bool setColorCellsAvailable(unsigned n,unsigned long *table);
	bool getColorCellValues(unsigned n,
		unsigned short *&ared,
		unsigned short *&agreen,
		unsigned short *&ablue);

	bool setSign(Uint16 bits,bool s);
	bool setWindowRange(Uint16 b,Uint16 t);
	bool setWindowLevelWidth(Uint16 level,Uint16 width);
	bool getWindowLevelWidth(Uint16 &level,Uint16 &width);

	bool setVOILUT(Uint16 first,Uint16 number,Uint16 depth,const Uint16 *table);

	char operator[](Uint16 index)
		{
			Assert(index<ngrays);
			Assert(graytocell);
			return graytocell[index];
		}

	const char *getPixelMap(Uint32 &mapsize) const
		{
			Assert(graytocell);
			mapsize=ncells;
			return graytocell;
		}
};

#endif /* __Header_usegray__ */
