#include "ge9800dc.h"
#include "ge9800.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "ge9800src.h"

bool
GE9800_Conversion::convertHeader(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(in);
	if (!ge9800hdr) ge9800hdr=new GE9800_Header_BothClass(in);
	Assert(ge9800hdr);

	ge9800hdr->ToDicom_Template(list);
	ge9800hdr->ToDicom_ManualMisc(list);
	ge9800hdr->ToDicom_ManualPlane(list);
	ge9800hdr->ToDicom_ManualDates(list);

	return true;
}

// See comments in ge9800.h about the importance of the scope
// of a GE9800_Conversion object and AttributeList object

bool
GE9800_Conversion::convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!ge9800hdr) ge9800hdr=new GE9800_Header_BothClass(in);
	Assert(ge9800hdr);

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

	pixeldatasrc=new GE9800_PixelDataSource(
		*in,
		GE9800_UsePixelDataMap,
		GE9800_Offset_PixelDataMap_ptr,
		GE9800_Offset_PixelData_ptr,
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
GE9800_Conversion::convertAll(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	if (!convertHeader(list,imagenumber)) return false;
	if (!convertPixelData(list,transfersyntax,imagenumber)) return false;
	return true;
}

