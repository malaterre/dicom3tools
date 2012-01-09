#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "imtn.h"
#include "imtncl.h"
#include "srcsink.h"
#include "imtnsrc.h"

IMTN_Conversion::IMTN_Conversion(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	imtnhdr=0;
	pixeldatasrc=0;
}

IMTN_Conversion::~IMTN_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (imtnhdr) delete imtnhdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

