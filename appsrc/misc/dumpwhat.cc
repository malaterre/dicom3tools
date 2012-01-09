#include <fstream.h>
#include <iomanip.h>
#include <stdlib.h>            // for atoi()
#include <ctype.h>
#include <string.h>

#include "basetype.h"
#include "ioopt.h"
#include "mesgtext.h"

const unsigned 	DefaultBufferSizeIncrement	= 1024;
const double 	ReasonableMinimumFloatToShow 	= 0.01;
	

inline unsigned char *
CopyOverlapping(unsigned char *to,const unsigned char *from,unsigned long n)
{
	while (n--) *to++=*from++;
	return to;
}

class Buffer {
	unsigned char *buffer;
	unsigned long bufsize;
	unsigned long bufincr;
	unsigned long length;
	unsigned long bytesread;
	unsigned long total;
	istream *istr;
	unsigned long offset;
	int over;

	void extendBuffer(void)
		{
			//cerr << "extendBuffer: start\n" << flush;
			//putState(cerr);
			unsigned char *oldbuf=buffer;
			unsigned long oldsize=bufsize;
			bufsize+=bufincr;
			buffer=new unsigned char[bufsize];
			if (length) memcpy(buffer,oldbuf,(int)length);
			if (oldsize) delete[] oldbuf;
			//cerr << "extendBuffer: done\n" << flush;
			//putState(cerr);
		}
	void cleanBuffer(void)
		{
			//cerr << "cleanBuffer: start\n" << flush;
			//putState(cerr);
			length-=bytesread;
			CopyOverlapping(buffer,buffer+bytesread,length);
			bytesread=0;
			//cerr << "cleanBuffer: done\n" << flush;
			//putState(cerr);
		}
	void putState(ostream& ostr)
		{
			ostr << "    putState:\n" << flush;
			ostr << "        bufsize   =" 
			     << bufsize << "\n" << flush;
			ostr << "        bufincr   =" 
			     << bufincr << "\n" << flush;
			ostr << "        length    =" 
			     << length << "\n" << flush;
			ostr << "        bytesread ="
			     << bytesread << "\n" << flush;
			ostr << "        offset    ="
			     << offset << "\n" << flush;
			ostr << "        total    ="
			     << total << "\n" << flush;
		}
public:
	Buffer(istream *str,unsigned long skip,unsigned long t)
		{
			//cerr << "Buffer(istream *str): start\n" << flush;
			buffer=0;
			bufsize=0;
			length=0;
			bytesread=0;
			istr=str;
			offset=0;
			total=t;
			over=0;
			bufincr=DefaultBufferSizeIncrement;

			// must be properly initialized by this point ...

			if (skip && read(skip) == 0) {
				cerr << "Error skipping first "
				     << dec << skip << " bytes\n" << flush;
				exit(1);
			}

			//cerr << "Buffer(istream *str): done\n" << flush;
			//putState(cerr);
		}

	const unsigned char * readLookAhead(unsigned long n)
		{
			//cerr << "readLookAhead: start(" << n << ")\n"
			//     << flush;
			//putState(cerr);
			if (over || (total && offset+n > total)) return 0;
			if (bufsize-bytesread < n)    cleanBuffer();
			while (bufsize-bytesread < n) extendBuffer();

			if (length-bytesread < n) 
				length+=istr->rdbuf()->sgetn(
					(char *)buffer+length,
					(int)(bufsize-length));

			const unsigned char *ptr;
			if (length-bytesread < n)
				ptr=0;			// failure
			else
				ptr=buffer+bytesread;

			//cerr << "readLookAhead: done returns "
			//     << (ptr ? ptr : (const unsigned char *)"<Null>")
			//     << "\n" << flush;
			//putState(cerr);
			return ptr;
		}

	const unsigned char *read(unsigned long n)
		{
			//cerr << "read: start(" << n << ")\n" << flush;
			//putState(cerr);
			const unsigned char *ptr = readLookAhead(n);
			if (ptr) {
				bytesread+=n;
				offset+=n;
			}
			else {
				// Attempt to read past end shuts us down.
				cerr << "read: attempt to read past end\n";
				over=1;
			}
			//cerr << "read: done returns "
			//     << (ptr ? ptr : (const unsigned char *)"<Null>")
			//     << "\n" << flush;
			return ptr;
		}

	int isMore(void)
		{
			//cerr << "isMore: start\n" << flush;
			return readLookAhead(1) ? 1 : 0;
		}

	unsigned long getOffset(void)	{ return offset; }
};

