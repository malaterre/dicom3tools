#ifndef __Header_attrtypt__
#define __Header_attrtypt__

/* ************************** Tag Attributes ************************* */

class TagAttribute : public Attribute {
private:
	ValueList<Tag> values;

	BinaryOutputStream& writeValues(BinaryOutputStream& stream);
public:
	TagAttribute(Tag t) : Attribute(t) {}
	virtual ~TagAttribute()	{}

	BinaryOutputStream& writeData(BinaryOutputStream& stream)
		{
			writeValues(stream);
		}

	TextOutputStream& writeData(TextOutputStream& stream);

	TextOutputStream& write(TextOutputStream& stream,
		ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false)
		{
			Attribute::writeBase(stream,dict,verbose,showUsedAndIE);
			stream << "{";
			writeData(stream);
			stream << "} ";
			return stream;
		}

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

	BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	bool	isTag(void) const	{ return true; }

	Uint16	getVM(void) const	{ return values.getVM(); }
	Uint32	getVL(void) const	{ return 4*getVM(); }
	Uint16	getValueSize(void) const	{ return 4; }

	bool	getValue(unsigned index,Uint16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Uint32& vp) const;
	bool	getValue(unsigned index,Int16& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Int32& vp) const;
	bool	getValue(unsigned index,Float32& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Float64& vp) const	{ return Attribute::getValue(index,vp); }
	bool	getValue(unsigned index,Tag& vp) const;
	bool	getValue(unsigned index,char * & rvalue) const	{ return Attribute::getValue(index,rvalue); }

	void	setValue(unsigned index,Uint16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Uint32 value);
	void	setValue(unsigned index,Int16 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Int32 value);
	void	setValue(unsigned index,Float32 value)	 	{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Float64 value)		{ Attribute::setValue(index,value); }
	void	setValue(unsigned index,Tag value);
	void	setValue(unsigned index,const char *str)	{ Attribute::setValue(index,str); }

	void	addValue(Uint16 value)				{ Attribute::addValue(value); }
	void	addValue(Uint32 value);
	void	addValue(Int16 value)				{ Attribute::addValue(value); }
	void	addValue(Int32 value);
	void	addValue(Float32 value)				{ Attribute::addValue(value); }
	void	addValue(Float64 value)				{ Attribute::addValue(value); }
	void	addValue(Tag value);
	void	addValue(const char *str)			{ Attribute::addValue(str); }

	void	addValues(unsigned number,Uint16 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Uint32 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Int16 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Int32 *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Float32 *vptr)	{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Float64 *vptr)	{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,Tag *vptr)		{ Attribute::addValues(number,vptr); }
	void	addValues(unsigned number,const char **vptr)	{ Attribute::addValues(number,vptr); }
	void	addValues(const char *vptr)			{ Attribute::addValues(vptr); }
};

#endif /* __Header_attrtypt__ */
