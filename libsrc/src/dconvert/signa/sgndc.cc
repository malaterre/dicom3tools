#include "sgndc.h"
#include "signa.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "sgnsrc.h"

bool
SGN_Conversion::convertHeader(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(in);
	if (!sgnhdr) sgnhdr=new SGN_Header_BothClass(in);
	Assert(sgnhdr);

	sgnhdr->ToDicom_Template(list);
	sgnhdr->ToDicom_ManualMisc(list);
	sgnhdr->ToDicom_ManualPlane(list);
	sgnhdr->ToDicom_ManualDates(list);

	return true;
}

// See comments in signa.h about the importance of the scope
// of a SGN_Conversion object and AttributeList object

bool
SGN_Conversion::convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!sgnhdr) sgnhdr=new SGN_Header_BothClass(in);
	Assert(sgnhdr);

	Uint16 bitsAllocated=
		AttributeValue((*list)[TagFromName(BitsAllocated)],16);
	Uint16 bitsStored=
		AttributeValue((*list)[TagFromName(BitsStored)],bitsAllocated);
	Uint16 highBit=
		AttributeValue((*list)[TagFromName(HighBit)],bitsStored-1u);
	Uint16 rows=
		AttributeValue((*list)[TagFromName(Rows)]);
	Uint16 cols=
		AttributeValue((*list)[TagFromName(Columns)]);

	Assert(rows);
	Assert(cols);

	Assert(pixeldatasrc==0);

	// Manually expand the offsets

	pixeldatasrc=new SGN_PixelDataSource(
		*in,
		SGN_Offset_IMAGEDATA_ptr,
		rows,
		cols);

	Assert(pixeldatasrc);

	(*list)+=new OtherUnspecifiedLargeAttribute(
		TagFromName(PixelData),
		pixeldatasrc,
		rows,
		cols,
		1, // frame
		1, // samples per pixel
		transfersyntax,
		0,bitsAllocated,bitsStored,highBit);

	return true;
}

bool
SGN_Conversion::convertAll(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	if (!convertHeader(list,imagenumber)) return false;
	if (!convertPixelData(list,transfersyntax,imagenumber)) return false;
	return true;
}

