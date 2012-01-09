#include <iostream.h>

#include "basetype.h"

#ifndef PNMBIGRAW
#define PNMBIGRAW 1
#endif

static Uint16
read16(istream& instream)
{
	Uint16 u;
	unsigned char buffer[2];
	instream.read((char *)buffer,2);
	// Sparc is a Big Endian machine
	u =  (Uint16)buffer[0];
	u <<= 8;
	u |= (Uint16)buffer[1];
	return u;
}

static Uint32
read32(istream& instream)
{
	Uint32 u;
	unsigned char buffer[4];
	instream.read((char *)buffer,4);
	// Sparc is a Big Endian machine
	u =  (Uint16)buffer[0];
	u <<= 8;
	u |= (Uint16)buffer[1];
	u <<= 8;
	u |= (Uint16)buffer[2];
	u <<= 8;
	u |= (Uint16)buffer[3];
	return u;
}

static void
write16little(ostream& outstream,Uint16 u)
{
#if	PNMBIGRAW
	unsigned char buffer[2];
	// Output is little endian
	buffer[0]=(unsigned char)(u);
	buffer[1]=(unsigned char)(u>>8);
	outstream.write((char *)buffer,2);
#else /* PNMBIGRAW */
	outstream << u << "\n";
#endif /* PNMBIGRAW */
}

static OurStreamPos
seekword(istream& instream,unsigned block,unsigned byte)
{
	// byte displacements ...
	const unsigned offset = block+byte;
	instream.seekg(offset,ios::beg);
	return instream.tellg();
}

static void
copygenesisimage(istream& instream,ostream& outstream,
	Uint16 width,Uint16 height,Uint16 depth,Uint16 compress,
	Uint16 *map_left,Uint16 *map_wide)
{
	(void)depth;
	unsigned row;
	Int16 last_pixel=0;
	for (row=0; row<height; ++row) {
		unsigned j;
		unsigned start;
		unsigned end;

		if (compress == 2 || compress == 4) { // packed or compacked
			start=map_left[row];
			end=start+map_wide[row];
		}
		else {
			start=0;
			end=width;
		}
		// Pad the first "empty" part of the line ...
		for (j=0; j<start; j++) write16little(outstream,0);

		if (compress == 3 || compress == 4) { // compressed or compacked
			while (start<end) {
				unsigned char byte;
				instream.read((char *)&byte,1);
				if (!instream) return;
				if (byte & 0x80) {
					unsigned char byte2;
					instream.read((char *)&byte2,1);
					if (!instream) return;
					if (byte & 0x40) {	// next word
						instream.read((char *)&byte,1);
						if (!instream) return;
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
				write16little(outstream,(Uint16)last_pixel);
				++start;
			}
		}
		else {
			while (start<end) {
				Uint16 u=read16(instream);
				if (!instream) return;
				write16little(outstream,u);
				++start;
			}
		}

		// Pad the last "empty" part of the line ...
		for (j=end; j<width; j++) write16little(outstream,0);
	}
}

int
main(int argc,char *argv[])
{
	if (argc != 1) {
		cerr << "Usage: " << argv[0] << " < infile > outfile\n" << flush;
		return 1;
	}

	// default for some environments is unbuffered which is slow++
	// the following improves performance 25 fold with libg++ ...

//	const unsigned bufsize=4096;
//	streambuf *in  = cin.rdbuf();
//	streambuf *out = cout.rdbuf();
//	static char inbuf[bufsize];
//	static char outbuf[bufsize];
//	in->setbuf(inbuf,sizeof(inbuf));
//	out->setbuf(outbuf,sizeof(outbuf));

	const unsigned filehdrblock	= 0;

	(void)seekword(cin,filehdrblock,4);
	unsigned datablock=read32(cin);

	(void)seekword(cin,filehdrblock,8);
	unsigned width=read32(cin);

	(void)seekword(cin,filehdrblock,12);
	unsigned height=read32(cin);

	(void)seekword(cin,filehdrblock,16);
	unsigned depth=read32(cin);

	(void)seekword(cin,filehdrblock,20);
	unsigned compress=read32(cin);

	(void)seekword(cin,filehdrblock,64);
	unsigned mapblock=read32(cin) + filehdrblock;

	(void)seekword(cin,filehdrblock,80);
	unsigned histblock=read32(cin) + filehdrblock;
	(void)seekword(cin,filehdrblock,84);
	unsigned histlng=read32(cin) + filehdrblock;

	unsigned int maxval=0;
	if (histlng) {
		(void)seekword(cin,histblock,12);
		maxval=read16(cin);
	}
	if (maxval == 0) maxval=(1<<depth)-1;
	if (maxval <= 255) maxval=256;	// force word not byte format

#if	PNMBIGRAW
	cout << "P5\n" << width << " " << height << "\n"
		    << maxval << "\n";
#else /* PNMBIGRAW */
	cout << "P2\n" << width << " " << height << "\n"
		    << maxval << "\n";
#endif /* PNMBIGRAW */

	{
		// Read the perimeter encoding map ...

		Uint16 map_left[512];	// max resolution 512
		Uint16 map_wide[512];

		if (compress == 2 || compress == 4) { // packed or compacked
			if (seekword(cin,mapblock,0) == OurStreamPos(EOF)) {
				cerr << "Map seek failed\n" << flush;
				return 1;
			}
			else {
				// Read the map ...

				unsigned i;
				for (i=0; i<height; ++i) {
					map_left[i]=read16(cin);
					map_wide[i]=read16(cin);
				}
				if (!cin) {
					cerr << "Map read failed\n" << flush;
					return 1;
				}
			}
		}

		// Decompress and output the image data ...

		if (seekword(cin,datablock,0) == OurStreamPos(EOF)) {
			cerr << "Image seek failed\n" << flush;
			return 1;
		}
		else {
			copygenesisimage(cin,cout,
				width,height,depth,compress,
				map_left,map_wide);
			if (!cin) {
				cerr << "Image read failed\n" << flush;
				return 1;
			}
			if (!cout.good()) {
				cerr << "Image write failed\n" << flush;
				return 1;
			}
		}
	}

	return 0;
}

