#include "shimdmp.h"
#include "shimptrs.h"
#include "shimdmpf.h"

// shim.tpl currently has no select="image" entries so
// the following doesn't make it into shimdmpf.h ...

void
SHIM_Header_BothClass::DumpSelectedImage(TextOutputStream *,unsigned)
{
}
