#include <iostream.h>

#include "attr.h"
#include "srcsink.h"
#include "errclass.h"
#include "lutclass.h"
#include "usepal.h"
#include "useindex.h"
#include "rdimage.h"
#include "rdindex.h"

Readable8BitIndexedColorImage::Readable8BitIndexedColorImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
	DicomLUT *RedLUT,DicomLUT *GreenLUT,DicomLUT *BlueLUT)
		: Readable8BitSinglePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableIndexedColorImage(RedLUT,GreenLUT,BlueLUT)
{}

Readable8BitIndexedColorImage::~Readable8BitIndexedColorImage()
{}

bool
Readable8BitIndexedColorImage::getColorCellsWanted(unsigned &nwanted,
		unsigned &nminimum)
{
	return UseableIndexedColorImage::getColorCellsWanted(nwanted,nminimum);
}

bool
Readable8BitIndexedColorImage::setColorCellsAvailable(unsigned n,
		unsigned long *table)
{
	return UseableIndexedColorImage::setColorCellsAvailable(n,table);
}

bool
Readable8BitIndexedColorImage::getColorCellValues(unsigned n,
		unsigned short *&red,
		unsigned short *&green,
		unsigned short *&blue)
{
	return UseableIndexedColorImage::getColorCellValues(n,red,green,blue);
}

Readable16BitIndexedColorImage::Readable16BitIndexedColorImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
	DicomLUT *RedLUT,DicomLUT *GreenLUT,DicomLUT *BlueLUT) 
		: Readable16BitSinglePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableIndexedColorImage(RedLUT,GreenLUT,BlueLUT)
{}

Readable16BitIndexedColorImage::~Readable16BitIndexedColorImage()
{}
