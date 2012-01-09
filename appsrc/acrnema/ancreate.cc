#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include <ctype.h>

#include "basetype.h"
#include "txstream.h"
#include "getoptns.h"
#include "mesgtext.h"

#define INPUTLINEBUFFERLENGTH 32768

// all the following is copied from dconvert/fltype.h

#include <float.h>

#ifdef POWINTEGEREXPONENTTYPE
#define	powi(m,e)	pow(m,(POWINTEGEREXPONENTTYPE)(e))
#else
#define	powi(m,e)	pow(m,(long)(e))
#endif

#ifdef __SC__
#define infinity()    HUGE_VAL
#define quiet_nan(x)  0
#endif /* __SC__ */

#ifdef __MWERKS__
#define infinity()    __inf()
#define quiet_nan(x)  0
#endif /* __MWERKS__ */

#ifdef USEDUMBINFINITYANDNAN
#if USEDUMBINFINITYANDNAN
#define infinity()    FLT_MAX
#define quiet_nan(x)  0
#endif
#endif


static void
write32(ostream& out,Uint32 u,int big)
{
//cerr << "write32 0x" << hex << u << dec << endl;
	unsigned char buffer[4];
	if (big) {
		buffer[0]=(unsigned char)(u>>24);
		buffer[1]=(unsigned char)(u>>16);
		buffer[2]=(unsigned char)(u>>8);
		buffer[3]=(unsigned char)u;
	}
	else {
		buffer[3]=(unsigned char)(u>>24);
		buffer[2]=(unsigned char)(u>>16);
		buffer[1]=(unsigned char)(u>>8);
		buffer[0]=(unsigned char)u;
	}
	out.write((char *)buffer,4);
}

static void
write16(ostream& out,Uint16 u,int big)
{
	unsigned char buffer[4];
	if (big) {
		buffer[0]=(unsigned char)(u>>8);
		buffer[1]=(unsigned char)u;
	}
	else {
		buffer[1]=(unsigned char)(u>>8);
		buffer[0]=(unsigned char)u;
	}
	out.write((char *)buffer,2);
}

static void
write8(ostream& out,unsigned char u)
{
	unsigned char buffer[1];
	buffer[0]=u;
	out.write((char *)buffer,1);
}

static void
writeFloat64(ostream& out,double value,int big) {
//cerr << "writeFloat64: " << value << endl;

	// copied from libsrc/include/dctool/attrtypf.h newSize8Value()

	Uint32 sign=0;
	Int32 exponent;
	Uint32 mantissahigh;
	Uint32 mantissalow;

	if (value < 0) {
		sign=0x80000000;
		value=-value;
	}

	if (value == 0) {
		exponent=0;
		mantissahigh=0;
		mantissalow=0;
	}
	else {
		exponent=(int)ourlog2(value);
		if (value < 1) --exponent;
		if (exponent <= -1022) {
			double mantissavalue = value/pow(2,-1022);
			mantissahigh=(Uint32)(mantissavalue*powi(2.0,20));
			mantissalow =(Uint32)(mantissavalue*powi(2.0,52));
			exponent=0;
		}
		else {
			if (exponent > 1023) exponent=1023;
			double mantissavalue = value/pow(2,exponent) - 1.0;
			mantissahigh=(Uint32)(mantissavalue*powi(2.0,20));
			mantissalow =(Uint32)(mantissavalue*powi(2.0,52));
			exponent=((exponent+1023)<<20)&0x7ff00000;
		}
	}

	Uint32 binaryhigh=sign+exponent+mantissahigh;

	unsigned char *barray = new unsigned char[8];
	if (big) {
		barray[0]=(unsigned char)(binaryhigh>>24);
		barray[1]=(unsigned char)(binaryhigh>>16);
		barray[2]=(unsigned char)(binaryhigh>>8);
		barray[3]=(unsigned char)(binaryhigh);
		barray[4]=(unsigned char)(mantissalow>>24);
		barray[5]=(unsigned char)(mantissalow>>16);
		barray[6]=(unsigned char)(mantissalow>>8);
		barray[7]=(unsigned char)(mantissalow);
	}
	else {
		barray[7]=(unsigned char)(binaryhigh>>24);
		barray[6]=(unsigned char)(binaryhigh>>16);
		barray[5]=(unsigned char)(binaryhigh>>8);
		barray[4]=(unsigned char)(binaryhigh);
		barray[3]=(unsigned char)(mantissalow>>24);
		barray[2]=(unsigned char)(mantissalow>>16);
		barray[1]=(unsigned char)(mantissalow>>8);
		barray[0]=(unsigned char)(mantissalow);
	}
	out.write((char *)barray,8);
}

