#include <fstream.h>

#include "basetype.h"
#include "mesgtext.h"
#include "getoptns.h"

const Uint16 JPEG_MARKER_APP0 = 0xffe0;
const Uint16 JPEG_MARKER_APP1 = 0xffe1;
const Uint16 JPEG_MARKER_APP2 = 0xffe2;
const Uint16 JPEG_MARKER_APP3 = 0xffe3;
const Uint16 JPEG_MARKER_APP4 = 0xffe4;
const Uint16 JPEG_MARKER_APP5 = 0xffe5;
const Uint16 JPEG_MARKER_APP6 = 0xffe6;
const Uint16 JPEG_MARKER_APP7 = 0xffe7;
const Uint16 JPEG_MARKER_APP8 = 0xffe8;
const Uint16 JPEG_MARKER_APP9 = 0xffe9;
const Uint16 JPEG_MARKER_APPA = 0xffea;
const Uint16 JPEG_MARKER_APPB = 0xffeb;
const Uint16 JPEG_MARKER_APPC = 0xffec;
const Uint16 JPEG_MARKER_APPD = 0xffed;
const Uint16 JPEG_MARKER_APPE = 0xffee;
const Uint16 JPEG_MARKER_APPF = 0xffef;

const Uint16 JPEG_MARKER_COM = 0xfffe;
const Uint16 JPEG_MARKER_DAC = 0xffcc;
const Uint16 JPEG_MARKER_DHP = 0xffde;
const Uint16 JPEG_MARKER_DHT = 0xffc4;
const Uint16 JPEG_MARKER_DNL = 0xffdc;
const Uint16 JPEG_MARKER_DQT = 0xffdb;
const Uint16 JPEG_MARKER_DRI = 0xffdd;
const Uint16 JPEG_MARKER_EOI = 0xffd9;
const Uint16 JPEG_MARKER_EXP = 0xffdf;

const Uint16 JPEG_MARKER_JPG = 0xffc8;

// left out reserved JPGn and RES
// (especially those with first bit (not just byte) zero ... new LS 0 stuffing)

const Uint16 JPEG_MARKER_RST0 = 0xffd0;
const Uint16 JPEG_MARKER_RST1 = 0xffd1;
const Uint16 JPEG_MARKER_RST2 = 0xffd2;
const Uint16 JPEG_MARKER_RST3 = 0xffd3;
const Uint16 JPEG_MARKER_RST4 = 0xffd4;
const Uint16 JPEG_MARKER_RST5 = 0xffd5;
const Uint16 JPEG_MARKER_RST6 = 0xffd6;
const Uint16 JPEG_MARKER_RST7 = 0xffd7;

const Uint16 JPEG_MARKER_SOF0 = 0xffc0;
const Uint16 JPEG_MARKER_SOF1 = 0xffc1;
const Uint16 JPEG_MARKER_SOF2 = 0xffc2;
const Uint16 JPEG_MARKER_SOF3 = 0xffc3;

const Uint16 JPEG_MARKER_SOF5 = 0xffc5;
const Uint16 JPEG_MARKER_SOF6 = 0xffc6;
const Uint16 JPEG_MARKER_SOF7 = 0xffc7;

const Uint16 JPEG_MARKER_SOF9 = 0xffc9;
const Uint16 JPEG_MARKER_SOFA = 0xffca;
const Uint16 JPEG_MARKER_SOFB = 0xffcb;

const Uint16 JPEG_MARKER_SOFD = 0xffcd;
const Uint16 JPEG_MARKER_SOFE = 0xffce;
const Uint16 JPEG_MARKER_SOFF = 0xffcf;

const Uint16 JPEG_MARKER_SOI = 0xffd8;
const Uint16 JPEG_MARKER_SOS = 0xffda;
const Uint16 JPEG_MARKER_TEM = 0xff01;

// New for JPEG-LS (14495-1:1997)

const Uint16 JPEG_MARKER_SOF55 = 0xfff7;
const Uint16 JPEG_MARKER_LSE   = 0xfff8;

const unsigned char JPEG_LSE_ID_L1   = 0x01;
const unsigned char JPEG_LSE_ID_L2   = 0x02;
const unsigned char JPEG_LSE_ID_L3   = 0x03;
const unsigned char JPEG_LSE_ID_L4   = 0x04;


Uint16 isFixedLengthJPEGSegment(Uint16 marker)
{
	Uint16 length;
	switch (marker) {
		case JPEG_MARKER_EXP:
			length=3; break;
		default:
			length=0; break;
	}
	return length;
}

bool isNoLengthJPEGSegment(Uint16 marker)
{
	bool nolength;
	switch (marker) {
	case JPEG_MARKER_SOI:
	case JPEG_MARKER_EOI:
	case JPEG_MARKER_TEM:
	case JPEG_MARKER_RST0:
	case JPEG_MARKER_RST1:
	case JPEG_MARKER_RST2:
	case JPEG_MARKER_RST3:
	case JPEG_MARKER_RST4:
	case JPEG_MARKER_RST5:
	case JPEG_MARKER_RST6:
	case JPEG_MARKER_RST7:
				nolength=true; break;
	default:
				nolength=false; break;
	}
	return nolength;
}

bool isVariableLengthJPEGSegment(Uint16 marker)
{
	return !isNoLengthJPEGSegment(marker) && !isFixedLengthJPEGSegment(marker);
}


