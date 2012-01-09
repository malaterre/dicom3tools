#ifndef __Header_attrtypf__
#define __Header_attrtypf__

// all the following is copied from dconvert/fltype.h

#include <float.h>

#ifdef POWINTEGEREXPONENTTYPE
#define	powi(m,e)	pow(m,(POWINTEGEREXPONENTTYPE)(e))
#else
#define	powi(m,e)	pow(m,(long)(e))
#endif

#ifdef __SC__
#define infinity()    HUGE_VAL
#define quiet_nan(x)  0
#endif /* __SC__ */

#ifdef __MWERKS__
#define infinity()    __inf()
#define quiet_nan(x)  0
#endif /* __MWERKS__ */

#ifdef USEDUMBINFINITYANDNAN
#if USEDUMBINFINITYANDNAN
#define infinity()    FLT_MAX
#define quiet_nan(x)  0
#endif
#endif

/* ********************* Floating Point Binary Attributes ********************* */

template<int Size>
class FloatBinaryAttribute : public Attribute {
private:
	ValueList<unsigned char *> values;

	BinaryOutputStream& writeValues(BinaryOutputStream& stream)
		{
			// Stored internally as big endian

			ValueListIterator<unsigned char *> i(values);
			while (!i) {
				const unsigned char *p=i();
				int direction;
				if (stream.isBigEndian()) {
					direction=1;
				}
				else if (stream.isLittleEndian()) {
					p=p+Size-1;
					direction=-1;
				}
				else {
					Assert(0);
				}
				short j=0;
				while (j < Size) {
					stream.write8(*p);
//cerr << "FloatBinaryAttribute:writeValues - writing " << dec << j << "th byte " << hex << unsigned(*p) << dec << endl;
					p+=direction;
					++j;
				}
				++i;
			}
			return stream;

		}

	unsigned char* newSize4Value(Float32 value) {
		Uint32 sign=0;
		Int32 exponent;
		Uint32 mantissa;

		if (value < 0) {
			sign=0x80000000;
			value=-value;
		}

		if (value == 0) {
			exponent=0;
			mantissa=0;
		}
		else {
			exponent=(int)ourlog2(value);
			if (value < 1) --exponent;
			if (exponent <= -126) {
				mantissa=(Uint32)((value/pow(2,-126))*(1<<23));
				exponent=0;
			}
			else {
				if (exponent > 127) exponent=127;
				mantissa=(Uint32)(((value/pow(2,exponent))-1)*(1<<23));
				exponent=((exponent+127)<<23)&0x7f800000;
			}
		}

		Uint32 binary=sign+exponent+mantissa;

		unsigned char *barray = new unsigned char[4];
		barray[0]=(unsigned char)(binary>>24);
		barray[1]=(unsigned char)(binary>>16);
		barray[2]=(unsigned char)(binary>>8);
		barray[3]=(unsigned char)(binary);

		return barray;
	}

	unsigned char* newSize8Value(Float64 value) {
//cerr << "FloatBinaryAttribute:newSize8Value: want " << dec << value << endl;
                Uint32 sign=0;
		Int32 exponent;
		Uint32 mantissahigh;
		Uint32 mantissalow;

		if (value < 0) {
			sign=0x80000000;
			value=-value;
		}

		if (value == 0) {
			exponent=0;
			mantissahigh=0;
			mantissalow=0;
		}
		else {
			exponent=(int)ourlog2(value);
			if (value < 1) --exponent;
			if (exponent <= -1022) {
                                double mantissavalue = value/pow(2,-1022);
				mantissahigh=(Uint32)(mantissavalue*powi(2.0,20));
                                mantissalow =(Uint32)(mantissavalue*powi(2.0,52));
				exponent=0;
			}
			else {
				if (exponent > 1023) exponent=1023;
                                double mantissavalue = value/pow(2,exponent) - 1.0;
				mantissahigh=(Uint32)(mantissavalue*powi(2.0,20));
                                mantissalow =(Uint32)(mantissavalue*powi(2.0,52));
				exponent=((exponent+1023)<<20)&0x7ff00000;
			}
		}

//cerr << "FloatBinaryAttribute:newSize8Value: sign 0x" << hex << sign << dec << endl;
//cerr << "FloatBinaryAttribute:newSize8Value: exponent 0x" << hex << exponent << dec << endl;
//cerr << "FloatBinaryAttribute:newSize8Value: mantissahigh 0x" << hex << mantissahigh << dec << endl;
//cerr << "FloatBinaryAttribute:newSize8Value: mantissalow 0x" << hex << mantissalow << dec << endl;

		Uint32 binaryhigh=sign+exponent+mantissahigh;

		unsigned char *barray = new unsigned char[8];
		barray[0]=(unsigned char)(binaryhigh>>24);
		barray[1]=(unsigned char)(binaryhigh>>16);
		barray[2]=(unsigned char)(binaryhigh>>8);
		barray[3]=(unsigned char)(binaryhigh);
		barray[4]=(unsigned char)(mantissalow>>24);
		barray[5]=(unsigned char)(mantissalow>>16);
		barray[6]=(unsigned char)(mantissalow>>8);
		barray[7]=(unsigned char)(mantissalow);
		return barray;
	}

