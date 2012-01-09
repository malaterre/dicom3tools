#ifndef __Header_ToByte__
#define __Header_ToByte__

// Create a class to insert words from the host native byte order
// to a specified byte order, making the choice of whether
// or not to swap the bytes as infrequently as possible, but
// still automatically at run-time

// NB. This implementation does not require that sizeof(Uint16) == 2
// but is optimized for that case ...

// Supporting classes ....

class ConvertUint16ToByteBase {
protected:
	SourceBase<Uint16> *source;
public:
	ConvertUint16ToByteBase(SourceBase<Uint16>& i)
		{
			source=&i;
		}
	virtual size_t read(void)			= 0;
	virtual const unsigned char *getBuffer(void) 	= 0; 
	virtual size_t getBufferCount(void) const	= 0;
	virtual int good(void) const	{ return source && source->good(); }
};

class ConvertUnswappedUint16ToByte : public ConvertUint16ToByteBase {
	const unsigned char *buffer;
	size_t length;
public:
	ConvertUnswappedUint16ToByte(SourceBase<Uint16>& i)
			: ConvertUint16ToByteBase(i)
		{
			buffer=0;
			length=0;
		}

	size_t read(void)
		{
			//cerr << "ConvertUnswappedUint16ToByte::read(void)\n" << flush;
			Assert(source);
			if (source->read()) {
				//cerr << "ConvertUnswappedUint16ToByte::read(void) success\n" << flush;
				size_t n=source->getBufferCount();
				//cerr << "ConvertUnswappedUint16ToByte::read(void) " << n << " values\n" << flush;
				buffer=(const unsigned char *)source->getBuffer();
				length=n*2;
			}
			else length=0;
			//cerr << "ConvertUnswappedUint16ToByte::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const unsigned char *getBuffer(void)	{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

class ConvertSwappedUint16ToByte : public ConvertUint16ToByteBase {
	Uint16 *buffer;
	size_t bufsize;		// In Uint16 words
	size_t length;		// In bytes
public:
	ConvertSwappedUint16ToByte(SourceBase<Uint16>& i)
			: ConvertUint16ToByteBase(i)
		{
			buffer=0;
			bufsize=0;
			length=0;
		}

	~ConvertSwappedUint16ToByte()
		{
			Assert((!bufsize && !buffer) || (bufsize && buffer));
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			//cerr << "ConvertSwappedUint16ToByte::read(void)\n" << flush;
			Assert(source);
			if (source->read()) {
				//cerr << "ConvertSwappedUint16ToByte::read(void) success\n" << flush;
				size_t n=source->getBufferCount();
				//cerr << "ConvertSwappedUint16ToByte::read(void) " << n << " values\n" << flush;
				if (bufsize < n+1) {
					Assert((!bufsize && !buffer) || (bufsize && buffer));
					if (buffer) delete[] buffer;
					bufsize=n+1;
					buffer=new Uint16[bufsize];
				}
				length=n*2;
				const unsigned char *src=(const unsigned char *)source->getBuffer();
				unsigned char *base=(unsigned char *)buffer;
				memcpy(base+1,src,n*2);
				while (n--) { *base=*(base+2); base+=2; }
			}
			else length=0;
			//cerr << "ConvertSwappedUint16ToByte::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const unsigned char *getBuffer(void)	{ return (unsigned char *)buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

template <class T>
class ConvertEachUint16ToByte : public ConvertUint16ToByteBase {
	T *buffer;
	size_t bufsize;		// In T words
	size_t length;		// In bytes
public:
	ConvertEachUint16ToByte(SourceBase<Uint16>& i)
			: ConvertUint16ToByteBase(i)
		{
			buffer=0;
			bufsize=0;
			length=0;
		}

	~ConvertEachUint16ToByte()
		{
			Assert((!bufsize && !buffer) || (bufsize && buffer));
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			//cerr << "ConvertEachUint16ToByte::read(void)\n" << flush;
			Assert(source);
			if (source->read()) {
				//cerr << "ConvertEachUint16ToByte::read(void) success\n" << flush;
				size_t n=source->getBufferCount();
				//cerr << "ConvertEachUint16ToByte::read(void) " << n << " values\n" << flush;
				if (bufsize < n) {
					Assert((!bufsize && !buffer) || (bufsize && buffer));
					if (buffer) delete[] buffer;
					bufsize=n;
					buffer=new T[bufsize];
				}
				length=n*sizeof(T);
				const Uint16 *src=source->getBuffer();
				T *ptr = buffer;
				while (n--) *ptr++=*src++;
			}
			else length=0;
			//cerr << "ConvertEachUint16ToByte::read(void) return " << length << " values\n" << flush;
			return length;
		}

	const unsigned char *getBuffer(void)	{ return (unsigned char *)buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

// These are the classes that are actually used as filters ...

class ConvertUint16ToByte : public FilterSourceToSinkBase<Uint16,unsigned char> {
	ConvertUint16ToByteBase *convertor;
public:
	ConvertUint16ToByte(SourceBase<Uint16>& i,Endian wanted)
			: FilterSourceToSinkBase<Uint16,unsigned char>(i)
		{
			Assert(wanted != NoEndian);
			if (wanted == ByteEndian)
				convertor=new ConvertEachUint16ToByte<unsigned char>(i);
			else if (sizeof(Uint16) == 2) {
				HostEndian hostendian;
				if (wanted == hostendian.getEndian()) {
//cerr << "ConvertUint16ToByte::unswapped" << endl;
					convertor=new ConvertUnswappedUint16ToByte(i);
				}
				else {
					convertor=new ConvertSwappedUint16ToByte(i);
				}
			}
			else {
				if (wanted == BigEndian) {
					convertor=new ConvertEachUint16ToByte<Uint16_B>(i);
				}
				else {
					convertor=new ConvertEachUint16ToByte<Uint16_L>(i);
				}
			}
			Assert(convertor);
		}

	size_t read(void)			{ return convertor->read(); }
	const unsigned char *getBuffer(void) 	{ return convertor->getBuffer(); } 
	size_t getBufferCount(void) const	{ return convertor->getBufferCount(); }
	int good(void) const			{ return convertor->good(); }
};

#endif // __Header_ToByte__
