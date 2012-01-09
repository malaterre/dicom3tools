#include "ge9800dmp.h"

#include "bnstream.h"
#include "transyn.h"

#include "ge9800.h"

bool
GE9800_Conversion::dumpCommon(ostream &o)
{
	Assert(in);
	if (!ge9800hdr) ge9800hdr=new GE9800_Header_BothClass(in);
	Assert(ge9800hdr);

	TextOutputStream out(o);

	ge9800hdr->DumpCommon(&out);

	return true;
}


bool
GE9800_Conversion::dumpSelectedImage(ostream &o,unsigned imagenumber)
{
	Assert(in);
	if (!ge9800hdr) ge9800hdr=new GE9800_Header_BothClass(in);
	Assert(ge9800hdr);

	TextOutputStream out(o);

	ge9800hdr->DumpSelectedImage(&out,imagenumber);

	return true;
}