class Parameters {
public:
	bool usecerr;
	bool bigendian;
	bool littleendian;
	bool showhex;
	bool showdec;
	bool showoct;
	bool showieeefloat;
	bool showvaxfloat;
	bool showstrings;
	bool showstringcontents;
	bool showchars;
	bool showtype;
	bool showkey;
	bool shownegative;
	bool showaddress;
	bool show1;
	bool show2;
	bool show4;
	bool elidezeroes;
	bool elidespaces;
	bool elidereport;
	bool restricted;
	unsigned long restrictionvalue;
	unsigned long maxbytes;
	unsigned long jumpbytes;
	unsigned long stringlength;

	Parameters(void)
		{
			usecerr=false;
			bigendian=false;
			littleendian=false;
			showhex=false;
			showdec=false;
			showoct=false;
			showieeefloat=false;
			showvaxfloat=false;
			showstrings=false;
			showstringcontents=false;
			showchars=false;
			showtype=false;
			showkey=false;
			shownegative=false;
			showaddress=false;
			show1=false;
			show2=false;
			show4=false;
			elidezeroes=true;
			elidespaces=true;
			elidereport=true;
			restricted=0;
			maxbytes=0;
			jumpbytes=0;
			stringlength=40;
		}

	void dump(ostream& o) const
		{
			o << "usecerr = " << usecerr << endl;
			o << "bigendian = " << bigendian << endl;
			o << "littleendian = " << littleendian << endl;
			o << "showhex = " << showhex << endl;
			o << "showdec = " << showdec << endl;
			o << "showoct = " << showoct << endl;
			o << "showieeefloat = " << showieeefloat << endl;
			o << "showvaxfloat = " << showvaxfloat << endl;
			o << "showstrings = " << showstrings << endl;
			o << "showstringcontents = " << showstringcontents << endl;
			o << "showchars = " << showchars << endl;
			o << "showtype = " << showtype << endl;
			o << "showkey = " << showkey << endl;
			o << "shownegative = " << shownegative << endl;
			o << "showaddress = " << showaddress << endl;
			o << "show1 = " << show1 << endl;
			o << "show2 = " << show2 << endl;
			o << "show4 = " << show4 << endl;
			o << "elidezeroes = " << elidezeroes << endl;
			o << "elidespaces = " << elidespaces << endl;
			o << "elidereport = " << elidereport << endl;
			o << "restricted = " << restricted << endl;
			o << "restrictionvalue = " << restrictionvalue << endl;
			o << "maxbytes = " << maxbytes << endl;
			o << "jumpbytes = " << jumpbytes << endl;
			o << "stringlength = " << stringlength << endl;
		}
};

typedef unsigned char  Uint8;
typedef unsigned short Uint16;
typedef unsigned long  Uint32;
typedef signed char    Int8;
typedef short          Int16;
typedef long           Int32;

Int32
SignExtended(Uint32 value,unsigned short length)
{
	Uint32 vmask;
	Uint32 smask;
	switch (length) {
		case 1:	vmask=0x7f; smask=0x80; break;
		case 2:	vmask=0x7fff; smask=0x8000; break;
		case 4:
		default:
			vmask=0x7fffffff; smask=0x80000000; break;
	}
	return (value&smask)
		? (((Int32)(-1)^vmask)|value)
		: (Int32)(value&vmask);

}

void
putHexInteger(ostream& ostr,Parameters& params,unsigned short length,Uint32 value,char pad)
{
	(void)params;
	unsigned short width = (length*8-1)/4u+1;
	unsigned short padding = (4*8-1)/4+1 - width;
	while (pad && padding--) ostr << pad;
	ostr << hex << "0x" << setfill('0') << setw(width) << value;
}

void
putOctInteger(ostream& ostr,Parameters& params,unsigned short length,Uint32 value,char pad)
{
	(void)params;
	unsigned short width = (length*8-1)/3u+1;
	unsigned short padding = (4*8-1)/3+1 - width;
	while (pad && padding--) ostr << pad;
	ostr << oct << "0" << setfill('0') << setw(width) << value;
}

void
putDecInteger(ostream& ostr,Parameters& params,unsigned short length,Uint32 value,char pad)
{
	unsigned short width = (length*8-1)/3u+1;
	const unsigned short maxwidth = (4*8-1)/3+1+1;

	ostrstream os;
	if (params.shownegative) {
		Int32 i = SignExtended(value,length);
		os << dec << setfill(' ') << setw(width) << i << ends;
	}
	else {
		os << dec << setfill(' ') << setw(width) << value << ends;
	}
	char *s = os.str();
	int padding=maxwidth-strlen(s);
	while (pad && padding-- > 0) ostr << pad;
	ostr << s;
	if (s) delete[] s;
}

void
putBinaryInteger(ostream& ostr,Parameters& params,unsigned short length,Uint32 value,char pad)
{
	if (params.showhex) {
		putHexInteger(ostr,params,length,value,pad);
		if (params.showdec || params.showoct) ostr << " ";
	}
	if (params.showdec) {
		putDecInteger(ostr,params,length,value,pad);
		if (params.showoct) ostr << " ";
	}
	if (params.showoct) putOctInteger(ostr,params,length,value,pad);
}

