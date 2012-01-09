#define	XXXX_Offset_STDHDR_ptr	256
#define	XXXX_Offset_IMGHDR_ptr	768
#define	XXXX_Offset_MRHDR_ptr	(XXXX_Offset_IMGHDR_ptr+192)
#define	XXXX_Offset_CTHDR_ptr	(XXXX_Offset_IMGHDR_ptr+192)
#define	XXXX_Offset_IMGHDR2_ptr	(XXXX_Offset_IMGHDR_ptr+320)

#define	XXXX_isct	(XXXX_HeaderInstance_IMGHDR->IMODELNO_Mod == 1)
#define	XXXX_ismr	(XXXX_HeaderInstance_IMGHDR->IMODELNO_Mod == 2)

#define	XXXX_Offset_PixelData_ptr	(XXXX_Offset_IMGHDR_ptr \
			+xxxxhdr->XXXX_HeaderInstance_IMGHDR->IBLOCK*1024)
