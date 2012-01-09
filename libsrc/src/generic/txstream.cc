#include "basetype.h"
#include "txstream.h"

ostream&
writeZeroPaddedNumber(ostream& o,Uint32 value,ios::fmtflags basetouse,int size) {
	Assert(size > 0 && size < 9);	// sanity check just in case
	if (value == 0) {
		// For some reason the C++ standard says zero
		// must be written as 0 not 0x0 when ios::showbase
		// is set, so override this stupidity that has now
		// been faithfully implemented by g++ as of 3.2.x
		o << (basetouse == ios::hex ? "0x" : "00");
		while (size--) o << "0";
	}
	else {
		o << resetiosflags(ios::adjustfield|ios::basefield)
		  << setiosflags(ios::showbase|ios::internal|basetouse)
		  << setfill('0') << setw(2+size)
		  << value
		  << dec << setfill(' ')
		  << resetiosflags(ios::adjustfield|ios::basefield);
	}
	return o;
}

ostream&
writeZeroPaddedHexNumber(ostream& o,Uint32 value,int size) {
	return writeZeroPaddedNumber(o,value,ios::hex,size);
}

