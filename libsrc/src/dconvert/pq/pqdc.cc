#include "pqdc.h"
#include "pq.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "pqsrc.h"

bool
PQ_Conversion::convertHeader(AttributeList *list)
{
	Assert(list);
	Assert(in);
	if (!pqhdr) pqhdr=new PQ_Header_BothClass(in);
	Assert(pqhdr);

	pqhdr->ToDicom_Template(list);
	pqhdr->ToDicom_ManualMisc(list);
	pqhdr->ToDicom_ManualPlane(list);
	pqhdr->ToDicom_ManualDates(list);

	return true;
}

// See comments in pq.h about the importance of the scope
// of a PQ_Conversion object and AttributeList object

bool
PQ_Conversion::convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax)
{
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!pqhdr) pqhdr=new PQ_Header_BothClass(in);
	Assert(pqhdr);

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

	pixeldatasrc=new PQ_PixelDataSource(
		*in,
		PQ_Offset_PixelData_ptr,
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
PQ_Conversion::convertAll(AttributeList *list,
			TransferSyntax *transfersyntax)
{
	if (!convertHeader(list)) return false;
	if (!convertPixelData(list,transfersyntax)) return false;
	return true;
}