void
putOffset(ostream& ostr,Parameters& params,Uint32 value)
{
	if (params.showaddress | params.showhex
	  | params.showdec     | params.showoct) {
		if (params.showkey) ostr << "offset=\"";
		else ostr << "[";
		if (params.showaddress) {
			putDecInteger(ostr,params,2,value,'\0');
		}
		else {
			putBinaryInteger(ostr,params,2,value,'\0');
		}
		if (params.showkey) ostr << "\"";
		else ostr << "]";
		ostr << " ";
	}
}

void
putOffsetRange(ostream& ostr,Parameters& params,Uint32 offset1,Uint32 offset2)
{
	if (params.showaddress | params.showhex
	  | params.showdec     | params.showoct) {
		putOffset(ostr,params,offset1);
		ostr << "to ";
		putOffset(ostr,params,offset2);
	}
}

void
putDescriptor(ostream& ostr,Parameters& params,
		char byteorder,unsigned long offset,char *desc)
{
	putOffset(ostr,params,offset);
	if (params.showtype) {
		if (params.showkey) ostr << "type=\"";
		if (byteorder)
			ostr << (byteorder == 'B' ? "B" : "L") << "_";
		ostr << desc;
		if (params.showkey) ostr << "\"";
		if (!byteorder)      ostr << "   ";
		ostr << " ";
	}
}

void
putBinaryDescriptor(ostream& ostr,Parameters& params,
		char byteorder,unsigned long offset,unsigned short length)
{
	ostrstream os;
	os << "int_" << dec << length*8 << ends;
	char *desc=os.str();
	putDescriptor(ostr,params,byteorder,offset,desc);
	if (desc) delete[] desc;
	ostr << "    ";
}

void
putBinaryIntegerWithDescription(ostream& ostr,Parameters& params,
		char byteorder,unsigned long offset,
		unsigned short length,Uint32 value)
{
	int show;

	if (params.restricted) {
		if (params.shownegative) {
			show = value < params.restrictionvalue
			   || (Uint32)(-SignExtended(value,length))
			    < params.restrictionvalue;
		}
		else {
			show = value < params.restrictionvalue;
		}
	}
	else
		show=1;

	if (show) {
		putBinaryDescriptor(ostr,params,byteorder,offset,length);
		putBinaryInteger(ostr,params,length,value,' ');
		ostr << "\n";
	}
}

void
putChar(ostream& ostr,Parameters& params,Uint8 value,char pad,char fill)
{
	(void)params;
	const unsigned short maxwidth = 3;

	ostrstream os;
	char c = isprint((char)value) ? (char)value : fill;
	os << "\'" << c << "\'" << ends;

	char *s = os.str();
	int padding=maxwidth-strlen(s);
	while (pad && padding-- > 0) ostr << pad;
	ostr << s;
	if (s) delete[] s;
}

void
putCharWithDescription(ostream& ostr,Parameters& params,
		unsigned long offset,Uint8 value,char fill)
{
	if (!params.restricted || isprint((char)value)) {
		putDescriptor(ostr,params,'\0',offset,"char");
		putChar(ostr,params,value,' ',fill);
		ostr << "\n";
	}
}


int
tryUint64(Buffer& buffer,char byteorder,Uint32& rvaluehigh,Uint32& rvaluelow)
{
	const unsigned char *start=buffer.readLookAhead(8);
	if (start) {
		int increment = (byteorder == 'B' ? 1 : -1);
		int count;
		const unsigned char *ptr = (byteorder == 'B' ? start : start+7);

		count=4;
		rvaluehigh=0;
		while (count--) {
			rvaluehigh<<=8;
			rvaluehigh|=*ptr;
			ptr+=increment;
		}
		count=4;
		rvaluelow=0;
		while (count--) {
			rvaluelow<<=8;
			rvaluelow|=*ptr;
			ptr+=increment;
		}
		return 1;
	}
	else
		return 0;
}

int
tryUint32(Buffer& buffer,char byteorder,Uint32& rvalue)
{
	const unsigned char *start=buffer.readLookAhead(4);
	if (start) {
		int increment = (byteorder == 'B' ? 1 : -1);
		int count = 4;
		const unsigned char *ptr = (byteorder == 'B' ? start : start+3);

		rvalue=0;
		while (count--) {
			rvalue<<=8;
			rvalue|=*ptr;
			ptr+=increment;
		}
		return 1;
	}
	else
		return 0;
}

