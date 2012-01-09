#ifndef __Header_attrtypo__
#define __Header_attrtypo__

/* ************************** Other Non-PixelData Attributes ************************* */

class OtherNonPixelAttribute : public Attribute {
protected:
	Uint32 lengthinbytes; 
	virtual BinaryOutputStream& writeValues(BinaryOutputStream& stream) = 0;
public:
	OtherNonPixelAttribute(Tag t);
	virtual ~OtherNonPixelAttribute();

	virtual BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return writeValues(stream);
		}
	virtual	TextOutputStream& writeData(TextOutputStream& stream) = 0;

	virtual	TextOutputStream& write(
		TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false) = 0;

	BinaryOutputStream& write(BinaryOutputStream& stream)
		{
			Attribute::writeBase(stream);
			writeValues(stream);
			return stream;
		}

	DicomOutputStream& write(DicomOutputStream& stream)
		{
			Attribute::writeBase(stream);
			writeValues(stream);
			return stream;
		}

	Uint16	getVM(void) const	{ return 1; }
	Uint32	getVL(void) const	{ return lengthinbytes; }
};

class OtherByteSmallNonPixelAttributeBase : public OtherNonPixelAttribute {
private:
	unsigned char *data;
	BinaryOutputStream& writeValues(BinaryOutputStream& stream);
public:
	OtherByteSmallNonPixelAttributeBase(Tag t);
	virtual ~OtherByteSmallNonPixelAttributeBase();

	const char *	getVR() const	{ return "OB"; }
	Uint16	getValueSize(void) const	{ return 1; }

	bool	isOtherByteNonPixel(void) const	{ return true; }

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return OtherNonPixelAttribute::writeData(stream);
		}
	TextOutputStream& writeData(TextOutputStream& stream);
	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false);
	BinaryOutputStream& write(BinaryOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }
	DicomOutputStream& write(DicomOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	getValue(unsigned index,Uint16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Uint32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float64& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Tag& vp) const		{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }
	bool	getValue(const unsigned char * & rvalue,Uint32 &rlength) const;
	bool	getValue(const Uint16 * & rvalue,Uint32 &rlengthinwords) const
								{ return Attribute::getValue(rvalue,rlengthinwords); }

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }
	void	setValue(const unsigned char *values,Uint32 length);
	void	setValue(const Uint16 *values,Uint32 lengthinwords)
								{ Attribute::setValue(values,lengthinwords); }
};

class OtherWordSmallNonPixelAttributeBase : public OtherNonPixelAttribute {
private:
	Uint16 *data;
	BinaryOutputStream& writeValues(BinaryOutputStream& stream);
public:
	OtherWordSmallNonPixelAttributeBase(Tag t);
	virtual ~OtherWordSmallNonPixelAttributeBase();

	const char *	getVR() const	{ return "OW"; }
	Uint16	getValueSize(void) const	{ return 2; }

	bool	isOtherWordNonPixel(void) const	{ return true; }

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return OtherNonPixelAttribute::writeData(stream);
		}
	TextOutputStream& writeData(TextOutputStream& stream);
	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false);
	BinaryOutputStream& write(BinaryOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }
	DicomOutputStream& write(DicomOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	getValue(unsigned index,Uint16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Uint32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float64& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Tag& vp) const		{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }
	bool	getValue(const unsigned char * & rvalue,Uint32 &rlength) const
								{ return Attribute::getValue(rvalue,rlength); }
	bool	getValue(const Uint16 * & rvalue,Uint32 &rlengthinwords) const;

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }
	void	setValue(const unsigned char *values,Uint32 length)
								{ Attribute::setValue(values,length); }
	void	setValue(const Uint16 *values,Uint32 lengthinwords);
};

class OtherByteLargeNonPixelAttributeBase : public OtherNonPixelAttribute {
private:
	BinaryInputStream *srcstream;
	OurStreamPos srcpos;
	OurStreamPos srclength;
	bool hasSrclength;

