#include "gawdmp.h"

#include "bnstream.h"
#include "transyn.h"

#include "gaw.h"

bool
GAW_Conversion::dumpCommon(ostream &o)
{
	Assert(in);
	if (!gawhdr) gawhdr=new GAW_Header_BothClass(in,fileinfo);
	Assert(gawhdr);

	TextOutputStream out(o);

	gawhdr->DumpCommon(&out);

	return true;
}