int
tryUint16(Buffer& buffer,char byteorder,Uint16& rvalue)
{
	const unsigned char *start=buffer.readLookAhead(2);
	if (start) {
		int increment = (byteorder == 'B' ? 1 : -1);
		int count = 2;
		const unsigned char *ptr = (byteorder == 'B' ? start : start+1);

		rvalue=0;
		while (count--) {
			rvalue<<=8;
			rvalue|=*ptr;
			ptr+=increment;
		}
		return 1;
	}
	else
		return 0;
}

int
tryUint8(Buffer& buffer,Uint8& rvalue)
{
	const unsigned char *start=buffer.readLookAhead(2);
	if (start) {
		rvalue=*start;
		return 1;
	}
	else
		return 0;
}

void
checkbinary(Buffer& buffer,ostream& ostr,char byteorder,Parameters& params)
{
	unsigned long offset=buffer.getOffset();

	if (params.show4 && offset%4 == 0) {
		Uint32 uvalue;
		if (tryUint32(buffer,byteorder,uvalue)) {
			putBinaryIntegerWithDescription(
				ostr,params,byteorder,offset,4,uvalue);
		}
	}

	if (params.show2 && offset%2 == 0) {
		Uint16 uvalue;
		if (tryUint16(buffer,byteorder,uvalue)) {
			putBinaryIntegerWithDescription(
				ostr,params,byteorder,offset,2,uvalue);
		}
	}
}

void
checkbinarybyte(Buffer& buffer,ostream& ostr,Parameters& params)
{
	unsigned long offset=buffer.getOffset();

	{
		Uint8 uvalue;
		if (tryUint8(buffer,uvalue)) {
			putBinaryIntegerWithDescription(
				ostr,params,'\0',offset,1,uvalue);
		}
	}
}

void
checkchars(Buffer& buffer,ostream& ostr,Parameters& params,char fill)
{
	unsigned long offset=buffer.getOffset();

	{
		Uint8 uvalue;
		if (tryUint8(buffer,uvalue)) {
			putCharWithDescription(
				ostr,params,offset,uvalue,fill);
		}
	}
}

class IeeeFloat32 {
	enum IeeeFloat32Class { IeeeFloat32Zero,
		IeeeFloat32Normal, IeeeFloat32NotNormal,
		IeeeFloat32NotANumber,
		IeeeFloat32PlusInfinity, IeeeFloat32MinusInfinity } cl;
	double value;
public:
	IeeeFloat32(void) { cl=IeeeFloat32Zero; value=0; }

	IeeeFloat32(Uint32 binary) {
		Int16 sign	=(Int16)((binary&0x80000000)>>31);
		Int16 exponent	=(Int16)((binary&0x7f800000)>>23);
		Uint32 mantissa	= binary&0x007fffff;

		if (exponent == 0) {
			if (mantissa == 0) {
				cl=IeeeFloat32Zero;
				value=0;
			}
			else {
				value=((double)mantissa/
					(1<<23))*pow(2.0,(long)-126);
				value = (sign == 0) ? value : -value;
				cl=IeeeFloat32NotNormal;
			}
		}
		else if (exponent == 255) {
			if (mantissa) {
				cl=IeeeFloat32NotANumber;
			}
			else {
				cl=sign ? IeeeFloat32MinusInfinity
					   : IeeeFloat32PlusInfinity;
			}
		}
		else {
			value=(1.0+(double)mantissa/
				(1<<23))*pow(2.0,(long)(exponent)-127);
			value = (sign == 0) ? value : -value;
			cl=IeeeFloat32Normal;
		}
	}

	double getValue(void) { return value; }

	int isNumber(void)
		{ return isZero() | isNormalized() | isNotNormalized(); }
	int isInfinity(void)
		{ return isPlusInfinity() | isMinusInfinity(); }

	int isZero(void)
		{ return cl==IeeeFloat32Zero; }
	int isNormalized(void)
		{ return cl==IeeeFloat32Normal; }
	int isNotNormalized(void)
		{ return cl==IeeeFloat32NotNormal; }
	int isNotANumber(void)
		{ return cl==IeeeFloat32NotANumber; }
	int isPlusInfinity(void)
		{ return cl==IeeeFloat32PlusInfinity; }
	int isMinusInfinity(void)
		{ return cl==IeeeFloat32MinusInfinity; }

	const char *describe(void) {
		const char *desc;
		switch (cl) {
			case IeeeFloat32Zero:
				desc="Zero"; break;
			case IeeeFloat32Normal:
				desc="Normalized"; break;
			case IeeeFloat32NotNormal:
				desc="Not Normalized"; break;
			case IeeeFloat32NotANumber:
				desc="Not A Number"; break;
			case IeeeFloat32PlusInfinity:
				desc="+Infinity"; break;
			case IeeeFloat32MinusInfinity:
				desc="-Infinity"; break;
			default:
				desc="Bad"; break;
		}
		return desc;
	}
};

