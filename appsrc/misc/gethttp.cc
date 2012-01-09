#include <ctype.h>
#include "basetype.h"
#include "ntstream.h"
#include "getoptns.h"
#include "mesgtext.h"

#ifndef REPLACESTANDARDIOBUFSIZE
#define REPLACESTANDARDIOBUFSIZE 16384
#endif

static void
choptrailing(char *p)
{
//cerr << "choptrailing: start <" << p << ">" << endl;
	char *end;
	for (end=p+strlen(p)-1;
		end >= p && (*end == 0
			|| *end == '\n'
			|| *end == '\r'
			|| *end == '\t'
			|| *end == ' ')
			; --end) {
//cerr << "choptrailing: chopping <" << *end << ">" << endl;
		*end=0;
	}
//cerr << "choptrailing: end <" << p << ">" << endl;
}

static bool
statusparse(char *src,char *&version,char *&code,char *&reason)
{
	bool success=false;

	// HTTP 1.0: Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
	// HTTP 1.0: "HTTP/" 1*DIGIT "." 1*DIGIT SP 3DIGIT SP
	if (strstr(src,"HTTP/") == src && isdigit(src[5]) && src[6] == '.' && isdigit(src[7])) {
		success=true;
		version=src+5;
		src[8]=0;

		code=src+9;
		char *p=code;
		while (isdigit(*p)) ++p;
		reason=p;
		while (*reason == ' ' || *reason == '\t') ++reason;
		*p=0;	// terminate code AFTER finding start of reason
	}
	// else HTTP 1.0: May be HTTP 0.9 simple response

	if (!success) {
		version=0;
		code=0;
		reason=0;
	}
	return success;
}

static char *
trtolower(char *p)
{
	char *self=p;
	if (p) {
		--p;
		while (*++p) if (isupper(*p)) *p=tolower(*p);
	}
	return self;
}

static bool
headerparse(char *src,char *&key,char *&args)
{
	bool success=false;

	key=src;
	while (*key == ' ' || *key == '\t') ++key;
	char *p=key;
	while (*p && (isalpha(*p) || *p == '-')) ++p;
	if (*p == ':') {
		success=true;

		*p=0;
		args=p+1;
		while (*args == ' ' || *args == '\t') ++args;
	}

	if (!success) {
		key=0;
		args=0;
	}
	return success;
}

static char *
nextword(char *start,char **next=0)
{
	while (*start == ' ' || *start == '\t') ++start;
	char *end=start;
	while (*end && *end != ' ' && *end != '\t') ++end;
	if (next) *next=*end ? end+1 : 0;
	*end=0;
	return start;
}

int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions options(argc,argv);
	bool verbose=options.get("verbose") || options.get("v");
	bool progress=options.get("progress") || options.get("p");

	const char *host;
	const char *port;
	const char *path;

	if (!options == 3) {
		host=options[0];
		port=options[1];
		path=options[2];
	}
	else
		bad=true;

	options.done();
	cerr << options.errors();

	if (!options.good() || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< " [-v|-verbose]"
			<< " [-p|-progress]"
			<< " host port path"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		return 1;
	}

	const size_t netbuflng=REPLACESTANDARDIOBUFSIZE;
	char *netbuf=new char[netbuflng];
	Assert(netbuf);
#ifdef USEBINARYFLAGFORINPUTOPENMODE
	inetstream in(host,port,"tcp",ios::in|ios::binary);
#else
	inetstream in(host,port,"tcp");