	BinaryOutputStream& writeValues(BinaryOutputStream& stream);
public:
	OtherByteLargeNonPixelAttributeBase(Tag t,BinaryInputStream &stream,OurStreamPos pos);
	OtherByteLargeNonPixelAttributeBase(Tag t,BinaryInputStream &stream,OurStreamPos pos,OurStreamPos length);
	virtual ~OtherByteLargeNonPixelAttributeBase();

	const char *	getVR() const	{ return "OB"; }
	Uint16	getValueSize(void) const	{ return 1; }

	bool	isOtherByteNonPixel(void) const	{ return true; }

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return OtherNonPixelAttribute::writeData(stream);
		}
	TextOutputStream& writeData(TextOutputStream& stream);
	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false);
	BinaryOutputStream& write(BinaryOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }
	DicomOutputStream& write(DicomOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	getValue(unsigned index,Uint16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Uint32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float64& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Tag& vp) const		{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }
	bool	getValue(const unsigned char * & rvalue,Uint32 &rlength) const;
	bool	getValue(const Uint16 * & rvalue,Uint32 &rlengthinwords) const
								{ return Attribute::getValue(rvalue,rlengthinwords); }

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }
	void	setValue(const unsigned char *values,Uint32 length);
	void	setValue(const Uint16 *values,Uint32 lengthinwords)
								{ Attribute::setValue(values,lengthinwords); }
};

class OtherWordLargeNonPixelAttributeBase : public OtherNonPixelAttribute {
private:
	BinaryInputStream *srcstream;
	OurStreamPos srcpos;
	Endian srcendian;

	BinaryOutputStream& writeValues(BinaryOutputStream& stream);
public:
	OtherWordLargeNonPixelAttributeBase(Tag t,BinaryInputStream &stream,OurStreamPos pos);
	virtual ~OtherWordLargeNonPixelAttributeBase();

	const char *	getVR() const	{ return "OW"; }
	Uint16	getValueSize(void) const	{ return 2; }

	bool	isOtherWordNonPixel(void) const	{ return true; }

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return OtherNonPixelAttribute::writeData(stream);
		}
	TextOutputStream& writeData(TextOutputStream& stream);
	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false);
	BinaryOutputStream& write(BinaryOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }
	DicomOutputStream& write(DicomOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	getValue(unsigned index,Uint16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Uint32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float64& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Tag& vp) const		{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }
	bool	getValue(const unsigned char * & rvalue,Uint32 &rlength) const
								{ return Attribute::getValue(rvalue,rlength); }
	bool	getValue(const Uint16 * & rvalue,Uint32 &rlengthinwords) const;

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }
	void	setValue(const unsigned char *values,Uint32 length)
								{ Attribute::setValue(values,length); }
	void	setValue(const Uint16 *values,Uint32 lengthinwords);
};

/* ********************* OF VR Attributes ********************* */

#ifdef CRAP
class OtherFloatSmallAttributeBase : public OtherNonPixelAttribute {
private:
	unsigned char *data;
	BinaryOutputStream& writeValues(BinaryOutputStream& stream);
public:
	OtherFloatSmallAttributeBase(Tag t);
	virtual ~OtherFloatSmallAttributeBase();

	const char *	getVR() const	{ return "OF"; }
	Uint16	getValueSize(void) const	{ return 4; }

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return OtherNonPixelAttribute::writeData(stream);
		}
	TextOutputStream& writeData(TextOutputStream& stream);
	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false);
	BinaryOutputStream& write(BinaryOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }
	DicomOutputStream& write(DicomOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	getValue(unsigned index,Uint16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Uint32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float64& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Tag& vp) const		{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }
	bool	getValue(const unsigned char * & rvalue,Uint32 &rlength) const;
	bool	getValue(const Uint16 * & rvalue,Uint32 &rlengthinwords) const
								{ return Attribute::getValue(rvalue,rlengthinwords); }

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }
	void	setValue(const unsigned char *values,Uint32 length);
	void	setValue(const Uint16 *values,Uint32 lengthinwords)
								{ Attribute::setValue(values,lengthinwords); }
};
#endif

class OtherFloatLargeAttributeBase : public OtherNonPixelAttribute {
private:
	BinaryInputStream *srcstream;
	OurStreamPos srcpos;
	Endian srcendian;

