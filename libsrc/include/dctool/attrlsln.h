#ifndef __Header_attrlsln__
#define __Header_attrlsln__

#include "attr.h"
#include "attrlist.h"
#include "transyn.h"
#include "elmdict.h"

Uint32
lengthOfFixedPartOfAttribute(
	const TransferSyntax *ts,const Attribute *a);

Uint32
lengthOfFixedPartOfAttribute(
	const TransferSyntax *metats,const TransferSyntax *datats,const Attribute *a);

Uint32
lengthOfEntireAttribute(const TransferSyntax *ts,const Attribute *a);


Uint32
lengthOfEntireAttribute(
	const TransferSyntax *metats,const TransferSyntax *datats,const Attribute *a);

#endif /* __Header_attrlsln__ */
