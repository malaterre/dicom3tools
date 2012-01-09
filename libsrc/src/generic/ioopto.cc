#include <fstream.h>

#include "basetype.h"
#include "ioopt.h"
#include "mesgtext.h"

#ifndef REPLACESTANDARDIOBUFSIZE
#define REPLACESTANDARDIOBUFSIZE 16384
#endif

static const char *options_output_file[] = {
	"of",
	"output-file",
	0
};

OutputOptions::OutputOptions(GetNamedOptions &options)
		: ErrorsInClass()
{
	filename=0;
	options.get(options_output_file,filename);
}

char *
OutputOptions::usage(void)
{
	ostrstream ostr;
	const char **ptr;
	const char *lead;

	ostr << " [";
	for (ptr=options_output_file,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " " << MMsgDC(OutputFile) << "]";

	ostr << ends;

	// string deletion becomes callers responsibility ...
	return ostr.str();
}

void
OutputOptions::done(void)
{
}

OutputOpenerFromOptions::OutputOpenerFromOptions(
		GetNamedOptions &options,
		const char *filename,
		ostream &cstr,
		bool raw)
	: ErrorsInClass()
{
	if (!filename && !options) {
		filename=options();
		++options;
	}

	if (filename) {
#ifdef USEBINARYFLAGFOROUTPUTOPENMODE
		ios::open_mode mode = raw
			? (ios::out|ios::trunc|ios::binary)
			: (ios::out|ios::trunc);
		ofstream *fstr=new ofstream(filename,mode);
#else
		(void)raw;
		ofstream *fstr=new ofstream(filename,ios::out|ios::trunc);
#endif
		if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
			str=0;
		}
		else
			str=fstr;
	}
	else {
		streambuf *sbuf  = cstr.rdbuf();
		str=new ostream(sbuf);
		// default ANSI cin/cout is unbuffered, slow, so ...
//		char *buf=new char[REPLACESTANDARDIOBUFSIZE];
//		if (buf) sbuf->setbuf(buf,REPLACESTANDARDIOBUFSIZE);
		// this buffer will never be delete[]'d but doesn't
		// matter ... how often would we be reusing cin/cout ?
	}

	if (!str || !*str) {
		errorstream << AMsgDC(FileWriteOpenFailed);
		if (filename) errorstream <<" - \"" << filename << "\"";
		errorstream << endl;
		good_flag=false;
	}
}

OutputOpenerFromOptions::~OutputOpenerFromOptions()
{
	if (str) delete str;
}

OutputOpenerFromOptions::operator streambuf *(void)
{
	Assert(str);
	return str->rdbuf();
}

