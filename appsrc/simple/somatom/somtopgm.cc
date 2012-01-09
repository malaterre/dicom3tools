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
	// Vax is a Little Endian machine
	u =  (Uint16)buffer[1];
	u <<= 8;
	u |= (Uint16)buffer[0];
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

	const unsigned binhdrblock	= 0;
	const unsigned datablock	= 2048;

	(void)seekword(cin,binhdrblock,66);
	char sizeflag;
	(void)cin.get(sizeflag);

	unsigned rows,cols;
	switch (sizeflag) {
		case 'B':	rows=cols=256; break;
		case 'C':	rows=cols=512; break;
		default:
				cerr << "Bad size <" << sizeflag
				     << ">\n" << flush;
				return 1;
	}

#if	PNMBIGRAW
	cout << "P5\n" << cols << " " << rows << "\n" << 65535 << "\n";
#else /* PNMBIGRAW */
	cout << "P2\n" << cols << " " << rows << "\n" << 65535 << "\n";
#endif /* PNMBIGRAW */

	if (seekword(cin,datablock,0) == OurStreamPos(EOF)) {
		cerr << "Image seek failed\n" << flush;
		return 1;
	}
	else {
		unsigned row;
		for (row=0; row<rows; ++row) {
			unsigned col;
			for (col=0; col<cols; ++col) {
				Uint16 u=read16(cin);
				write16little(cout,u);
			}
		}
		if (!cin) {
			cerr << "Image read failed\n" << flush;
			return 1;
		}
		if (!cout.good()) {
			cerr << "Image write failed\n" << flush;
			return 1;
		}
	}

	return 0;
}

