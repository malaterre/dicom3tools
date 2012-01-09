#ifndef __Header_ToPack__
#define __Header_ToPack__

// Create a class to pack bitsallocated length words into
// 16 bit words (whose byte order is still that of the host)

// Supporting classes ....

class PackUint16Base {
protected:
	SourceBase<Uint16> *source;
public:
	PackUint16Base(SourceBase<Uint16>& i)
		{
			source=&i;
		}
	virtual size_t read(void)			= 0;
	virtual const Uint16 *getBuffer(void) 		= 0; 
	virtual size_t getBufferCount(void) const	= 0;
	virtual int good(void) const	{ return source && source->good(); }
};

class PackUint16To16 : public PackUint16Base {
	const Uint16 *buffer;
	size_t length;
public:
	PackUint16To16(SourceBase<Uint16>& i)
			: PackUint16Base(i)
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

class PackUint16To12 : public PackUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
public:
	PackUint16To12(SourceBase<Uint16>& i)
			: PackUint16Base(i)
		{
			buffer=0;
			bufsize=0;
			length=0;
		}

	~PackUint16To12()
		{
			Assert((!bufsize && !buffer) || (bufsize && buffer));
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			//cerr << "PackUint16To12::read(void)\n" << flush;
			Assert(source);
			if (source->read()) {
				//cerr << "PackUint16To12::read(void) success\n" << flush;
				unsigned long n=source->getBufferCount();
				//cerr << "PackUint16To12::read(void) " << n << " source words\n" << flush;
				Assert((n*3)%4==0);
				n=(n*3)/4;
				//cerr << "PackUint16To12::read(void) " << n << " dest words\n" << flush;
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
					*dst++= *(src  )        & 0x0fff
					      |(*(src+1) << 12) & 0xf000;
					*dst++=(*(src+1) >> 4 ) & 0x00ff
					      |(*(src+2) << 8 ) & 0xff00;
					*dst++=(*(src+2) >> 8 ) & 0x000f
					      |(*(src+3) << 4 ) & 0xfff0;
					src+=4;
					n-=3;
				}
			}
			else length=0;
			//cerr << "PackUint16To12::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

class PackUint16To8 : public PackUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
public:
	PackUint16To8(SourceBase<Uint16>& i)
			: PackUint16Base(i)
		{
			buffer=0;
			bufsize=0;
			length=0;
		}

	~PackUint16To8()
		{
			Assert((!bufsize && !buffer) || (bufsize && buffer));
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			//cerr << "PackUint16To8::read(void)\n" << flush;
			Assert(source);
			if (source->read()) {
				//cerr << "PackUint16To8::read(void) success\n" << flush;
				size_t n=source->getBufferCount();
				//cerr << "PackUint16To8::read(void) " << n << " source words\n" << flush;
				Assert(n%2==0);
				n=n/2;
				//cerr << "PackUint16To8::read(void) " << n << " dest words\n" << flush;
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
					*dst++= *(src  )        & 0x00ff
					      |(*(src+1) << 8 ) & 0xff00;
					src+=2;
					--n;
				}
			}
			else length=0;
			//cerr << "PackUint16To8::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};


// These are the classes that are actually used as filters ...

class PackUint16 : public FilterSourceToSinkBase<Uint16,Uint16> {
	PackUint16Base *convertor;
public:
	PackUint16(SourceBase<Uint16>& i,
			unsigned short bitsallocated)
		: FilterSourceToSinkBase<Uint16,Uint16>(i)
		{
			Assert(bitsallocated <= 16);

			if (bitsallocated == 16)
				convertor=new PackUint16To16(i);
			else if (bitsallocated == 12)
				convertor=new PackUint16To12(i);
			else if (bitsallocated == 8)
				convertor=new PackUint16To8(i);
			else
				convertor=0;

			Assert(convertor);
		}

	size_t read(void)			{ return convertor->read(); }
	const Uint16 *getBuffer(void) 		{ return convertor->getBuffer(); } 
	size_t getBufferCount(void) const	{ return convertor->getBufferCount(); }
	int good(void) const			{ return convertor->good(); }
};

#endif // __Header_ToPack__
