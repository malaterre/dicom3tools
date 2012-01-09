#ifndef __Header_UnEncap__
#define __Header_UnEncap__

class UnencapsulatePixelData : public SourceBase<unsigned char> {
private:
	istream *istr;
	Uint32 lefttoreadthisfragment;
	int fragmentnumber;
	bool finished;
	bool bad;

	unsigned char *buffer;
	size_t maxlength;
	size_t length;

	unsigned char b[4];

	Uint16 read16(void)
		{
			Uint16 u;
			istr->read((char *)b,2);
			u =  (Uint16)b[1];
			u <<= 8;
			u |= (Uint16)b[0];
			return u;
		}

	Uint32 read32(void)
		{
			Uint32 u;
			istr->read((char *)b,4);
			u =  (Uint32)b[3];
			u <<= 8;
			u |= (Uint32)b[2];
			u <<= 8;
			u |= (Uint32)b[1];
			u <<= 8;
			u |= (Uint32)b[0];
			return u;
		}

public:
	UnencapsulatePixelData(istream& i)
		{
			istr=&i;
			fragmentnumber=-1;	// the first is the offset table
			lefttoreadthisfragment=0;
			finished=false;
			bad=!istr->good();
			buffer=new unsigned char[DEFAULTCHUNKSIZE];
			Assert(buffer);
			maxlength=DEFAULTCHUNKSIZE;
			length=0;
		}

	size_t read(void)
		{
			// We do this the hardwired way - for encapsulated data:
			// - non-pixel data is always LE, including fragment delimiters and lengths
			// - 1st item is offset table, may have zero VL
			// - other items are fragments
			// - finally sequence delimitation tag (with zero VL)
			// - each delimiter is 2 byte group,2 byte element, 4 byte VL, little endian
			// - Item tag      is (0xfffe,0xe000) (GE mistake is 0xfeff,0x00e0 or 0xe000,0xfffe)
			// - Seq delimiter is (0xfffe,0xe0dd) (GE mistake is 0xfeff,0xdde0 or 0xe0dd,0xfffe)
			// - when GE mistake is present, fragment 32 bit VL is also swapped

			length=0;

			while (!lefttoreadthisfragment && !finished && !bad) {
//cerr << "UnencapsulatePixelData::read starting fragment" << endl;
				Uint16 group=read16();
				if (!istr) { bad=true; break; }
				Uint16 element=read16();
				if (!istr) { bad=true; break; }
				Uint32 vl=read32();
				if (!istr) { bad=true; break; }
//cerr << "UnencapsulatePixelData::read fragment group=0x" << hex << group << " element=0x" << element << " length=0x" << vl << " (" << dec << vl << " dec)" << endl;
				if (group == 0xfffe || group == 0xfeff || group == 0xe000 || group == 0xe0dd) {
					if (group != 0xfffe) {
//cerr << "UnencapsulatePixelData::unexpected group (? bad byte order)=" << hex << group << dec << endl;
					}
					if (element == 0xe0dd || element == 0xdde0 || group == 0xe0dd) {	// Sequence Delimiter Tag
//cerr << "UnencapsulatePixelData::read Sequence Delimiter Tag" << endl;
						if (element != 0xe0dd) {
//cerr << "UnencapsulatePixelData::unexpected element (? bad byte order)=0x" << hex << element << dec << endl;
						}
						//Assert(vl == 0);	// just ignore it ... some Opal images have bad value here
						finished=true;
					}
					else /* if (element == 0xe000) */ {	// Item Tag
cerr << "UnencapsulatePixelData::read Item Tag length=0x" << hex << vl << " (" << dec << vl << " dec)" << endl;
						bool vlbyteorderwrong=false;
						if (element != 0xe000) {
//cerr << "UnencapsulatePixelData::unexpected element (? bad byte order)=0x" << hex << element << dec << endl;
							vlbyteorderwrong=true;
						}
						if (++fragmentnumber > 0) {
							Assert(vl);	// Zero length fragments thought not to be legal
							if (vlbyteorderwrong) {
								lefttoreadthisfragment=
									 (((Uint32)vl&0xff000000)>>24)
									+(((Uint32)vl&0x00ff0000)>>8)
									+(((Uint32)vl&0x0000ff00)<<8)
									+(((Uint32)vl&0x000000ff)<<24);
//cerr << "UnencapsulatePixelData::assuming VL also had bad byte order, using 0x" << hex << lefttoreadthisfragment << dec << endl;
							}
							else {
								lefttoreadthisfragment=vl;
							}
//cerr << "UnencapsulatePixelData::read next fragment with lefttoreadthisfragment=" << dec << lefttoreadthisfragment << endl;
						}
						else {
							// skip the offset table
//cerr << "UnencapsulatePixelData::read skip the offset table" << endl;
							Assert(vl%4 == 0);
							unsigned i=0;
							while (vl) {
								Uint32 offset=read32();
								if (!istr) { bad=true; break; }
//cerr << "UnencapsulatePixelData::read [" << i << "] offset " << offset << endl;
								vl-=4;
								++i;
							}
							//if (istr->rdbuf()->PubSeekOff(vl,ios::cur) == PubSeekBad || istr->fail())
							//	bad=true;
						}
					}
#ifdef CRAP
					else {
						// bad tag element in encapsulated data 
//cerr << "UnencapsulatePixelData::read bad tag element in encapsulated data" << endl;
						bad=true;
					}
#endif
				}
				else {
					// bad tag group in encapsulated data
//cerr << "UnencapsulatePixelData::read bad tag group in encapsulated data" << endl;
					bad=true;
				}
			}

			if (lefttoreadthisfragment && !bad) {
//cerr << "UnencapsulatePixelData::read reading with lefttoreadthisfragment=" << dec << lefttoreadthisfragment << endl;
				length=unsigned(lefttoreadthisfragment > maxlength ? maxlength : lefttoreadthisfragment);
				if (istr->read((char *)buffer,length)) {
					length=istr->gcount();
//cerr << "UnencapsulatePixelData::read read length=" << dec << length << endl;
				}
				else {
					bad=true;
					length=0;
//cerr << "UnencapsulatePixelData::read bad read" << endl;
				}
				lefttoreadthisfragment-=length;
			}

//cerr << "UnencapsulatePixelData::read returning length=" << dec << length << endl;
			return length;
		}

	const unsigned char *getBuffer(void)		{ return buffer; }
	size_t getBufferCount(void) const		{ return length; }

	int good(void) const				{ return !bad; }
};

#endif /* __Header_UnEncap__ */
