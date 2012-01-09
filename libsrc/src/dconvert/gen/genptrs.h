#define	GEN_Offset_FILEHDR_ptr		(fileinfo.explicitFileHdrPtr)

#define	GEN_Offset_UIDHDR_ptr		(fileinfo.explicitFileHdrPtr+GEN_HeaderInstance_FILEHDR->IH_img_p_id)
#define	GEN_Offset_UNPACKHDR_ptr	(fileinfo.explicitFileHdrPtr+GEN_HeaderInstance_FILEHDR->IH_img_p_unpack)
#define	GEN_Offset_COMPRESSHDR_ptr	(fileinfo.explicitFileHdrPtr+GEN_HeaderInstance_FILEHDR->IH_img_p_compress)
#define	GEN_Offset_HISTOHDR_ptr		(fileinfo.explicitFileHdrPtr+GEN_HeaderInstance_FILEHDR->IH_img_p_histo)
#define	GEN_Offset_TEXTHDR_ptr		(fileinfo.explicitFileHdrPtr+GEN_HeaderInstance_FILEHDR->IH_img_p_text)
#define	GEN_Offset_GRAPHICSHDR_ptr	(fileinfo.explicitFileHdrPtr+GEN_HeaderInstance_FILEHDR->IH_img_p_graphics)
#define	GEN_Offset_DBHDR_ptr		(fileinfo.explicitFileHdrPtr+GEN_HeaderInstance_FILEHDR->IH_img_p_dbHdr)
#define	GEN_Offset_USERHDR_ptr		(fileinfo.explicitFileHdrPtr+GEN_HeaderInstance_FILEHDR->IH_img_p_user)

#define	GEN_Offset_SUITEHDR_ptr		(fileinfo.useExplicitOffsets ? fileinfo.explicitSuiteHdrPtr  : Uint32(GEN_HeaderInstance_FILEHDR->IH_img_p_suite))
#define	GEN_Offset_EXAMHDR_ptr		(fileinfo.useExplicitOffsets ? fileinfo.explicitExamHdrPtr   : Uint32(GEN_HeaderInstance_FILEHDR->IH_img_p_exam))
#define	GEN_Offset_SERIESHDR_ptr	(fileinfo.useExplicitOffsets ? fileinfo.explicitSeriesHdrPtr : Uint32(GEN_HeaderInstance_FILEHDR->IH_img_p_series))

#define	GEN_Offset_UIDHDR_lng		GEN_HeaderInstance_FILEHDR->IH_img_l_id
#define	GEN_Offset_UNPACKHDR_lng	GEN_HeaderInstance_FILEHDR->IH_img_l_unpack
#define	GEN_Offset_COMPRESSHDR_lng	GEN_HeaderInstance_FILEHDR->IH_img_l_compress
#define	GEN_Offset_HISTOHDR_lng		GEN_HeaderInstance_FILEHDR->IH_img_l_histo
#define	GEN_Offset_TEXTHDR_lng		GEN_HeaderInstance_FILEHDR->IH_img_l_text
#define	GEN_Offset_GRAPHICSHDR_lng	GEN_HeaderInstance_FILEHDR->IH_img_l_graphics
#define	GEN_Offset_DBHDR_lng		GEN_HeaderInstance_FILEHDR->IH_img_l_dbHdr
#define	GEN_Offset_USERHDR_lng		GEN_HeaderInstance_FILEHDR->IH_img_l_user

//#define	GEN_Offset_SUITEHDR_lng		GEN_HeaderInstance_FILEHDR->IH_img_l_suite
//#define	GEN_Offset_EXAMHDR_lng		GEN_HeaderInstance_FILEHDR->IH_img_l_exam
//#define	GEN_Offset_SERIESHDR_lng	GEN_HeaderInstance_FILEHDR->IH_img_l_series

#define GEN_hasuid			(GEN_Offset_UIDHDR_lng      != 0)
#define GEN_hasunpack			(GEN_Offset_UNPACKHDR_lng   != 0)
#define GEN_hascompress			(GEN_Offset_COMPRESSHDR_lng != 0)
#define GEN_hashisto			(GEN_Offset_HISTOHDR_lng    != 0)
#define GEN_hastext			(GEN_Offset_TEXTHDR_lng     != 0)
#define GEN_hasgraphics			(GEN_Offset_GRAPHICSHDR_lng != 0)
#define GEN_hasuser			(GEN_Offset_USERHDR_lng     != 0)

#define	GEN_Offset_MRHDR_ptr		(fileinfo.useExplicitOffsets ? fileinfo.explicitImageHdrPtr : Uint32(GEN_HeaderInstance_FILEHDR->IH_img_p_image))
#define	GEN_Offset_CTHDR_ptr		(fileinfo.useExplicitOffsets ? fileinfo.explicitImageHdrPtr : Uint32(GEN_HeaderInstance_FILEHDR->IH_img_p_image))

//#define	GEN_Offset_MRHDR_lng		GEN_HeaderInstance_FILEHDR->IH_img_l_image
//#define	GEN_Offset_CTHDR_lng		GEN_HeaderInstance_FILEHDR->IH_img_l_image

#define GEN_isct			(strcmp(GEN_HeaderInstance_EXAMHDR->EX_ex_typ,"CT") == 0)
#define GEN_ismr			(strcmp(GEN_HeaderInstance_EXAMHDR->EX_ex_typ,"MR") == 0)

#define	GEN_Offset_PixelData_ptr	GEN_HeaderInstance_FILEHDR->IH_img_hdr_length + (fileinfo.pixelDataLengthFieldPresent ? 4 : 0)
