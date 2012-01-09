#include "pqdmp.h"

#include "bnstream.h"
#include "transyn.h"

#include "pq.h"

bool
PQ_Conversion::dumpCommon(ostream &o)
{
	Assert(in);
	if (!pqhdr) pqhdr=new PQ_Header_BothClass(in);
	Assert(pqhdr);

	TextOutputStream out(o);

	pqhdr->DumpCommon(&out);

	return true;
}


bool
PQ_Conversion::dumpSelectedImage(ostream &o)
{
	Assert(in);
	if (!pqhdr) pqhdr=new PQ_Header_BothClass(in);
	Assert(pqhdr);

	TextOutputStream out(o);

	pqhdr->DumpSelectedImage(&out);

	return true;
}

