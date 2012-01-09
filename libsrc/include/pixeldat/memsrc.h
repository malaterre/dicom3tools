#ifndef __Header_MemSrc__
#define __Header_MemSrc__

#include "srcsink.h"

template<class T>
class Memory_PixelDataSource : public SourceBase<T> {
protected:
	Uint16 rows;
	Uint16 columns;

	Uint16 row;
	T *buffer;
	T *bufptr;

public:
	Memory_PixelDataSource(Uint16 r,Uint16 c,T *srcbuffer)	// Takes ownership of srcbuffer memory
			: SourceBase<T>()
		{
			rows=r;
			Assert(rows);
			columns=c;
			Assert(columns);

			// Allocate and buffer the entire array, not just one row ...

			buffer=srcbuffer;
			Assert(buffer);
			bufptr=buffer-columns;		// position BEFORE buffer, since 1st "read" increments it
			row=0;
		}

	~Memory_PixelDataSource()
		{
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			bufptr+=columns;
			++row;
			return columns;
		}

	const T *getBuffer(void)
		{
			Assert (bufptr >= buffer);	// Just in case someone calls getBuffer() without read()
			return bufptr;
		}

	size_t getBufferCount(void) const	{ return columns; }

	int good(void) const	{ return row < rows; }
};


#endif // __Header_MemSrc__
