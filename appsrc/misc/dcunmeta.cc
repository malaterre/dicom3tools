#include <iostream.h>

#include "basetype.h"
#include "mesgtext.h"

#ifndef REPLACESTANDARDIOBUFSIZE
#define REPLACESTANDARDIOBUFSIZE 16384
#endif

#ifndef LOCALBUFSIZE
#define LOCALBUFSIZE 16384
#endif

void
readLE16(istream &stream,Uint16 &u)
{
	unsigned char buffer[2];
	stream.read((char *)buffer,2);
	u =  (Uint16)buffer[1];
	u <<= 8;
	u |= (Uint16)buffer[0];
}

void
readLE32(istream &stream,Uint32 &u)
{
	unsigned char buffer[4];
	stream.read((char *)buffer,4);
	u =  (Uint32)buffer[3];
	u <<= 8;
	u |= (Uint32)buffer[2];
	u <<= 8;
	u |= (Uint32)buffer[1];
	u <<= 8;
	u |= (Uint32)buffer[0];
}

void
writeLE16(ostream &stream,Uint16 u)
{
	char buffer[2];
	buffer[1]=(unsigned char)(u>>8);
	buffer[0]=(unsigned char)u;
	stream.write(buffer,2);
}

int
main(int argc,char **argv)
{
	(void)argc; (void)argv;

	char preamble[128];
	cin.read(preamble,128);
	if (cin.fail()) {
		cerr << EMsgDC(ReadFailed) << endl;
		return false;
	}

	char magic[4];
	cin.read(magic,4);
	if (cin.fail()) {
		cerr << EMsgDC(ReadFailed) << endl;
		return false;
	}

	if (strncmp(magic,"DICM",4) != 0) {
		cerr << EMsgDC(NotDICOMMessage)
		     << " " << MMsgDC(Or) << " "
		     << MMsgDC(MissingMetaheader) 
		     << endl;
		return false;
	}

	Uint16 group;
	readLE16(cin,group);
	Uint16 element;
	readLE16(cin,element);
	if (cin.fail()) {
		cerr << EMsgDC(TagReadFailed) << endl;
		return false;
	}

	if (group != 0x0002 || element != 0x0000) {
		cerr << EMsgDC(MissingAttribute)
		     << " - File Meta Information Group Length"
		     << endl;
		return false;
	}

	char VR[3];
	cin.read(VR,2);
	if (cin.fail() || strncmp(VR,"UL",2) != 0) {
		cerr << EMsgDC(BadValueRepresentation)
		     << " - File Meta Information Group Length";
		if (!cin.fail()) {
			VR[2]=0;
			cerr << " = \"" << VR << "\"";
		}
		cerr << endl;
		return false;
	}

	Uint16 VL;
	readLE16(cin,VL);
	if (cin.fail() || VL != 4) {
		cerr << EMsgDC(BadValueLength)
		     << " - File Meta Information Group Length";
		if (!cin.fail())
			cerr << " = \"" << VL << "\"";
		cerr << endl;
		return false;
	}

	Uint32 grouplength;
	readLE32(cin,grouplength);
	if (cin.fail()) {
		cerr << EMsgDC(BadAttributeValue)
		     << " - File Meta Information Group Length"
		     << endl;
		return false;
	}

	cin.seekg(grouplength,ios::cur);
	if (cin.fail()) {
		cerr << EMsgDC(MissingMetaheader)
		     << " - " << MMsgDC(SeekFailed)
		     << endl;
		return false;
	}

#ifdef CRAP
	// default ANSI cin/cout is unbuffered, slow, so ...

	streambuf *isbuf  = cin.rdbuf();
	istream *istr=new istream(isbuf);
	if (istr) {
		char *ibuf=new char[REPLACESTANDARDIOBUFSIZE];
		if (ibuf) isbuf->setbuf(ibuf,REPLACESTANDARDIOBUFSIZE);
	}
	else {
		istr=&cin;
	}

	streambuf *osbuf  = cout.rdbuf();
	ostream *ostr=new ostream(osbuf);
	if (ostr) {
		char *obuf=new char[REPLACESTANDARDIOBUFSIZE];
		if (obuf) osbuf->setbuf(obuf,REPLACESTANDARDIOBUFSIZE);
	}
	else {
		ostr=&cout;
	}
	// Be careful not to use cin and cout beyond this point
#else
	istream *istr=&cin;
	ostream *ostr=&cout;
#endif

	while (istr && !istr->eof()) {
		char buffer[LOCALBUFSIZE];
		istr->read(buffer,LOCALBUFSIZE);
		ostr->write(buffer,istr->gcount());
	}

}
