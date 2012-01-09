#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "pq.h"
#include "pqcl.h"
#include "srcsink.h"
#include "pqsrc.h"

PQ_Conversion::PQ_Conversion(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	pqhdr=0;
	pixeldatasrc=0;
}

PQ_Conversion::~PQ_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (pqhdr) delete pqhdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

