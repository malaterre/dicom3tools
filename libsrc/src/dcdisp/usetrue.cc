#include "basetype.h"
#include "usetrue.h"

UseableTrueColorImage::UseableTrueColorImage(void)
{
	nused=0;
	rgbtocell=0;
	red=0;
	green=0;
	blue=0;
}

UseableTrueColorImage::~UseableTrueColorImage()
{
	if (rgbtocell) delete[] rgbtocell;
	if (red) delete[] red;
	if (green) delete[] green;
	if (blue) delete[] blue;
}

bool
UseableTrueColorImage::getColorCellsWanted(unsigned &nwanted,unsigned &nminimum)
{
	// 332 palette is 256 long

	nused=nwanted=nminimum=256;
	return true;
}

bool
UseableTrueColorImage::setColorCellsAvailable(unsigned n,unsigned long *table)
{
	// not having enough color cells is not supported yet :(

	Assert(n >= nused);

	rgbtocell=new unsigned char[nused];
	Assert(rgbtocell);

	unsigned i;
	for (i=0; i<n; ++i) {
		rgbtocell[i]=(unsigned char)table[i];
	}
	return true;
}

bool
UseableTrueColorImage::getColorCellValues(unsigned n,unsigned short *&ared,unsigned short *&agreen,unsigned short *&ablue)
{
	Assert(n == 256);
	Assert(nused == 256);
	ared=red=new unsigned short [nused];
	Assert(red);
	agreen=green=new unsigned short [nused];
	Assert(green);
	ablue=blue=new unsigned short [nused];
	Assert(blue);

	// dirty trick here ...because a 3/3/2 palette needs to be
	// 256 long (which means we have to tolerate flashing)
	// the problem of where black and white are arises. Now the
	// X convention is colorcell 0 is white and 1 is black which
	// seems ass backwards until you consider the possibility of
	// wrapping a linear palette around by 1 ... almost as fast
	// to index, and black and white are always accessible ...
	
	unsigned short rval;
	unsigned short gval;
	unsigned short bval;
	unsigned short *rptr=red+1;	// offset by 1 so 1=black
	unsigned short *gptr=green+1;
	unsigned short *bptr=blue+1;
	unsigned short rcount;
	unsigned short gcount;
	unsigned short bcount;
	for (rcount=0,rval=0; rcount<8; ++rcount,rval+=0x2492) {
		for (gcount=0,gval=0; gcount<8; ++gcount,gval+=0x2492) {
			for (bcount=0,bval=0; bcount<4; ++bcount,bval+=0x5555) {
//cerr << "UseableTrueColorImage::getColorCellValues"
//     << " rval=" << hex << rval
//     << " gval=" << hex << gval
//     << " bval=" << hex << bval
//     << dec << endl;
				if (rptr-red == 255) {
					// wrap around last entry so 0=white
					*red=rval;
					*green=gval;
					*blue=bval;
				}
				else {
					*rptr++=rval;
					*gptr++=gval;
					*bptr++=bval;
				}
			}
		}
	}
	return true;
}
