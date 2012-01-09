// Manually define classes to handle "method" based blocks
// (equivalent to automatically generated stuff in imtnhdrp.h)

struct IMTN_HeaderClass_FILEHDR_Entry {
	char		itemname[7];
	char 		itemtype;	// 'B' or'I' or 'F'
	Uint16_L	wordoffset;	// from 1
	Uint16_L	itemsize;	// number of itemtype elements
};

class IMTN_HeaderClass_FILEHDR {
	IMTN_HeaderClass_FILEHDR_Entry *entry;
	char *data;
	unsigned nEntries;

	const IMTN_HeaderClass_FILEHDR_Entry *operator[](const char *index) const;
public:
	IMTN_HeaderClass_FILEHDR(istream *ist,
		Uint16 fhentries,Uint16 fhcount,
		Uint16 fhdata,Uint16 blkfhdata); // Defined in imtnhdrc.cc

	char   *IMTN_Method_ExtractTaggedString (const char *index) const;
	Int16   IMTN_Method_ExtractTaggedInteger(const char *index) const;
	Float64 IMTN_Method_ExtractTaggedFloat  (const char *index) const;
};

struct IMTN_HeaderClass_SLICEHDR_Entry {
	char		itemname[7];
	char 		itemtype;	// 'B' or'I' or 'F'
	Uint16_L	wordoffset;	// from 1
	Uint16_L	itemsize;	// number of itemtype elements
};

class IMTN_HeaderClass_SLICEHDR {
	IMTN_HeaderClass_SLICEHDR_Entry *entry;
	Uint16_L *sliceposition;
	char **headerdata;
	unsigned nEntries;
	unsigned nSlices;

	const IMTN_HeaderClass_SLICEHDR_Entry *operator[](const char *index) const;
public:
	IMTN_HeaderClass_SLICEHDR(istream *ist,
		Uint16 nslices,
		Uint16 slentries,Uint16 slcount,
		Uint16 sltbposn,Uint16 sltbsize,
		Uint16 blkslentries,Uint16 blksltbposn,
		Uint16 blksltbdata); // Defined in imtnhdrc.cc

	char   *IMTN_Method_ExtractTaggedString (unsigned slice,const char *index) const;
	Int16   IMTN_Method_ExtractTaggedInteger(unsigned slice,const char *index) const;
	Float64 IMTN_Method_ExtractTaggedFloat  (unsigned slice,const char *index) const;
};

