#ifndef __Header_attrtyps__
#define __Header_attrtyps__

#include "strtype.h"

/* ************************ String Attributes ************************ */

class StringAttribute : public Attribute {
private:
	friend class TextAttribute;
protected:
	ValueList<char *> values;
	bool trailingNullByte;
	bool embeddedNullByte;
	bool trailingSpace;
		
	TextOutputStream& writePaddedValues(TextOutputStream& stream,
		char pad);

	TextOutputStream& writePadded(TextOutputStream& stream,
		char pad,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false)
		{
			Attribute::writeBase(stream,dict,verbose,showUsedAndIE);
			stream << "<";
			writePaddedValues(stream,pad);
			stream << "> ";
			return stream;
		}

	BinaryOutputStream& writePaddedValues(BinaryOutputStream& stream,
		char pad);

	BinaryOutputStream& writePadded(BinaryOutputStream& stream,char pad)
		{
			Attribute::writeBase(stream);
			writePaddedValues(stream,pad);
			return stream;
		}

	DicomOutputStream& writePadded(DicomOutputStream& stream,char pad)
		{
			Attribute::writeBase(stream);
			writePaddedValues(stream,pad);
			return stream;
		}
public:
	StringAttribute(Tag t) : Attribute(t)
		{
			trailingNullByte=false;
			embeddedNullByte=false;
			trailingSpace=false;
		}
	
	virtual ~StringAttribute()	{}

	virtual BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			writePaddedValues(stream,' ');
		}

	virtual TextOutputStream& writeData(TextOutputStream& stream)
		{
			writePaddedValues(stream,' ');
			return stream;
		}

	virtual TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false)
		{
			writePadded(stream,' ',dict,verbose,showUsedAndIE);
			return stream;
		}

	virtual BinaryOutputStream& write(BinaryOutputStream& stream)
		{
			writePadded(stream,' ');
			return stream;
		}

	virtual DicomOutputStream& write(DicomOutputStream& stream)
		{
			writePadded(stream,' ');
			return stream;
		}

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	isString(void) const	{ return true; }

	Uint16	getVM(void) const	{ return values.getVM(); }
	Uint32	getVL(void) const;
	Uint16	getValueSize(void) const	{ return 1; }

	bool	isEmptyOrHasAnyEmptyValue(void) const;
	bool	isEmptyOrHasAllEmptyValues(void) const;

	// NB. seems silly to have to redefine those methods that
	// just call the base class methods, but it seems that if
	// ANY method distinguished only by its parameter types
	// is redefined, they all have to be :(. This is enforced
	// by SunPro C++ but not g++.

	// Note also that the use of virtual is limited to those
	// methods we know are going to be redefined in specific
	// derived classes ... this is not intended to be a generic
	// base class.

	bool	getValue(unsigned index,Uint16& vp) const;
	bool	getValue(unsigned index,Uint32& vp) const;
	bool	getValue(unsigned index,Int16& vp) const;
	bool	getValue(unsigned index,Int32& vp) const;
	bool	getValue(unsigned index,Float32& vp) const;
	bool	getValue(unsigned index,Float64& vp) const;
	bool	getValue(unsigned index,Tag& vp) const			{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const;

	virtual void	setValue(unsigned index,Uint16 value);
	virtual void	setValue(unsigned index,Uint32 value);
	virtual void	setValue(unsigned index,Int16 value);
	virtual void	setValue(unsigned index,Int32 value);
	virtual void	setValue(unsigned index,Float32 value);
	virtual void	setValue(unsigned index,Float64 value);
	virtual void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	virtual void	setValue(unsigned index,const char *str);

	virtual void	addValue(Uint16 value);
	virtual void	addValue(Uint32 value);
	virtual void	addValue(Int16 value);
	virtual void	addValue(Int32 value);
	virtual void	addValue(Float32 value);
	virtual void	addValue(Float64 value);
	virtual void	addValue(Tag value)				{ Attribute::addValue(value); }
	virtual void	addValue(const char *str);

	virtual void	addValues(unsigned number,Uint16 *vptr)		{ Attribute::addValues(number,vptr); }
	virtual void	addValues(unsigned number,Uint32 *vptr)		{ Attribute::addValues(number,vptr); }
	virtual void	addValues(unsigned number,Int16 *vptr)		{ Attribute::addValues(number,vptr); }
	virtual void	addValues(unsigned number,Int32 *vptr)		{ Attribute::addValues(number,vptr); }
	virtual void	addValues(unsigned number,Float32 *vptr)	{ Attribute::addValues(number,vptr); }
	virtual void	addValues(unsigned number,Float64 *vptr)	{ Attribute::addValues(number,vptr); }
	virtual void	addValues(unsigned number,Tag *vptr)		{ Attribute::addValues(number,vptr); }
	virtual void	addValues(unsigned number,const char **vptr)	{ Attribute::addValues(number,vptr); }
	virtual void	addValues(const char *vptr);
};

