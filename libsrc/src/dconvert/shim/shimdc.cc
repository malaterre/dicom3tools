#include "shimdc.h"
#include "shim.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "shimsrc.h"

bool
SHIM_Conversion::convertHeader(AttributeList *list,unsigned imagenumber)
{
	Assert(list);
	Assert(in);
	if (!shimhdr) shimhdr=new SHIM_Header_BothClass(in);
	Assert(shimhdr);

	shimhdr->ToDicom_Template(list,imagenumber);
	shimhdr->ToDicom_ManualMisc(list,imagenumber);
	shimhdr->ToDicom_ManualPlane(list,imagenumber);
	shimhdr->ToDicom_ManualDates(list,imagenumber);

	return true;
}

// See comments in shim.h about the importance of the scope
// of a SHIM_Conversion object and AttributeList object

bool
SHIM_Conversion::convertPixelData(AttributeList *list,
		TransferSyntax *transfersyntax,unsigned imagenumber)
{
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!shimhdr) shimhdr=new SHIM_Header_BothClass(in);
	Assert(shimhdr);

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

	in->seekg(SHIM_Offset_IndexEntry_ptr(imagenumber),ios::beg);
	if (!in->good()) return false;
	unsigned char *indexentry = new unsigned char[SHIM_IndexEntry_lng];
	Assert(indexentry);
	in->read((char *)indexentry,SHIM_IndexEntry_lng);
	if (!in->good()) return false;

	Assert(pixeldatasrc==0);
	pixeldatasrc=new SHIM_PixelDataSource(
		*in,
		SHIM_Offset_PixelData_ptr(indexentry),
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

	if (indexentry) delete[] indexentry;

	return true;
}

bool
SHIM_Conversion::convertAll(AttributeList *list,
	TransferSyntax *transfersyntax,
	unsigned imagenumber)
{
	if (!convertHeader(list,imagenumber)) return false;
	if (!convertPixelData(list,transfersyntax,imagenumber)) return false;
	return true;
}

