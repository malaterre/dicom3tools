#include "gendc.h"
#include "gen.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "gensrc.h"

bool
GEN_Conversion::convertHeader(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(in);
	if (!genhdr) genhdr=new GEN_Header_BothClass(in,fileinfo);
	Assert(genhdr);

	genhdr->ToDicom_Template(list);
	genhdr->ToDicom_ManualMisc(list);
	genhdr->ToDicom_ManualPlane(list);
	genhdr->ToDicom_ManualDates(list);

	return true;
}

// See comments in gen.h about the importance of the scope
// of a GEN_Conversion object and AttributeList object

bool
GEN_Conversion::convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!genhdr) genhdr=new GEN_Header_BothClass(in,fileinfo);
	Assert(genhdr);

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

	pixeldatasrc=new GEN_PixelDataSource(
		*in,
		GEN_Offset_FILEHDR_ptr+genhdr->GEN_HeaderInstance_FILEHDR->IH_img_hdr_length
			 + (fileinfo.pixelDataLengthFieldPresent ? 4 : 0),
		GEN_Offset_FILEHDR_ptr+genhdr->GEN_HeaderInstance_FILEHDR->IH_img_p_unpack,
		genhdr->GEN_HeaderInstance_FILEHDR->IH_img_l_unpack,
		genhdr->GEN_HeaderInstance_FILEHDR->IH_img_compress,
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
GEN_Conversion::convertAll(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	if (!convertHeader(list,imagenumber)) return false;
	if (!convertPixelData(list,transfersyntax,imagenumber)) return false;
	return true;
}

