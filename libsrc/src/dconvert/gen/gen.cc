#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "gen.h"
#include "gencl.h"
#include "srcsink.h"
#include "gensrc.h"

void
GEN_Conversion::init(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	genhdr=0;
	pixeldatasrc=0;
}

GEN_Conversion::GEN_Conversion(istream &i,ostream &e)
{
	init(i,e);
	// use GEN_FileStructureInformation as constructed already
}

GEN_Conversion::GEN_Conversion(istream &i,ostream &e,
		bool explicitoff,
		bool lengthpresent,
		Uint32 fileptr,
		Uint32 suiteptr,
		Uint32 examptr,
		Uint32 seriesptr,
		Uint32 imageptr)
{
	init(i,e);
	fileinfo=GEN_FileStructureInformation(
		explicitoff,	// explicit
		lengthpresent,	// length field
		fileptr,	// file header
		suiteptr,	// suite header
		examptr,	// exam header
		seriesptr,	// series header
		imageptr);	// image header
}

GEN_Conversion::~GEN_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (genhdr) delete genhdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

