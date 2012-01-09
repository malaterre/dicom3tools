#include "xxxxdc.h"
#include "xxxx.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "xxxxsrc.h"

bool
XXXX_Conversion::convertHeader(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(in);
	if (!xxxxhdr) xxxxhdr=new XXXX_Header_BothClass(in);
	Assert(xxxxhdr);

	xxxxhdr->ToDicom_Template(list);
	xxxxhdr->ToDicom_ManualMisc(list);
	xxxxhdr->ToDicom_ManualPlane(list);
	xxxxhdr->ToDicom_ManualDates(list);

	return true;
}

// See comments in xxxx.h about the importance of the scope
// of a XXXX_Conversion object and AttributeList object

bool
XXXX_Conversion::convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!xxxxhdr) xxxxhdr=new XXXX_Header_BothClass(in);
	Assert(xxxxhdr);

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

	pixeldatasrc=new XXXX_PixelDataSource(
		*in,
		XXXX_Offset_PixelData_ptr,
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
XXXX_Conversion::convertAll(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	if (!convertHeader(list,imagenumber)) return false;
	if (!convertPixelData(list,transfersyntax,imagenumber)) return false;
	return true;
}

