#ifndef __Header_txstream__
#define __Header_txstream__

#include <iostream.h>
#include <iomanip.h>

class TextOutputStream : public ostream {
public:
	TextOutputStream(streambuf *buf) : ostream(buf)
		{
			setf(ios::showbase,ios::showbase);
		}

	TextOutputStream(ostream& ostr) : ostream(ostr.rdbuf())
		{
			setf(ios::showbase,ios::showbase);
		}
};

ostream& writeZeroPaddedNumber(ostream& o,Uint32 value,ios::fmtflags basetouse,int size);
ostream& writeZeroPaddedHexNumber(ostream& o,Uint32 value,int size);

#endif /* __Header_txstream__ */