	BinaryOutputStream& writeValues(BinaryOutputStream& stream);
public:
	OtherFloatLargeAttributeBase(Tag t,BinaryInputStream &stream,OurStreamPos pos);
	virtual ~OtherFloatLargeAttributeBase();

	const char *	getVR() const	{ return "OF"; }
	Uint16	getValueSize(void) const	{ return 4; }

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return OtherNonPixelAttribute::writeData(stream);
		}
	TextOutputStream& writeData(TextOutputStream& stream);
	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false);
	BinaryOutputStream& write(BinaryOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }
	DicomOutputStream& write(DicomOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	getValue(unsigned index,Uint16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Uint32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float64& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Tag& vp) const		{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }
	bool	getValue(const unsigned char * & rvalue,Uint32 &rlength) const;
	bool	getValue(const Uint16 * & rvalue,Uint32 &rlengthinwords) const
								{ return Attribute::getValue(rvalue,rlengthinwords); }

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }
	void	setValue(const unsigned char *values,Uint32 length);
	void	setValue(const Uint16 *values,Uint32 lengthinwords)
								{ Attribute::setValue(values,lengthinwords); }
};

/* ********************* Unknown VR Attributes ********************* */

class UnknownSmallAttributeBase : public OtherNonPixelAttribute {
private:
	unsigned char *data;
	BinaryOutputStream& writeValues(BinaryOutputStream& stream);
public:
	UnknownSmallAttributeBase(Tag t);
	virtual ~UnknownSmallAttributeBase();

	const char *	getVR() const	{ return "UN"; }
	Uint16	getValueSize(void) const	{ return 1; }

	bool	isUnknown(void) const	{ return true; }

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return OtherNonPixelAttribute::writeData(stream);
		}
	TextOutputStream& writeData(TextOutputStream& stream);
	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false);
	BinaryOutputStream& write(BinaryOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }
	DicomOutputStream& write(DicomOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	getValue(unsigned index,Uint16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Uint32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float64& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Tag& vp) const		{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }
	bool	getValue(const unsigned char * & rvalue,Uint32 &rlength) const;
	bool	getValue(const Uint16 * & rvalue,Uint32 &rlengthinwords) const
								{ return Attribute::getValue(rvalue,rlengthinwords); }

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }
	void	setValue(const unsigned char *values,Uint32 length);
	void	setValue(const Uint16 *values,Uint32 lengthinwords)
								{ Attribute::setValue(values,lengthinwords); }

	void	addValues(const char *);
};

class UnknownLargeAttributeBase : public OtherNonPixelAttribute {
private:
	BinaryInputStream *srcstream;
	OurStreamPos srcpos;

	BinaryOutputStream& writeValues(BinaryOutputStream& stream);
public:
	UnknownLargeAttributeBase(Tag t,BinaryInputStream &stream,OurStreamPos pos);
	virtual ~UnknownLargeAttributeBase();

	const char *	getVR() const	{ return "UN"; }
	Uint16	getValueSize(void) const	{ return 1; }

	bool	isUnknown(void) const	{ return true; }

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			return OtherNonPixelAttribute::writeData(stream);
		}
	TextOutputStream& writeData(TextOutputStream& stream);
	TextOutputStream& write(TextOutputStream& stream,ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false);
	BinaryOutputStream& write(BinaryOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }
	DicomOutputStream& write(DicomOutputStream& stream) { return OtherNonPixelAttribute::write(stream); }

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	getValue(unsigned index,Uint16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Uint32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float64& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Tag& vp) const		{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }
	bool	getValue(const unsigned char * & rvalue,Uint32 &rlength) const;
	bool	getValue(const Uint16 * & rvalue,Uint32 &rlengthinwords) const
								{ return Attribute::getValue(rvalue,rlengthinwords); }

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }
	void	setValue(const unsigned char *values,Uint32 length);
	void	setValue(const Uint16 *values,Uint32 lengthinwords)
								{ Attribute::setValue(values,lengthinwords); }
};

#endif /* __Header_attrtypo__ */
