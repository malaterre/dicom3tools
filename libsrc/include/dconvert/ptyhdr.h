#ifndef __Header_ptyhdr__
#define __Header_ptyhdr__

#include <iostream.h>

#include "basetype.h"
#include "convtype.h"
#include "fltype.h"
#include "strtype.h"
#include "datetype.h"
#include "dateconv.h"

static inline bool
ReadProprietaryHeader(istream *istr,long offset,size_t length,char *buffer)
{
	istr->seekg(offset,ios::beg);
	if (istr->fail()) {
		return false;
	}
	else {
		istr->read(buffer,length);
		return istr->good() && istr->gcount() == length;
	}
}

#endif /* __Header_ptyhdr__ */
