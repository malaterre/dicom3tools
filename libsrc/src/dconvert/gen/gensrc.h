#ifndef __Header_gensrc__
#define __Header_gensrc__

class GEN_PixelDataSource : public SourceBase<Uint16> {
	istream *istr;
	long pixeloffset;
	long mapoffset;
	long maplength;
	long offset;
	Uint16 compress;
	Uint16 rows;
	Uint16 columns;

	Int16 last_pixel;
	Uint16 currentrow;
	Uint16 *linebuffer;

	Uint16	*map_left;
	Uint16	*map_wide;

	unsigned char ubuffer[2];

	Uint16 readUint16(void)
		{
			Uint16 u;
			istr->read((char *)ubuffer,2);
			// Genesis is a sun3 which is a Big Endian machine
			u =  (Uint16)ubuffer[0];
			u <<= 8;
			u |= (Uint16)ubuffer[1];
			return u;
	}

	bool readMap(void)
		{
			Assert(maplength);
			Assert(maplength == 4*rows);

			istr->seekg(mapoffset,ios::beg);
			if (!(istr && istr->good())) return false;

			map_left=new Uint16[rows];
			map_wide=new Uint16[rows];
			if (!map_left || !map_wide) return false;
			unsigned i;
			for (i=0; i<rows; ++i) {
				map_left[i]=readUint16();
				map_wide[i]=readUint16();
			}
			return istr && istr->good();
		}
public:
	GEN_PixelDataSource(istream& i,long pixoff,long mapoff,long maplng,Uint16 cflag,Uint16 r,Uint16 c)
			: SourceBase<Uint16>()
		{
			istr=&i;
			pixeloffset=pixoff;
			mapoffset=mapoff;
			maplength=maplng;
			compress=cflag;
			rows=r;
			Assert(rows);
			columns=c;
			Assert(columns);

			currentrow=0;		// used as index into map
			last_pixel=0;		// used for DPCM decoding

			linebuffer=new Uint16[columns];
			Assert(linebuffer);

			map_left=0;
			map_wide=0;
		}

	~GEN_PixelDataSource()
		{
			if (linebuffer) delete[] linebuffer;
			if (map_left) delete[] map_left;
			if (map_wide) delete[] map_wide;
		}

	size_t read(void)
		{
			if (currentrow == 0) {
				Assert(istr);
				if (compress == 2 || compress == 4) { // packed or compacked
					readMap();
				}
				istr->seekg(pixeloffset,ios::beg);
			}

			if (!good() || currentrow >= rows) return 0;

			Uint16 *ptr=linebuffer;
			unsigned start;
			unsigned end;
			unsigned j;

			if (compress == 2 || compress == 4) { // packed or compacked
				start=map_left[currentrow];
				end=start+map_wide[currentrow];
			}
			else {
				start=0;
				end=columns;
			}

			Assert(start <= columns);
			Assert(end <= columns);

			// Pad the first "empty" part of the line ...
			for (j=0; j<start; j++)*ptr++=0;

			// Copy the middle of the line

			if (compress == 3 || compress == 4) { // compressed or compacked
				while (start<end) {
					unsigned char byte;
					istr->read((char *)&byte,1);
					if (!istr) break;
					if (byte & 0x80) {
						unsigned char byte2;
						istr->read((char *)&byte2,1);
						if (!istr) break;
						if (byte & 0x40) {	// next word
							istr->read((char *)&byte,1);
							if (!istr) break;
							last_pixel=
							    (((Uint16)byte2<<8)+byte);
						}
						else {			// 14 bit delta
							if (byte & 0x20) byte|=0xe0;
							else byte&=0x1f;
							last_pixel+=
							    (((Int16)byte<<8)+byte2);
						}
					}
					else {				// 7 bit delta
						if (byte & 0x40) byte|=0xc0;
						last_pixel+=(signed char)byte;
					}
					*ptr++=(Uint16)last_pixel;
					++start;
				}
			}
			else {
				while (start<end) {
					Uint16 u=readUint16();
					if (!istr) break;
					*ptr++=u;
					++start;
				}
			}

			// Pad the last "empty" part of the line ...
			for (j=end; j<columns; j++) *ptr++=0;

			++currentrow;

			return (istr && istr->good()) ? columns : 0;
		}

	const Uint16 *getBuffer(void)		{ return linebuffer; }
	size_t getBufferCount(void) const	{ return columns; }

	int good(void) const	{ return istr && istr->good() && currentrow < rows; }
};

#endif // __Header_gensrc__
