#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "ge9800.h"
#include "ge9800cl.h"
#include "srcsink.h"
#include "ge9800src.h"

GE9800_Conversion::GE9800_Conversion(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	ge9800hdr=0;
	pixeldatasrc=0;
}

GE9800_Conversion::~GE9800_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (ge9800hdr) delete ge9800hdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

