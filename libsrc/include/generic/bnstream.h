#ifndef __Header_bnstream__
#define __Header_bnstream__

#include <iostream.h>
//#include <iomanip.h>

#include "basetype.h"
#include "endtype.h"

// Note that great care has to be taken adding insert and
// extract operators as these classes (and any derived classes)
// are themselves derived from iostream classes which DO NOT
// define insert and extract methods as virtual, so method
// calls from base objects will NOT call the derived objects
// methods

// Furthermore, as iostream classes declare the operators as
// methods of the classes rather than global functions, then
// they can not be overloaded with replacement global functions
// or an ambiguity arises, hence the classes here MUST KNOW ABOUT
// the objects that are inserted or extracted, hence the
// need for forward declarations (blech :( ).

// Finally, if such operators are declared here, then those of
// the base class become invisible, hence useful guys like
// operator<<(const char *) need to be passed on.

class Tag;	// for operator<<() and operator>>()

class BinaryInputStream : public istream {
private:
	Endian endian;
	char buffer[4];
	bool swapped32big;
public:
	BinaryInputStream(streambuf *buf,Endian e=NoEndian)
		: istream(buf)
		{ endian=e; swapped32big=false; }

	BinaryInputStream(istream& istr,Endian e=NoEndian)
		: istream(istr.rdbuf())
		{ endian=e; swapped32big=false; }

	virtual ~BinaryInputStream(void)	{}

	bool 	isBigEndian(void)	{ return endian == BigEndian; }
	bool 	isLittleEndian(void)	{ return endian == LittleEndian; }
	bool	isSwapped32Big(void)	{ return swapped32big; }
	Endian 	getEndian(void)				{ return endian; }
	void 	setEndian(Endian e=LittleEndian)	{ endian=e; }
	void	setSwapped32Big(void)			{ swapped32big=true; }

	unsigned char read8(void)
		{
			Assert(endian!=NoEndian);
			char u;
			read(&u,1);
			return (unsigned char)u;
		}
	Uint16 read16(void)
		{
			Assert(endian!=NoEndian);
			Uint16 u;
			read(buffer,2);
			if (isBigEndian()) {
				u =  (Uint16)((unsigned char *)buffer)[0];
				u <<= 8;
				u |= (Uint16)((unsigned char *)buffer)[1];
			}
			else {
				u =  (Uint16)((unsigned char *)buffer)[1];
				u <<= 8;
				u |= (Uint16)((unsigned char *)buffer)[0];
			}
			return u;
		}
	Uint32 read32(void)
		{
			Assert(endian!=NoEndian);
			Uint32 u;
			read(buffer,4);
			if (isBigEndian()) {
				if (swapped32big) {
					u =  (Uint32)((unsigned char *)buffer)[2];
					u <<= 8;
					u |= (Uint32)((unsigned char *)buffer)[3];
					u <<= 8;
					u |= (Uint32)((unsigned char *)buffer)[0];
					u <<= 8;
					u |= (Uint32)((unsigned char *)buffer)[1];
				}
				else {
					u =  (Uint32)((unsigned char *)buffer)[0];
					u <<= 8;
					u |= (Uint32)((unsigned char *)buffer)[1];
					u <<= 8;
					u |= (Uint32)((unsigned char *)buffer)[2];
					u <<= 8;
					u |= (Uint32)((unsigned char *)buffer)[3];
				}
			}
			else {
				u =  (Uint32)((unsigned char *)buffer)[3];
				u <<= 8;
				u |= (Uint32)((unsigned char *)buffer)[2];
				u <<= 8;
				u |= (Uint32)((unsigned char *)buffer)[1];
				u <<= 8;
				u |= (Uint32)((unsigned char *)buffer)[0];
			}
			return u;
		}

	BinaryInputStream& operator>>(Uint8& rhs)
		{
			rhs=(Uint8)read8();
			return *this;
		}
	BinaryInputStream& operator>>(Uint16& rhs)
		{
			rhs=read16();
			return *this;
		}
	BinaryInputStream& operator>>(Uint32& rhs)
		{
			rhs=read32();
			return *this;
		}
	BinaryInputStream& operator>>(Tag& rhs);	// in attrtag.cc

//	istream& seekg(OurStreamPos p)
//              {
//			Assert(0);	// Too unportable to be used
//			(void)p;
//			return *this;
//              }
//	istream& seekg(OurStreamOff o,OurSeekDir d)
//              {
//			Assert(0);	// Too unportable to be used
//			(void)o; (void)d;
//			return *this;
//              }
};

class BinaryOutputStream : public ostream {
private:
	Endian endian;
	unsigned char buffer[4];
public:
	BinaryOutputStream(streambuf *buf,Endian e=NoEndian)
		: ostream(buf)
		{ endian=e; }

	BinaryOutputStream(ostream &ostr,Endian e=NoEndian)
		: ostream(ostr.rdbuf())
		{ endian=e; }

	virtual ~BinaryOutputStream(void)	{}

	bool 	isBigEndian(void)	{ return endian == BigEndian; }
	bool 	isLittleEndian(void)	{ return endian == LittleEndian; }
	Endian 	getEndian(void)				{ return endian; }
	void 	setEndian(Endian e=LittleEndian)	{ endian=e; }

	void write8(unsigned char u)
		{
			Assert(endian!=NoEndian);
			write((char *)&u,1);
		}
	void write16(Uint16 u)
		{
			Assert(endian!=NoEndian);
			if (isBigEndian()) {
				buffer[0]=(unsigned char)(u>>8);
				buffer[1]=(unsigned char)u;
			}
			else {
				buffer[1]=(unsigned char)(u>>8);
				buffer[0]=(unsigned char)u;
			}
			write((char *)buffer,2);
		}
	void write32(Uint32 u)
		{
			Assert(endian!=NoEndian);
			if (isBigEndian()) {
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
			write((char *)buffer,4);
		}

	BinaryOutputStream& operator<<(Uint8 rhs)
		{
			write8((unsigned char)rhs);
			return *this;
		}
	BinaryOutputStream& operator<<(Uint16 rhs)
		{
			write16(rhs);
			return *this;
		}
	BinaryOutputStream& operator<<(Uint32 rhs)
		{
			write32(rhs);
			return *this;
		}
	BinaryOutputStream& operator<<(Tag rhs);	// in attrtag.cc

	BinaryOutputStream& operator<<(const char *string)
		{
			((ostream &)*this) << string;
			return *this;
		}

	BinaryOutputStream& operator<<(char c)
		{
			((ostream &)*this) << c;
			return *this;
		}
};

#endif /* __Header_bnstream__ */