class IeeeFloat64 {
	enum IeeeFloat64Class { IeeeFloat64Zero,
		IeeeFloat64Normal, IeeeFloat64NotNormal,
		IeeeFloat64NotANumber,
		IeeeFloat64PlusInfinity, IeeeFloat64MinusInfinity } cl;
	double value;
public:
	IeeeFloat64(void) { cl=IeeeFloat64Zero; value=0; }

	IeeeFloat64(Uint32 high,Uint32 low) {
		Int16 sign	=(Int16)((high&0x80000000)>>31);
		Int16 exponent	=(Int16)((high&0x7ff00000)>>20);
		Uint32 mantissahigh = high&0x000fffff;
		Uint32 mantissalow  = low;
		double mantissavalue = (double)mantissahigh * pow(2.0,(long)32)
				     + (double)mantissalow;

		if (exponent == 0) {
			if (mantissahigh == 0 && mantissalow == 0) {
				cl=IeeeFloat64Zero;
				value=0;
			}
			else {
				value=(mantissavalue/pow(2.0,(long)52))
					*pow(2.0,(long)-1022);
				value = (sign == 0) ? value : -value;
				cl=IeeeFloat64NotNormal;
			}
		}
		else if (exponent == 2047) {
			if (mantissahigh || mantissalow) {
				cl=IeeeFloat64NotANumber;
			}
			else {
				cl=sign ? IeeeFloat64MinusInfinity
					   : IeeeFloat64PlusInfinity;
			}
		}
		else {
			value=(1.0+mantissavalue/pow(2.0,(long)52))
				*pow(2.0,(long)(exponent)-1023);
			value = (sign == 0) ? value : -value;
			cl=IeeeFloat64Normal;
		}
	}

	double getValue(void) { return value; }

	int isNumber(void)
		{ return isZero() | isNormalized() | isNotNormalized(); }
	int isInfinity(void)
		{ return isPlusInfinity() | isMinusInfinity(); }

	int isZero(void)
		{ return cl==IeeeFloat64Zero; }
	int isNormalized(void)
		{ return cl==IeeeFloat64Normal; }
	int isNotNormalized(void)
		{ return cl==IeeeFloat64NotNormal; }
	int isNotANumber(void)
		{ return cl==IeeeFloat64NotANumber; }
	int isPlusInfinity(void)
		{ return cl==IeeeFloat64PlusInfinity; }
	int isMinusInfinity(void)
		{ return cl==IeeeFloat64MinusInfinity; }

	const char *describe(void) {
		const char *desc;
		switch (cl) {
			case IeeeFloat64Zero:
				desc="Zero"; break;
			case IeeeFloat64Normal:
				desc="Normalized"; break;
			case IeeeFloat64NotNormal:
				desc="Not Normalized"; break;
			case IeeeFloat64NotANumber:
				desc="Not A Number"; break;
			case IeeeFloat64PlusInfinity:
				desc="+Infinity"; break;
			case IeeeFloat64MinusInfinity:
				desc="-Infinity"; break;
			default:
				desc="Bad"; break;
		}
		return desc;
	}
};

class VaxFloat_F {
	enum VaxFloat_FClass { VaxFloat_F_Zero,VaxFloat_F_Number } cl;
	double value;
public:
	VaxFloat_F(void) { cl=VaxFloat_F_Zero; value=0; }

	VaxFloat_F(Uint32 binary) {
		Uint32 mantissalow	= (binary&0xffff0000)>>16;
		Int16 sign		=(Int16)((binary&0x00008000)>>15);
		Int16 exponent		=(Int16)((binary&0x00007f80)>>7);
		Uint32 mantissahigh	= binary&0x0000007f;
		double mantissavalue    = (double)mantissahigh *
			pow(2.0,(long)16) + (double)mantissalow;
		double mantissafraction = mantissavalue/pow(2.0,(long)23);

		if (exponent == 0&& mantissalow == 0 && mantissahigh == 0) {
			cl=VaxFloat_F_Zero;
			value=0;
		}
		else {
		// Always normalized ... the 1.0 is the "hidden" bit in VaxSpeak
		// the exponent is excess 128, eg 10000010 is +2
		// but refers to a binary point to the LEFT of the hidden bit
		// hence the -129 rather than -128

			value=(1.0+mantissafraction)
				*pow(2.0,(long)(exponent)-129);
			value = (sign == 0) ? value : -value;
			cl=VaxFloat_F_Number;
		}
	}

	double getValue(void) { return value; }

	int isNumber(void)
		{ return cl==VaxFloat_F_Number; }
	int isZero(void)
		{ return cl==VaxFloat_F_Zero; }

	const char *describe(void) {
		const char *desc;
		switch (cl) {
			case VaxFloat_F_Zero:
				desc="Zero"; break;
			case VaxFloat_F_Number:
				desc="Number"; break;
			default:
				desc="Bad"; break;
		}
		return desc;
	}
};

