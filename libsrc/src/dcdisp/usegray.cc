#include <iostream.h>	// only for debugging
#include <iomanip.h>	// only for debugging

#include "basetype.h"
#include "usegray.h"

UseableWindowedGrayImage::UseableWindowedGrayImage(
	bool invertedgrayscale,Uint16 bits,bool issigned)
{
	Assert(bits);
	inverted=invertedgrayscale;
	mingray=0;
	Uint32 mx=(Uint32(1)<<bits)-1;
	Assert(mx <= Uint16_MAX);
	maxgray=Uint16(mx);
	Assert(maxgray >= mingray);
	ngrays=maxgray-mingray+1;

	(void)setSign(bits,issigned);

//cerr << "UseableWindowedGrayImage::UseableWindowedGrayImage" << "\n"
//     << "\t invertedgrayscale=" << dec << invertedgrayscale << "\n"
//     << "\t bits=" << dec << bits << "\n"
//     << "\t issigned=" << dec << issigned << "\n"
//     << "\t mingray=" << dec << mingray << "\n"
//     << "\t mx=" << dec << mx << "\n"
//     << "\t maxgray=" << dec << maxgray << "\n"
//     << "\t ngrays=" << dec << ngrays << "\n"
//     << "\t signxor(hex)=" << hex << signxor << dec << "\n"
//     << endl;

	graytocell=new char[ngrays];
	Assert(graytocell);
	ncells=0;
	indextocell=0;
	red=0;
	green=0;
	blue=0;
	needdefaultwindowsetting=true;
}

UseableWindowedGrayImage::~UseableWindowedGrayImage()
{
	if (graytocell) delete[] graytocell;
	if (indextocell) delete[] indextocell;
	if (red) delete[] red;
	if (green) delete[] green;
	if (blue) delete[] blue;
}

	// gray is the real gray value in the image pixel data
	// index is the gray value windowed to a range 0..ncells
	// cell is the actual number of the cell used in the display array

	// strategy is ...
	//	allocate lut of gray to cell
	//	request <= 244 cells
	//	load those cells with a linear gray scale
	//	fill graytocell lut with cell numbers
	//	operator[] returns cell number given gray value

	// indextocell is kept around in case windowing is changed and
	// need to recalculate graytocell

bool
UseableWindowedGrayImage::setSign(Uint16 bits,bool issigned)
{
	signxor=issigned
		? 1<<(bits-1)
		: 0;

	return true;
}

bool UseableWindowedGrayImage::getColorCellsWanted(unsigned &nwanted,
		unsigned &nminimum)
{
	nwanted=256;
	nminimum=8;
//cerr << "UseableWindowedGrayImage::getColorCellsWanted nwanted=" << dec << nwanted << endl;
//cerr << "UseableWindowedGrayImage::getColorCellsWanted nminimum=" << dec << nminimum << endl;
	return true;
}

bool
UseableWindowedGrayImage::setColorCellsAvailable(unsigned n,
		unsigned long *table)
{
//cerr << "UseableWindowedGrayImage::setColorCellsAvailable n=" << dec << n << endl;
	Assert(n);
	ncells=n;
	indextocell=new char[ncells];
	Assert(indextocell);

	unsigned i;
	for (i=0; i<ncells; ++i) {
		Assert(i<n);
		Assert(table[i] < 256);
		indextocell[i]=(char)table[i];
	}
	return true;
}

bool
UseableWindowedGrayImage::getColorCellValues(unsigned n,
		unsigned short *&ared,
		unsigned short *&agreen,
		unsigned short *&ablue)
{
//cerr << "UseableWindowedGrayImage::getColorCellValues n=" << dec << n << endl;
	Assert(n==ncells);
	ared=red=new unsigned short [ncells];
	Assert(red);
	agreen=green=new unsigned short [ncells];
	Assert(green);
	ablue=blue=new unsigned short [ncells];
	Assert(blue);

	unsigned i;
	for (i=0; i<ncells; ++i) {
		unsigned short k =(unsigned short)(Uint32(0xffff)*i/(ncells-1));
//cerr << "i=" << dec << i
//     << " k=" << hex << k
//     << dec << endl;
		red[i]=k;
		green[i]=k;
		blue[i]=k;
	}
	return needdefaultwindowsetting
		? setWindowRange(mingray,maxgray) : true;
}

bool
UseableWindowedGrayImage::setWindowRange(Uint16 b,Uint16 t)
{
	bottom=b;
	top=t;

//cerr << "UseableWindowedGrayImage::setWindowRange"
//     << " bottom=" << dec << bottom
//     << " top=" << dec << top
//     << endl;

	Assert(bottom<=top);
	Assert(bottom>=mingray);
	Assert(top<=maxgray);
	Assert(indextocell);
	Assert(graytocell);

	needdefaultwindowsetting=false;

	unsigned long i;
	for (i=0; i<bottom; ++i) {
		graytocell[(inverted ? (ngrays-i-1) : i)^signxor]=indextocell[0];
	}
	for (i=bottom; i<=top; ++i) {
		long index=((i-bottom)*ncells)/(top-bottom+1);
//cerr << "UseableWindowedGrayImage::setWindowRange"
//     << " mapping gray=" << dec << i
//     << " to index=" << dec << index
//     << endl;
		Assert(index>=0);
		Assert(index<ncells);
		graytocell[(inverted ? (ngrays-i-1) : i)^signxor]=indextocell[index];
	}
	for (i=top+1; i<ngrays; ++i) {
		graytocell[(inverted ? (ngrays-i-1) : i)^signxor]=indextocell[ncells-1];
	}
	return true;
}

