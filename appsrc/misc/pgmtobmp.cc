#include <ctype.h>

#include "basetype.h"
#include "txstream.h"
#include "ioopt.h"
#include "mesgtext.h"

static
char
pnm_getc(istream &istr)		// 1st non-comment, non-whitespace, 0 on failure
{
	char c;
	while (istr.get(c) && (c == ' ' || c == '\t' || c == '\n'));
	if (!istr) return 0;
	if (c == '#') {		// skip comments
		while (istr.get(c) && c != '\n');
		if (!istr) return 0;
		while (istr.get(c) && (c == ' ' || c == '\t' || c == '\n'));
		if (!istr) return 0;
	}
	return c;
}

static
int
pnm_getu(istream &istr)		// get unsigned value, -1 on failure
{
	char c=pnm_getc(istr);	// skips comments and whitespace
	if(!c || !isdigit(c)) return -1;
	unsigned value=0;
	while (c && isdigit(c)) {
		value*=10;
		value+=(c-'0');
		istr.get(c);
	}
	// discards single trailing white space
	return value;
}

static void
bmp_put8u(ostream &out,unsigned char byte)
{
	out.write((char *)&byte,1);
}

static void
bmp_put16u(ostream &out,Uint16 word)
{
	// little endian
	unsigned char byte;
	byte=word;
	out.write((char *)&byte,1);
	byte=word>>8;
	out.write((char *)&byte,1);
}

static void
bmp_put32u(ostream &out,Uint32 word)
{
	// little endian
	unsigned char byte;
	byte=word;
	out.write((char *)&byte,1);
	byte=word>>8;
	out.write((char *)&byte,1);
	byte=word>>16;
	out.write((char *)&byte,1);
	byte=word>>24;
	out.write((char *)&byte,1);
}

static void
bmp_writeFixedGrayscaleIdentityColormapHeader(ostream &out,int width,int height,int padBytes)
{
	// See amongst other references "http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html"
	
	out.put('B'); out.put('M');
	Uint32 pixelDataOffset
		= 54				/* file header + info header fixed length */
		+ 256 * 4;			/* 256 entry palette  of R G B and reserved (alpha) single byte values */
	Uint32 fileLength
		= pixelDataOffset
		+ (width+padBytes) * height;	/* the uncompressed pixel data */
	bmp_put32u(out,fileLength);
	bmp_put16u(out,0);		/* reserved 1 */
	bmp_put16u(out,0);		/* reserved 2 */
	bmp_put32u(out,pixelDataOffset);
	
	bmp_put32u(out,40);		/* the fixed size of the info header that we are now writing */
	bmp_put32u(out,width);
	bmp_put32u(out,height);
	bmp_put16u(out,1);		/* number of planes is 1 */
	bmp_put16u(out,8);		/* number of bits is 8 */
	bmp_put32u(out,0);		/* compression type is none */
	bmp_put32u(out,0);		/* size of image in bytes (may be zero) */
	bmp_put32u(out,0);		/* horizontal pixels per meter is unspecified */
	bmp_put32u(out,0);		/* vertical pixels per meter is unspecified */
	bmp_put32u(out,0);		/* colors used is unspecified (will be derived from bit count) */
	bmp_put32u(out,0);		/* number of colors that are important (all) */
	
	/* write 256 entry identity grayscale palette (content same as index) */
	int i;
	for (i=0; i<256; ++i) {
		bmp_put8u(out,i);	/* R value */
		bmp_put8u(out,i);	/* G value */
		bmp_put8u(out,i);	/* B value */
		bmp_put8u(out,0);	/* reserved (alpha ?) value */
	}
}

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	InputOptions		input_options(options);
	OutputOptions	 	output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	input_options.done();
	output_options.done();
	options.done();

	InputOpenerFromOptions input_opener(
		options,input_options.filename,cin);
	OutputOpenerFromOptions output_opener(
		options,output_options.filename,cout);

	cerr << input_options.errors();
	cerr << output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!input_options.good()
	 || !output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< input_options.usage()
			<< output_options.usage()
			<< " [-v|-verbose]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	istream in(input_opener);
	ostream out(output_opener);

	TextOutputStream log (cerr);

	char magic=pnm_getc(in);
	char format;
	in.get(format);

	if (!in || magic != 'P') {
		log << EMsgDC(UnrecognizedFormat) << " - 'Pn' "
		    << MMsgDC(NotFound) << endl;
		return 1;
	}

	switch (format) {
		case '1':	// text PBM
		case '2':	// text PGM
		case '3':	// text PPM
		case '4':	// raw PBM
		case '6':	// raw PPM
			log << EMsgDC(Unsupported) << " - 'P"
			    << format << "'"
			    << endl;
			return 1;
			//break;
		case '5':	// raw PGM
			break;
		default:
			log << EMsgDC(UnrecognizedFormat) << " - 'P"
			    << format << "'"
			    << endl;
			return 1;
			//break;
	}

	int width = pnm_getu(in);
	int height = pnm_getu(in);
	int maxval = pnm_getu(in);

	if (verbose) {
		log << "format P" << format << endl;
		log << "width " << dec << width << endl;
		log << "height " << dec << height << endl;
		log << "maxval " << dec << maxval << endl;
	}

	if (!in || width == -1 || height == -1 || maxval == -1) {
		log << EMsgDC(UnrecognizedFormat) << endl;
		return 1;
	}
	
	if (maxval > 255) {
			log << EMsgDC(Unsupported) << " - maxval more than 256, is "
			    << maxval
			    << endl;
			return 1;
	}

	int padBytes = 4 - (width % 4);		/* BMP file rows are encoded with zero padding to 4 byte boundaries */
	if (padBytes == 4) padBytes = 0;
	if (verbose) {
		log << "padBytes " << dec << padBytes << endl;
	}
	
	bmp_writeFixedGrayscaleIdentityColormapHeader(out,width,height,padBytes);

	// Last whitespace discarded ... at start of data
	
	// BMP files are encoded bottom to top, not top to bottom
	// so read all rows in and write in reverse order

	char *buffer=new char[width*height];
	Assert(buffer);

	char *ptr = buffer;
	int rowsLeft = height;
	while (rowsLeft--) {
		in.read(ptr,width);
		if (!in || in.gcount() != width) {
			log << EMsgDC(ReadFailed) << endl;
			return 1;
		}
		ptr+=width;
	}
	
	ptr = buffer + width*height;
	rowsLeft = height;
	while (rowsLeft--) {
		ptr-=width;
		out.write(ptr,width);
		int i;
		for (i=0; i<padBytes; ++i) {
			bmp_put8u(out,0);		/* pad with 0 */
		}
		if (!out) {
			log << EMsgDC(WriteFailed) << endl;
			return 1;
		}
	}

	return 0;
}

