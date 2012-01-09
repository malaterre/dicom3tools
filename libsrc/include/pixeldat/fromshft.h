#ifndef __Header_FromShift__
#define __Header_FromShift__

// Create a class to extract bitsstored length words starting at highbit
// from within 16 bit words (whose byte order is already that of the host)

// handles shifting so lsb is in bit 0
// doesn't yet handle sign extension

// Supporting classes ....

class UnshiftUint16Base {
protected:
	SourceBase<Uint16> *source;
public:
	UnshiftUint16Base(SourceBase<Uint16>& i)
		{
			source=&i;
		}
	virtual size_t read(void)			= 0;
	virtual const Uint16 *getBuffer(void) 		= 0; 
	virtual size_t getBufferCount(void) const	= 0;
	virtual int good(void) const	{ return source && source->good(); }
};

class UnshiftUint16General : public UnshiftUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
	Uint16 mask;
	unsigned short rightshift;
public:
	UnshiftUint16General(SourceBase<Uint16>& i,unsigned short bitsstored,unsigned short highbit)
			:UnshiftUint16Base(i)
		{
			buffer=0;
			bufsize=0;
			length=0;

			mask=(Uint16)(((Uint32)1<<bitsstored)-1);
			rightshift=highbit+1-bitsstored;

			// sign extension not implemented yet
		}

	~UnshiftUint16General()
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
				while (n--) *dst++=(*src++>>rightshift)&mask;
			}
			else length=0;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

// These are the classes that are actually used as filters ...

class UnshiftUint16 : public FilterSourceToSinkBase<Uint16,Uint16> {
	UnshiftUint16Base *convertor;
public:
	UnshiftUint16(SourceBase<Uint16>& i,
			unsigned short bitsallocated,unsigned short bitsstored,unsigned short highbit)
		: FilterSourceToSinkBase<Uint16,Uint16>(i)
		{
			Assert(bitsallocated <= 16u);
			Assert(bitsstored <= bitsallocated);
			Assert(highbit <= bitsallocated-1u);

			// no optimizations yet ...
			convertor=new UnshiftUint16General(i,bitsstored,highbit);

			Assert(convertor);
		}

	size_t read(void)			{ return convertor->read(); }
	const Uint16 *getBuffer(void) 		{ return convertor->getBuffer(); } 
	size_t getBufferCount(void) const	{ return convertor->getBufferCount(); }
	int good(void) const			{ return convertor->good(); }
};

#endif // __Header_FromShift__
