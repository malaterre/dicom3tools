#ifndef __Header_ToShift__
#define __Header_ToShift__

// Create a class to insert bitsstored length words starting at highbit
// into 16 bit words (whose byte order is already that of the host)

// handles shifting so lsb is in bit 0
// doesn't yet handle sign extension

// Supporting classes ....

class ShiftUint16Base {
protected:
	SourceBase<Uint16> *source;
public:
	ShiftUint16Base(SourceBase<Uint16>& i)
		{
			source=&i;
		}
	virtual size_t read(void)			= 0;
	virtual const Uint16 *getBuffer(void) 		= 0; 
	virtual size_t getBufferCount(void) const	= 0;
	virtual int good(void) const	{ return source && source->good(); }
};

class ShiftUint16General : public ShiftUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
	Uint16 mask;
	unsigned short leftshift;
public:
	ShiftUint16General(SourceBase<Uint16>& i,unsigned short bitsstored,unsigned short highbit)
			:ShiftUint16Base(i)
		{
			buffer=0;
			bufsize=0;
			length=0;

			leftshift=highbit+1-bitsstored;
			mask=(Uint16)(((Uint32)1<<bitsstored)-1)<<leftshift;

			// sign extension not implemented yet
		}

	~ShiftUint16General()
		{
			Assert((!bufsize && !buffer) || (bufsize && buffer));
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			Assert(source);
			if (source->read()) {
				length=source->getBufferCount();

				if (bufsize < length) {
					Assert((!bufsize && !buffer) || (bufsize && buffer));
					if (buffer) delete[] buffer;
					bufsize=length;
					buffer=new Uint16[bufsize];
				}

				const Uint16 *src=source->getBuffer();
				Uint16 *dst=buffer;
				size_t n=length;
				while (n--) *dst++=(*src++<<leftshift)&mask;
			}
			else length=0;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

// These are the classes that are actually used as filters ...

class ShiftUint16 : public FilterSourceToSinkBase<Uint16,Uint16> {
	ShiftUint16Base *convertor;
public:
	ShiftUint16(SourceBase<Uint16>& i,
			unsigned short bitsallocated,unsigned short bitsstored,unsigned short highbit)
		: FilterSourceToSinkBase<Uint16,Uint16>(i)
		{
			Assert(bitsallocated <= 16u);
			Assert(bitsstored <= bitsallocated);
			Assert(highbit <= bitsallocated-1u);

			// no optimizations yet ...
			convertor=new ShiftUint16General(i,bitsstored,highbit);

			Assert(convertor);
		}

	size_t read(void)			{ return convertor->read(); }
	const Uint16 *getBuffer(void) 		{ return convertor->getBuffer(); } 
	size_t getBufferCount(void) const	{ return convertor->getBufferCount(); }
	int good(void) const			{ return convertor->good(); }
};

#endif // __Header_ToShift__
