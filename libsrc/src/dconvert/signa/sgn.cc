#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "signa.h"
#include "sgncl.h"
#include "srcsink.h"
#include "sgnsrc.h"

void
SGN_Conversion::init(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	sgnhdr=0;
	pixeldatasrc=0;
}

SGN_Conversion::SGN_Conversion(istream &i,ostream &e)
{
	init(i,e);
	// use SGN_FileStructureInformation as constructed already
}

SGN_Conversion::~SGN_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (sgnhdr) delete sgnhdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

