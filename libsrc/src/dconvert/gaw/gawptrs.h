#define	GAW_Offset_FILEHDR_ptr		(fileinfo.explicitFileHdrPtr)

#define	GAW_Offset_UIDHDR_ptr		(fileinfo.explicitFileHdrPtr+GAW_HeaderInstance_FILEHDR->IH_img_p_id)
#define	GAW_Offset_UNPACKHDR_ptr	(fileinfo.explicitFileHdrPtr+GAW_HeaderInstance_FILEHDR->IH_img_p_unpack)
#define	GAW_Offset_COMPRESSHDR_ptr	(fileinfo.explicitFileHdrPtr+GAW_HeaderInstance_FILEHDR->IH_img_p_compress)
#define	GAW_Offset_HISTOHDR_ptr		(fileinfo.explicitFileHdrPtr+GAW_HeaderInstance_FILEHDR->IH_img_p_histo)
#define	GAW_Offset_TEXTHDR_ptr		(fileinfo.explicitFileHdrPtr+GAW_HeaderInstance_FILEHDR->IH_img_p_text)
#define	GAW_Offset_GRAPHICSHDR_ptr	(fileinfo.explicitFileHdrPtr+GAW_HeaderInstance_FILEHDR->IH_img_p_graphics)
#define	GAW_Offset_DBHDR_ptr		(fileinfo.explicitFileHdrPtr+GAW_HeaderInstance_FILEHDR->IH_img_p_dbHdr)
#define	GAW_Offset_USERHDR_ptr		(fileinfo.explicitFileHdrPtr+GAW_HeaderInstance_FILEHDR->IH_img_p_user)

#define	GAW_Offset_SUITEHDR_ptr		(fileinfo.useExplicitOffsets ? fileinfo.explicitSuiteHdrPtr  : Uint32(GAW_HeaderInstance_FILEHDR->IH_img_p_suite))
#define	GAW_Offset_EXAMHDR_ptr		(fileinfo.useExplicitOffsets ? fileinfo.explicitExamHdrPtr   : Uint32(GAW_HeaderInstance_FILEHDR->IH_img_p_exam))
#define	GAW_Offset_SERIESHDR_ptr	(fileinfo.useExplicitOffsets ? fileinfo.explicitSeriesHdrPtr : Uint32(GAW_HeaderInstance_FILEHDR->IH_img_p_series))

#define	GAW_Offset_UIDHDR_lng		GAW_HeaderInstance_FILEHDR->IH_img_l_id
#define	GAW_Offset_UNPACKHDR_lng	GAW_HeaderInstance_FILEHDR->IH_img_l_unpack
#define	GAW_Offset_COMPRESSHDR_lng	GAW_HeaderInstance_FILEHDR->IH_img_l_compress
#define	GAW_Offset_HISTOHDR_lng		GAW_HeaderInstance_FILEHDR->IH_img_l_histo
#define	GAW_Offset_TEXTHDR_lng		GAW_HeaderInstance_FILEHDR->IH_img_l_text
#define	GAW_Offset_GRAPHICSHDR_lng	GAW_HeaderInstance_FILEHDR->IH_img_l_graphics
#define	GAW_Offset_DBHDR_lng		GAW_HeaderInstance_FILEHDR->IH_img_l_dbHdr
#define	GAW_Offset_USERHDR_lng		GAW_HeaderInstance_FILEHDR->IH_img_l_user

//#define	GAW_Offset_SUITEHDR_lng		GAW_HeaderInstance_FILEHDR->IH_img_l_suite
//#define	GAW_Offset_EXAMHDR_lng		GAW_HeaderInstance_FILEHDR->IH_img_l_exam
//#define	GAW_Offset_SERIESHDR_lng	GAW_HeaderInstance_FILEHDR->IH_img_l_series

#define GAW_hasuid			(GAW_Offset_UIDHDR_lng      != 0)
#define GAW_hasunpack			(GAW_Offset_UNPACKHDR_lng   != 0)
#define GAW_hascompress			(GAW_Offset_COMPRESSHDR_lng != 0)
#define GAW_hashisto			(GAW_Offset_HISTOHDR_lng    != 0)
#define GAW_hastext			(GAW_Offset_TEXTHDR_lng     != 0)
#define GAW_hasgraphics			(GAW_Offset_GRAPHICSHDR_lng != 0)
#define GAW_hasuser			(GAW_Offset_USERHDR_lng     != 0)

#define	GAW_Offset_MRHDR_ptr		(fileinfo.useExplicitOffsets ? fileinfo.explicitImageHdrPtr : Uint32(GAW_HeaderInstance_FILEHDR->IH_img_p_image))
#define	GAW_Offset_CTHDR_ptr		(fileinfo.useExplicitOffsets ? fileinfo.explicitImageHdrPtr : Uint32(GAW_HeaderInstance_FILEHDR->IH_img_p_image))

//#define	GAW_Offset_MRHDR_lng		GAW_HeaderInstance_FILEHDR->IH_img_l_image
//#define	GAW_Offset_CTHDR_lng		GAW_HeaderInstance_FILEHDR->IH_img_l_image

#define GAW_isct			(strcmp(GAW_HeaderInstance_EXAMHDR->EX_ex_typ,"CT") == 0)
#define GAW_ismr			(strcmp(GAW_HeaderInstance_EXAMHDR->EX_ex_typ,"MR") == 0)

#define	GAW_Offset_PixelData_ptr	GAW_HeaderInstance_FILEHDR->IH_img_hdr_length + (fileinfo.pixelDataLengthFieldPresent ? 4 : 0)
