#ifndef __Header_srcsink__
#define __Header_srcsink__

#include "buffer.h"

#ifndef DEFAULTCHUNKSIZE
#define DEFAULTCHUNKSIZE 16384
#endif

template<class T>
class SourceBase {
public:
	SourceBase(void)	{}
	virtual size_t read(void) = 0;
	virtual const T *getBuffer(void) = 0;
	virtual size_t getBufferCount(void) const = 0;
	virtual int good(void) const = 0;
};

template<class T>
class SourceBuffered : public SourceBase<T> {
protected:
	Buffer<T> buffer;
public:
	SourceBuffered(void) : SourceBase<T>()	{}

	const T *getBuffer(void)			{ return buffer.getValues(); }
	size_t getBufferCount(void) const		{ return buffer.getCount(); }

	size_t readFromStream(istream &istr,size_t n)
		{
			//cerr << "SourceBuffered::read(istream &istr," << dec << n << ")\n" << flush;
			buffer.clear(); buffer.extend(n);
			istr.read((char *)buffer.getValues(),n*sizeof(T));
			size_t count=istr.gcount()/sizeof(T);
			//if (n > count) {
				//cerr << "SourceBuffered::read(istream &istr,size_t n) read " << dec << count << " but expected " << n << endl;
			//}
			buffer.setCount(count);
			//cerr << "SourceBuffered::read(istream &istr,size_t n) read " << dec << count << endl;
			return count;
		}
};


template<class T>
class Source : public SourceBuffered<T> {
	istream *istr;
	size_t chunkSize;
	unsigned long bytesLeft;
	int bad;
public:
	Source(istream& i,size_t chunksize=DEFAULTCHUNKSIZE,unsigned long byteswanted=ULONG_MAX)
			: SourceBuffered<T>()
		{
			istr=&i;
			chunkSize=chunksize;
			bytesLeft=byteswanted;
			bad=0;
		}

	size_t read(void)
		{
//cerr << "Source::read(void) bytesLeft=" << dec << bytesLeft << endl;
			size_t n;
			if (good() && bytesLeft > 0) {
				size_t readsize = (size_t)(chunkSize > bytesLeft ? bytesLeft : chunkSize);
//cerr << "Source::read(void) readsize=" << dec << readsize << endl;
				n=SourceBuffered<T>::readFromStream(*istr,readsize);
//cerr << "Source::read(void) n=" << dec << n << endl;
				bytesLeft-=n;
				if (n != readsize) bad=1;
			}
			else {
				// Trying to read past previous failure or finish
				bad=1;
			}
			return good() ? n : 0;
		}

	int good(void) const
		{
			Assert(istr);
//cerr << "Source::good() istr->good()=" << (istr->good() ? "T":"F") << endl;
//cerr << "Source::good() bad=" << bad << endl;
			return istr->good() && !bad;
		}
};

template<class T>
class Sink {
	ostream *ostr;
	SourceBase<T> *source;
public:
	Sink(ostream& o,SourceBase<T>& i)
		{
			ostr=&o;
			source=&i;
		}

	void write(unsigned long nwords=ULONG_MAX)
		{
			//cerr << "Sink::write nwords=" << nwords << endl;
			Assert(source);
			Assert(ostr);
			while (nwords && source->good() && good()) {
				//cerr << "Sink::operator<< in loop nwords=" << nwords << "\n" << flush;
				size_t n=source->read();
				//cerr << "Sink::operator<< read " << n << "\n" << flush;
				if (!n) break; // premature end unless nwords=0xffffffff
				if (n>nwords) n=(size_t)nwords;
				if (n) ostr->write((const char *)(source->getBuffer()),n*sizeof(T));
				nwords-=n;
				if (nwords == 1 && sizeof(T) == 1 && !source->good()) {		// need to do this now, because will not loop again because source will not be good
					// handle special case of single byte input rows*cols is odd, which is legitimate (e.g., pnmtodc), and single byte padding is needed ... (000136),(000185)
					//cerr << "Sink::write padding odd rows*cols to even byte\n" << flush;
					const char pad = 0x00;
					ostr->write(&pad,1);
				}
				//cerr << "Sink::operator<< loop end\n" << flush;
			}
			//cerr << "Sink::write return\n" << flush;
		}

