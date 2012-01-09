//#include <iostream.h>	// for debugging only

#include "ptyhdr.h"
#include "shimptrs.h"
#include "shimhdrm.h"
#include "shimhdrp.h"
#include "shimhdrw.h"
#include "shimhdrc.h"

#include "fltype.h"

const Uint16 SHIM_BlockSize = 512;

// Manually generated constructors for "method" based classes declared in shimhdrm.h

// If the args are changed here, change the SHIM_MethodConstructorArgs macros in shimptrs.h

/* ------------------------------ File Header ------------------------------ */

SHIM_HeaderClass_FILEHDR::SHIM_HeaderClass_FILEHDR(istream *istr,
	Uint16 fhentries,Uint16 fhcount,Uint16 fhdata,Uint16 blkfhdata)
{
	// Load the file header parts ...

	nEntries=0;

	unsigned long offset=fhentries*SHIM_BlockSize;
	size_t length=fhcount*sizeof(SHIM_HeaderClass_FILEHDR_Entry);
//cerr << "SHIM_HeaderClass_FILEHDR::SHIM_HeaderClass_FILEHDR: entry offset=" << dec << offset << endl;
//cerr << "SHIM_HeaderClass_FILEHDR::SHIM_HeaderClass_FILEHDR: entry length=" << dec << length << endl;
	Assert(sizeof(SHIM_HeaderClass_FILEHDR_Entry) == 12);
	entry=new SHIM_HeaderClass_FILEHDR_Entry[fhcount];
	Assert(entry);
	istr->seekg(offset,ios::beg);
	if (!istr->good()) return;

	// Since the entries are 12 bytes and the blocks are 512 bytes,
	// there are 42 entries per block, with 8 bytes of padding to
	// prevent an entry spanning a block boundary (grump)

	unsigned entriesleft=fhcount;
	const unsigned entriesperblock=SHIM_BlockSize/sizeof(SHIM_HeaderClass_FILEHDR_Entry);
	const unsigned npadbytes=SHIM_BlockSize-entriesperblock*sizeof(SHIM_HeaderClass_FILEHDR_Entry);
	Assert(entriesperblock == 42);
	Assert(npadbytes == 8);
	SHIM_HeaderClass_FILEHDR_Entry *entryptr=entry;
	while (entriesleft) {
		unsigned entriesthistry=entriesleft > entriesperblock ? entriesperblock : entriesleft;

		istr->read((char*)entryptr,entriesthistry*sizeof(SHIM_HeaderClass_FILEHDR_Entry));
		if (!istr->good()) return;

		entriesleft-=entriesthistry;
		entryptr+=entriesthistry;

		char dummy[npadbytes];
		istr->read(dummy,npadbytes);
		if (!istr->good()) return;
	}

	offset=fhdata*SHIM_BlockSize;
	length=blkfhdata*SHIM_BlockSize;
//cerr << "SHIM_HeaderClass_FILEHDR::SHIM_HeaderClass_FILEHDR: data offset=" << dec << offset << endl;
//cerr << "SHIM_HeaderClass_FILEHDR::SHIM_HeaderClass_FILEHDR: data length=" << dec << length << endl;
	data=new char[length];
	Assert(data);
	istr->seekg(offset,ios::beg);
	if (!istr->good()) return;
	istr->read(data,length);
	if (!istr->good()) return;

	nEntries=fhcount;

	// Think about how caller handles errors ...
	// see what ReadProprietaryHeader() would do

//	int i;
//	SHIM_HeaderClass_FILEHDR_Entry *ptr;
//	for (i=0,ptr=entry; i<nEntries; ++i,++ptr) {
//		cerr << "SHIM_HeaderClass_FILEHDR::SHIM_HeaderClass_FILEHDR: "
//		     << "[" << dec << i << "]"
//		     << " itemname=" << ptr->itemname
//		     << " itemtype=" << ptr->itemtype
//		     << " wordoffset=" << ptr->wordoffset
//		     << " itemsize=" << ptr->itemsize
//		     << endl;
//	}
}

const SHIM_HeaderClass_FILEHDR_Entry *
SHIM_HeaderClass_FILEHDR::operator[](const char *index) const
{
	Assert(index);
	int i=0;
	SHIM_HeaderClass_FILEHDR_Entry *ptr=entry;
	while (i<nEntries && ptr && ptr->itemname && strcmp(ptr->itemname,index) != 0) { ++i; ++ptr; }
	return i<nEntries ? entry+i : 0;
}

char *
SHIM_HeaderClass_FILEHDR::SHIM_Method_ExtractTaggedString(const char *index) const
{
	const SHIM_HeaderClass_FILEHDR_Entry *ptr=(*this)[index];
	char *s;
	if (ptr && ptr->itemtype == 'B') {
		s=new char[ptr->itemsize+1];
		strncpy(s,data+(ptr->wordoffset-1)*2,ptr->itemsize);
		s[ptr->itemsize]='\0';
	}
	else
		s=0;

	return s;
}

