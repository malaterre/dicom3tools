#include <iostream.h>

#include "attr.h"
#include "srcsink.h"
#include "errclass.h"
#include "lutclass.h"
#include "usepal.h"
#include "useindex.h"
#include "usetrue.h"
#include "rdimage.h"
#include "rdargb.h"

ReadableInterleaved32BitARGBImage::ReadableInterleaved32BitARGBImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
	DicomLUT *RedLUT,DicomLUT *GreenLUT,DicomLUT *BlueLUT)
		: ReadableInterleaved32BitMultiplePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableTrueColorImage(),
		  UseableIndexedColorImage(RedLUT,GreenLUT,BlueLUT)
{}

ReadableNonInterleaved32BitARGBImage::ReadableNonInterleaved32BitARGBImage(
	SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
	Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
	DicomLUT *RedLUT,DicomLUT *GreenLUT,DicomLUT *BlueLUT)
		: ReadableNonInterleaved32BitMultiplePlaneImage(
			s,vColumns,vRows,vNumberOfFrames,
			vBitsAllocated,vBitsStored,vHighBit),
		  UseableTrueColorImage(),
		  UseableIndexedColorImage(RedLUT,GreenLUT,BlueLUT)
{}
