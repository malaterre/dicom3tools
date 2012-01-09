#include "imtndc.h"
#include "imtn.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "imtnsrc.h"

bool
IMTN_Conversion::convertHeader(AttributeList *list,unsigned imagenumber)
{
	Assert(list);
	Assert(in);
	if (!imtnhdr) imtnhdr=new IMTN_Header_BothClass(in);
	Assert(imtnhdr);

	imtnhdr->ToDicom_Template(list,imagenumber);
	imtnhdr->ToDicom_ManualMisc(list,imagenumber);
	imtnhdr->ToDicom_ManualPlane(list,imagenumber);
	imtnhdr->ToDicom_ManualDates(list,imagenumber);

	return true;
}

// See comments in imtn.h about the importance of the scope
// of a IMTN_Conversion object and AttributeList object

bool
IMTN_Conversion::convertPixelData(AttributeList *list,
		TransferSyntax *transfersyntax,unsigned imagenumber)
{
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!imtnhdr) imtnhdr=new IMTN_Header_BothClass(in);
	Assert(imtnhdr);

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
	pixeldatasrc=new IMTN_PixelDataSource(
		*in,
		IMTN_Offset_PixelData_ptr,
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
IMTN_Conversion::convertAll(AttributeList *list,
	TransferSyntax *transfersyntax,
	unsigned imagenumber)
{
	if (!convertHeader(list,imagenumber)) return false;
	if (!convertPixelData(list,transfersyntax,imagenumber)) return false;
	return true;
}