	void write(unsigned long nskip,unsigned long nwords)
		{
			//cerr << "Sink::write nskip=" << nskip << endl;
			//cerr << "Sink::write nwords=" << nwords << endl;
			unsigned long ntotal = (nwords == ULONG_MAX) ? ULONG_MAX : nskip+nwords;
				//cerr << "Sink::operator<< loop end\n" << flush;
			Assert(source);
			Assert(ostr);
			while (ntotal && source->good() && good()) {
				//cerr << "Sink::operator<< in loop ntotal=" << ntotal << "\n" << flush;
				//cerr << "Sink::operator<< in loop nskip=" << nskip << "\n" << flush;
				size_t n=source->read();
				//cerr << "Sink::operator<< read " << n << "\n" << flush;
				if (!n) break;	// premature end unless nwords=0xffffffff
				if (n>ntotal) n=(size_t)ntotal;
				size_t offset=0;
				if (nskip >= n) {
					//cerr << "Sink::operator<< skipping " << n << "\n" << flush;
					nskip-=n;
					ntotal-=n;
					continue;
				}
				//cerr << "Sink::operator<< writing " << (n-nskip) << " after skipping " << nskip << "\n" << flush;
				if (n) ostr->write((const char *)(source->getBuffer()+nskip),(n-nskip)*sizeof(T));
				ntotal-=n;
				nskip=0;
				if (ntotal == 1 && sizeof(T) == 1 && !source->good()) {		// need to do this now, because will not loop again because source will not be good
					// handle special case of single byte input rows*cols is odd, which is legitimate (e.g., pnmtodc), and single byte padding is needed ... (000136),(000185)
					//cerr << "Sink::write padding odd rows*cols to even byte\n" << flush;
					const char pad = 0x00;
					ostr->write(&pad,1);
				}
				//cerr << "Sink::operator<< loop end\n" << flush;
			}
			//cerr << "Sink::write return\n" << flush;
		}

	int good(void) const	{ Assert(ostr); return ostr->good(); }
};

template<class Tin>
class SourceToSink {
protected:
	SourceBase<Tin> *source;
public:
	SourceToSink(SourceBase<Tin>& i)
		{
			source=&i;
		}
};

// These classes are the basis for filters ...

template<class Tin,class Tout>
class FilterSourceToSinkBase : public SourceToSink<Tin>, public SourceBase<Tout> {
public:
	FilterSourceToSinkBase(SourceBase<Tin>& i): SourceToSink<Tin>(i), SourceBase<Tout>() {}
};

template<class Tin,class Tout>
class FilterSourceToSinkBufferedBase : public SourceToSink<Tin>, public SourceBuffered<Tout> {
public:
	FilterSourceToSinkBufferedBase(SourceBase<Tin>& i): SourceToSink<Tin>(i), SourceBuffered<Tout>() {}
};

// These two are provided as examples of how to write filters ...

template<class T>
class CopySourceToSink : public FilterSourceToSinkBase<T,T> {
	const T *buffer;
	size_t length;
public:
	CopySourceToSink(SourceBase<T>& i)
			: FilterSourceToSinkBase<T,T>(i)
		{}

	size_t read(void)
		{
			Assert(SourceToSink<T>::source);
			if (SourceToSink<T>::source->read()) {
				size_t n=SourceToSink<T>::source->getBufferCount();
				buffer=(const T *)SourceToSink<T>::source->getBuffer();
				length=n;
			}
			else length=0;
			return length;
		}

	int good(void) const	{ return SourceToSink<T>::source && SourceToSink<T>::source->good(); }
	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return length; }
};

template<class T>
class DuplicateSourceToSink : public FilterSourceToSinkBufferedBase<T,T> {
public:
	DuplicateSourceToSink(SourceBase<T>& i)
			: FilterSourceToSinkBufferedBase<T,T>(i)
		{}

	size_t read(void)
		{
			//cerr << "DuplicateSourceToSink::read(void)\n" << flush;
			FilterSourceToSinkBufferedBase<T,T>::buffer.clear();
			if (FilterSourceToSinkBufferedBase<T,T>::source && FilterSourceToSinkBufferedBase<T,T>::source->read()) {
				//cerr << "DuplicateSourceToSink::read(void) success\n" << flush;
				size_t n=FilterSourceToSinkBufferedBase<T,T>::source->getBufferCount();
				//cerr << "DuplicateSourceToSink::read(void) " << n << " values\n" << flush;
				const T *inptr=FilterSourceToSinkBufferedBase<T,T>::source->getBuffer();
				FilterSourceToSinkBufferedBase<T,T>::buffer.add(inptr,n);
			}
			//cerr << "DuplicateSourceToSink::read(void) return " << getBufferCount() << " values\n" << flush;
			return FilterSourceToSinkBufferedBase<T,T>::getBufferCount();
		}

