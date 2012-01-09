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
	// Pace is a Big Endian machine
	u =  (Uint16)buffer[0];
	u <<= 8;
	u |= (Uint16)buffer[1];
	return u;
}

static void
write16little(ostream& outstream,Uint16 u)
{
#ifdef	PNMBIGRAW
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
seekword(istream& instream,unsigned offset,unsigned word)
{
	// byte offset from 0 ...
	instream.seekg(offset+word,ios::beg);
	return instream.tellg();
}

static void
copypaceimage(istream& instream,ostream& outstream,
	      Uint16 width,Uint16 height)
{
// NB. the exclusive or with 0x8000 makes the signed Pace values unsigned
// which is what the PGM convention is ... just omit the ^0x8000
// everywhere if you want the data left signed.

	unsigned i;
	Int16 pixel=0;
	enum Mode { Difference, Reference } mode = Reference;
	for (i=0; i<height*width;) {
		unsigned char byte;
		instream.read((char *)&byte,1);
		if (!instream) break;
		if (byte == 0x80) {		// Mode switch
			if (mode == Difference)
				mode=Reference;
			else
				mode=Difference;
		}
		else if (byte == 0x81) {	// Run length flag
			instream.read((char *)&byte,1);
			if (!instream) break;
			unsigned repeat=byte;
			i+=repeat;
			while (repeat--) write16little(outstream,pixel^0x8000);
		}
		else {
			if (mode == Difference) {
				pixel+=(signed char)byte;
			}
			else {
				pixel=byte<<8;
				instream.read((char *)&byte,1);
				if (!instream) break;
				pixel|=byte;
			}
			write16little(outstream,pixel^0x8000);
			++i;
		}
	}
	if (!instream) cerr << "Premature EOF byte " << i << "\n" << flush;
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

	unsigned imageheaderblock=768;
	unsigned datablock=1792;

	(void)seekword(cin,imageheaderblock,416);
	unsigned width=read16(cin);

	(void)seekword(cin,imageheaderblock,418);
	unsigned height=read16(cin);

	// Only use maxval of 4095 here if you leave the data signed,
	// ie. didn't xor with 0x8000

#ifdef	PNMBIGRAW
	cout << "P5\n" << width << " " << height << "\n"
		    << 65535 << "\n";
#else /* PNMBIGRAW */
	cout << "P2\n" << width << " " << height << "\n"
		    << 65535 << "\n";
#endif /* PNMBIGRAW */

	// Decompress and output the image data ...

	if (seekword(cin,datablock,0) == OurStreamPos(EOF)) {
		cerr << "Image seek failed\n" << flush;
		return 1;
	}
	else {
		copypaceimage(cin,cout,width,height);
		if (!cin) {
			cerr
			  << "Image read failed\n"
			  << flush;
			return 1;
		}
		if (!cout.good()) {
			cerr
			  << "Image write failed\n"
			  << flush;
			return 1;
		}
	}

	return 0;
}

