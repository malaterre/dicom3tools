#include <fstream.h>

#include "basetype.h"
#include "endtype.h"
#include "bnopt.h"
#include "mesgtext.h"

#ifndef REPLACESTANDARDIOBUFSIZE
#define REPLACESTANDARDIOBUFSIZE 16384
#endif

static const char *options_input_file[] = {
	"if",
	"input-file",
	0
};

static const char *options_input_byteorder[] = {
	"input-byteorder",
	"input-endian",
	0
};

BinaryInputOptions::BinaryInputOptions(GetNamedOptions &options)
		: ErrorsInClass()
{
	filename=0;
	options.get(options_input_file,filename);
}

BinaryInputOptionsWithByteOrder::BinaryInputOptionsWithByteOrder(
		GetNamedOptions &options)
	: BinaryInputOptions(options)
{
	byteorder=NoEndian;
	const char *byteorderstring=0;
	if (options.get(options_input_byteorder,byteorderstring)) {
		if (strcmp(byteorderstring,"big") == 0)
			byteorder=BigEndian;
		else if (strcmp(byteorderstring,"little") == 0)
			byteorder=LittleEndian;
		else {
			errorstream << EMsgDC(BadByteOrder) << endl;
			good_flag=false;
		}
	}
}

char *
BinaryInputOptions::usage(void)
{
	ostrstream ostr;
	const char **ptr;
	const char *lead;

	ostr << " [";
	for (ptr=options_input_file,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " " << MMsgDC(InputFile) << "]";

	ostr << ends;

	// string deletion becomes callers responsibility ...
	return ostr.str();
}

char *
BinaryInputOptionsWithByteOrder::usage(void)
{
	ostrstream ostr;
	const char **ptr;
	const char *lead;

	ostr << " [";
	for (ptr=options_input_byteorder,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " big|little]";

	char *baseoptions = BinaryInputOptions::usage();

	if (baseoptions) {
		ostr << baseoptions;
		delete[] baseoptions;
	}

	ostr << ends;

	// string deletion becomes callers responsibility ...
	return ostr.str();
}

void
BinaryInputOptions::done(void)
{
}

// NB. What is the significance of fstr being deleted after
// assignment to another stream ? Should the BinaryInputOpenerFromOptions
// object be required to remain in scope as long as the
// assigned to stream is still in use ??

BinaryInputOpenerFromOptions::BinaryInputOpenerFromOptions(
		GetNamedOptions &options,
		const char *filename,
		istream &cstr)
	: ErrorsInClass()
{
	if (!filename && !options) {
		filename=options();
		++options;
	}

	if (filename) {
#ifdef USEBINARYFLAGFORINPUTOPENMODE
		ifstream *fstr=new ifstream(filename,ios::in|ios::binary);
#else
		ifstream *fstr=new ifstream(filename);
#endif
		if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
			str=0;
		}
		else
			str=fstr;
	}
	else {
		streambuf *sbuf  = cstr.rdbuf();
		str=new istream(sbuf);
		// default ANSI cin/cout is unbuffered, slow, so ...
//		char *buf=new char[REPLACESTANDARDIOBUFSIZE];
//		if (buf) sbuf->setbuf(buf,REPLACESTANDARDIOBUFSIZE);
		// this buffer will never be delete[]'d but doesn't
		// matter ... how often would we be reusing cin/cout ?
	}

	if (!str || !*str) {
		errorstream << AMsgDC(FileReadOpenFailed);
		if (filename) errorstream <<" - \"" << filename << "\"";
		errorstream << endl;
		good_flag=false;
	}
}

BinaryInputOpenerFromOptions::~BinaryInputOpenerFromOptions()
{
	if (str) delete str;
}

BinaryInputOpenerFromOptions::operator istream *(void)
{
	return str;
}