class NonNumericStringAttribute : public StringAttribute {
public:
	NonNumericStringAttribute(Tag t) : StringAttribute(t) {}
	virtual ~NonNumericStringAttribute()	{}

	NonNumericStringAttribute(Tag t,const char *v)
		: StringAttribute(t)	{ addValue(v); }
	NonNumericStringAttribute(Tag t,Uint16 v)
		: StringAttribute(t)	{ addValue(v); }
	NonNumericStringAttribute(Tag t,Uint32 v)
		: StringAttribute(t)	{ addValue(v); }
	NonNumericStringAttribute(Tag t,Int16 v)
		: StringAttribute(t)	{ addValue(v); }
	NonNumericStringAttribute(Tag t,Int32 v)
		: StringAttribute(t)	{ addValue(v); }
	NonNumericStringAttribute(Tag t,Float32 v)
		: StringAttribute(t)	{ addValue(v); }
	NonNumericStringAttribute(Tag t,Float64 v)
		: StringAttribute(t)	{ addValue(v); }
};

class NumericStringAttribute : public StringAttribute {
public:
	NumericStringAttribute(Tag t) : StringAttribute(t) {}
	virtual ~NumericStringAttribute()	{}
	NumericStringAttribute(Tag t,const char *v)
		: StringAttribute(t)	{ addValue(v); }
	NumericStringAttribute(Tag t,Uint16 v)
		: StringAttribute(t)	{ addValue(v); }
	NumericStringAttribute(Tag t,Uint32 v)
		: StringAttribute(t)	{ addValue(v); }
	NumericStringAttribute(Tag t,Int16 v)
		: StringAttribute(t)	{ addValue(v); }
	NumericStringAttribute(Tag t,Int32 v)
		: StringAttribute(t)	{ addValue(v); }
	NumericStringAttribute(Tag t,Float32 v)
		: StringAttribute(t)	{ addValue(v); }
	NumericStringAttribute(Tag t,Float64 v)
		: StringAttribute(t)	{ addValue(v); }

	bool	isNumeric(void) const		{ return 1; }
	bool	isNumericString(void) const	{ return 1; }
};

class TextAttribute : public NonNumericStringAttribute {
public:
	TextAttribute(Tag t) : NonNumericStringAttribute(t) {}
	TextAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t)	{ addValue(v); }

	virtual ~TextAttribute()	{}

	virtual void	setValue(unsigned index,Uint16 value)		{ NonNumericStringAttribute::setValue(index,value); }
	virtual void	setValue(unsigned index,Uint32 value)		{ NonNumericStringAttribute::setValue(index,value); }
	virtual void	setValue(unsigned index,Int16 value)		{ NonNumericStringAttribute::setValue(index,value); }
	virtual void	setValue(unsigned index,Int32 value)		{ NonNumericStringAttribute::setValue(index,value); }
	virtual void	setValue(unsigned index,Float32 value)	 	{ NonNumericStringAttribute::setValue(index,value); }
	virtual void	setValue(unsigned index,Float64 value)		{ NonNumericStringAttribute::setValue(index,value); }
	virtual void	setValue(unsigned index,Tag value)		{ NonNumericStringAttribute::setValue(index,value); }
	virtual void	setValue(unsigned index,const char *value);

	virtual void	addValue(Uint16 value)				{ NonNumericStringAttribute::addValue(value); }
	virtual void	addValue(Uint32 value)				{ NonNumericStringAttribute::addValue(value); }
	virtual void	addValue(Int16 value)				{ NonNumericStringAttribute::addValue(value); }
	virtual void	addValue(Int32 value)				{ NonNumericStringAttribute::addValue(value); }
	virtual void	addValue(Float32 value)				{ NonNumericStringAttribute::addValue(value); }
	virtual void	addValue(Float64 value)				{ NonNumericStringAttribute::addValue(value); }
	virtual void	addValue(Tag value)				{ NonNumericStringAttribute::addValue(value); }
	virtual void	addValue(const char *value);

};

