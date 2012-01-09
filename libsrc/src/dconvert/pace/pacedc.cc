#include "pacedc.h"
#include "pace.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "pacesrc.h"


bool
PACE_Conversion::convertHeader(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(in);
	if (!pacehdr) pacehdr=new PACE_Header_BothClass(in);
	Assert(pacehdr);

	pacehdr->ToDicom_Template(list);
	pacehdr->ToDicom_ManualMisc(list);
	pacehdr->ToDicom_ManualPlane(list);
	pacehdr->ToDicom_ManualDates(list);

	return true;
}

// See comments in pac.he about the importance of the scope
// of a PACE_Conversion object and AttributeList object

bool
PACE_Conversion::convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!pacehdr) pacehdr=new PACE_Header_BothClass(in);
	Assert(pacehdr);

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

	pixeldatasrc=new PACE_PixelDataSource(
		*in,
		PACE_Offset_PixelData_ptr,
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
PACE_Conversion::convertAll(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	if (!convertHeader(list,imagenumber)) return false;
	if (!convertPixelData(list,transfersyntax,imagenumber)) return false;
	return true;
}

