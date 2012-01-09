#ifndef __Header_ConvertBit__
#define __Header_ConvertBit__

// Create a class to change srcbitsstored length words (in low order bits)
// into dstbitsstored length words (in low order bits)
// (whose byte order is already that of the host)
//
// Note that only up to 16 bits are supported
// Note that srcbitsstored may be > or = or < dstbitsstored

// Supporting classes ....

class ConvertBitUint16Base {
protected:
	SourceBase<Uint16> *source;
public:
	ConvertBitUint16Base(SourceBase<Uint16>& i)
		{
			source=&i;
		}
	virtual size_t read(void)			= 0;
	virtual const Uint16 *getBuffer(void) 		= 0; 
	virtual size_t getBufferCount(void) const	= 0;
	virtual int good(void) const	{ return source && source->good(); }
};

class ConvertBitUint16Same : public ConvertBitUint16Base {
	const Uint16 *buffer;
	size_t length;
public:
	ConvertBitUint16Same(SourceBase<Uint16>& i,unsigned short,unsigned short)
			: ConvertBitUint16Base(i)
		{
//cerr << "ConvertBitUint16Same::ConvertBitUint16Same" << endl;
			buffer=0;
			length=0;
		}

	size_t read(void)
		{
			Assert(source);
			if (source->read()) {
				buffer=(const Uint16 *)source->getBuffer();
				length=source->getBufferCount();
			}
			else length=0;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

class ConvertBitUint16Shrink : public ConvertBitUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
	Uint16 mask;
	unsigned short rightshift;
public:
	ConvertBitUint16Shrink(SourceBase<Uint16>& i,unsigned short srcbitsstored,unsigned short dstbitsstored)
			:ConvertBitUint16Base(i)
		{
//cerr << "ConvertBitUint16Shrink::ConvertBitUint16Shrink: from " << dec << srcbitsstored << " to " << dstbitsstored << endl;
			buffer=0;
			bufsize=0;
			length=0;

			Assert(srcbitsstored>dstbitsstored);
			rightshift=srcbitsstored-dstbitsstored;
			mask=(Uint16)(((Uint32)1<<dstbitsstored)-1);
		}

	~ConvertBitUint16Shrink()
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

class ConvertBitUint16Expand : public ConvertBitUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
	Uint16 mask;
	unsigned short leftshift;
public:
	ConvertBitUint16Expand(SourceBase<Uint16>& i,unsigned short srcbitsstored,unsigned short dstbitsstored)
			:ConvertBitUint16Base(i)
		{
//cerr << "ConvertBitUint16Expand::ConvertBitUint16Expand: from " << dec << srcbitsstored << " to " << dstbitsstored << endl;
			buffer=0;
			bufsize=0;
			length=0;

			Assert(dstbitsstored>srcbitsstored);
			leftshift=dstbitsstored-srcbitsstored;
			mask=(Uint16)(((Uint32)1<<dstbitsstored)-1);

			// replicating to fill low order bits not implemented yet ... use zeroes
		}

	~ConvertBitUint16Expand()
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

class ConvertBitUint16 : public FilterSourceToSinkBase<Uint16,Uint16> {
	ConvertBitUint16Base *convertor;
public:
	ConvertBitUint16(SourceBase<Uint16>& i,
			unsigned short srcbitsstored,unsigned short dstbitsstored)
		: FilterSourceToSinkBase<Uint16,Uint16>(i)
		{
			Assert(srcbitsstored <= 16u);
			Assert(dstbitsstored <= 16u);

			if (srcbitsstored < dstbitsstored) {
				convertor=new ConvertBitUint16Expand(i,srcbitsstored,dstbitsstored);
			}
			else if (srcbitsstored > dstbitsstored) {
				convertor=new ConvertBitUint16Shrink(i,srcbitsstored,dstbitsstored);
			}
			else {
				convertor=new ConvertBitUint16Same(i,srcbitsstored,dstbitsstored);
			}
			Assert(convertor);
		}

	size_t read(void)			{ return convertor->read(); }
	const Uint16 *getBuffer(void) 		{ return convertor->getBuffer(); } 
	size_t getBufferCount(void) const	{ return convertor->getBufferCount(); }
	int good(void) const			{ return convertor->good(); }
};

#endif // __Header_ConvertBit__