bool
UseableWindowedGrayImage::setWindowLevelWidth(Uint16 level,Uint16 width)
{
//cerr << "UseableWindowedGrayImage::setImageLevelWidth"
//     << " level=" << dec << level
//     << " width=" << dec << width
//     << endl;
	long lbottom=(long)level-width/2;
//cerr << "UseableWindowedGrayImage::setImageLevelWidth"
//     << " lbottom=" << dec << lbottom
//     << endl;
	if (lbottom < mingray) lbottom=mingray;
	if (lbottom > maxgray) lbottom=maxgray;	 // just in case
//cerr << "UseableWindowedGrayImage::setImageLevelWidth"
//     << " lbottom clamped to=" << dec << lbottom
//     << endl;
	long ltop=(long)level+width/2;
//cerr << "UseableWindowedGrayImage::setImageLevelWidth"
//     << " ltop=" << dec << ltop
//     << endl;
	if (ltop > maxgray) ltop=maxgray;
	if (ltop < mingray) ltop=mingray;	// just in case
//cerr << "UseableWindowedGrayImage::setImageLevelWidth"
//     << " ltop clamped to=" << dec << ltop
//     << endl;
	Assert(lbottom<=Uint16_MAX);
	Assert(ltop<=Uint16_MAX);
	return setWindowRange(Uint16(lbottom),Uint16(ltop));
}

bool
UseableWindowedGrayImage::getWindowLevelWidth(Uint16 &level,Uint16 &width)
{
	long llevel=(top+bottom)/2;
	long lwidth=top-bottom;
	Assert(llevel<=Uint16_MAX);
	Assert(lwidth<=Uint16_MAX);
	level=Uint16(llevel);
	width=Uint16(lwidth);
	return true;
}


bool
UseableWindowedGrayImage::setVOILUT(Uint16 first,Uint16 number,Uint16 depth,const Uint16 *table)
{
//cerr << "UseableWindowedGrayImage::setVOILUT"
//     << " first=" << dec << first
//     << " number=" << dec << number
//     << " depth=" << dec << depth
//     << endl;

	bottom=first;
	Assert(number >= 1);
	top=first+(number-1);

//cerr << "UseableWindowedGrayImage::setVOILUT"
//     << " bottom=" << dec << bottom
//     << " top=" << dec << top
//     << endl;

	if (bottom < mingray) bottom=mingray;
	if (top > maxgray) top=maxgray;

//cerr << "UseableWindowedGrayImage::setVOILUT"
//     << " clamped bottom=" << dec << bottom
//     << " clamped top=" << dec << top
//     << endl;

	Assert(bottom<=top);
	Assert(bottom>=mingray);
	Assert(top<=maxgray);
	Assert(indextocell);
	Assert(graytocell);

	// theoretically, LUT output range should be related to
	// the depth (in the third LUT descriptor), however it is
	// often sent as much less ... hence the check for the
	// actual range here ...
#ifdef USELUTDEPTHFORRANGE
	unsigned long lutoutputrange=1lu<<depth;
	Uint16 lutmin=0;
#else
	Uint16 lutmin=Uint16((1lu<<depth)-1);
	Uint16 lutmax=0;

	{
		int i;
		for (i=0; i<number; ++i) {
			if (table[i] < lutmin) lutmin=table[i];
			if (table[i] > lutmax) lutmax=table[i];
		}
	}
	unsigned long lutoutputrange=lutmax-lutmin+1;

cerr << "UseableWindowedGrayImage::setVOILUT"
     << " lutmax=" << dec << lutmax
     << endl;
#endif

cerr << "UseableWindowedGrayImage::setVOILUT"
     << " lutoutputrange=" << dec << lutoutputrange
     << " lutmin=" << dec << lutmin
     << endl;

//cerr << "UseableWindowedGrayImage::setVOILUT"
//     << " ncells=" << dec << ncells
//     << endl;

	unsigned long i;

	unsigned long bottomindex=((table[0]-lutmin)*ncells)/lutoutputrange;
	Assert(bottomindex>=0);
	Assert(bottomindex<ncells);
	char bottomcell=indextocell[inverted ? (ncells-bottomindex-1) : bottomindex];
	for (i=0; i<=bottom; ++i) {
		graytocell[i^signxor]=bottomcell;
	}

//cerr << "UseableWindowedGrayImage::setVOILUT"
//     << " bottomindex=" << dec << bottomindex
//     << " bottomcell=" << dec << (unsigned)bottomcell
//     << endl;

	for (i=bottom+1; i<top; ++i) {
		unsigned long lutoutput=table[i-bottom];
		unsigned long index=((lutoutput-lutmin)*ncells)/lutoutputrange;
		Assert(index>=0);
		Assert(index<ncells);
		graytocell[i^signxor]=indextocell[inverted ? (ncells-index-1) : index];
//cerr << "UseableWindowedGrayImage::setVOILUT"
//     << " i=" << dec << i
//     << " i^signxor=" << dec << (i^signxor)
//     << " lutoutput=" << dec << lutoutput
//     << " index=" << dec << index
//     << " inverted ? (ncells-index-1) : index=" << dec << (inverted ? (ncells-index-1) : index)
//     << " cell=" << dec << (unsigned)(unsigned char)indextocell[inverted ? (ncells-index-1) : index]
//     << endl;

	}

	unsigned long topindex=((table[number-1]-lutmin)*ncells)/lutoutputrange;
	Assert(topindex>=0);
	Assert(topindex<ncells);
	char topcell=indextocell[inverted ? (ncells-topindex-1) : topindex];
	for (i=top; i<ngrays; ++i) {
		graytocell[i^signxor]=indextocell[topcell];
	}

//cerr << "UseableWindowedGrayImage::setVOILUT"
//     << " topindex=" << dec << topindex
//     << " topcell=" << dec << (unsigned)topcell
//     << endl;

	needdefaultwindowsetting=false;

	return true;
}

