#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "gaw.h"
#include "gawcl.h"
#include "srcsink.h"
#include "gawsrc.h"

void
GAW_Conversion::init(istream &i,ostream &e)
{
	in=new BinaryInputStream(i,BigEndian);
	err=new TextOutputStream(e);
	gawhdr=0;
	pixeldatasrc=0;
}

GAW_Conversion::GAW_Conversion(istream &i,ostream &e)
{
	init(i,e);
	// use GAW_FileStructureInformation as constructed already
}

GAW_Conversion::GAW_Conversion(istream &i,ostream &e,
		bool explicitoff,
		bool lengthpresent,
		Uint32 fileptr,
		Uint32 suiteptr,
		Uint32 examptr,
		Uint32 seriesptr,
		Uint32 imageptr)
{
	init(i,e);
	fileinfo=GAW_FileStructureInformation(
		explicitoff,	// explicit
		lengthpresent,	// length field
		fileptr,	// file header
		suiteptr,	// suite header
		examptr,	// exam header
		seriesptr,	// series header
		imageptr);	// image header
}

GAW_Conversion::~GAW_Conversion()
{
	Assert(in);
	if (in) delete in;
	Assert(err);
	if (err) delete err;

	if (gawhdr) delete gawhdr;
	if (pixeldatasrc) delete pixeldatasrc;
}

