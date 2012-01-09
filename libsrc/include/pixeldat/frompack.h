#ifndef __Header_FromPack__
#define __Header_FromPack__

// Create a class to unpack bitsallocated length words from within
// 16 bit words (whose byte order is already that of the host)

// Supporting classes ....

class UnpackUint16Base {
protected:
	SourceBase<Uint16> *source;
public:
	UnpackUint16Base(SourceBase<Uint16>& i)
		{
			source=&i;
		}
	virtual size_t read(void)			= 0;
	virtual const Uint16 *getBuffer(void) 		= 0; 
	virtual size_t getBufferCount(void) const	= 0;
	virtual int good(void) const	{ return source && source->good(); }
};

class UnpackUint16From16 : public UnpackUint16Base {
	const Uint16 *buffer;
	size_t length;
public:
	UnpackUint16From16(SourceBase<Uint16>& i)
			: UnpackUint16Base(i)
		{
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

class UnpackUint16From12 : public UnpackUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
public:
	UnpackUint16From12(SourceBase<Uint16>& i)
			: UnpackUint16Base(i)
		{
			buffer=0;
			bufsize=0;
			length=0;
		}

	~UnpackUint16From12()
		{
			Assert((!bufsize && !buffer) || (bufsize && buffer));
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			//cerr << "UnpackUint16From12::read(void)\n" << flush;
			Assert(source);
			if (source->read()) {
				//cerr << "UnpackUint16From12::read(void) success\n" << flush;
				unsigned long n=source->getBufferCount();
				//cerr << "UnpackUint16From12::read(void) " << n << " source words\n" << flush;
				Assert((n*4)%3==0);
				n=(n*4)/3;
				//cerr << "UnpackUint16From12::read(void) " << n << " dest words\n" << flush;
				if (bufsize < n) {
					Assert((!bufsize && !buffer) || (bufsize && buffer));
					if (buffer) delete[] buffer;
					bufsize=(size_t)n;
					buffer=new Uint16[bufsize];
				}
				length=(size_t)n;
				const Uint16 *src=source->getBuffer();
				Uint16 *dst=buffer;
				while (n) {
					*dst++= *(src  )        & 0x0fff;
					*dst++=(*(src  ) >> 12) & 0x000f
					      |(*(src+1) << 4 ) & 0x0ff0;
					*dst++=(*(src+1) >> 8 ) & 0x00ff
					      |(*(src+2) << 8 ) & 0x0f00;
					*dst++=(*(src+2) >> 4 ) & 0x0fff;
					src+=3;
					n-=4;
				}
			}
			else length=0;
			//cerr << "UnpackUint16From12::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

class UnpackUint16From8 : public UnpackUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
public:
	UnpackUint16From8(SourceBase<Uint16>& i)
			: UnpackUint16Base(i)
		{
			buffer=0;
			bufsize=0;
			length=0;
		}

	~UnpackUint16From8()
		{
			Assert((!bufsize && !buffer) || (bufsize && buffer));
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			//cerr << "UnpackUint16From8::read(void)\n" << flush;
			Assert(source);
			if (source->read()) {
				//cerr << "UnpackUint16From8::read(void) success\n" << flush;
				size_t n=source->getBufferCount();
				//cerr << "UnpackUint16From8::read(void) " << n << " source words\n" << flush;
				n=n*2;
				//cerr << "UnpackUint16From8::read(void) " << n << " dest words\n" << flush;
				if (bufsize < n) {
					Assert((!bufsize && !buffer) || (bufsize && buffer));
					if (buffer) delete[] buffer;
					bufsize=n;
					buffer=new Uint16[bufsize];
				}
				length=n;
				const Uint16 *src=source->getBuffer();
				Uint16 *dst=buffer;
				while (n) {
					*dst++= *(src  )        & 0x00ff;
					*dst++=(*(src  ) >> 8 ) & 0x00ff;
					++src;
					n-=2;
				}
			}
			else length=0;
			//cerr << "UnpackUint16From8::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};


// These are the classes that are actually used as filters ...

class UnpackUint16 : public FilterSourceToSinkBase<Uint16,Uint16> {
	UnpackUint16Base *convertor;
public:
	UnpackUint16(SourceBase<Uint16>& i,
			unsigned short bitsallocated)
		: FilterSourceToSinkBase<Uint16,Uint16>(i)
		{
			Assert(bitsallocated <= 16);

			if (bitsallocated == 16)
				convertor=new UnpackUint16From16(i);
			else if (bitsallocated == 12)
				convertor=new UnpackUint16From12(i);
			else if (bitsallocated == 8)
				convertor=new UnpackUint16From8(i);
			else
				convertor=0;

			Assert(convertor);
		}

	size_t read(void)			{ return convertor->read(); }
	const Uint16 *getBuffer(void) 		{ return convertor->getBuffer(); } 
	size_t getBufferCount(void) const	{ return convertor->getBufferCount(); }
	int good(void) const			{ return convertor->good(); }
};

#endif // __Header_FromPack__
