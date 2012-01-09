#ifndef __Header_FromByte__
#define __Header_FromByte__

// Create a class to convert words from a specified byte order
// to the host native byte order, making the choice of whether
// or not to swap the bytes as infrequently as possible, but
// still automatically at run-time

// NB. This implementation does not require that sizeof(Uint16) == 2
// but is optimized for that case ...

// Supporting classes ....

class ConvertByteToUint16Base {
protected:
	SourceBase<unsigned char> *source;
public:
	ConvertByteToUint16Base(SourceBase<unsigned char>& i)
		{
			source=&i;
		}
	virtual size_t read(void)			= 0;
	virtual const Uint16 *getBuffer(void) 		= 0; 
	virtual size_t getBufferCount(void) const	= 0;
	virtual int good(void) const	{ return source && source->good(); }
};

class ConvertUnswappedByteToUint16 : public ConvertByteToUint16Base {
	const Uint16 *buffer;
	size_t length;
public:
	ConvertUnswappedByteToUint16(SourceBase<unsigned char>& i)
			: ConvertByteToUint16Base(i)
		{
			buffer=0;
			length=0;
		}

	size_t read(void)
		{
//cerr << "ConvertUnswappedByteToUint16::read(void)\n" << flush;
			Assert(source);
			if (source->read()) {
//cerr << "ConvertUnswappedByteToUint16::read(void) success\n" << flush;
				size_t n=source->getBufferCount();
//cerr << "ConvertUnswappedByteToUint16::read(void) " << n << " values\n" << flush;
				buffer=(const Uint16 *)source->getBuffer();
				length=n/2;
			}
			else length=0;
//cerr << "ConvertUnswappedByteToUint16::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

class ConvertSwappedByteToUint16 : public ConvertByteToUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
public:
	ConvertSwappedByteToUint16(SourceBase<unsigned char>& i)
			: ConvertByteToUint16Base(i)
		{
			buffer=0;
			bufsize=0;
			length=0;
		}

	~ConvertSwappedByteToUint16()
		{
			Assert((!bufsize && !buffer) || (bufsize && buffer));
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
//cerr << "ConvertSwappedByteToUint16::read(void)\n" << flush;
			Assert(source);
			if (source->read()) {
//cerr << "ConvertSwappedByteToUint16::read(void) success\n" << flush;
				size_t n=source->getBufferCount();
//cerr << "ConvertSwappedByteToUint16::read(void) " << n << " values\n" << flush;
				Assert(n%2==0);
				n=n/2;
				if (bufsize < n+1) {
					Assert((!bufsize && !buffer) || (bufsize && buffer));
					if (buffer) delete[] buffer;
					bufsize=n+1;
					buffer=new Uint16[bufsize];
				}
				length=n;
				const unsigned char *src=source->getBuffer();
				unsigned char *base=(unsigned char *)buffer;
				memcpy(base+1,src,n*2);
				while (n--) { *base=*(base+2); base+=2; }
			}
			else length=0;
//cerr << "ConvertSwappedByteToUint16::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

template <class T>
class ConvertEachByteToUint16 : public ConvertByteToUint16Base {
	Uint16 *buffer;
	size_t bufsize;
	size_t length;
public:
	ConvertEachByteToUint16(SourceBase<unsigned char>& i)
			: ConvertByteToUint16Base(i)
		{
			buffer=0;
			bufsize=0;
			length=0;
		}

	~ConvertEachByteToUint16()
		{
			Assert((!bufsize && !buffer) || (bufsize && buffer));
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
//cerr << "ConvertEachByteToUint16::read(void)\n" << flush;
			Assert(source);
//cerr << "ConvertEachByteToUint16::read(void) sizeof(T)" << sizeof(T) << "\n" << flush;
			if (source->read()) {
//cerr << "ConvertEachByteToUint16::read(void) success\n" << flush;
				size_t n=source->getBufferCount();
				Assert(n%sizeof(T) == 0);
				n=n/sizeof(T);
//cerr << "ConvertEachByteToUint16::read(void) " << n << " values\n" << flush;
				if (bufsize < n) {
//cerr << "ConvertEachByteToUint16::read(void) reallocating buffer from " << bufsize << " to " << n << "\n" << flush;
					Assert((!bufsize && !buffer) || (bufsize && buffer));
					if (buffer) delete[] buffer;
					bufsize=n;
					buffer=new Uint16[bufsize];
				}
				length=n;
				const T *src=(const T *)source->getBuffer();
				Uint16 *ptr = buffer;
				while (n--) *ptr++=*src++;
			}
			else length=0;
//cerr << "ConvertEachByteToUint16::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

// These are the classes that are actually used as filters ...

class ConvertByteToUint16 : public FilterSourceToSinkBase<unsigned char,Uint16> {
	ConvertByteToUint16Base *convertor;
public:
	ConvertByteToUint16(SourceBase<unsigned char>& i,Endian wanted)
			: FilterSourceToSinkBase<unsigned char,Uint16>(i)
		{
			Assert(wanted != NoEndian);
			if (wanted == ByteEndian)
				convertor=new ConvertEachByteToUint16<unsigned char>(i);
			else if (sizeof(Uint16) == 2) {
//cerr << "ConvertEachByteToUint16::wanted = " << (wanted == LittleEndian ? "Little" : "Big") << endl;
				HostEndian hostendian;
				if (wanted == hostendian.getEndian()) {
//cerr << "ConvertEachByteToUint16::unswapped" << endl;
					convertor=new ConvertUnswappedByteToUint16(i);
				}
				else {
//cerr << "ConvertEachByteToUint16::swapped" << endl;
					convertor=new ConvertSwappedByteToUint16(i);
				}
			}
			else {
				if (wanted == BigEndian) {
//cerr << "ConvertEachByteToUint16::manual big" << endl;
					convertor=new ConvertEachByteToUint16<Uint16_B>(i);
				}
				else {
//cerr << "ConvertEachByteToUint16::manual little" << endl;
					convertor=new ConvertEachByteToUint16<Uint16_L>(i);
				}
			}
			Assert(convertor);
		}

	size_t read(void)			{ return convertor->read(); }
	const Uint16 *getBuffer(void) 		{ return convertor->getBuffer(); } 
	size_t getBufferCount(void) const	{ return convertor->getBufferCount(); }
	int good(void) const			{ return convertor->good(); }
};

#endif // __Header_FromByte__
