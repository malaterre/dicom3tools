#ifndef __Header_buffer__
#define __Header_buffer__

#ifndef DEFAULTMINIMUMBUFFERSIZE
#define DEFAULTMINIMUMBUFFERSIZE 512
#endif

template<class T>
class Buffer {
protected:
	T *head;
	size_t size;
	size_t count;
	size_t minimum;
public:
	Buffer(void)
		{
			//cerr << "Buffer<T>::Buffer()\n" << flush;
			head=0;
			size=0;
			count=0;
			minimum=DEFAULTMINIMUMBUFFERSIZE;
		}

	Buffer(size_t m)
		{
			//cerr << "Buffer<T>::Buffer()\n" << flush;
			head=0;
			size=0;
			count=0;
			minimum=m;
		}

	virtual ~Buffer(void)
		{
			if (head) delete[] head;
		}

	void clear(void)
		{
			count=0;
		}

	void extend(size_t reqsize)
		{
			//cerr << "Buffer<T>::extend\n" << flush;
			if (reqsize > size) {
				size_t newsize=size;
				while (newsize < reqsize) newsize+=minimum;
				//cerr << "Buffer<T>::extend from "
				//     << dec << size << " requested " << reqsize
				//     << " actual " << newsize
				//     << "\n" << flush;
				T *oldhead=head;
				head=new T[newsize];
				size=newsize;
				if (oldhead) {
					if(count)
						memcpy((char *)head,
						       (char *)oldhead,
						       count*sizeof(T));
					delete[] oldhead;
				}
			}
		}

	void add(const T *values,size_t n)
		{
			//cerr << "Buffer<T>::add(T *values,size_t n)\n" << flush;
			extend(count+n);
			memcpy((char *)(head+count),(char*)values,n*sizeof(T));
			count+=n;
		}

	void add(T value)
		{
			//cerr << "Buffer<T>::add(T value) = "
			//     << dec << (unsigned long)value
			//     << " 0x" << hex << (unsigned long)value
			//     << "\n" << flush;
			extend(count+1);
			head[count++]=value;
		}

	void operator+=(T value)
		{
			//cerr << "Buffer<T>::operator+=\n" << flush;
			add(value);
		}

	size_t getSize(void) const		{ return size; }
	size_t getCount(void) const		{ return count; }
	void   setCount(size_t n)		{ count=n; }
	T * getValues(void)			{ return head; }

	// NB. Allowing this internal access facilitates reading directly
	// into buffers for example, without copying, eg.
	//
	//	istream istr;
	//	buffer.clear(); buffer.extend(n);
	//	istr.read(buffer.getValues(),n);
	//	buffer.setCount(istr.gcount());
};

template<class T>
class BufferIterator {
protected:
	Buffer<T> *buffer;
	const T *ptr;
	size_t count;
public:
	BufferIterator(Buffer<T>& b)
		{
			//cerr << "BufferIterator(Buffer<T>& b)\n" << flush;
			buffer=&b;
			first();
		}

	void first(void)
		{
			ptr=buffer->getValues();
			count=0;
		}

	int ismore(void)	{ return count < buffer->getCount(); }

	void next(void)		{ ++count; }

	T value(void)	        { if (ptr) return ptr[count];
				  else { T r = 0; return r; } }

	int operator!()		{ return ismore(); }
	void operator++()	{ next(); }		// prefix  ++i
	void operator++(int)	{ next(); }		// postfix i++
	T operator()()		{ return value(); }
};

#endif /* __Header_buffer__ */

