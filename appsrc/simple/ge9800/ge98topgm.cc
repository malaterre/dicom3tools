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
	// DataGeneral is a Big Endian machine
	u =  (Uint16)buffer[0];
	u <<= 8;
	u |= (Uint16)buffer[1];
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
seekword(istream& instream,unsigned block,unsigned word)
{
	// 256 word blocks,16 bit words ...
	const unsigned offset = (block*256+word)*2;
	instream.seekg(offset,ios::beg);
	return instream.tellg();
}

static void
copy9800image(istream& instream,ostream& outstream,
	      Uint16 width,Uint16 height,Uint16 *map)
{
	unsigned i;
	Int16 last_pixel;

	last_pixel=0;
	for (i=0; i<height; ++i) {
		unsigned start;
		unsigned end;
		unsigned j;

		if (map) {
			unsigned line	= map[i];
			start	= width/2-line;
			end	= start+line*2;
		}
		else {
			start	= 0;
			end	= width;
		}

		// Pad the first "empty" part of the line ...
		for (j=0; j<start; j++) write16little(outstream,0);

		// Copy the middle of the line (compressed or uncompressed)
		while (start<end) {
			unsigned char byte;
			instream.read((char *)&byte,1);
			if (!instream) {
				cerr << "Premature EOF row " << i
				     << " col " << start
				     << "\n" << flush;
				break;
			}
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
				instream.read((char *)&byte,1);
				if (!instream) break;
				last_pixel+=byte;
			}
			write16little(outstream,(Uint16)last_pixel & 0x0fff);
			++start;
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

	(void)seekword(cin,0,35);
	unsigned imageheaderblock=read16(cin);

	(void)seekword(cin,0,37);
	unsigned mapblock=read16(cin);

	(void)seekword(cin,0,38);
	unsigned datablock=read16(cin);

	if (imageheaderblock)
		imageheaderblock=(imageheaderblock-1)/256;
	else
		imageheaderblock=2;

	(void)seekword(cin,imageheaderblock,12);
	unsigned grouptype=read16(cin);		// (2=scout,3=std,4=dyn,5=arr)

	(void)seekword(cin,imageheaderblock,123);
	unsigned imagesize=read16(cin);

	(void)seekword(cin,imageheaderblock,131);
	unsigned detectorsperview=read16(cin);	// detectors per view

	(void)seekword(cin,imageheaderblock,136);
	unsigned viewsperscan=read16(cin);	// views/scan

	(void)seekword(cin,imageheaderblock,174);
	unsigned usemap=read16(cin);		// 1=y,2=n

	(void)seekword(cin,imageheaderblock,217);
	unsigned filetype=read16(cin);		// (1=prospective,2=scout,
						//  3=retrospective,4=segmented,
						//  5=screensave,6=plot)

	if (mapblock)
		mapblock=(mapblock-1)/256;
	else
		mapblock=(grouptype == 5) ? 9 : 4;

	if (datablock)
		datablock=(datablock-1)/256;
	else
		datablock=(grouptype == 5) ? 11 : 6;

	unsigned height;
	unsigned width;

	if (filetype == 2) {	// scout
		width=detectorsperview;
		height=viewsperscan;
	}
	else {
		width=imagesize;
		height=imagesize;
	}

#if	PNMBIGRAW
	cout << "P5\n" << width << " " << height << "\n"
		    << 4095 << "\n";
#else /* PNMBIGRAW */
	cout << "P2\n" << width << " " << height << "\n"
		    << 4095 << "\n";
#endif /* PNMBIGRAW */

	{
		// Find the start of the perimeter encoding map ...

		if (usemap == 1 && seekword(cin,mapblock,0) == OurStreamPos(EOF)) {
			cerr << "Map seek failed\n" << flush;
			return 1;
		}
		else {
			// Read in the map ...

			Uint16 *map;
			if (usemap == 1) {
				if ((map=new Uint16[height]) == 0) {
					cerr << "Can't create map\n" << flush;
					return 1;
				}
				unsigned i;
				for (i=0; i<height; ++i)
					map[i]=read16(cin);
				if (!cin) {
					cerr << "Map read failed\n" << flush;
					return 1;
				}
			}
			else {
				map=0;
			}
			{
				// Decompress and output the image data ...

				if (seekword(cin,datablock,0)
				    == OurStreamPos(EOF)) {
					cerr << "Image seek failed\n" << flush;
					return 1;
				}
				else {
					copy9800image(cin,cout,
					      width,height,map);
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
			}
		}
	}

	return 0;
}

