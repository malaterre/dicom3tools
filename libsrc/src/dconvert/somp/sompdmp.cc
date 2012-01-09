#include "sompdmp.h"

#include "bnstream.h"
#include "transyn.h"

#include "somp.h"

bool
SOMP_Conversion::dumpCommon(ostream &o)
{
	Assert(in);
	if (!somphdr) somphdr=new SOMP_Header_BothClass(in);
	Assert(somphdr);

	TextOutputStream out(o);

	somphdr->DumpCommon(&out);

	return true;
}