static Uint16 read8(istream &istr)
{
	unsigned char b;
	istr.read((char *)&b,1);
	return (Uint16)b;
}

static Uint16 read16(istream &istr)	// big-endian
{
	Uint16 u;
	unsigned char b[2];
	istr.read((char *)b,2);
	u =  (Uint16)b[0];
	u <<= 8;
	u |= (Uint16)b[1];
	return u;
}

static void write(ostream *ostr,unsigned char b)
{
	unsigned char buf[1];
	buf[0]=b;
	ostr->write((char *)buf,1);
}

static void write16(ostream *ostr,Uint16 u)	// big-endian
{
	write(ostr,(unsigned char)((u>>8)&0xff));
	write(ostr,(unsigned char)(u&0xff));
}

static char *makeNewFilename(int filecount,const char *outprefix,const char *outsuffix)
{
	ostrstream ostr;
	if (outprefix) ostr << outprefix;
	int zeropadcount=filecount;
	int digits=1;
	while (zeropadcount > 9) {
		++digits;
		zeropadcount=zeropadcount/10;
	}
	while (6-digits > 0) {
		++digits;
		ostr << "0";
	}
	ostr << filecount;
	if (outsuffix) ostr << outsuffix;
	ostr << ends;
	return ostr.str();
}


int
main(int argc,char *argv[])
{
#ifndef REPLACESTANDARDIOBUFSIZE
#define REPLACESTANDARDIOBUFSIZE 16384
#endif

//	streambuf *sbuf  = cin.rdbuf();
//	// default ANSI cin/cout is unbuffered, slow, so ...
//	char *buf=new char[REPLACESTANDARDIOBUFSIZE];
//	if (buf) sbuf->setbuf(buf,REPLACESTANDARDIOBUFSIZE);

	GetNamedOptions options(argc,argv);

	const char *outprefix = 0;
	(void) options.get("prefix",outprefix);
	const char *outsuffix = 0;
	(void) options.get("suffix",outsuffix);

	bool verbose=options.get("v")||options.get("verbose");

	options.done();

	cerr << options.errors();

	if (!options) {
		cerr << MMsgDC(Usage) << ": " << options.command()
		     << " [-prefix output_prefix]"
		     << " [-suffix output_suffix]"
		     << " [-v|verbose]"
		     << endl;
		exit(1);
	}

	bool writing = false;
	int filecount = 0;
	char *filename = 0;
	ofstream *out = 0;

	Uint16 markerprefix=read8(cin);		// will write later,prn.
 	while (1) {
		if (!cin) {
			//cerr << "End of file" << endl;
			break;
		}
		if (markerprefix != 0xff) {	// byte of entropy-coded segment
			if (writing) write(out,(unsigned char)markerprefix);
			markerprefix=read8(cin);
			continue;
		}
		Uint16 marker=read8(cin);
		if (!cin) {
			// End of file immediately after marker flag 0xff ... presumably was padding
			break;
		}
		if (marker == 0) {		// 0xff byte of entropy-coded segment ... ignore following zero byte
			if (writing) write16(out,marker|0xff00);
			markerprefix=read8(cin);
			continue;
		}
		else if (marker == 0xff) {	// 0xff byte of padding
			if (writing) write(out,(unsigned char)0xff);
			markerprefix=marker;	// the first 0xff is padding, the 2nd may be the start of a marker
			continue;
		}
		else if ((marker & 0x80) == 0) {	// 0xff byte of JPEG-LS entropy-coded segment ... ignore following zero BIT (not byte)
			if (writing) write16(out,marker|0xff00);	// doesn't need too look at the remaining 7 entropy coded segment bits
			markerprefix=read8(cin);
			continue;
		}

		// Definitely have a marker ...

		marker|=0xff00;			// convention is to express them with the leading ff

		//cerr << "Marker " << hex << marker << endl;

		if (marker == JPEG_MARKER_SOI) {
			//cerr << "SOI Marker" << endl;
			++filecount;
			filename=makeNewFilename(filecount,outprefix,outsuffix);
		if (verbose) cerr << "Writing file = <" << filename << ">" << endl;
#ifdef USEBINARYFLAGFOROUTPUTOPENMODE
			out=new ofstream(filename,ios::out|ios::trunc|ios::binary);
#else
			out=new ofstream(filename,ios::out|ios::trunc);
#endif
			writing=true;
			write16(out,marker);
		}
		else if (marker == JPEG_MARKER_EOI) {
			//cerr << "EOI Marker" << endl;
			if (out) {
				if (writing) write16(out,marker);
				out->close();
				delete out;
			}
			writing=false;
		}
		else if (writing) write16(out,marker);

		Uint16 length;
		if (isVariableLengthJPEGSegment(marker)) {
			length=read16(cin);
			if (!cin) {
				cerr << "Error - variable length marker without length" << endl;
				return 1;
			}
			if (writing) write16(out,length);
			length-=2;	// NB. the length includes itself(but not the marker)
  		}
 		else {
			length=isFixedLengthJPEGSegment(marker);
		}
		if (length > 0) {
			unsigned char *buffer=new unsigned char[length];
			cin.read((char *)buffer,length);
			if (!cin || cin.gcount() != length) {
				cerr << "Error - couldn't read parameter sequence" << endl;
				return 1;
			}
			if (writing) out->write((char *)buffer,length);
			if (buffer) delete[] buffer;
		}
		markerprefix=read8(cin);	// will write next loop prn.
	}

	return 0;
}

