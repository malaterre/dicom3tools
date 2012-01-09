#ifndef __Header_ge9800src__
#define __Header_ge9800src__

class GE9800_PixelDataSource : public SourceBase<Uint16> {
	istream *istr;
	long offset;
	Uint16 rows;
	Uint16 columns;
	Uint16 row;
	Uint16 col;
	Uint16 *map;
	Uint16 *buffer;
	Int16 last_pixel;
public:
	GE9800_PixelDataSource(istream& i,bool usemap,long mapoff,long off,Uint16 r,Uint16 c)
			: SourceBase<Uint16>()
		{
			istr=&i;
			offset=off;
			rows=r;
			Assert(rows);
			columns=c;
			Assert(columns);
			row=0;
			col=0;
			buffer=new Uint16[columns];
			Assert(buffer);
			last_pixel=0;
			if (usemap) {
				map = new Uint16[rows];
				Assert(map);
				Assert(istr);
				istr->seekg(mapoff,ios::beg);
				unsigned i;
				for (i=0; i<rows; ++i) {
					unsigned char bytes[2];
					istr->read((char *)bytes,2);
					Assert(istr->good());
					// Big endian ...
					map[i]=(bytes[0]<<8)|bytes[1];
				}
			}
			else {
				map = NULL;
			}
		}

	~GE9800_PixelDataSource()
		{
			if (map) delete[] map;
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			if (row == 0) {
				Assert(istr);
				istr->seekg(offset,ios::beg);
			}
			
			unsigned 	start;
			unsigned 	end;
			if (map) {
				unsigned line = map[row];
				start = columns/2-line;
				end	= start+line*2;
			}
			else {
				start = 0;
				end	= columns;
			}

			if (!istr->good() || row++ >= rows) return 0;

			Uint16 *ptr=buffer;

			// Pad the first "empty" part of the line ...
			for (col=0; col<start; col++) *ptr++=0;
			
			Assert(col == start);
			
			while (col < end) {
				unsigned char byte;
				istr->read((char *)&byte,1);
				if (!istr->good()) return 0;
				if (byte & 0x80) {
					signed char delta;
					if (byte & 0x40) {
						delta=byte;
					}
					else {
						delta=byte & 0x3f;
					}
					last_pixel+=delta;
				}
				else {
					last_pixel=byte << 8;
					istr->read((char *)&byte,1);
					if (!istr->good()) return 0;
					last_pixel+=byte;
				}
				*ptr++=(Uint16)last_pixel & 0x0fff;
				++col;
			}
			
			Assert(col == end);
			
			// Pad the last "empty" part of the line ...
			for (col=end; col<columns; col++) *ptr++=0;
			
			return columns;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const	{ return columns; }

	int good(void) const	{ return istr && istr->good() && row < rows; }
};

#endif // __Header_ge9800src__
