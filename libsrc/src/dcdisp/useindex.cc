#include "attr.h"
#include "errclass.h"
#include "lutclass.h"
#include "usepal.h"
#include "useindex.h"

UseableIndexedColorImage::UseableIndexedColorImage(
		DicomLUT *RedLUT,DicomLUT *GreenLUT,DicomLUT *BlueLUT)
{
	palette=new UseablePalette(RedLUT,GreenLUT,BlueLUT);
	Assert(palette);
	palettetocell=0;
	npalette=0;
	palettetoindex=0;
	nused=0;
	red=0;
	green=0;
	blue=0;
}

UseableIndexedColorImage::~UseableIndexedColorImage()
{
	if (palette) delete palette;
	if (palettetocell) delete[] palettetocell;
	if (palettetoindex) delete[] palettetoindex;
	if (red) delete[] red;
	if (green) delete[] green;
	if (blue) delete[] blue;
}

bool
UseableIndexedColorImage::getColorCellsWanted(unsigned &nwanted,
		unsigned &nminimum)
{
	Assert(palette);
	Uint32 nstart=palette->getLength();
//cerr << "UseableIndexedColorImage::getColorCellsWanted nstart=" << dec << nstart << endl;
	palettetoindex=new unsigned short [nstart];
	Assert(palettetoindex);
	red=new unsigned short [nstart];
	Assert(red);
	green=new unsigned short [nstart];
	Assert(green);
	blue=new unsigned short [nstart];
	Assert(blue);

	// Only want cells for unique values in palette ...

	Uint32 i;
	for (i=0; i<nstart; ++i) {
		Assert(i<=Uint16_MAX);
		Uint16 r;
		Uint16 g;
		Uint16 b;
		if (palette->get16BitColorIndex(Uint16(i),r,g,b)) {
//cerr << "UseableIndexedColorImage::getColorCellsWanted testing color"
//     << " i=" << dec << i
//     << " r=" << hex << r
//     << " g=" << hex << g
//     << " b=" << hex << b
//     << dec << endl;
			Uint32 j=0;
			while (1) {
				Assert(j<=Uint16_MAX);
				if (j == nused) {
					red[j]=r;
					green[j]=g;
					blue[j]=b;
					palettetoindex[i]=Uint16(j);
//cerr << "UseableIndexedColorImage::getColorCellsWanted new"
//     << " i=" << dec << i
//     << " j=" << dec << j
//     << endl;
					++nused;
					break;
				}
				Assert(j<nused);
				if (red[j] == r && green[j] == g && blue[j] == b) {
					palettetoindex[i]=Uint16(j);
//cerr << "UseableIndexedColorImage::getColorCellsWanted existing"
//     << " i=" << dec << i
//     << " j=" << dec << j
//     << endl;
					break;
				}
				++j;
			}
		}
		else
			Assert(0);
	}

//cerr << "UseableIndexedColorImage::getColorCellsWanted nused=" << dec << nused << endl;
	Assert(nused <= UINT_MAX);
	nwanted=nminimum=unsigned(nused);
	return true;
}

bool
UseableIndexedColorImage::setColorCellsAvailable(unsigned n,
		unsigned long *table)
{
//cerr << "UseableIndexedColorImage::setColorCellsAvailable n=" << dec << n << endl;
	// not having enough color cells is not supported yet :(

	Assert(n >= nused);

	npalette=palette->getLength();
	palettetocell=new Uint16[npalette];
	Assert(palettetocell);

	unsigned i;
	for (i=0; i<npalette; ++i) {
		Uint16 index=palettetoindex[i];
//cerr << "UseableIndexedColorImage::setColorCellsAvailable"
//     << " i=" << dec << i
//     << " index=" << dec << index
//     << " table[index]=" << dec << table[index]
//     << endl;
		Assert(table[index] <= Uint16_MAX);
		palettetocell[i]=(Uint16)table[index];
	}
	return true;
}

bool
UseableIndexedColorImage::getColorCellValues(unsigned n,
	unsigned short *&ared,
	unsigned short *&agreen,
	unsigned short *&ablue)
{
//cerr << "UseableIndexedColorImage::getColorCellValues n=" << dec << n << endl;
	Assert(n <= nused);
	Assert(red);
	ared=red;
	Assert(green);
	agreen=green;
	Assert(blue);
	ablue=blue;

	return true;
}