void
putIeeeFloat32(ostream& ostr,Parameters& params,IeeeFloat32 value,char pad)
{
	(void)params;
	unsigned short width = 8;
	const unsigned short maxwidth = 14;

	ostrstream os;
	if (value.isNumber()) {
		os << dec << setfill(' ') << setw(width)
		   << value.getValue() << ends;
	}
	else {
		os << value.describe() << ends;
	}
	char *s = os.str();
	int padding=maxwidth-strlen(s);
	while (pad && padding-- > 0) ostr << pad;
	ostr << s;
	if (s) delete[] s;
}

void
putIeeeFloat64(ostream& ostr,Parameters& params,IeeeFloat64 value,char pad)
{
	(void)params;
	unsigned short width = 8;
	const unsigned short maxwidth = 14;

	ostrstream os;
	if (value.isNumber()) {
		os << dec << setfill(' ') << setw(width)
		   << value.getValue() << ends;
	}
	else {
		os << value.describe() << ends;
	}
	char *s = os.str();
	int padding=maxwidth-strlen(s);
	while (pad && padding-- > 0) ostr << pad;
	ostr << s;
	if (s) delete[] s;
}

void
putVaxFloat_F(ostream& ostr,Parameters& params,VaxFloat_F value,char pad)
{
	(void)params;
	unsigned short width = 8;
	const unsigned short maxwidth = 14;

	ostrstream os;
	if (value.isNumber()) {
		os << dec << setfill(' ') << setw(width)
		   << value.getValue() << ends;
	}
	else {
		os << value.describe() << ends;
	}
	char *s = os.str();
	int padding=maxwidth-strlen(s);
	while (pad && padding-- > 0) ostr << pad;
	ostr << s;
	if (s) delete[] s;
}

void
putIeeeFloat32WithDescription(ostream& ostr,Parameters& params,
		char byteorder,unsigned long offset,IeeeFloat32 value)
{
	int show;

	if (params.restricted) {
		if (value.isZero()) {
			show=1;
		}
		else if (value.isNumber()) {
			double avalue = fabs(value.getValue());
			show = avalue < (double)params.restrictionvalue
			    && avalue > ReasonableMinimumFloatToShow;
		}
		else
			show=0;
	}
	else
		show=1;

	if (show) {
		putDescriptor(ostr,params,byteorder,offset,"f_ieee32");
		putIeeeFloat32(ostr,params,value,' ');
		ostr << "\n";
	}
}

void
putIeeeFloat64WithDescription(ostream& ostr,Parameters& params,
		char byteorder,unsigned long offset,IeeeFloat64 value)
{
	int show;

	if (params.restricted) {
		if (value.isZero()) {
			show=1;
		}
		else if (value.isNumber()) {
			double avalue = fabs(value.getValue());
			show = avalue < (double)params.restrictionvalue
			    && avalue > ReasonableMinimumFloatToShow;
		}
		else
			show=0;
	}
	else
		show=1;

	if (show) {
		putDescriptor(ostr,params,byteorder,offset,"f_ieee64");
		putIeeeFloat64(ostr,params,value,' ');
		ostr << "\n";
	}
}

void
putVaxFloat_FWithDescription(ostream& ostr,Parameters& params,
		char byteorder,unsigned long offset,VaxFloat_F value)
{
	int show;

	if (params.restricted) {
		if (value.isZero()) {
			show=1;
		}
		else if (value.isNumber()) {
			double avalue = fabs(value.getValue());
			show = avalue < (double)params.restrictionvalue
			    && avalue > ReasonableMinimumFloatToShow;
		}
		else
			show=0;
	}
	else
		show=1;

	if (show) {
		putDescriptor(ostr,params,byteorder,offset,"f_vax_f ");
		putVaxFloat_F(ostr,params,value,' ');
		ostr << "\n";
	}
}

int
tryIeeeFloat32(Buffer& buffer,char byteorder,IeeeFloat32& rvalue)
{
	Uint32 binary;
	if (tryUint32(buffer,byteorder,binary)) {
		IeeeFloat32 value(binary);
		rvalue=value;
		return 1;
	}
	else
		return 0;
}

int
tryIeeeFloat64(Buffer& buffer,char byteorder,IeeeFloat64& rvalue)
{
	Uint32 high,low;
	if (tryUint64(buffer,byteorder,high,low)) {
		IeeeFloat64 value(high,low);
		rvalue=value;
		return 1;
	}
	else
		return 0;
}

int
tryVaxFloat_F(Buffer& buffer,char byteorder,VaxFloat_F& rvalue)
{
	Uint32 binary;
	if (tryUint32(buffer,byteorder,binary)) {
		VaxFloat_F value(binary);
		rvalue=value;
		return 1;
	}
	else
		return 0;
}

