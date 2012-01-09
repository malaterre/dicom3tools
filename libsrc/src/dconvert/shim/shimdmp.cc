#include "shimdmp.h"

#include "bnstream.h"
#include "transyn.h"

#include "shim.h"

bool
SHIM_Conversion::dumpCommon(ostream &o)
{
	Assert(in);
	if (!shimhdr) shimhdr=new SHIM_Header_BothClass(in);
	Assert(shimhdr);

	TextOutputStream out(o);

	shimhdr->DumpCommon(&out);

	return true;
}

bool
SHIM_Conversion::dumpSelectedImage(ostream &o,unsigned imagenumber)
{
	Assert(in);
	if (!shimhdr) shimhdr=new SHIM_Header_BothClass(in);
	Assert(shimhdr);

	TextOutputStream out(o);

	shimhdr->DumpSelectedImage(&out,imagenumber);

	return true;
}

