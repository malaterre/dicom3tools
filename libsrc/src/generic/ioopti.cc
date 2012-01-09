#include <fstream.h>

#include "basetype.h"
#include "ioopt.h"
#include "mesgtext.h"

#ifndef REPLACESTANDARDIOBUFSIZE
#define REPLACESTANDARDIOBUFSIZE 16384
#endif

static const char *options_input_file[] = {
	"if",
	"input-file",
	0
};

InputOptions::InputOptions(GetNamedOptions &options)
		: ErrorsInClass()
{
	filename=0;
	options.get(options_input_file,filename);
}

char *
InputOptions::usage(void)
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

void
InputOptions::done(void)
{
}

// NB. What is the significance of fstr being deleted after
// assignment to another stream ? Should the InputOpenerFromOptions
// object be required to remain in scope as long as the
// assigned to stream is still in use ??

InputOpenerFromOptions::InputOpenerFromOptions(
		GetNamedOptions &options,
		const char *filename,
		istream &cstr,
		bool raw)
	: ErrorsInClass()
{
	if (!filename && !options) {
		filename=options();
		++options;
	}

	if (filename) {
#ifdef USEBINARYFLAGFORINPUTOPENMODE
		ios::open_mode mode = raw ? (ios::in|ios::binary) : ios::in;
		ifstream *fstr=new ifstream(filename,mode);
#else
		(void)raw;
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

InputOpenerFromOptions::~InputOpenerFromOptions()
{
	if (str) delete str;
}

InputOpenerFromOptions::operator streambuf *(void)
{
	Assert(str);
	return str->rdbuf();
}

