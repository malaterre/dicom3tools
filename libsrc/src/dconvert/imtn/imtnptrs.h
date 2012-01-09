#define	IMTN_Offset_MAINHDR_ptr	0

// note the comma preceeding the constructor args ...

#define	IMTN_MethodConstructorArgs_FILEHDR			\
		,IMTN_HeaderInstance_MAINHDR->FHENTRIES,	\
		 IMTN_HeaderInstance_MAINHDR->FHCOUNT,		\
		 IMTN_HeaderInstance_MAINHDR->FHDATA,		\
		 IMTN_HeaderInstance_MAINHDR->BLKFHDATA

#define	IMTN_MethodConstructorArgs_SLICEHDR			\
		,IMTN_HeaderInstance_FILEHDR->IMTN_Method_ExtractTaggedInteger("NSLICE"),	\
		 IMTN_HeaderInstance_MAINHDR->SLENTRIES,	\
		 IMTN_HeaderInstance_MAINHDR->SLCOUNT,		\
		 IMTN_HeaderInstance_MAINHDR->SLTBPOSN,		\
		 IMTN_HeaderInstance_MAINHDR->SLTBSIZE,  	\
		 IMTN_HeaderInstance_MAINHDR->BLKSLENTRIES,	\
		 IMTN_HeaderInstance_MAINHDR->BLKSLTBPOSN,	\
		 IMTN_HeaderInstance_MAINHDR->BLKSLTBDATA

#define	IMTN_Offset_PixelData_ptr	(imtnhdr->IMTN_HeaderInstance_SLICEHDR \
		->IMTN_Method_ExtractTaggedInteger(imagenumber,"ISDATP")*512)