	// these two routines are derived from dconvert/fltype.h

	bool	getSize4Value(unsigned index,Float32& vp) const {
		Assert(Size == 4);
		unsigned char *barray;
		bool success=values.getValue(index,barray);
		if (success) {
			Uint32 binary = (((((barray[0]<<8)|barray[1])<<8)|barray[2])<<8)|barray[3];
			double value;
			Int16 sign	=(Int16)((binary&0x80000000)>>31);
			Int16 exponent	=(Int16)((binary&0x7f800000)>>23);
			Uint32 mantissa	= binary&0x007fffff;

			if (exponent == 0) {
				if (mantissa == 0)
					value=0;
				else {
					value=((double)mantissa/
						(1<<23))*powi(2.0,-126);
					value = (sign == 0) ? value : -value;
				}
			}
			else if (exponent == 255) {
				if (mantissa)
					value=quiet_nan(0);
				else
					value=infinity();
			}
			else {
				value=(1.0+(double)mantissa/
					(1<<23))*powi(2.0,exponent-127);
				value = (sign == 0) ? value : -value;
			}
			vp=value;
		}
		else {
			vp=0;
		}
		return success;
	}

	bool	getSize8Value(unsigned index,Float64& vp) const {
		Assert(Size == 8);
		unsigned char *barray;
		bool success=values.getValue(index,barray);
		if (success) {

			Uint32 high = (((((barray[0]<<8)|barray[1])<<8)|barray[2])<<8)|barray[3];
			Uint32 low  = (((((barray[4]<<8)|barray[5])<<8)|barray[6])<<8)|barray[7];
			double value;
			Int16 sign	=(Int16)((high&0x80000000)>>31);
			Int16 exponent	=(Int16)((high&0x7ff00000)>>20);
			Uint32 mantissahigh = high&0x000fffff;
			Uint32 mantissalow  = low &0xffffffff;
			double mantissavalue = (double)mantissahigh * powi(2.0,32)
					     + (double)mantissalow;

			if (exponent == 0) {
				if (mantissahigh == 0 && mantissalow == 0)
					value=0;
				else {
					value=(mantissavalue/powi(2.0,52))
						*powi(2.0,-1022);
					value = (sign == 0) ? value : -value;
				}
			}
			else if (exponent == 255) {			// ? if this should be 1023
				if (mantissahigh || mantissalow)
					value=quiet_nan(0);
				else
					value=infinity();
			}
			else {
				value=(1.0+mantissavalue/powi(2.0,52))
					*powi(2.0,exponent-1023);
				value = (sign == 0) ? value : -value;
			}
			vp=value;
		}
		else {
			vp=0;
		}
		return success;
	}


public:
	FloatBinaryAttribute(Tag t) : Attribute(t)
		{
			Assert(Size == 4 || Size == 8);
		}