void
checkieeefloat(Buffer& buffer,ostream& ostr,char byteorder,Parameters& params)
{
	unsigned long offset=buffer.getOffset();

	if (offset%8 == 0) {
		IeeeFloat64 value;
		if (tryIeeeFloat64(buffer,byteorder,value)) {
			putIeeeFloat64WithDescription(
				ostr,params,byteorder,offset,value);
		}
	}
	if (offset%4 == 0) {
		IeeeFloat32 value;
		if (tryIeeeFloat32(buffer,byteorder,value)) {
			putIeeeFloat32WithDescription(
				ostr,params,byteorder,offset,value);
		}
	}
}

void
checkvaxfloat(Buffer& buffer,ostream& ostr,char byteorder,Parameters& params)
{
	unsigned long offset=buffer.getOffset();

	if (offset%4 == 0) {
		VaxFloat_F value;
		if (tryVaxFloat_F(buffer,byteorder,value)) {
			putVaxFloat_FWithDescription(
				ostr,params,byteorder,offset,value);
		}
	}
}

int
checkstrings(Buffer& buffer,ostream& ostr,Parameters& params,
	unsigned minstrlen,unsigned maxstrlen)
{
	static unsigned lengthleft=0;	// remember state between calls

	if (lengthleft) --lengthleft;

	if (!lengthleft) {
		unsigned test;
		for (test=0; test < maxstrlen; ++test) {
			const unsigned char *ptr=buffer.readLookAhead(test+1);
			if (!ptr || !isprint(ptr[test])) break;
		}
		if (test >= minstrlen) {
			lengthleft=test;

			unsigned long offset=buffer.getOffset();
			putOffsetRange(ostr,params,offset,offset+test-1);
			if (params.showtype) {
				if (params.showkey) ostr << "type=\"";
				ostr << "string";
				if (params.showkey) ostr << "\"";
				ostr << " ";
			}

			if (params.showkey) ostr << "length=\"";
			else ostr << "(";
			putDecInteger(ostr,params,2,test,'\0');
			if (params.showkey) ostr << "\"";
			else ostr << ")";
			ostr << " ";

			const unsigned char *ptr=buffer.readLookAhead(test);
			if (ptr) {
				char *s = new char [test+1];
				strncpy(s,(const char *)ptr,test); s[test]=0;
				ostr << "\"" << s << "\"\n";
			}
			else {
				cerr << "checkstrings: Assertion failed"
				     << "ptr should not be zero\n" << flush;
				exit(1);
			}
		}
		else {
			lengthleft=0;
		}
	}
	return lengthleft;
}

int
elide(Buffer& buffer,ostream& ostr,
	Parameters& params,unsigned char value,char *desc,
	unsigned minrunlength)
{
	const unsigned char *ptr=buffer.readLookAhead(minrunlength);

	if (ptr) {
		while (minrunlength--)
			if (ptr[minrunlength] != value) return 0;
		unsigned long count=0;
		unsigned long startposn=buffer.getOffset();
		while ((ptr=buffer.readLookAhead(1)) && *ptr == value) {
			buffer.read(1);
			++count;
		}
		if (params.elidereport) {
			putOffsetRange(ostr,params,
				startposn,buffer.getOffset()-1);

			if (params.showkey) ostr << "type=\"";
			ostr << desc;
			if (params.showkey) ostr << "\"";
			ostr << " ";

			if (params.showkey) ostr << "length=\"";
			else ostr << "(";
			putDecInteger(ostr,params,2,count,'\0');
			if (params.showkey) ostr << "\"";
			else ostr << ")";
			ostr << "\n";
		}
	}
	else
		return 0;
}

