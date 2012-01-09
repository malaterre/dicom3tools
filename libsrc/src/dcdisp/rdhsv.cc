#include <iostream.h>

#include "basetype.h"
#include "srcsink.h"
#include "usetrue.h"
#include "rdimage.h"
#include "rdhsv.h"

ReadableInterleaved24BitHSVImage::ReadableInterleaved24BitHSVImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit)
		: ReadableInterleaved24BitMultiplePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableTrueColorImage()
{}

ReadableNonInterleaved24BitHSVImage::ReadableNonInterleaved24BitHSVImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit)
		: ReadableNonInterleaved24BitMultiplePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableTrueColorImage()
{}