	int good(void) const	{ return FilterSourceToSinkBufferedBase<T,T>::source && FilterSourceToSinkBufferedBase<T,T>::source->good(); }
};

template<class Tin,class Tout>
class ConvertSourceToSink : public FilterSourceToSinkBufferedBase<Tin,Tout> {
public:
	ConvertSourceToSink(SourceBase<Tin>& i)
			: FilterSourceToSinkBufferedBase<Tin,Tout>(i)
		{}

	size_t read(void)
		{
			//cerr << "ConvertSourceToSink::read(void)\n" << flush;
			FilterSourceToSinkBufferedBase<Tin,Tout>::buffer.clear();
			if (FilterSourceToSinkBufferedBase<Tin,Tout>::source && FilterSourceToSinkBufferedBase<Tin,Tout>::source->read()) {
				//cerr << "ConvertSourceToSink::read(void) success\n" << flush;
				size_t n=FilterSourceToSinkBufferedBase<Tin,Tout>::source->getBufferCount();
				//cerr << "ConvertSourceToSink::read(void) " << n << " values\n" << flush;
				const Tin *inptr=FilterSourceToSinkBufferedBase<Tin,Tout>::source->getBuffer();

				while (n--) FilterSourceToSinkBufferedBase<Tin,Tout>::buffer.add((Tout)(*inptr++));
			}
			//cerr << "ConvertSourceToSink::read(void) return " << getBufferCount() << " values\n" << flush;
			return FilterSourceToSinkBufferedBase<Tin,Tout>::getBufferCount();
		}

	int good(void) const	{ return FilterSourceToSinkBufferedBase<Tin,Tout>::source && FilterSourceToSinkBufferedBase<Tin,Tout>::source->good(); }
};

template<class Tin,class Tout,int swapSpan>
class ConvertSourceToSinkSwapping : public FilterSourceToSinkBufferedBase<Tin,Tout> {
public:
	ConvertSourceToSinkSwapping(SourceBase<Tin>& i)
			: FilterSourceToSinkBufferedBase<Tin,Tout>(i)
		{}

	size_t read(void)
		{
			//cerr << "ConvertSourceToSink::read(void)\n" << flush;
			FilterSourceToSinkBufferedBase<Tin,Tout>::buffer.clear();
			if (FilterSourceToSinkBufferedBase<Tin,Tout>::source && FilterSourceToSinkBufferedBase<Tin,Tout>::source->read()) {
				//cerr << "ConvertSourceToSink::read(void) success\n" << flush;
				size_t n=FilterSourceToSinkBufferedBase<Tin,Tout>::source->getBufferCount();
				//cerr << "ConvertSourceToSink::read(void) " << n << " values\n" << flush;
				const Tin *inptr=FilterSourceToSinkBufferedBase<Tin,Tout>::source->getBuffer();
				Assert(n%swapSpan == 0);
				//while (n--) buffer.add((Tout)(*inptr++));
				while (n > 0) {
					int j;
					for (j=swapSpan; j > 0; --j) {
						FilterSourceToSinkBufferedBase<Tin,Tout>::buffer.add((Tout)(*(inptr+j-1)));
					}
					inptr+=swapSpan;
					n-=swapSpan;
				}
			}
			//cerr << "ConvertSourceToSink::read(void) return " << getBufferCount() << " values\n" << flush;
			return FilterSourceToSinkBufferedBase<Tin,Tout>::getBufferCount();
		}

	int good(void) const	{ return FilterSourceToSinkBufferedBase<Tin,Tout>::source && FilterSourceToSinkBufferedBase<Tin,Tout>::source->good(); }
};

template<class Tin,class Tout>
class PointFilter {
public:
	PointFilter() {}
	virtual Tout filter(Tin) = 0;
	virtual Tout filter(Tin,size_t offset) = 0;
	virtual bool isOffsetIndependent() = 0;
};

template<class Tin,class Tout>
class PointFilterIndependentOfOffset : public PointFilter<Tin,Tout>  {
public:
	PointFilterIndependentOfOffset() {}
	virtual Tout filter(Tin) = 0;
	Tout filter(Tin,size_t offset) { Assert(0); }
	bool isOffsetIndependent() { return true; }
};

template<class Tin,class Tout>
class PointFilterDependentOnOffset : public PointFilter<Tin,Tout> {
public:
	PointFilterDependentOnOffset() {}
	Tout filter(Tin) { Assert(0); }
	virtual Tout filter(Tin,size_t offset) = 0;
	bool isOffsetIndependent() { return false; }
};


