#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "xxxx.h"
#include "xxxxcl.h"
#include "srcsink.h"
#include "xxxxsrc.h"

XXXX_Conversion::XXXX_Conversion(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	xxxxhdr=0;
	pixeldatasrc=0;
}

XXXX_Conversion::~XXXX_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (xxxxhdr) delete xxxxhdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