static void
writeFloat32(ostream& out,float value,int big) {
//cerr << "writeFloat32: " << f << endl;

	// copied from libsrc/include/dctool/attrtypf.h newSize4Value()
	
	Uint32 sign=0;
	Int32 exponent;
	Uint32 mantissa;

	if (value < 0) {
		sign=0x80000000;
		value=-value;
	}

	if (value == 0) {
		exponent=0;
		mantissa=0;
	}
	else {
		exponent=(int)ourlog2(value);
		if (value < 1) --exponent;
		if (exponent <= -126) {
			mantissa=(Uint32)((value/pow(2,-126))*(1<<23));
			exponent=0;
		}
		else {
			if (exponent > 127) exponent=127;
			mantissa=(Uint32)(((value/pow(2,exponent))-1)*(1<<23));
			exponent=((exponent+127)<<23)&0x7f800000;
		}
	}

	Uint32 binary=sign+exponent+mantissa;

	unsigned char *barray = new unsigned char[4];
	if (big) {
		barray[0]=(unsigned char)(binary>>24);
		barray[1]=(unsigned char)(binary>>16);
		barray[2]=(unsigned char)(binary>>8);
		barray[3]=(unsigned char)(binary);
	}
	else {
		barray[3]=(unsigned char)(binary>>24);
		barray[2]=(unsigned char)(binary>>16);
		barray[1]=(unsigned char)(binary>>8);
		barray[0]=(unsigned char)(binary);
	}
	out.write((char *)barray,4);
}

/* Recognized commands are:

(hex group,hex element) anything VR=<vr> VL=<hex vl> [hexvals]
(hex group,hex element) anything VR=<vr> VL=<hex vl> <strvals>
%implicit
%explicit
%little
%big

%item
%enditem
%endseq

datavalues

VL will be replaced with actual VL unless OB,OW or SQ

*/

static void
parseError(const char *str,char c)
{
	cerr << "Parse error in entry - \"" << str 
	     << "\" expected, got \'" << c << "\'\n" << flush;
}

static int
getCommand(istream& in)
{
	int command=0;
	const unsigned buflng = 10;
	char buffer[buflng];
		char c;
		do {
			in >> c;
			if (!in.good()) return 0;
		} while (isspace(c));
		if (c == '#') {					// start of comment
			do {					// skip to end of line
				in >> c;
				if (!in.good()) return 0;
			} while (c != '\n' && c != '\r');
			do {					// skip ends of lines
				in >> c;
				if (!in.good()) return 0;
			} while (c == '\n' || c == '\r');
		}
		// c should now be non-white space beginning of command
		switch (c) {
			case '(':
				command='(';
				break;
			case '%':
				in >> setw(buflng-1) >> buffer;
				if (!in.good()) break;
				if (strcmp(buffer,"implicit") == 0) command='i';
				if (strcmp(buffer,"explicit") == 0) command='e';
				if (strcmp(buffer,"little") == 0)   command='l';
				if (strcmp(buffer,"big") == 0)      command='b';
				if (strcmp(buffer,"byte") == 0)     command='B';
				if (strcmp(buffer,"short") == 0)    command='S';
				if (strcmp(buffer,"word") == 0)     command='S';
				if (strcmp(buffer,"long") == 0)     command='L';
				if (strcmp(buffer,"item") == 0)     command='T';
				if (strcmp(buffer,"enditem") == 0)  command='M';
				if (strcmp(buffer,"endseq") == 0)   command='Q';
				break;
			default:
				if (isdigit(c)) {
					command=c;
					in.putback(c);
				}
				else {
					parseError("attribute,command or value",
						c);
				}
				break;
		}
	return command;
}

static void
getFloat64FromString(istream& in,double &d) {
	in >> dec >> d;
}

static void
getFloat32FromString(istream& in,float &f) {
	in >> dec >> f;
}

static void
getHex32(istream& in,Uint32& v)
{
	char c;
	int i;

	in >> hex >> v;
	if (v == 0 && (i=in.peek()) == 'x') {
		in.get(c);
		in >> hex >> v;
	}
	// this check seems to fail on trailing ']' delimiter with gcc 3 :(
	//if (!in.good()) {
	//	parseError("hex number",0);
	//}
}

static void
getHex16(istream& in,Uint16& v)
{
	Uint32 v32;
	getHex32(in,v32);
	v=(Uint16)v32;
}


