#ifndef __Header_usetrue__
#define __Header_usetrue__

class UseableTrueColorImage
{
private:
	Uint16 nused;
	unsigned char *rgbtocell;
	unsigned short *red;
	unsigned short *green;
	unsigned short *blue;
public:
	UseableTrueColorImage(void);
	virtual ~UseableTrueColorImage();

	bool getColorCellsWanted(unsigned &nwanted,unsigned &nminimum);
	bool setColorCellsAvailable(unsigned n,unsigned long *table);
	bool getColorCellValues(unsigned n,
		unsigned short *&ared,
		unsigned short *&agreen,
		unsigned short *&ablue);

	char mapPixel(Uint16 r,Uint16 g,Uint16 b)
		{
			// should be 8 bit data in lower part of 16 bit words
			Assert(r<256);
			Assert(g<256);
			Assert(b<256);
			Assert(nused == 256);
			Assert(rgbtocell);
			// The +1 is to wrap around so 0=white and 1=black
			// a la the default X convention
			unsigned char index=(unsigned char)(((r)&0xe0)|((g>>3)&0x1c)|((b>>6)&0x03)+1);
			return rgbtocell[index];
		}
};

#endif /* __Header_usetrue__ */
