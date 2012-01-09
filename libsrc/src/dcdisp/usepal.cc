#include "attr.h"
#include "errclass.h"
#include "lutclass.h"
#include "usepal.h"

UseablePalette::UseablePalette(DicomLUT *r,DicomLUT *g,DicomLUT *b)
{
//cerr << "UseablePalette::UseablePalette(r,g,b) this=" << this << endl;
	RedLUT=r;
	GreenLUT=g;
	BlueLUT=b;
	Assert(RedLUT);
	Assert(GreenLUT);
	Assert(BlueLUT);
}

UseablePalette::~UseablePalette()
{
	if (RedLUT) delete RedLUT;
	if (GreenLUT) delete GreenLUT;
	if (BlueLUT) delete BlueLUT;
}

Uint32
UseablePalette::getLength(void)
{
//cerr << "UseablePalette::getLength" << endl;
	Assert(RedLUT->getLength() == GreenLUT->getLength()
	    && RedLUT->getLength() == BlueLUT->getLength());
//cerr << "UseablePalette::getLength returns=" << RedLUT->getLength() << endl;
	return RedLUT->getLength();
}