static int
getParsedEntry(istream& in,Uint16& group,Uint16& element,
	char *VR,Uint32& VL,int& valuetype,char **valuestring,bool verbose,int commandcount)
{
	char c;
	const unsigned buflng = INPUTLINEBUFFERLENGTH;
	char buffer[buflng];	// enough for longest value string
	char *p;

	// First '(' has already been read as command

	// Find "nnnn,nnnn)" ...

	in >> resetiosflags(ios::basefield) >> group;
	if (!in.good()) {
		return -1;	// Failure
	}
	in >> c;
	if (c != ',') {
		parseError(",",c);
		return -1;	// Failure
	}
	in >> resetiosflags(ios::basefield) >> element;
	if (!in.good()) {
		return -1;	// Failure
	}
	in >> c;
	if (c != ')') {
		parseError(")",c);
		return -1;	// Failure
	}

	if (verbose) {
		cerr << dec << commandcount << ": (";
		writeZeroPaddedHexNumber(cerr,group,4);
		cerr << ",";
		writeZeroPaddedHexNumber(cerr,element,4);
		cerr << ") " << endl;
	}

	// Skip any fields (eg. from dc3dump) found "VR=<xx>" ...
	do {
		in >> buffer;
	} while (in.good() && strncmp(buffer,"VR",2) != 0);

	if (!in.good()) {
		parseError("VR",0);
		return -1;	// Failure
	}
	for (p=buffer; *p && *p != '<';++p);
	strncpy(VR,p+1,2);
	VR[2]=0;
	if (*(p+3) != '>') {
		parseError("> after VR",*(p+3));
		return -1;	// Failure
	}

	// Find "VL=<nn>" ... 	(nn is in hex)

	do {
		in >> buffer;
	} while (in.good() && strncmp(buffer,"VL",2) != 0);

	if (!in.good()) {
		parseError("VL",0);
		return -1;	// Failure
	}
	for (p=buffer; *p && *p != '<';++p);
	istrstream istr(p+1);
	istr >> resetiosflags(ios::basefield) >> VL;
//cerr << "vl=" << VL << endl;
	if (!istr.good()) {
		return -1;	// Failure
	}
 	istr >> c;
	if (c != '>') {
		parseError("> after VL",c);
		return -1;	// Failure
	}

	in >> c;
	char traildelim;
	switch (c) {
		case '<':
			traildelim='>';
			break;
		case '[':
			traildelim=']';
			break;
		case '{':
			traildelim='}';
			break;
		default:
			parseError("<values> or [values] or {values}",c);
			return -1;	// Failure
	}
	valuetype=c;
	for (p=buffer; p<buffer+buflng; ++p) {
		in.get(c);	// Don't skip whitespace
		if (!in.good()) {
			parseError("> or ] or } after value",0);
			return -1;	// Failure
		}
		else {
			if (c == traildelim) {
				*p=0;
				break;
			}
			else {
				*p=c;
			}
		}
	}
	*valuestring=new char[strlen(buffer)+1];
	strcpy(*valuestring,buffer);

	return 0;		// Success
}

static void
writeAttribute(ostream& out,Uint16 group,Uint16 element,
	char *vr,Uint32 vl,int type,char *string,
	int explicitvr,int big)
{
	write16(out,group,big);
	write16(out,element,big);

	Uint32 actualvl;
	switch (type) {
		case '<':	// string
			if ((actualvl=strlen(string))%2) ++actualvl;
			break;
		case '[':	// hex
			if (*string) {
				char *p;
				for (actualvl=1,p=string; *p; ++p)
					if (!isxdigit(*p) && *p != 'x' && !isspace(*p)) 
						++actualvl;
			}
			else actualvl=0;
			actualvl*=(vr[1] == 'L')?4:2;
			break;
		case '{':	// float
			if (*string) {
				char *p;
				for (actualvl=1,p=string; *p; ++p)
					if (!isdigit(*p) && *p != '.' && *p != '-' && *p != 'e' && *p != 'E' && !isspace(*p)) 
						++actualvl;
			}
			else actualvl=0;
			actualvl*=(vr[1] == 'D')?8:4;
			break;
	}

	if (actualvl != vl) {
		if (strcmp(vr,"OB") == 0
		 || strcmp(vr,"OF") == 0
		 || strcmp(vr,"OW") == 0
		 || strcmp(vr,"SQ") == 0) {
			actualvl=vl;
		}
		else {
			cerr << "Specified VL ";
			writeZeroPaddedHexNumber(cerr,vl,8);
			cerr << " wrong, using ";
			writeZeroPaddedHexNumber(cerr,actualvl,8);
			cerr << " (VR is " << vr << ")\n" << flush;
			//cerr << "VR[0]=" << (unsigned)(vr[0]) << endl;
			//cerr << "VR[1]=" << (unsigned)(vr[1]) << endl;
			//cerr << "VR[2]=" << (unsigned)(vr[2]) << endl;
		}
	}

	if (explicitvr) {
		out.write(vr,2);			// 2 byte explicitvr vr
		if (strcmp(vr,"OB") == 0
		 || strcmp(vr,"OF") == 0
		 || strcmp(vr,"OW") == 0
		 || strcmp(vr,"UN") == 0
		 || strcmp(vr,"UT") == 0
		 || strcmp(vr,"SQ") == 0) {
			write16(out,0,big);		// 2 byte reserved
			write32(out,actualvl,big);	// 4 byte value length
		}
		else {
			write16(out,(Uint16)actualvl,big);	// 2 byte length
		}
	}
	else {
		write32(out,actualvl,big);
	}

	char pad = (strcmp(vr,"UI") == 0) ? 0 : ' ';
	switch (type) {
		case '<':	// string
			out << string;
			if (strlen(string)%2) out << pad;
			break;
		case '[':	// hex
			if (*string) {
				istrstream istr(string);
				while (istr.good()) {
					Uint32 u;
					char c;
					getHex32(istr,u);
					istr >> c;		// delimiter
					if (vr[1] == 'B')
						write8(out,(unsigned char)u);
					else if (vr[1] == 'L')
						write32(out,u,big);
					else
						write16(out,(Uint16)u,big);
				}
			}
			break;
		case '{':	// float
			if (*string) {
				istrstream istr(string);
				while (istr.good()) {
					if (vr[1] == 'D') {
						double d;
						getFloat64FromString(istr,d);
						char c;
						istr >> c;		// delimiter
						writeFloat64(out,d,big);
					}
					else {
						float f;
						getFloat32FromString(istr,f);
						char c;
						istr >> c;		// delimiter
						writeFloat32(out,f,big);
					}
				}
			}
			break;
	}
}

