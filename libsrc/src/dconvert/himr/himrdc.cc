#include "himrdc.h"
#include "himr.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "himrsrc.h"

bool
HIMR_Conversion::convertHeader(AttributeList *list)
{
	Assert(list);
	Assert(in);
	if (!himrhdr) himrhdr=new HIMR_Header_BothClass(in);
	Assert(himrhdr);

	himrhdr->ToDicom_Template(list);
	himrhdr->ToDicom_ManualMisc(list);
	himrhdr->ToDicom_ManualPlane(list);
	himrhdr->ToDicom_ManualDates(list);

	return true;
}

// See comments in himr.h about the importance of the scope
// of a HIMR_Conversion object and AttributeList object

bool
HIMR_Conversion::convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax)
{
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!himrhdr) himrhdr=new HIMR_Header_BothClass(in);
	Assert(himrhdr);

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

	pixeldatasrc=new HIMR_PixelDataSource(
		*in,
		HIMR_Offset_PixelData_ptr,
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
HIMR_Conversion::convertAll(AttributeList *list,
			TransferSyntax *transfersyntax)
{
	if (!convertHeader(list)) return false;
	if (!convertPixelData(list,transfersyntax)) return false;
	return true;
}

