#define	SHIM_Offset_MAINHDR_ptr	0

#define	SHIM_isExtendedHeader			(shimhdr->SHIM_HeaderInstance_MAINHDR->ZHREV > 255)
#define SHIM_MAINHDR_lng			(SHIM_isExtendedHeader ? 2048 : 512)
#define SHIM_IndexEntry_lng			(SHIM_isExtendedHeader ? 256 : 48)
#define SHIM_Offset_IndexEntry_ptr(image)	(SHIM_MAINHDR_lng+image*SHIM_IndexEntry_lng)

#define SHIM_Offset_PixelData_ptr(entry)	((shimhdr->SHIM_HeaderInstance_MAINHDR->ZIBLKA - 1 \
						  + (SHIM_isExtendedHeader \
							? ((Uint32(*(entry+50))<<24) + (Uint32(*(entry+51))<<16) + (Uint32(*(entry+48))<< 8) + Uint32(*(entry+49))) \
							: ((Uint32(*(entry+28))<< 8) +  Uint32(*(entry+29)) ) ) \
						 ) * 512)