static void
writeItem(ostream& out,int explicitvr,int big)
{
	write16(out,0xfffe,big);
	write16(out,0xe000,big);
	write32(out,0xffffffff,big);
}

static void
writeItemDelimiter(ostream& out,int explicitvr,int big)
{
	write16(out,0xfffe,big);
	write16(out,0xe00d,big);
	write32(out,0x00000000,big);
}

static void
writeSequenceDelimiter(ostream& out,int explicitvr,int big)
{
	write16(out,0xfffe,big);
	write16(out,0xe0dd,big);
	write32(out,0x00000000,big);
}

static void
writePreamble(ostream& out)
{
	char zeroes[128];
	memset(zeroes,0,128);
	out.write(zeroes,128);
	out.write("DICM",4);
}

int
main(int argc, char *argv[])
{
	GetNamedOptions options(argc,argv);
	bool explicitvr	=options.get("explicit")||options.get("e");
	bool big	=options.get("big")||options.get("b");
	if (big) explicitvr=true;

	bool preamble=options.get("preamble")||options.get("p");

	bool verbose=options.get("verbose")||options.get("v");

	options.done();

	cerr << options.errors();

	if (!options.good() || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< " [-b|big]"
			<< " [-e|explicit]"
			<< " [-p|preamble]"
			<< " [-v|verbose]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	if (preamble) {
		writePreamble(cout);
	}

	int datawordlength=1;
	int commandcount=0;
	int command;
	while (command=getCommand(cin)) {
		++commandcount;
		Uint16 group;
		Uint16 element;
		char vr[3];
		Uint32 vl;
		int type;
		char *string;
		switch (command) {
			case '(':
				if (getParsedEntry(cin,group,element,
						vr,vl,type,&string,verbose,commandcount) == 0) {
					writeAttribute(cout,group,element,
						vr,vl,type,string,
						explicitvr,big);
				}
				if (strcmp(vr,"OB") == 0) datawordlength=1;
				if (strcmp(vr,"OW") == 0) datawordlength=2;
				break;
			case 'i':
				explicitvr=0;
				break;
			case 'e':
				explicitvr=1;
				break;
			case 'l':
				big=0;
				break;
			case 'b':
				big=1;
				explicitvr=1;	// no such thing as implicit big
				break;
			case 'B':
				datawordlength=1;
				break;
			case 'S':
				datawordlength=2;
				break;
			case 'L':
				datawordlength=4;
				break;
			case 'T':
				writeItem(cout,explicitvr,big);
				break;
			case 'M':
				writeItemDelimiter(cout,explicitvr,big);
				break;
			case 'Q':
				writeSequenceDelimiter(cout,explicitvr,big);
				break;
			default:
				if (isdigit(command)) {
					Uint32 value;
					cin >> resetiosflags(ios::basefield)
					    >> value;
					switch (datawordlength) {
						case 1:
							cout.put((char)value);
							break;
						case 2:
							write16(cout,
							    (Uint16)value,big);
							break;
						case 4:
							write32(cout,value,big);
							break;
					}
				}
				break;
		}
	}

	return 0;
}

