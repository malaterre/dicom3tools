#include <iostream.h>

#include "basetype.h"
#include "srcsink.h"
#include "usetrue.h"
#include "rdimage.h"
#include "rdrgb.h"

ReadableInterleaved24BitRGBImage::ReadableInterleaved24BitRGBImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit)
		: ReadableInterleaved24BitMultiplePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableTrueColorImage()
{}

ReadableInterleaved24BitRGBImage::~ReadableInterleaved24BitRGBImage()
{}

bool
ReadableInterleaved24BitRGBImage::getColorCellsWanted(unsigned &nwanted,
		unsigned &nminimum)
{
	return UseableTrueColorImage::getColorCellsWanted(nwanted,nminimum);
}

bool
ReadableInterleaved24BitRGBImage::setColorCellsAvailable(unsigned n,
		unsigned long *table)
{
	return UseableTrueColorImage::setColorCellsAvailable(n,table);
}

bool
ReadableInterleaved24BitRGBImage::getColorCellValues(unsigned n,
		unsigned short *&red,
		unsigned short *&green,
		unsigned short *&blue)
{
	return UseableTrueColorImage::getColorCellValues(n,red,green,blue);
}

ReadableNonInterleaved24BitRGBImage::ReadableNonInterleaved24BitRGBImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit)
		: ReadableNonInterleaved24BitMultiplePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableTrueColorImage()
{}

ReadableNonInterleaved24BitRGBImage::~ReadableNonInterleaved24BitRGBImage()
{}

bool
ReadableNonInterleaved24BitRGBImage::getColorCellsWanted(unsigned &nwanted,
		unsigned &nminimum)
{
	return UseableTrueColorImage::getColorCellsWanted(nwanted,nminimum);
}

bool
ReadableNonInterleaved24BitRGBImage::setColorCellsAvailable(unsigned n,
		unsigned long *table)
{
	return UseableTrueColorImage::setColorCellsAvailable(n,table);
}

bool
ReadableNonInterleaved24BitRGBImage::getColorCellValues(unsigned n,
		unsigned short *&red,
		unsigned short *&green,
		unsigned short *&blue)
{
	return UseableTrueColorImage::getColorCellValues(n,red,green,blue);
}

