#include <iostream.h>

#include "basetype.h"
#include "srcsink.h"
#include "usetrue.h"
#include "rdimage.h"
#include "rdcmyk.h"

ReadableInterleaved32BitCMYKImage::ReadableInterleaved32BitCMYKImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit)
		: ReadableInterleaved32BitMultiplePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableTrueColorImage()
{}

ReadableNonInterleaved32BitCMYKImage::ReadableNonInterleaved32BitCMYKImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit)
		: ReadableNonInterleaved32BitMultiplePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableTrueColorImage()
{}
