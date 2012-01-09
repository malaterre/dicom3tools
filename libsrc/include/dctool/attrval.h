#ifndef __Header_attrval__
#define __Header_attrval__

#include "strtype.h"

// Allows use of default type conversions to extract a single value
// of the appropriate type, if present, else a default value
// may be supplied (which will be 0 if absent), eg:
//
// AttributeList list;
// Uint16 bitsStored=AttributeValue(list[TagFromName(BitsStored)],16);

class AttributeValue {
	Attribute 	*a;
	unsigned long 	defaultinteger;
	double 		defaultfloat;
	const char 		*defaultstring;
public:
	AttributeValue(Attribute *attr,double def=0)
		{
			a=attr;
			defaultinteger=(unsigned long)def;
			defaultfloat=def;
			defaultstring=0;
		}

	AttributeValue(Attribute *attr,const char *defstr)
		{
			a=attr;
			defaultinteger=0;
			defaultfloat=0;
			defaultstring=defstr;
		}

	// If more than one value, only returns the first value

	operator Uint16 () const
		{
			Uint16 value;
			return (a && a->isNumeric() && a->getVM() >= 1 && a->getValue((Uint16)0,value))
				? value : (Uint16)defaultinteger;
		}
	operator Uint32 () const
		{
			Uint32 value;
			return (a && a->isNumeric() && a->getVM() >= 1 && a->getValue((Uint16)0,value))
				? value : (Uint32)defaultinteger;
		}
	operator Int16 () const
		{
			Int16 value;
			return (a && a->isNumeric() && a->getVM() >= 1 && a->getValue((Uint16)0,value))
				? value : (Int16)defaultinteger;
		}
	operator Int32 () const
		{
			Int32 value;
			return (a && a->isNumeric() && a->getVM() >= 1 && a->getValue((Uint16)0,value))
				? value : (Int32)defaultinteger;
		}
	operator Float32 () const
		{
			Float32 value;
			return (a && a->isNumeric() && a->getVM() >= 1 && a->getValue((Uint16)0,value))
				? value : (Float32)defaultfloat;
		}
	operator char * () const
		{
			// NB. getValue() returns a copy, so don't need another StrDup(value)
			char * value;
			return (a && a->isString() && a->getVM() >= 1 && a->getValue((Uint16)0,value))
				? value : StrDup(defaultstring);
		}
};

#endif /* __Header_attrval__ */