#endif
	in.setbuf(netbuf,netbuflng);
	// There is no netstream class that does bidirectional i/o ...
	ostream out(in.rdbuf());
	in.tie(&out);		// this is necessary if interactive

	if (!in.good()) {
		cerr << "Read open failed" << endl;
		return 1;
	}
	if (!out.good()) {
		cerr << "Write open failed" << endl;
		return 1;
	}

	out << "GET " << (path ? path : "/") << " HTTP/1.0\r\n\r" << endl;

	// HHTP 1.0: the length of a line is not limited :(
	const size_t buflng=REPLACESTANDARDIOBUFSIZE;
	char *buffer=new char[buflng];
	Assert(buffer);

	unsigned lines=0;
	char *statusversion,*statuscode,*statusreason;
	char *contenttype=0;
	char *contentlength=0;
	char *contentencoding=0;
	while (1) {
		if (!in.getline(buffer,buflng,'\n')) {
			cerr << "Error - End of input in Head" << endl;
			return 1;
		}
		choptrailing(buffer);
		char *headerkey,*headerargs;

		if (!strlen(buffer)) {
			// HHTP 1.0: Empty line ends head
			if (lines) {
				if (verbose) cerr << "End of Head" << endl;
				break;	// Only good way out of loop
			}
			else {
				cerr << "Error - Empty Head :(" << endl;
				return 1;
			}
		}
		else if (++lines == 1) {
			// HHTP 1.0: Must be status line
			char *sversion,*scode,*sreason;
			if (statusparse(buffer,sversion,scode,sreason)) {
				statusversion=strdup(sversion);
				statuscode=strdup(scode);
				statusreason=strdup(sreason);
				if (verbose) cerr << "Full Response Status: version=<" << statusversion << "> code=<" << statuscode << "> reason=<" << statusreason << ">" << endl;
			}
			else {
				if (verbose) cerr << "Simple Response: <" << buffer << ">" << endl;
			}
		}
		else if (*buffer == ' ' || *buffer == '\t') {
			// HHTP 1.0: continuation lines not implemented :(
			if (verbose) cerr << "Continue: <" << buffer << ">" << endl;
		}
		else if (headerparse(buffer,headerkey,headerargs)) {
			trtolower(headerkey);
			if (verbose) cerr << "Header: key=<" << headerkey << "> args=<" << headerargs << ">" << endl;
			if      (strcmp(headerkey,"content-type") == 0) contenttype=strdup(trtolower(nextword(headerargs)));
			else if (strcmp(headerkey,"content-length") == 0) contentlength=strdup(nextword(headerargs));
			else if (strcmp(headerkey,"content-encoding") == 0) contentencoding=strdup(trtolower(nextword(headerargs)));
		}
		else {
			cerr << "Other:  <" << buffer << ">" << endl;
		}
	}

	if (verbose) cerr << "statusversion <" << (statusversion ? statusversion : "-NONE-") << ">" << endl;
	if (verbose) cerr << "statuscode <" << (statuscode ? statuscode : "-NONE-") << ">" << endl;
	if (verbose) cerr << "statusreason <" << (statusreason ? statusreason : "-NONE-") << ">" << endl;
	if (verbose) cerr << "contenttype <" << (contenttype ? contenttype : "-NONE-") << ">" << endl;
	if (verbose) cerr << "contentlength <" << (contentlength ? contentlength : "-NONE-") << ">" << endl;
	if (verbose) cerr << "contentencoding <" << (contentencoding ? contentencoding : "-NONE-") << ">" << endl;

	if (!statuscode || atoi(statuscode) != 200) {
		cerr << "Error - bad status code <"
		     << (statuscode ? statuscode : "-NONE-") << "> reason <"
		     << (statusreason ? statusreason : "-NONE-") << "> for path <"
		     << (path ? path : "-NONE-") << ">"
		     << endl;
		return 1;
	}

	long length;
	if (!contentlength || !(length=atol(contentlength))) {
		cerr << "Error - bad Content-length <" << (contentlength ? contentlength : "-NONE-") << ">" << endl;
		return 1;
	}

	while (length > 0) {
		int wanted=int(length > buflng ? buflng : length);
		in.read(buffer,wanted);
		int got=in.gcount();
		if (got) in.clear();	// blech :( Don't ask !
		if (progress)
			cerr << "length=" << length << " wanted=" << wanted << " got=" << got << "          \r" << flush;
		if (in.fail()) {
			if (progress) cerr << endl;
			cerr << "Error - premature EOF - " << length << " remaining" << endl;
			return 1;
		}
		cout.write(buffer,got);
		length-=got;
	}
	if (progress) cerr << endl;
	if (verbose) cerr << "Done" << endl;

	if (buffer) delete[] buffer;
	if (netbuf) delete[] netbuf;

	// strdup() probably uses malloc/free(rather than new[]/delete[] ...
	if (statusversion) free(statusversion);
	if (statuscode) free(statuscode);
	if (statusreason) free(statusreason);
	if (contenttype) free(contenttype);
	if (contentlength) free(contentlength);
	if (contentencoding) free(contentencoding);

	return 0;
}

