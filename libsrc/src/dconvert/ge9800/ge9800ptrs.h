// Offsets are in bytes from 0
// Offsets extracted from header are in 2 byte words - blocks are 512 bytes long

#define	GE9800_Offset_GLOBALHDR_ptr	0

// these are not qualified by "ge9800hdr->", since they are used within methods of that instance's class (GE9800_Conversion) ...
#define	GE9800_Offset_EXAMHDR_ptr	(GE9800_HeaderInstance_GLOBALHDR->EXAMHDRPTR		? (GE9800_HeaderInstance_GLOBALHDR->EXAMHDRPTR   - 1u)*2 : 512)
#define	GE9800_Offset_IMAGE1HDR_ptr	(GE9800_HeaderInstance_GLOBALHDR->IMAGE1HDRPTR	? (GE9800_HeaderInstance_GLOBALHDR->IMAGE1HDRPTR - 1u)*2 : 1024)
#define	GE9800_Offset_IMAGE2HDR_ptr	(GE9800_HeaderInstance_GLOBALHDR->IMAGE2HDRPTR	? (GE9800_HeaderInstance_GLOBALHDR->IMAGE2HDRPTR - 1u)*2 : 1536)

// these are  qualified by "ge9800hdr->", since they are used outside methods of that instance's class (GE9800_Conversion) ...
#define GE9800_Offset_PixelDataMap_ptr	(ge9800hdr->GE9800_HeaderInstance_GLOBALHDR->IMAGEMAPPTR   ? (ge9800hdr->GE9800_HeaderInstance_GLOBALHDR->IMAGEMAPPTR  - 1u)*2  : (ge9800hdr->GE9800_HeaderInstance_IMAGE1HDR->GroupType == 5 ?  9 : 4)*512 )
#define GE9800_Offset_PixelData_ptr		(ge9800hdr->GE9800_HeaderInstance_GLOBALHDR->IMAGEDATAPTR  ? (ge9800hdr->GE9800_HeaderInstance_GLOBALHDR->IMAGEDATAPTR - 1u)*2  : (ge9800hdr->GE9800_HeaderInstance_IMAGE1HDR->GroupType == 5 ? 11 : 6)*512 )

#define	GE9800_UsePixelDataMap		(ge9800hdr->GE9800_HeaderInstance_IMAGE1HDR->UseImageMap == 1 ? true : false)			// 1=y,2=n