template<class Tin,class Tout>
class ConvertSourceToSinkWithFilter : public FilterSourceToSinkBufferedBase<Tin,Tout> {
public:
	ConvertSourceToSinkWithFilter(SourceBase<Tin>& i)
			: FilterSourceToSinkBufferedBase<Tin,Tout>(i)
		{
		}
	
	virtual size_t read(void) = 0;

	int good(void) const	{ return FilterSourceToSinkBufferedBase<Tin,Tout>::source && FilterSourceToSinkBufferedBase<Tin,Tout>::source->good(); }
};

template<class Tin,class Tout>
class ConvertSourceToSinkWithFilterIndependentOfOffset : public ConvertSourceToSinkWithFilter<Tin,Tout> {
private:
	PointFilterIndependentOfOffset<Tin,Tout> *filter;
public:
	ConvertSourceToSinkWithFilterIndependentOfOffset(SourceBase<Tin>& i,PointFilterIndependentOfOffset<Tin,Tout> *f)
			: ConvertSourceToSinkWithFilter<Tin,Tout>(i)
		{
			Assert(f);
			filter=f;
		}

	size_t read(void)
		{
			//cerr << "ConvertSourceToSinkWithFilter::read(void)\n" << flush;
			ConvertSourceToSinkWithFilter<Tin,Tout>::buffer.clear();
			if (ConvertSourceToSinkWithFilter<Tin,Tout>::source && ConvertSourceToSinkWithFilter<Tin,Tout>::source->read()) {
				//cerr << "ConvertSourceToSinkWithFilter::read(void) success\n" << flush;
				size_t n=ConvertSourceToSinkWithFilter<Tin,Tout>::source->getBufferCount();
				//cerr << "ConvertSourceToSinkWithFilter::read(void) " << n << " values\n" << flush;
				const Tin *inptr=ConvertSourceToSinkWithFilter<Tin,Tout>::source->getBuffer();

				while (n--) ConvertSourceToSinkWithFilter<Tin,Tout>::buffer.add(filter->filter((Tout)(*inptr++)));
			}
			//cerr << "ConvertSourceToSinkWithFilter::read(void) return " << getBufferCount() << " values\n" << flush;
			return ConvertSourceToSinkWithFilter<Tin,Tout>::getBufferCount();
		}

	int good(void) const	{ return ConvertSourceToSinkWithFilter<Tin,Tout>::source && ConvertSourceToSinkWithFilter<Tin,Tout>::source->good(); }
};


template<class Tin,class Tout>
class ConvertSourceToSinkWithFilterDependentOnOffset : public ConvertSourceToSinkWithFilter<Tin,Tout> {
private:
	PointFilterDependentOnOffset<Tin,Tout> *filter;
	size_t offset;
public:
	ConvertSourceToSinkWithFilterDependentOnOffset(SourceBase<Tin>& i,PointFilterDependentOnOffset<Tin,Tout> *f)
			: ConvertSourceToSinkWithFilter<Tin,Tout>(i)
		{
			Assert(f);
			filter=f;
			offset=0;
		}

	size_t read(void)
		{
			//cerr << "ConvertSourceToSinkWithFilter::read(void)\n" << flush;
			ConvertSourceToSinkWithFilter<Tin,Tout>::buffer.clear();
			if (ConvertSourceToSinkWithFilter<Tin,Tout>::source && ConvertSourceToSinkWithFilter<Tin,Tout>::source->read()) {
				//cerr << "ConvertSourceToSinkWithFilter::read(void) success\n" << flush;
				size_t n=ConvertSourceToSinkWithFilter<Tin,Tout>::source->getBufferCount();
				//cerr << "ConvertSourceToSinkWithFilter::read(void) " << n << " values\n" << flush;
				const Tin *inptr=ConvertSourceToSinkWithFilter<Tin,Tout>::source->getBuffer();

				while (n--) ConvertSourceToSinkWithFilter<Tin,Tout>::buffer.add(filter->filter((Tout)(*inptr++),offset++));
			}
			//cerr << "ConvertSourceToSinkWithFilter::read(void) return " << getBufferCount() << " values\n" << flush;
			return ConvertSourceToSinkWithFilter<Tin,Tout>::getBufferCount();
		}

	int good(void) const	{ return ConvertSourceToSinkWithFilter<Tin,Tout>::source && ConvertSourceToSinkWithFilter<Tin,Tout>::source->good(); }
};

class SupplySource {
public:
	SupplySource(void) {}
	virtual ~SupplySource(void) {}
	virtual class SourceBase<Uint16> *getSource(void) = 0;
};

#endif /* __Header_srcsink__ */