Int16
SHIM_HeaderClass_FILEHDR::SHIM_Method_ExtractTaggedInteger(const char *index) const
{
	// Item size > 1 NYI
	// ... IDEMAP itemtype=I wordoffset=79 itemsize=63

	const SHIM_HeaderClass_FILEHDR_Entry *ptr=(*this)[index];
	Uint16 u;
	if (ptr && ptr->itemtype == 'I' && ptr->itemsize == 1) {
		// little endian ...
		// used unsigned char * else will sign extend :(
		u=(*((unsigned char *)data+(ptr->wordoffset-1)*2+1) << 8)
		 +(*((unsigned char *)data+(ptr->wordoffset-1)*2));
	}
	else
		u=0xfffe;

//cerr << "SHIM_HeaderClass_FILEHDR::SHIM_Method_ExtractTaggedInteger:"
//     << " index=" << (index ? index : "")
//     << " returns " << dec << u
//     << endl;

	return Int16(u);
}

Float64
SHIM_HeaderClass_FILEHDR::SHIM_Method_ExtractTaggedFloat(const char *index) const
{
	// Item size > 1 NYI
	// ... BOLTIM itemtype=F wordoffset=246 itemsize=2

	const SHIM_HeaderClass_FILEHDR_Entry *ptr=(*this)[index];

	if (ptr && ptr->itemtype == 'F' && ptr->itemsize == 1) {
		return *(const Vax_Float_F *)(data+(ptr->wordoffset-1)*2);
	}
	else
		return 9999;
}

/* ------------------------------ Slice Header ------------------------------ */

SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR(istream *istr,
		Uint16 nslices,
		Uint16 slentries,Uint16 slcount,
		Uint16 sltbposn,Uint16 sltbsize,
		Uint16 blkslentries,Uint16 blksltbposn,
		Uint16 blksltbdata)
{
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: start nslices=" << dec << nslices << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: start slentries=" << dec << slentries << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: start slcount=" << dec << slcount << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: start sltbposn=" << dec << sltbposn << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: start sltbsize=" << dec << sltbsize << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: start blkslentries=" << dec << blkslentries << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: start blksltbposn=" << dec << blksltbposn << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: start blksltbdata=" << dec << blksltbdata << endl;

	(void)sltbsize;
	(void)blkslentries;
	(void)blksltbposn;

	// Load the slice header parts ...

	nEntries=0;
	nSlices=0;

	unsigned long offset=slentries*SHIM_BlockSize;
	size_t length=slcount*sizeof(SHIM_HeaderClass_SLICEHDR_Entry);
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: entry offset=" << dec << offset << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: entry length=" << dec << length << endl;
	Assert(sizeof(SHIM_HeaderClass_SLICEHDR_Entry) == 12);
	entry=new SHIM_HeaderClass_SLICEHDR_Entry[slcount];
	Assert(entry);
	istr->seekg(offset,ios::beg);
	if (!istr->good()) return;

	// Since the entries are 12 bytes and the blocks are 512 bytes,
	// there are 42 entries per block, with 8 bytes of padding to
	// prevent an entry spanning a block boundary (grump)

	unsigned entriesleft=slcount;
	const unsigned entriesperblock=SHIM_BlockSize/sizeof(SHIM_HeaderClass_SLICEHDR_Entry);
	const unsigned npadbytes=SHIM_BlockSize-entriesperblock*sizeof(SHIM_HeaderClass_SLICEHDR_Entry);
	Assert(entriesperblock == 42);
	Assert(npadbytes == 8);
	SHIM_HeaderClass_SLICEHDR_Entry *entryptr=entry;
	while (entriesleft) {
		unsigned entriesthistry=entriesleft > entriesperblock ? entriesperblock : entriesleft;

		istr->read((char*)entryptr,entriesthistry*sizeof(SHIM_HeaderClass_SLICEHDR_Entry));
		if (!istr->good()) return;

		entriesleft-=entriesthistry;
		entryptr+=entriesthistry;

		char dummy[npadbytes];
		istr->read(dummy,npadbytes);
		if (!istr->good()) return;
	}

	// read the slice position table

	offset=sltbposn*SHIM_BlockSize;
	length=nslices*sizeof(Uint16_L);
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: position offset=" << dec << offset << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: position length=" << dec << length << endl;
	sliceposition=new Uint16_L[nslices];
	Assert(sliceposition);
	istr->seekg(offset,ios::beg);
	if (!istr->good()) return;
	
	unsigned i;

	istr->read((char *)sliceposition,length);
	if (!istr->good()) return;

//	for (i=0; i<nslices; ++i) {
//		cerr << "sliceposition[" << dec << i << "]=" << dec << sliceposition[i] << endl;
//	}

	// read the offsets of the slice header data
	// by dereferencing the slice position table block pointers

	headerdata=new char *[nslices];
	Assert(headerdata);

	for (i=0; i<nslices; ++i) {
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: " 
//     << "[" << dec << i << "]"
//     << " dereference the slice position table block pointers"
//     << endl;
		// dereference the slice position table block pointers ...

		offset=sliceposition[i]*SHIM_BlockSize;
		length=blksltbdata*SHIM_BlockSize;
		headerdata[i]=new char[length];

//cerr << " sliceposition[i]=" << dec << sliceposition[i] << endl;
//cerr << " offset=" << dec << offset << endl;
//cerr << " blksltbdata=" << dec << blksltbdata << endl;
//cerr << " length=" << dec << length << endl;

		istr->seekg(offset,ios::beg);
		if (!istr->good()) return;
		istr->read(headerdata[i],length);
		if (!istr->good()) return;
	}

	nEntries=slcount;
	nSlices=nslices;

//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: end nEntries=" << dec << nEntries << endl;
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: end nSlices=" << dec << nSlices << endl;

	// Think about how caller handles errors ...
	// see what ReadProprietaryHeader() would do

//	SHIM_HeaderClass_SLICEHDR_Entry *ptr;
//	for (i=0,ptr=entry; i<nEntries; ++i,++ptr) {
//		cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: "
//		     << "[" << dec << i << "]"
//		     << " itemname=" << ptr->itemname
//		     << " itemtype=" << ptr->itemtype
//		     << " wordoffset=" << ptr->wordoffset
//		     << " itemsize=" << ptr->itemsize
//		     << endl;
//	}

//	for (i=0; i<nSlices; ++i) {
//		cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_HeaderClass_SLICEHDR: " 
//		     << "[" << dec << i << "]"
//		     << " sliceposition=" << sliceposition[i]
//		     << endl;
//	}

	// BTW. the first Uint16_L field of the headerdata for each slice
	// is ISDATP which is 512 byte block offset to slice image data
}

