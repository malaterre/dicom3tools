#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "himr.h"
#include "himrcl.h"
#include "srcsink.h"
#include "himrsrc.h"

HIMR_Conversion::HIMR_Conversion(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	himrhdr=0;
	pixeldatasrc=0;
}

HIMR_Conversion::~HIMR_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (himrhdr) delete himrhdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

