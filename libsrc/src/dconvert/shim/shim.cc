#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "shim.h"
#include "shimcl.h"
#include "srcsink.h"
#include "shimsrc.h"

SHIM_Conversion::SHIM_Conversion(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	shimhdr=0;
	pixeldatasrc=0;
}

SHIM_Conversion::~SHIM_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (shimhdr) delete shimhdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

