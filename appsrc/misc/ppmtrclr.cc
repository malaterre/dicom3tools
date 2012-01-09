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

int
main(int argc,char **argv)
{
	Assert(0);	// this just doesn't work ... transformed Cb and Cr values will not fit into 8 bits

	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	InputOptions		input_options(options);
	OutputOptions	 	output_options(options);

	bool verbose = options.get("verbose") || options.get("v");
	
	bool transformInForwardDirection = options.get("forward") || !options.get("inverse");
	
	int transformChoice = 1;	// RCT
	if (options.get("rct")) {
		transformChoice = 1;	// RCT
	}
	else if (options.get("ict")) {
		transformChoice = 2;	// ICT
	}

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
			<< " [-rct|-ict]"
			<< " [-forward|inverse]"
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

	unsigned samplesperpixel;
	switch (format) {
		case '6':	// raw PPM
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
		log << EMsgDC(OnlyOneBytePerChannelSupported) << endl;
		return 1;
	}

	out << "P6" << "\n";
	out << width << " " << height << "\n";
	out << 255 << "\n";

	// Last whitespace discarded ... at start of data

	size_t rowlength=width*3;		// always one byte per channel, 3 samples per pixel
	char *buffer=new char[rowlength];
	Assert(buffer);

	while (height--) {
		in.read(buffer,rowlength);
		if (!in || in.gcount() != rowlength) {
			log << EMsgDC(ReadFailed) << endl;
			return 1;
		}
		char *ptr = buffer;
		if (transformInForwardDirection) {
			if (transformChoice == 1) {				// RCT
				for (int column=0; column<width; ++column) {
					short r = *ptr;
					short g = *(ptr+1);
					short b = *(ptr+2);
					*ptr++ = (char)((r + 2*g + b)/4);
					*ptr++ = (char)(b-g);
					*ptr++ = (char)(r-g);
				}
			}
			else if (transformChoice == 2) {		// ICT
				for (int column=0; column<width; ++column) {
					float r = *ptr;
					float g = *(ptr+1);
					float b = *(ptr+2);
					*ptr++ = (char)( 0.29900*r + 0.58700*g + 0.11400*b);
					*ptr++ = (char)(-0.16875*r - 0.33126*g + 0.50000*b);
					*ptr++ = (char)( 0.50000*r - 0.41869*g - 0.08131*b);
				}
			}
			else {
				Assert(0);
			}
		}
		else {
			if (transformChoice == 1) {				// RCT
				short signExtensionMask = (-1) << 8;	// since we do not know how long a short actually is
				for (int column=0; column<width; ++column) {
					short y = *ptr;
					short cb = *(ptr+1);			// ? need to sign extend
					if (cb&0x80) cb = cb | signExtensionMask;
					short cr = *(ptr+2);			// ? need to sign extend
					if (cr&0x80) cr|= cr | signExtensionMask;
					//short g = y - (cr + cb)/4;
					short floored = (cr + cb);
					floored = (floored < 0 && (floored%4) != 0) ? (floored/4)-1 : (floored/4);	// i.e., -2.25 is -3, not -2
					short g = y - floored;
					*ptr++ = (char)(cr + g);
					*ptr++ = (char)(g);
					*ptr++ = (char)(cb + g);
				}
			}
			else if (transformChoice == 2) {		// ICT
				for (int column=0; column<width; ++column) {
					float y = *ptr;
					float cb = *(ptr+1);
					float cr = *(ptr+2);
					*ptr++ = (char)round(y + 1.40200*cr);
					*ptr++ = (char)round(y - 0.34413*cb - 0.71414*cr);
					*ptr++ = (char)round(y + 1.77200*cb);
				}
			}
			else {
				Assert(0);
			}
		}
		out.write(buffer,rowlength);
		if (!out) {
			log << EMsgDC(WriteFailed) << endl;
			return 1;
		}
	}

	return 0;
}

