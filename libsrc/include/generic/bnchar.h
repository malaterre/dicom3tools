#ifndef __Header_bnchar__
#define __Header_bnchar__

#include "basetype.h"
#include "endtype.h"

static inline
Uint16 extract16(const unsigned char *buffer,Endian endian)
{
	Assert(endian!=NoEndian);
	Uint16 u;
	if (endian == BigEndian) {
		u =  (Uint16)buffer[0];
		u <<= 8;
		u |= (Uint16)buffer[1];
	}
	else {
		u =  (Uint16)buffer[1];
		u <<= 8;
		u |= (Uint16)buffer[0];
	}
	return u;
}

static inline
Uint32 extract32(const unsigned char *buffer,Endian endian)
{
	Assert(endian!=NoEndian);
	Uint32 u;
	if (endian == BigEndian) {
		u =  (Uint32)buffer[0];
		u <<= 8;
		u |= (Uint32)buffer[1];
		u <<= 8;
		u |= (Uint32)buffer[2];
		u <<= 8;
		u |= (Uint32)buffer[3];
	}
	else {
		u =  (Uint32)buffer[3];
		u <<= 8;
		u |= (Uint32)buffer[2];
		u <<= 8;
		u |= (Uint32)buffer[1];
		u <<= 8;
		u |= (Uint32)buffer[0];
	}
	return u;
}

static inline
void insert16(Uint16 u,unsigned char *buffer,Endian endian)
{
	Assert(endian!=NoEndian);
	if (endian == BigEndian) {
		buffer[0]=(unsigned char)(u>>8);
		buffer[1]=(unsigned char)u;
	}
	else {
		buffer[1]=(unsigned char)(u>>8);
		buffer[0]=(unsigned char)u;
	}
}

static inline
void insert32(Uint32 u,unsigned char *buffer,Endian endian)
{
	Assert(endian!=NoEndian);
	if (endian == BigEndian) {
		buffer[0]=(unsigned char)(u>>24);
		buffer[1]=(unsigned char)(u>>16);
		buffer[2]=(unsigned char)(u>>8);
		buffer[3]=(unsigned char)u;
	}
	else {
		buffer[3]=(unsigned char)(u>>24);
		buffer[2]=(unsigned char)(u>>16);
		buffer[1]=(unsigned char)(u>>8);
		buffer[0]=(unsigned char)u;
	}
}

static inline
void insert16incr(Uint16 u,unsigned char *&buffer,Endian endian)
{
	insert16(u,buffer,endian);
	buffer+=2;
}

static inline
void insert32incr(Uint32 u,unsigned char *&buffer,Endian endian)
{
	insert32(u,buffer,endian);
	buffer+=4;
}

#endif /* __Header_bnchar__ */
