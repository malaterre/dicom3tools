#define	PACE_Offset_STDHDR_ptr	256
#define	PACE_Offset_IMGHDR_ptr	768
#define	PACE_Offset_MRHDR_ptr	(PACE_Offset_IMGHDR_ptr+192)
#define	PACE_Offset_CTHDR_ptr	(PACE_Offset_IMGHDR_ptr+192)
#define	PACE_Offset_IMGHDR2_ptr	(PACE_Offset_IMGHDR_ptr+320)

#define	PACE_isct	(PACE_HeaderInstance_IMGHDR->IMODELNO_Mod == 1)
#define	PACE_ismr	(PACE_HeaderInstance_IMGHDR->IMODELNO_Mod == 2)

#define	PACE_Offset_PixelData_ptr	(PACE_Offset_IMGHDR_ptr \
			+pacehdr->PACE_HeaderInstance_IMGHDR->IBLOCK*1024)
