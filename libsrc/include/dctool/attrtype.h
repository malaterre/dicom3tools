#ifndef __Header_attrtype__
#define __Header_attrtype__

#include "attrtypb.h"

#ifndef LARGESTOTHERDATATOKEEPINMEMORY
#define LARGESTOTHERDATATOKEEPINMEMORY 524288
#endif

/* ************************** Tag Attributes ************************* */

class AttributeTagAttribute : public TagAttribute {
public:
	AttributeTagAttribute(Tag t) : TagAttribute(t) {}
	AttributeTagAttribute(Tag t,Tag v)
		: TagAttribute(t)
		{ addValue(v); }
	const char *	getVR() const	{ return "AT"; }
};

/* ************************ String Attributes ************************ */

class ApplicationEntityAttribute : public NonNumericStringAttribute {
public:
	ApplicationEntityAttribute(Tag t) : NonNumericStringAttribute(t) {}
	ApplicationEntityAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}
	const char *	getVR() const	{ return "AE"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class AgeStringAttribute : public NonNumericStringAttribute {
public:
	AgeStringAttribute(Tag t) : NonNumericStringAttribute(t) {}
	AgeStringAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}

	const char *	getVR() const	{ return "AS"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class CodeStringAttribute : public NonNumericStringAttribute {
public:
	CodeStringAttribute(Tag t) : NonNumericStringAttribute(t) {}

	CodeStringAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}
	CodeStringAttribute(Tag t,const char *v1,const char *v2)
		: NonNumericStringAttribute(t)
		{
			addValue(v1); addValue(v2);
		}
	CodeStringAttribute(Tag t,const char *v1,const char *v2,const char *v3)
		: NonNumericStringAttribute(t)
		{
			addValue(v1); addValue(v2); addValue(v3);
		}

	const char *	getVR() const	{ return "CS"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class CodeStringFileComponentAttribute : public CodeStringFileComponentAttributeBase {
public:
	CodeStringFileComponentAttribute(Tag t) : CodeStringFileComponentAttributeBase(t) {}

	CodeStringFileComponentAttribute(Tag t,const char *v)
		: CodeStringFileComponentAttributeBase(t,v) {}

	const char *	getVR() const	{ return "CS"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class DateStringAttribute : public NonNumericStringAttribute {
public:
	DateStringAttribute(Tag t) : NonNumericStringAttribute(t) {}

	DateStringAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}

	DateStringAttribute(Tag t,Date const & date);

	DateStringAttribute(Tag t,DateTime const & datetime);

	const char *	getVR() const	{ return "DA"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class DateTimeStringAttribute : public NonNumericStringAttribute {
public:
	DateTimeStringAttribute(Tag t) : NonNumericStringAttribute(t) {}

	DateTimeStringAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}

	DateTimeStringAttribute(Tag t,Date const & date,Time const & time);

	const char *	getVR() const	{ return "DT"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class DecimalStringAttribute : public NumericStringAttribute {
public:
	DecimalStringAttribute(Tag t) : NumericStringAttribute(t) {}

	DecimalStringAttribute(Tag t,const char *v)
		: NumericStringAttribute(t,v)	{}
	DecimalStringAttribute(Tag t,Uint16 v)
		: NumericStringAttribute(t,v)	{}
	DecimalStringAttribute(Tag t,Uint32 v)
		: NumericStringAttribute(t,v)	{}
	DecimalStringAttribute(Tag t,Int16 v)
		: NumericStringAttribute(t,v)	{}
	DecimalStringAttribute(Tag t,Int32 v)
		: NumericStringAttribute(t,v)	{}
	DecimalStringAttribute(Tag t,Float32 v)
		: NumericStringAttribute(t,v)	{}
	DecimalStringAttribute(Tag t,Float64 v)
		: NumericStringAttribute(t,v)	{}

	DecimalStringAttribute(Tag t,Float64 v1,Float64 v2)
		: NumericStringAttribute(t)
		{ addValue(v1); addValue(v2); }
	DecimalStringAttribute(Tag t,Float64 v1,Float64 v2,Float64 v3)
		: NumericStringAttribute(t)
		{ addValue(v1); addValue(v2);  addValue(v3); }
	DecimalStringAttribute(Tag t,
			Float64 v1,Float64 v2,Float64 v3,
			Float64 v4,Float64 v5,Float64 v6)
		: NumericStringAttribute(t)
		{
			addValue(v1); addValue(v2);  addValue(v3);
			addValue(v4); addValue(v5);  addValue(v6);
		}

	const char *	getVR() const	{ return "DS"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class IntegerStringAttribute : public NumericStringAttribute {
public:
	IntegerStringAttribute(Tag t) : NumericStringAttribute(t) {}

	IntegerStringAttribute(Tag t,const char *v)
		: NumericStringAttribute(t,v)	{}
	IntegerStringAttribute(Tag t,Uint16 v)
		: NumericStringAttribute(t,v)	{}
	IntegerStringAttribute(Tag t,Uint32 v)
		: NumericStringAttribute(t,v)	{}
	IntegerStringAttribute(Tag t,Int16 v)
		: NumericStringAttribute(t,v)	{}
	IntegerStringAttribute(Tag t,Int32 v)
		: NumericStringAttribute(t,v)	{}

	const char *	getVR() const	{ return "IS"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class LongStringAttribute : public NonNumericStringAttribute {
public:
	LongStringAttribute(Tag t) : NonNumericStringAttribute(t) {}

	LongStringAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}
	LongStringAttribute(Tag t,Uint16 v)
		: NonNumericStringAttribute(t,v) {}
	LongStringAttribute(Tag t,Uint32 v)
		: NonNumericStringAttribute(t,v) {}
	LongStringAttribute(Tag t,Int16 v)
		: NonNumericStringAttribute(t,v) {}
	LongStringAttribute(Tag t,Int32 v)
		: NonNumericStringAttribute(t,v) {}
	LongStringAttribute(Tag t,Float32 v)
		: NonNumericStringAttribute(t,v) {}
	LongStringAttribute(Tag t,Float64 v)
		: NonNumericStringAttribute(t,v) {}

	const char *	getVR() const	{ return "LO"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class LongTextAttribute : public LongTextAttributeBase {
public:
	LongTextAttribute(Tag t) : LongTextAttributeBase(t) {}
	LongTextAttribute(Tag t,const char *v)
		: LongTextAttributeBase(t,v) {}
	const char *	getVR() const	{ return "LT"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class PersonNameAttribute : public NonNumericStringAttribute {
public:
	PersonNameAttribute(Tag t) : NonNumericStringAttribute(t) {}
	PersonNameAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}
	const char *	getVR() const	{ return "PN"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class ShortStringAttribute : public NonNumericStringAttribute {
public:
	ShortStringAttribute(Tag t) : NonNumericStringAttribute(t) {}

	ShortStringAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}
	ShortStringAttribute(Tag t,Uint16 v)
		: NonNumericStringAttribute(t,v) {}
	ShortStringAttribute(Tag t,Uint32 v)
		: NonNumericStringAttribute(t,v) {}
	ShortStringAttribute(Tag t,Int16 v)
		: NonNumericStringAttribute(t,v) {}
	ShortStringAttribute(Tag t,Int32 v)
		: NonNumericStringAttribute(t,v) {}
	ShortStringAttribute(Tag t,Float32 v)
		: NonNumericStringAttribute(t,v) {}
	ShortStringAttribute(Tag t,Float64 v)
		: NonNumericStringAttribute(t,v) {}

	const char *	getVR() const	{ return "SH"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class ShortTextAttribute : public TextAttribute {
public:
	ShortTextAttribute(Tag t) : TextAttribute(t) {}
	ShortTextAttribute(Tag t,const char *v)
		: TextAttribute(t,v) {}
	const char *	getVR() const	{ return "ST"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class TimeStringAttribute : public NonNumericStringAttribute {
public:
	TimeStringAttribute(Tag t) : NonNumericStringAttribute(t) {}

	TimeStringAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}

	TimeStringAttribute(Tag t,Time const & time);

	TimeStringAttribute(Tag t,DateTime const & datetime);

	const char *	getVR() const	{ return "TM"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

class UIStringAttribute : public NonNumericStringAttribute {
public:
	UIStringAttribute(Tag t) : NonNumericStringAttribute(t) {}
	UIStringAttribute(Tag t,const char *v)
		: NonNumericStringAttribute(t,v) {}

	const char *	getVR() const	{ return "UI"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			writePaddedValues(stream,0);
		}

	TextOutputStream& writeData(TextOutputStream& stream)
		{
			writePaddedValues(stream,0);
			return stream;
		}

	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false)
		{
			writePadded(stream,0,dict,verbose);
			return stream;
		}

	BinaryOutputStream& write(BinaryOutputStream& stream)
		{
			writePadded(stream,0);
			return stream;
		}

	DicomOutputStream& write(DicomOutputStream& stream)
		{
			writePadded(stream,0);
			return stream;
		}
};

class UnlimitedTextAttribute : public LongTextAttributeBase {
public:
	UnlimitedTextAttribute(Tag t) : LongTextAttributeBase(t) {}
	UnlimitedTextAttribute(Tag t,const char *v)
		: LongTextAttributeBase(t,v) {}
	const char *	getVR() const	{ return "UT"; }
	bool validateVR(TextOutputStream& stream,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const;
};

/* ********************* Numeric Binary Attributes ********************* */

class UnsignedShortAttribute : public NumericBinaryAttribute<Uint16,2> {
public:
	UnsignedShortAttribute(Tag t) : NumericBinaryAttribute<Uint16,2>(t) {}
	UnsignedShortAttribute(Tag t,Uint16 v)
		: NumericBinaryAttribute<Uint16,2>(t)
		{ addValue(v); }
	UnsignedShortAttribute(Tag t,Uint16 v1,Uint16 v2,Uint16 v3)
		: NumericBinaryAttribute<Uint16,2>(t)
		{ addValue(v1); addValue(v2);  addValue(v3); }
	const char *	getVR() const	{ return "US"; }
	~UnsignedShortAttribute() {} // only because buggy g++ 2.7.0 freaks
};

class UnsignedLongAttribute : public NumericBinaryAttribute<Uint32,4> {
public:
	UnsignedLongAttribute(Tag t) : NumericBinaryAttribute<Uint32,4>(t) {}
	UnsignedLongAttribute(Tag t,Uint32 v)
		: NumericBinaryAttribute<Uint32,4>(t)
		{ addValue(v); }
	const char *	getVR() const	{ return "UL"; }
	~UnsignedLongAttribute() {} // only because buggy g++ 2.7.0 freaks
};

class SignedShortAttribute : public NumericBinaryAttribute<Int16,2> {
public:
	SignedShortAttribute(Tag t) : NumericBinaryAttribute<Int16,2>(t) {}
	SignedShortAttribute(Tag t,Int16 v)
		: NumericBinaryAttribute<Int16,2>(t)
		{ addValue(v); }
	const char *	getVR() const	{ return "SS"; }
	~SignedShortAttribute() {} // only because buggy g++ 2.7.0 freaks
};

class SignedLongAttribute : public NumericBinaryAttribute<Int32,4> {
public:
	SignedLongAttribute(Tag t) : NumericBinaryAttribute<Int32,4>(t) {}
	SignedLongAttribute(Tag t,Int32 v)
		: NumericBinaryAttribute<Int32,4>(t)
		{ addValue(v); }
	const char *	getVR() const	{ return "SL"; }
	~SignedLongAttribute() {} // only because buggy g++ 2.7.0 freaks
};

class UnspecifiedShortAttribute : public NumericBinaryAttribute<Uint16,2> {
public:
	UnspecifiedShortAttribute(Tag t)
		: NumericBinaryAttribute<Uint16,2>(t) {}
	UnspecifiedShortAttribute(Tag t,Uint16 v)
		: NumericBinaryAttribute<Uint16,2>(t)
		{ addValue(v); }
	const char *	getVR() const	{ return "XS"; }
	~UnspecifiedShortAttribute() {} // only because buggy g++ 2.7.0 freaks
};

class UnspecifiedLongAttribute : public NumericBinaryAttribute<Uint32,4> {
public:
	UnspecifiedLongAttribute(Tag t)
		: NumericBinaryAttribute<Uint32,4>(t) {}
	UnspecifiedLongAttribute(Tag t,Uint32 v)
		: NumericBinaryAttribute<Uint32,4>(t)
		{ addValue(v); }
	const char *	getVR() const	{ return "XL"; }
	~UnspecifiedLongAttribute() {} // only because buggy g++ 2.7.0 freaks
};

/* ********************* Floating Point Binary Attributes ********************* */

class FloatSingleAttribute : public FloatBinaryAttribute<4> {
public:
	FloatSingleAttribute(Tag t)
		: FloatBinaryAttribute<4>(t) {}
	FloatSingleAttribute(Tag t,Float32 v)
		: FloatBinaryAttribute<4>(t)
		{ addValue(v); }
	const char *	getVR() const	{ return "FL"; }
	~FloatSingleAttribute() {} // only because buggy g++ 2.7.0 freaks
};

class FloatDoubleAttribute : public FloatBinaryAttribute<8> {
public:
	FloatDoubleAttribute(Tag t)
		: FloatBinaryAttribute<8>(t) {}
	FloatDoubleAttribute(Tag t,Float64 v)
		: FloatBinaryAttribute<8>(t)
		{ addValue(v); }
	FloatDoubleAttribute(Tag t,Float64 v1,Float64 v2)
		: FloatBinaryAttribute<8>(t)
		{ addValue(v1); addValue(v2); }
	FloatDoubleAttribute(Tag t,Float64 v1,Float64 v2,Float64 v3)
		: FloatBinaryAttribute<8>(t)
		{ addValue(v1); addValue(v2);  addValue(v3); }
	FloatDoubleAttribute(Tag t,
			Float64 v1,Float64 v2,Float64 v3,
			Float64 v4,Float64 v5,Float64 v6)
		: FloatBinaryAttribute<8>(t)
		{
			addValue(v1); addValue(v2);  addValue(v3);
			addValue(v4); addValue(v5);  addValue(v6);
		}
	const char *	getVR() const	{ return "FD"; }
	~FloatDoubleAttribute() {} // only because buggy g++ 2.7.0 freaks
};

/* ********************* Other Non-PixelData Attributes ********************* */

class OtherByteSmallNonPixelAttribute : public OtherByteSmallNonPixelAttributeBase {
public:
	OtherByteSmallNonPixelAttribute(Tag t) : OtherByteSmallNonPixelAttributeBase(t) {}
	virtual ~OtherByteSmallNonPixelAttribute() {}
};

class OtherWordSmallNonPixelAttribute : public OtherWordSmallNonPixelAttributeBase {
public:
	OtherWordSmallNonPixelAttribute(Tag t) : OtherWordSmallNonPixelAttributeBase(t) {}
	virtual ~OtherWordSmallNonPixelAttribute() {}
};

class OtherByteLargeNonPixelAttribute : public OtherByteLargeNonPixelAttributeBase {
public:
	OtherByteLargeNonPixelAttribute(Tag t,BinaryInputStream &stream,OurStreamPos pos)
		: OtherByteLargeNonPixelAttributeBase(t,stream,pos) {}
	virtual ~OtherByteLargeNonPixelAttribute() {}
};

class OtherWordLargeNonPixelAttribute : public OtherWordLargeNonPixelAttributeBase {
public:
	OtherWordLargeNonPixelAttribute(Tag t,BinaryInputStream &stream,OurStreamPos pos)
		: OtherWordLargeNonPixelAttributeBase(t,stream,pos) {}
	virtual ~OtherWordLargeNonPixelAttribute() {}
};

//class OtherFloatSmallAttribute : public OtherFloatSmallAttributeBase {
//public:
//	OtherFloatSmallAttribute(Tag t) : OtherFloatSmallAttributeBase(t) {}
//	virtual ~OtherFloatSmallAttribute() {}
//};

class OtherFloatLargeAttribute : public OtherFloatLargeAttributeBase {
public:
	OtherFloatLargeAttribute(Tag t,BinaryInputStream &stream,OurStreamPos pos)
		: OtherFloatLargeAttributeBase(t,stream,pos) {}
	virtual ~OtherFloatLargeAttribute() {}
};

/* ********************* Unknown VR Attributes ********************* */

class UnknownSmallAttribute : public UnknownSmallAttributeBase {
public:
	UnknownSmallAttribute(Tag t) : UnknownSmallAttributeBase(t) {}
	virtual ~UnknownSmallAttribute() {}
};

class UnknownLargeAttribute : public UnknownLargeAttributeBase {
public:
	UnknownLargeAttribute(Tag t,BinaryInputStream &stream,OurStreamPos pos)
		: UnknownLargeAttributeBase(t,stream,pos) {}
	virtual ~UnknownLargeAttribute() {}
};

#endif /* __Header_attrtype__ */
