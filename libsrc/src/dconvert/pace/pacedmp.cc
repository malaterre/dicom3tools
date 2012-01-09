#include "pacedmp.h"

#include "bnstream.h"
#include "transyn.h"

#include "pace.h"

bool
PACE_Conversion::dumpCommon(ostream &o)
{
	Assert(in);
	if (!pacehdr) pacehdr=new PACE_Header_BothClass(in);
	Assert(pacehdr);

	TextOutputStream out(o);

	pacehdr->DumpCommon(&out);

	return true;
}