class LongTextAttributeBase : public TextAttribute {
public:
	LongTextAttributeBase(Tag t) : TextAttribute(t) {}
	LongTextAttributeBase(Tag t,const char *v)
		: TextAttribute(t)	{ addValue(v); }

	virtual ~LongTextAttributeBase()	{}

	void	setValue(unsigned index,Uint16 value)		{ TextAttribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ TextAttribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ TextAttribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ TextAttribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ TextAttribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ TextAttribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ TextAttribute::setValue(index,value); }
	void	setValue(unsigned index,const char *value);

	void	addValue(Uint16 value)				{ NonNumericStringAttribute::addValue(value); }
	void	addValue(Uint32 value)				{ NonNumericStringAttribute::addValue(value); }
	void	addValue(Int16 value)				{ NonNumericStringAttribute::addValue(value); }
	void	addValue(Int32 value)				{ NonNumericStringAttribute::addValue(value); }
	void	addValue(Float32 value)				{ NonNumericStringAttribute::addValue(value); }
	void	addValue(Float64 value)				{ NonNumericStringAttribute::addValue(value); }
	void	addValue(Tag value)				{ NonNumericStringAttribute::addValue(value); }
	void	addValue(const char *value);

	void	addValues(unsigned number,Uint16 *vptr)		{ TextAttribute::addValues(number,vptr); }
	void	addValues(unsigned number,Uint32 *vptr)		{ TextAttribute::addValues(number,vptr); }
	void	addValues(unsigned number,Int16 *vptr)		{ TextAttribute::addValues(number,vptr); }
	void	addValues(unsigned number,Int32 *vptr)		{ TextAttribute::addValues(number,vptr); }
	void	addValues(unsigned number,Float32 *vptr)	{ TextAttribute::addValues(number,vptr); }
	void	addValues(unsigned number,Float64 *vptr)	{ TextAttribute::addValues(number,vptr); }
	void	addValues(unsigned number,Tag *vptr)		{ TextAttribute::addValues(number,vptr); }
	void	addValues(unsigned number,const char **vptr)	{ TextAttribute::addValues(number,vptr); }
	void	addValues(const char *vptr);
};

class CodeStringFileComponentAttributeBase : public NonNumericStringAttribute {
public:
	CodeStringFileComponentAttributeBase(Tag t) : NonNumericStringAttribute(t) {}
	CodeStringFileComponentAttributeBase(Tag t,const char *v)
		: NonNumericStringAttribute(t)	{ addValues(v); }

	virtual ~CodeStringFileComponentAttributeBase()	{}

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *value)	{ NonNumericStringAttribute::setValue(index,value); }

	void	addValue(Uint16 value)				{ Attribute::addValue(value); }
	void	addValue(Uint32 value)				{ Attribute::addValue(value); }
	void	addValue(Int16 value)				{ Attribute::addValue(value); }
	void	addValue(Int32 value)				{ Attribute::addValue(value); }
	void	addValue(Float32 value)				{ Attribute::addValue(value); }
	void	addValue(Float64 value)				{ Attribute::addValue(value); }
	void	addValue(Tag value)				{ Attribute::addValue(value); }
	void	addValue(const char *value)			{ NonNumericStringAttribute::addValue(value); }

	void	addValues(unsigned number,Uint16 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Uint32 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Int16 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Int32 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Float32 *vptr)	{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Float64 *vptr)	{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Tag *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,const char **vptr)	{ NonNumericStringAttribute::addValues(number,vptr); }
	void	addValues(const char *vptr);
};

#endif /* __Header_attrtyps__ */
