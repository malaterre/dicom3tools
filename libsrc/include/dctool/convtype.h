#ifndef __Header_convtype__
#define __Header_convtype__

//#include "dctool/basetype.h"		// should already be included
//#include "dctool/endtype.h"		// should already be included

// The following types may be assigned and manipulated, or used
// in arrays or expressions, as if they were basic types.
//
// However they have EXACTLY the expected length and may be
// mapped onto external types, without alignment restrictions
// if used as components of structures.
//
// In particular sizeof(Uint16_L)==2, sizeof(Uint32_L)==4, etc.
//
// Note however that the assignment and increment operators
// like += and ++, are not implemented.

template <class T,int Hi,int Lo,int Signed>
class Word_Base {
private:
	unsigned char bytes[((Hi>Lo)?Hi-Lo:Lo-Hi)+1];
public:
	Word_Base(void)
		{
			*this=0;
		}

	Word_Base(T word)
		{
			*this=word;
		}

	Word_Base<T,Hi,Lo,Signed>& operator=(T word)
		{
			unsigned index=Lo;
			while (1) {
				bytes[index]=(unsigned char)word;
				word>>=8;
				if (index == Hi) break;
				index+=(Lo>Hi)?-1:1;
			};
			return *this;
		}

	Word_Base<T,Hi,Lo,Signed>& operator=(const Word_Base<T,Hi,Lo,Signed>& i)
		{
			// NB. be very careful about the size here !!!!
			memcpy((char *)this,(const char *)&i,sizeof *this);
			return *this;
		}

	operator T() const
		{
			T word=0;
			unsigned index=Hi;
			// sign extend if necessary in case word is longer than expected
			word=(Signed && (bytes[index]&0x80)) ? (T)ULONG_MAX : (T)0;
			while (1) {
				word<<=8;
				word|=bytes[index];
				if (index == Lo) break;
				index+=(Hi>Lo)?-1:1;
			};
			return word;
		}
};

// Note that the 8 bit types (which have no byte order) are
// returned as longer types, in order to a) shut up the
// shift >= 8 warnings (they should probably not use the
// template) and b) to allow use in iostreams without being
// written as chars, which gets annoying !

typedef Word_Base<Uint16,0,0,0>  Uint8_8;
typedef Word_Base<Uint16,1,0,0> Uint16_L;
typedef Word_Base<Uint16,0,1,0> Uint16_B;
typedef Word_Base<Uint32,3,0,0> Uint32_L;
typedef Word_Base<Uint32,0,3,0> Uint32_B;
typedef Word_Base<Int16,0,0,1>   Int8_8;
typedef Word_Base<Int16,1,0,1>  Int16_L;
typedef Word_Base<Int16,0,1,1>  Int16_B;
typedef Word_Base<Int32,3,0,1>  Int32_L;
typedef Word_Base<Int32,0,3,1>  Int32_B;

#endif // __Header_convtype__
