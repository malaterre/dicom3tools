#include "gawdc.h"
#include "gaw.h"

#include "attrmxls.h"
#include "attrval.h"
#include "attrothr.h"
#include "elmconst.h"

#include "gawsrc.h"

bool
GAW_Conversion::convertHeader(AttributeList *list,unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(in);
	if (!gawhdr) gawhdr=new GAW_Header_BothClass(in,fileinfo);
	Assert(gawhdr);

	gawhdr->ToDicom_Template(list);
	gawhdr->ToDicom_ManualMisc(list);
	gawhdr->ToDicom_ManualPlane(list);
	gawhdr->ToDicom_ManualDates(list);

	return true;
}

// See comments in gaw.h about the importance of the scope
// of a GAW_Conversion object and AttributeList object

bool
GAW_Conversion::convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	Assert(imagenumber==0);
	Assert(list);
	Assert(transfersyntax);
	Assert(in);
	if (!gawhdr) gawhdr=new GAW_Header_BothClass(in,fileinfo);
	Assert(gawhdr);

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

	pixeldatasrc=new GAW_PixelDataSource(
		*in,
		GAW_Offset_FILEHDR_ptr+gawhdr->GAW_HeaderInstance_FILEHDR->IH_img_hdr_length
			 + (fileinfo.pixelDataLengthFieldPresent ? 4 : 0),
		GAW_Offset_FILEHDR_ptr+gawhdr->GAW_HeaderInstance_FILEHDR->IH_img_p_unpack,
		gawhdr->GAW_HeaderInstance_FILEHDR->IH_img_l_unpack,
		gawhdr->GAW_HeaderInstance_FILEHDR->IH_img_compress,
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
GAW_Conversion::convertAll(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber)
{
	if (!convertHeader(list,imagenumber)) return false;
	if (!convertPixelData(list,transfersyntax,imagenumber)) return false;
	return true;
}

