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

	unsigned samplesperpixel;
	switch (format) {
		case '1':	// text PBM
		case '2':	// text PGM
		case '3':	// text PPM
		case '4':	// raw PBM
			log << EMsgDC(Unsupported) << " - 'P"
			    << format << "'"
			    << endl;
			return 1;
			//break;
		case '5':	// raw PGM
			samplesperpixel=1;
			break;
		case '6':	// raw PPM
			samplesperpixel=3;
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

	// Last whitespace discarded ... at start of data

	unsigned wordlength = (maxval <= 255) ? 1 : 2;
	size_t rowlength=width*wordlength*samplesperpixel;
	char *buffer=new char[rowlength];
	Assert(buffer);

	while (height--) {
		in.read(buffer,rowlength);
		if (!in || in.gcount() != rowlength) {
			log << EMsgDC(ReadFailed) << endl;
			return 1;
		}
		out.write(buffer,rowlength);
		if (!out) {
			log << EMsgDC(WriteFailed) << endl;
			return 1;
		}
	}

	return 0;
}

