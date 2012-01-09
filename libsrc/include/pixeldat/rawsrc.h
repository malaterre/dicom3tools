#ifndef __Header_RawSrc__
#define __Header_RawSrc__

#include "srcsink.h"

class Raw_PixelDataSourceBase : public SourceBase<Uint16> {
protected:
	istream *istr;
	long offset;
	Uint16 rows;
	Uint16 columns;
	Uint16 frames;
	Uint16 samples;

	Uint32 totalrows;
	Uint32 row;
	Uint16 col;
	Uint16 frame;
	Uint16 *buffer;
public:
	Raw_PixelDataSourceBase(istream& i,long off,Uint16 r,Uint16 c,
		Uint16 f,Uint16 s)
			: SourceBase<Uint16>()
		{
			istr=&i;
			offset=off;
			rows=r;
			Assert(rows);
			columns=c;
			Assert(columns);
			frames=f;
			Assert(frames);
			samples=s;
			Assert(samples);

			totalrows=((Uint32)rows)*frames;
			row=0;
			col=0;
			frame=0;
			buffer=new Uint16[c*s];
			Assert(buffer);
		}

	virtual ~Raw_PixelDataSourceBase()
		{
			if (buffer) delete[] buffer;
		}

	virtual size_t read(void) = 0;

	const Uint16 *getBuffer(void)		{ return buffer; }

	size_t getBufferCount(void) const	{ return col*samples; }

	int good(void) const	{ return istr && istr->good() && row < totalrows; }
};

class Raw_PixelDataSource_LittleEndian : public Raw_PixelDataSourceBase {
public:
	Raw_PixelDataSource_LittleEndian(istream& i,long off,Uint16 r,Uint16 c,
		Uint16 f,Uint16 s)
			: Raw_PixelDataSourceBase(i,off,r,c,f,s)
		{}

	size_t read(void)
		{
			if (row == 0 && frame == 0) {
				Assert(istr);
				istr->seekg(offset,ios::beg);
			}
			col=0;
			if (!istr->good() || row++ >= totalrows) return 0;
			Uint16 *ptr=buffer;
			while (col < columns) {
				Uint16 sample=0;
				while (sample < samples) {
					unsigned char bytes[2];
					istr->read((char *)bytes,2);
					if (!istr->good()) return 0;
					// Little endian ...
					Uint16 pixel;
					pixel=(bytes[1]<<8)|bytes[0];
					*ptr++=pixel; ++sample;
				}
				++col;
			}
			return col*samples;
		}
};

class Raw_PixelDataSource_BigEndian : public Raw_PixelDataSourceBase {
public:
	Raw_PixelDataSource_BigEndian(istream& i,long off,Uint16 r,Uint16 c,
		Uint16 f,Uint16 s)
			: Raw_PixelDataSourceBase(i,off,r,c,f,s)
		{}

	size_t read(void)
		{
			if (row == 0 && frame == 0) {
				Assert(istr);
				istr->seekg(offset,ios::beg);
			}
			col=0;
			if (!istr->good() || row++ >= totalrows) return 0;
			Uint16 *ptr=buffer;
			while (col < columns) {
				Uint16 sample=0;
				while (sample < samples) {
					unsigned char bytes[2];
					istr->read((char *)bytes,2);
					if (!istr->good()) return 0;
					// Big endian ...
					Uint16 pixel;
					pixel=(bytes[0]<<8)|bytes[1];
					*ptr++=pixel; ++sample;
				}
				++col;
			}
			return col*samples;
		}
};

class Raw_PixelDataSource_Byte : public Raw_PixelDataSourceBase {
public:
	Raw_PixelDataSource_Byte(istream& i,long off,Uint16 r,Uint16 c,
		Uint16 f,Uint16 s)
			: Raw_PixelDataSourceBase(i,off,r,c,f,s)
		{}

	size_t read(void)
		{
			if (row == 0 && frame == 0) {
				Assert(istr);
				istr->seekg(offset,ios::beg);
			}
			col=0;
			if (!istr->good() || row++ >= totalrows) return 0;
			Uint16 *ptr=buffer;
			while (col < columns) {
				Uint16 sample=0;
				while (sample < samples) {
					unsigned char byte;
					istr->read((char *)&byte,1);
					if (!istr->good()) return 0;
					*ptr++=Uint16(byte); ++sample;
				}
				++col;
			}
			return col*samples;
		}
};


#endif // __Header_RawSrc__