int
main(int argc,char **argv)
{
	GetNamedOptions 	options(argc,argv);
	InputOptions		input_options(options);

	Parameters params;	// sets up some default values also

	bool foundoption=false;

	// should be more selective about when foundoption gets set

	if (options.get("all") || options.get("a")) {
		foundoption=true;
		params.bigendian=true;
		params.littleendian=true;
		params.showhex=true;
		params.showdec=true;
		params.showoct=true;
		params.showieeefloat=true;
		params.showvaxfloat=true;
		params.showstrings=true;
		params.showchars=true;
		params.show1=true;
		params.show2=true;
		params.show4=true;
		params.showtype=true;
	}

	foundoption=(params.bigendian		=options.get("big") 		|| options.get("b")) || foundoption;
	foundoption=(params.littleendian	=options.get("little") 		|| options.get("l")) || foundoption;
	foundoption=(params.showchars		=options.get("chars") 		|| options.get("c")) || foundoption;
	foundoption=(params.showdec		=options.get("dec") 		|| options.get("d")) || foundoption;
	foundoption=(params.showoct		=options.get("oct") 		|| options.get("o")) || foundoption;
	foundoption=(params.showieeefloat	=options.get("float") 		|| options.get("f")) || foundoption;
	foundoption=(params.showvaxfloat	=options.get("vax") 		|| options.get("v")) || foundoption;
	foundoption=(params.showhex		=options.get("hex") 		|| options.get("h") || options.get("x")) || foundoption;
	foundoption=(params.showkey		=options.get("key") 		|| options.get("k")) || foundoption;
	foundoption=(params.shownegative	=options.get("neg") 		|| options.get("n")) || foundoption;
	foundoption=(params.showaddress		=options.get("position") 	|| options.get("p")) || foundoption;
	foundoption=(params.showstrings		=options.get("strings") 	|| options.get("s")) || foundoption;
	foundoption=(params.showstringcontents	=options.get("stringcontent") 	|| options.get("w")) || foundoption;
	foundoption=(params.showtype		=options.get("type") 		|| options.get("t")) || foundoption;
	foundoption=(params.show1		=options.get("byte") 		|| options.get("1")) || foundoption;
	foundoption=(params.show2		=options.get("short") 		|| options.get("2")) || foundoption;
	foundoption=(params.show4		=options.get("long") 		|| options.get("4")) || foundoption;

	foundoption=(params.usecerr		=options.get("cerr") 		|| options.get("e")) || foundoption;

	if (options.get("noreport") || options.get("n")) {
		foundoption=true;
		params.elidereport=false;
	}

	if (options.get("showzeroes") || options.get("z")) {
		foundoption=true;
		params.elidezeroes=false;
		params.elidespaces=false;
	}

	foundoption=(options.get("jump",params.jumpbytes) || options.get("j",params.jumpbytes)) || foundoption;
	foundoption=(options.get("max",params.maxbytes) || options.get("m",params.maxbytes)) || foundoption;
	foundoption=(params.restricted=options.get("restrict",params.restrictionvalue) || options.get("r",params.restrictionvalue)) || foundoption;
	foundoption=(options.get("stringlength",params.stringlength) || options.get("y",params.stringlength)) || foundoption;

	if (!foundoption) {		// default behaviour
		params.showdec=1;
		params.showchars=1;
		params.show1=1;
		params.showaddress=1;
	}

	bool bad=false;

	if ((params.show2 || params.show4 || params.showieeefloat) && !(params.littleendian || params.bigendian)) {
		cerr << "Must specify byte order -l or -b for -2,-4,-f" << endl;
		bad=true;
	}

	if ( (params.show1 || params.show2 || params.show4) && !(params.showdec || params.showoct || params.showhex)
	 || !(params.show1 || params.show2 || params.show4) &&  (params.showdec || params.showoct || params.showhex)) {
		cerr << "Must specify -o,-d or -h for -1,-2 or -4" << endl;
		bad=true;
	}

	params.dump(cerr);

	input_options.done();
	options.done();

	InputOpenerFromOptions input_opener(options,input_options.filename,cin);

	cerr << input_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();

	if (!input_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< input_options.usage()
			<< " [-a -b -c -d -e -f -h -j skipbytes"
			<< " -k -l -m maxbytes -n -o -p -r restrictvalue"
			<< " -s -t -v -w -x -y -z -1 -2 -4]"
			<< " [" << MMsgDC(InputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		exit(1);
	}

	istream instream(input_opener);

	ostream *outstream=params.usecerr ? &cerr : &cout;

	// *************** Actually do something ***************

	Buffer buffer(&instream,params.jumpbytes,params.maxbytes);

	while (buffer.isMore()) {
		if (params.elidezeroes
		 && elide(buffer,*outstream,params,0,"zeroes",params.stringlength))
			continue;
		if (params.elidespaces
		 && elide(buffer,*outstream,params,' ',"spaces",params.stringlength))
			continue;

		if (!params.showstrings
		 || !checkstrings(buffer,*outstream,params,4,params.stringlength)
		 || params.showstringcontents) {
			if (params.bigendian) {
				checkbinary(buffer,*outstream,'B',params);
				if (params.showieeefloat)
					checkieeefloat(buffer,
						*outstream,'B',params);
				if (params.showvaxfloat)
					checkvaxfloat(buffer,
						*outstream,'B',params);
			}
			if (params.littleendian) {
				checkbinary(buffer,*outstream,'L',params);
				if (params.showieeefloat)
					checkieeefloat(buffer,
						*outstream,'L',params);
				if (params.showvaxfloat)
					checkvaxfloat(buffer,
						*outstream,'L',params);
			}
			if (params.show1)
				checkbinarybyte(buffer,*outstream,params);
			if (params.showchars)
				checkchars(buffer,*outstream,params,'.');
		}
		buffer.read(1);
	}

	return 0;
}