	virtual ~FloatBinaryAttribute() {}

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return writeValues(stream);
		}

	TextOutputStream& writeData(TextOutputStream& stream)
		{
			int n = getVM();
			int i;
			for (i=0; i<n; ++i) {
				if (i > 0) stream <<",";
				Float64 value;
				if (getValue(i,value)) {
					stream << value;
				}
				else {
					stream << "XXXX";
				}
			}
			return stream;
		}

	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false)
		{
			Attribute::writeBase(stream,dict,verbose,showUsedAndIE);
			stream << "{";
			writeData(stream);
			stream << "} ";
			return stream;
		}

	BinaryOutputStream& write(BinaryOutputStream& stream)
		{
			Attribute::writeBase(stream);
			writeValues(stream);
			return stream;
		}

	DicomOutputStream& write(DicomOutputStream& stream)
		{
			Attribute::writeBase(stream);
			writeValues(stream);
			return stream;
		}

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length)
		{
			Assert(length%Size == 0);

			while (length && stream.good()) {
				// Stored internally as big endian

				unsigned char *p=new unsigned char[Size];
				values.addValue(p);	// takes responsibility for dealloc
							// note we are adding the value before filling it !
//cerr << "FloatBinaryAttribute:read - allocated p= " << hex << (unsigned long)p << dec << endl;
				int direction;
				if (stream.isBigEndian()) {
					direction=1;
				}
				else if (stream.isLittleEndian()) {
					p=p+Size-1;
					direction=-1;
				}
				else {
					Assert(0);
				}
				short j=0;
				while (j < Size) {
					*p=stream.read8();
//cerr << "FloatBinaryAttribute:read - using p= " << hex << (unsigned long)p << dec << endl;
//cerr << "FloatBinaryAttribute:read - read " << dec << j << "th byte " << hex << unsigned(*p) << dec << endl;
					p+=direction;
					++j;
				}
				length-=Size;
			}
			return stream;
		}

	bool	isNumeric(void) const		{ return true; }

	Uint16	getVM(void) const	{ return values.getVM(); }
	Uint32	getVL(void) const	{ return Size*getVM(); }
	Uint16	getValueSize(void) const	{ return Size; }

	bool	getValue(unsigned index,Uint16& vp) const
		{
			Float64 value;
			bool success=getValue(index,value);
			if (success) vp=(Int32)value;
			return success;
		}

	bool	getValue(unsigned index,Uint32& vp) const
		{
			Float64 value;
			bool success=getValue(index,value);
			if (success) vp=(Int32)value;
			return success;
		}

	bool	getValue(unsigned index,Int16& vp) const
		{
			Float64 value;
			bool success=getValue(index,value);
			if (success) vp=(Int32)value;
			return success;
		}

	bool	getValue(unsigned index,Int32& vp) const
		{
			Float64 value;
			bool success=getValue(index,value);
			if (success) vp=(Int32)value;
			return success;
		}

	bool	getValue(unsigned index,Float32& vp) const
		{
			if (Size == 4) {
				return getSize4Value(index,vp);
			}
			else {
				Float64 value;
				bool success=getSize8Value(index,value);
				vp=(Float32)value;
				return success;
			}
		}

	bool	getValue(unsigned index,Float64& vp) const
		{
			if (Size == 8) {
				return getSize8Value(index,vp);
			}
			else {
				Float32 value;
				bool success=getSize4Value(index,value);
				vp=(Float64)value;
				return success;
			}
		}
	bool	getValue(unsigned index,Tag& vp) const		{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }

	void	addValue(Uint16 value)
		{
			values.addValue(Size == 4 ? newSize4Value((Float32)value) : newSize8Value((Float64)value));
		}

	void	addValue(Uint32 value)
		{
			values.addValue(Size == 4 ? newSize4Value((Float32)value) : newSize8Value((Float64)value));
		}

	void	addValue(Int16 value)
		{
			values.addValue(Size == 4 ? newSize4Value((Float32)value) : newSize8Value((Float64)value));
		}

	void	addValue(Int32 value)
		{
			values.addValue(Size == 4 ? newSize4Value((Float32)value) : newSize8Value((Float64)value));
		}

	void	addValue(Float32 value)
		{
			values.addValue(Size == 4 ? newSize4Value(value) : newSize8Value((Float64)value));
		}

	void	addValue(Float64 value)
		{
			values.addValue(Size == 4 ? newSize4Value((Float32)value) : newSize8Value(value));
		}

	void	addValue(Tag value)				{ Attribute::addValue(value); }
	
	void	addValue(const char *str)			{ addValue(atof(str)); }

	void	addValues(unsigned number,Uint16 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Uint32 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Int16 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Int32 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Float32 *vptr)	{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Float64 *vptr)	{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Tag *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,const char **vptr)	{ Attribute::addValues(number,vptr); }

	void	addValues(const char *vptr)
		{
			const char *p;
			unsigned number;
			for (number=1,p=vptr; *p; ++p)
				if (*p == '\\') ++number;
			Float64 *tptr = new double[number];
			unsigned i;
			for (i=0,p=vptr; i<number; ++i) {
				tptr[i]=(Float64)atof(p);
				if ((p=strchr(p,'\\')) != 0) ++p;
			}
			addValues(number,tptr);
			if (tptr) delete[] tptr;
		}
};

#endif /* __Header_attrtypf__ */


