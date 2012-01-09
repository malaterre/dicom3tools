#include <fstream.h>

#include "basetype.h"
#include "dcopt.h"
#include "transynu.h"
#include "mesgtext.h"

#ifndef REPLACESTANDARDIOBUFSIZE
#define REPLACESTANDARDIOBUFSIZE 16384
#endif

static const char *options_input_file[] = {
	"if",
	"input-file",
	0
};

static const char *options_input_nolengthtoend[] = {
	"input-nolengthtoend",
	0
};

static const char *options_input_nometaheader[] = {
	"input-nometa",
	0
};

static const char *options_input_ignoreoutofordertags[] = {
	"ignoreoutofordertags",
	0
};

static const char *options_input_useUSVRForLUTDataIfNotExplicit[] = {
	"usvrlutdata",
	0
};

static const char *options_input_transfersyntax[] = {
	"input-transfersyntax",
	"input-ts",
	0
};

static const char *options_input_defaulttransfersyntax[] = {
	"input-default",
	0
};

static const char *options_input_byteorder[] = {
	"input-byteorder",
	"input-endian",
	0
};

static const char *options_input_vr[] = {
	"input-vr",
	0
};

DicomInputOptions::DicomInputOptions(GetNamedOptions &options)
		: ErrorsInClass()
{
	filename=0;
	options.get(options_input_file,filename);

	usemetaheader=!(options.get(options_input_nometaheader));

	uselengthtoend=!(options.get(options_input_nolengthtoend));

	ignoreoutofordertags=options.get(options_input_ignoreoutofordertags);
	
	useUSVRForLUTDataIfNotExplicit=options.get(options_input_useUSVRForLUTDataIfNotExplicit);

	transfersyntaxuid=0;
	options.get(options_input_transfersyntax,transfersyntaxuid);

	if (options.get(options_input_defaulttransfersyntax)) {
		if (transfersyntaxuid) {
			errorstream << EMsgDC(AlreadyHaveTransferSyntax)
				    << endl;
			good_flag=false;
		}
		else
			transfersyntaxuid=DefaultTransferSyntaxUID;
	}

	const char *byteorder=0;
	if (options.get(options_input_byteorder,byteorder)) {
		if (strcmp(byteorder,"big") != 0
		 && strcmp(byteorder,"little") != 0) {
			errorstream << EMsgDC(BadByteOrder) << endl;
			good_flag=false;
		}
	}
	if (byteorder && transfersyntaxuid) {
		errorstream << EMsgDC(AlreadyHaveTransferSyntax) << endl;
		good_flag=false;
	}

	const char *vr=0;
	if (options.get(options_input_vr,vr)) {
		if (strcmp(vr,"explicit") != 0
		 && strcmp(vr,"implicit") != 0) {
			errorstream << EMsgDC(BadByteOrder) << endl;
			good_flag=false;
			vr=0;
		}
	}
	if (vr && transfersyntaxuid) {
		errorstream << EMsgDC(AlreadyHaveTransferSyntax) << endl;
		good_flag=false;
		vr=0;
	}

	if ((byteorder && !vr) || (vr && !byteorder)) {
		errorstream << EMsgDC(NeedBothVRAndByteOrder) << endl;
		good_flag=false;
	}

	if (!transfersyntaxuid && byteorder && vr) {
		if (strcmp(byteorder,"little") == 0) {
			if (strcmp(vr,"explicit") == 0)
				transfersyntaxuid=
					ExplicitVRLittleEndianTransferSyntaxUID;
			else
				transfersyntaxuid=
					ImplicitVRLittleEndianTransferSyntaxUID;
		}
		else {
			if (strcmp(vr,"explicit") == 0)
				transfersyntaxuid=
					ExplicitVRBigEndianTransferSyntaxUID;
			else {
				errorstream << EMsgDC(BadImplicitBig) << endl;
				good_flag=false;
			}
		}
	}
}

char *
DicomInputOptions::usage(void)
{
	ostrstream ostr;
	const char **ptr;
	const char *lead;

	ostr << " [";
	for (ptr=options_input_nolengthtoend,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_input_nometaheader,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_input_ignoreoutofordertags,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_input_useUSVRForLUTDataIfNotExplicit,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_input_transfersyntax,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " " << MMsgDC(UID) << "]";

	ostr << " [";
	for (ptr=options_input_defaulttransfersyntax,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_input_byteorder,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " big|little]";

	ostr << " [";
	for (ptr=options_input_vr,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " implicit|explicit]";

	ostr << " [";
	for (ptr=options_input_file,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " " << MMsgDC(InputFile) << "]";

	ostr << ends;

	// string deletion becomes callers responsibility ...
	return ostr.str();
}

void
DicomInputOptions::done(void)
{
}

// NB. What is the significance of fstr being deleted after
// assignment to another stream ? Should the DicomInputOpenerFromOptions
// object be required to remain in scope as long as the
// assigned to stream is still in use ??

DicomInputOpenerFromOptions::DicomInputOpenerFromOptions(
		GetNamedOptions &options,
		const char *filename,
		istream &cstr)
	: ErrorsInClass()
{
	if (!filename && !options) {
		filename=options();
		++options;
	}
	filenameUsed=filename;

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

DicomInputOpenerFromOptions::DicomInputOpenerFromOptions(
		GetNamedOptions &options,
		const char *filename)
	: ErrorsInClass()
{
	if (!filename && !options) {
		filename=options();
		++options;
	}
	filenameUsed=filename;

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
		str=0;
	}

	if (!str || !*str) {
		errorstream << AMsgDC(FileReadOpenFailed);
		if (filename) errorstream <<" - \"" << filename << "\"";
		errorstream << endl;
		good_flag=false;
	}
}

DicomInputOpenerFromOptions::~DicomInputOpenerFromOptions()
{
#ifdef TRACE_DESTRUCTORS
cerr << "DicomInputOpenerFromOptions::~DicomInputOpenerFromOptions" << endl;
#endif
	if (str) delete str;
}

DicomInputOpenerFromOptions::operator istream *(void)
{
	return str;
}

