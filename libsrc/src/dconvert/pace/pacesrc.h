#ifndef __Header_pacesrc__
#define __Header_pacesrc__

class PACE_PixelDataSource : public SourceBase<Uint16> {
	istream *istr;
	long offset;
	Uint16 rows;
	Uint16 columns;
	Int16	pixel;
	unsigned runlengthleftover;
	enum { Difference, Reference } mode;
	Uint16 row;
	Uint16 col;
	Uint16 *buffer;
public:
	PACE_PixelDataSource(istream& i,long off,Uint16 r,Uint16 c)
			: SourceBase<Uint16>()
		{
			istr=&i;
			offset=off;
			rows=r;
			Assert(rows);
			columns=c;
			Assert(columns);
			pixel=0;
			runlengthleftover=0;
			mode=Reference;
			row=0;
			col=0;
			buffer=new Uint16[columns];
			Assert(buffer);
		}

	~PACE_PixelDataSource()
		{
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			if (row == 0) {
				Assert(istr);
				istr->seekg(offset,ios::beg);
			}
			col=0;
			if (!good() || row++ >= rows) return 0;
			Uint16 *ptr=buffer;
			while (col < columns) {
				if (runlengthleftover) {
					unsigned colsleft=columns-col;
					unsigned runinthisline = (runlengthleftover > colsleft)
						? colsleft : runlengthleftover;
					runlengthleftover-=runinthisline;
					col+=runinthisline;
					while (runinthisline--) *ptr++=pixel;
				}
				else {
					unsigned char byte;
					istr->read((char *)&byte,1);
					if (!istr->good()) break;
					if (byte == 0x80) {		// Mode switch
						if (mode == Difference)
							mode=Reference;
						else
							mode=Difference;
					}
					else if (byte == 0x81) {	// Run length flag
						istr->read((char *)&byte,1);
						if (!istr->good()) break;
						runlengthleftover=byte;
					}
					else {
						if (mode == Difference) {
							pixel+=(signed char)byte;
						}
						else {
							pixel=byte<<8;
							istr->read((char *)&byte,1);
							if (!istr->good()) break;
							pixel|=byte;
						}
						*ptr++=pixel; ++col;
					}
				}
			}
			return col;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return col; }

	int good(void) const	{ return istr && istr->good() && row < rows; }
};

#endif // __Header_pacesrc__
