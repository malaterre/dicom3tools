#include <fstream.h>

#include "basetype.h"
#include "endtype.h"
#include "bnopt.h"
#include "mesgtext.h"

#ifndef REPLACESTANDARDIOBUFSIZE
#define REPLACESTANDARDIOBUFSIZE 16384
#endif

static const char *options_output_file[] = {
	"of",
	"output-file",
	0
};

static const char *options_output_byteorder[] = {
	"byteorder",
	"endian",
	"output-byteorder",
	"output-endian",
	0
};

BinaryOutputOptions::BinaryOutputOptions(GetNamedOptions &options)
		: ErrorsInClass()
{
	filename=0;
	options.get(options_output_file,filename);
}

BinaryOutputOptionsWithByteOrder::BinaryOutputOptionsWithByteOrder(
		GetNamedOptions &options)
	: BinaryOutputOptions(options)
{
	byteorder=NoEndian;
	const char *byteorderstring=0;
	if (options.get(options_output_byteorder,byteorderstring)) {
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
BinaryOutputOptions::usage(void)
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

char *
BinaryOutputOptionsWithByteOrder::usage(void)
{
	ostrstream ostr;
	const char **ptr;
	const char *lead;

	ostr << " [";
	for (ptr=options_output_byteorder,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " big|little]";

	char *baseoptions = BinaryOutputOptions::usage();

	if (baseoptions) {
		ostr << baseoptions;
		delete[] baseoptions;
	}

	ostr << ends;

	// string deletion becomes callers responsibility ...
	return ostr.str();
}

void
BinaryOutputOptions::done(void)
{
}

BinaryOutputOpenerFromOptions::BinaryOutputOpenerFromOptions(
		GetNamedOptions &options,
		const char *filename,
		ostream &cstr)
	: ErrorsInClass()
{
	if (!filename && !options) {
		filename=options();
		++options;
	}

	if (filename) {
#ifdef USEBINARYFLAGFOROUTPUTOPENMODE
		ofstream *fstr=new ofstream(filename,
			ios::out|ios::trunc|ios::binary);
#else
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

BinaryOutputOpenerFromOptions::~BinaryOutputOpenerFromOptions()
{
	if (str) delete str;
}

BinaryOutputOpenerFromOptions::operator ostream *(void)
{
	return str;
}

