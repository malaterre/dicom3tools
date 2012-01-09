#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "pace.h"
#include "pacecl.h"
#include "srcsink.h"
#include "pacesrc.h"

PACE_Conversion::PACE_Conversion(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	pacehdr=0;
	pixeldatasrc=0;
}

PACE_Conversion::~PACE_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (pacehdr) delete pacehdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

