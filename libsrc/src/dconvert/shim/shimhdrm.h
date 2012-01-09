// Manually define classes to handle "method" based blocks
// (equivalent to automatically generated stuff in shimhdrp.h)

struct SHIM_HeaderClass_FILEHDR_Entry {
	char		itemname[7];
	char 		itemtype;	// 'B' or'I' or 'F'
	Uint16_L	wordoffset;	// from 1
	Uint16_L	itemsize;	// number of itemtype elements
};

class SHIM_HeaderClass_FILEHDR {
	SHIM_HeaderClass_FILEHDR_Entry *entry;
	char *data;
	unsigned nEntries;

	const SHIM_HeaderClass_FILEHDR_Entry *operator[](const char *index) const;
public:
	SHIM_HeaderClass_FILEHDR(istream *ist,
		Uint16 fhentries,Uint16 fhcount,
		Uint16 fhdata,Uint16 blkfhdata); // Defined in shimhdrc.cc

	char   *SHIM_Method_ExtractTaggedString (const char *index) const;
	Int16   SHIM_Method_ExtractTaggedInteger(const char *index) const;
	Float64 SHIM_Method_ExtractTaggedFloat  (const char *index) const;
};

struct SHIM_HeaderClass_SLICEHDR_Entry {
	char		itemname[7];
	char 		itemtype;	// 'B' or'I' or 'F'
	Uint16_L	wordoffset;	// from 1
	Uint16_L	itemsize;	// number of itemtype elements
};

class SHIM_HeaderClass_SLICEHDR {
	SHIM_HeaderClass_SLICEHDR_Entry *entry;
	Uint16_L *sliceposition;
	char **headerdata;
	unsigned nEntries;
	unsigned nSlices;

	const SHIM_HeaderClass_SLICEHDR_Entry *operator[](const char *index) const;
public:
	SHIM_HeaderClass_SLICEHDR(istream *ist,
		Uint16 nslices,
		Uint16 slentries,Uint16 slcount,
		Uint16 sltbposn,Uint16 sltbsize,
		Uint16 blkslentries,Uint16 blksltbposn,
		Uint16 blksltbdata); // Defined in shimhdrc.cc

	char   *SHIM_Method_ExtractTaggedString (unsigned slice,const char *index) const;
	Int16   SHIM_Method_ExtractTaggedInteger(unsigned slice,const char *index) const;
	Float64 SHIM_Method_ExtractTaggedFloat  (unsigned slice,const char *index) const;
};