const SHIM_HeaderClass_SLICEHDR_Entry *
SHIM_HeaderClass_SLICEHDR::operator[](const char *index) const
{
	Assert(index);
	int i=0;
	SHIM_HeaderClass_SLICEHDR_Entry *ptr=entry;
	while (i<nEntries && ptr && ptr->itemname && strcmp(ptr->itemname,index) != 0) { ++i; ++ptr; }
	return i<nEntries ? entry+i : 0;
}

char *
SHIM_HeaderClass_SLICEHDR::SHIM_Method_ExtractTaggedString(unsigned slice,const char *index) const
{
	Assert(slice < nSlices);
	const SHIM_HeaderClass_SLICEHDR_Entry *ptr=(*this)[index];
	char *s;
	if (ptr && ptr->itemtype == 'B') {
		s=new char[ptr->itemsize+1];
		strncpy(s,headerdata[slice]+(ptr->wordoffset-1)*2,ptr->itemsize);
		s[ptr->itemsize]='\0';
	}
	else
		s=0;

	return s;
}

Int16
SHIM_HeaderClass_SLICEHDR::SHIM_Method_ExtractTaggedInteger(unsigned slice,const char *index) const
{
//cerr << "SHIM_HeaderClass_SLICEHDR::SHIM_Method_ExtractTaggedInteger:"
//     << " nSlices=" << dec << nSlices
//     << " slice=" << dec << slice
//     << " index=" << (index ? index : "")
//     << endl;
	Assert(slice < nSlices);
	// Item size > 1 NYI

	const SHIM_HeaderClass_SLICEHDR_Entry *ptr=(*this)[index];
	Uint16 u;
	if (ptr && ptr->itemtype == 'I' && ptr->itemsize == 1) {
		// little endian ...
		// used unsigned char * else will sign extend :(
		u=(*((unsigned char *)headerdata[slice]+(ptr->wordoffset-1)*2+1) << 8)
		 +(*((unsigned char *)headerdata[slice]+(ptr->wordoffset-1)*2));
	}
	else
		u=0xffe0;

	return Int16(u);
}

Float64
SHIM_HeaderClass_SLICEHDR::SHIM_Method_ExtractTaggedFloat(unsigned slice,const char *index) const
{
	Assert(slice < nSlices);
	// Item size > 1 NYI

	const SHIM_HeaderClass_SLICEHDR_Entry *ptr=(*this)[index];

	if (ptr && ptr->itemtype == 'F' && ptr->itemsize == 1) {
		return *(const Vax_Float_F *)(headerdata[slice]+(ptr->wordoffset-1)*2);
	}
	else
		return 9999;
}

