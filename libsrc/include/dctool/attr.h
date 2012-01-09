// public interface for Attribute class

#ifndef __Header_attr__
#define __Header_attr__

#include "ie.h"
#include "attrtag.h"
#include "charset.h"

class AttributeList;
class OtherUnspecifiedLargeAttributeBase;
class ElementDictionary;

class Attribute {
private:
	Tag	tag;
	Uint32	byteoffset;
	bool used;
	InformationEntity ie;
protected:
	TextOutputStream&	writeBase(
		TextOutputStream& stream,
		ElementDictionary *dict=0,
		bool verbose=false,
		bool showUsedAndIE=false);
	BinaryOutputStream&	writeBase(BinaryOutputStream&);
	DicomOutputStream&	writeBase(DicomOutputStream&);
public:
	Attribute(Tag t);
	virtual ~Attribute();

	void setTag(Tag t) { tag=t; }
	
	void	setByteOffset(Uint32 offset);
	Uint32	getByteOffset(void) const;

	void	setUsed(void)  { used=true; }
	void	setUnused(void)  { used=false; }
	bool	isUsed(void) const { return used; }
	
	void	setInformationEntity(InformationEntity i) { ie = i; }
	InformationEntity	getInformationEntity() { return ie; }

	virtual BinaryOutputStream& writeData(BinaryOutputStream& stream);
	virtual TextOutputStream& writeData(TextOutputStream& stream);

	virtual	TextOutputStream&	write(
		TextOutputStream& stream,
		ElementDictionary *dict=0,
		bool verbose=false,
		bool showUsedAndIE=false) = 0;
	virtual	BinaryOutputStream&	write(BinaryOutputStream&) = 0;
	virtual	DicomOutputStream&	write(DicomOutputStream& stream) = 0;

	virtual BinaryInputStream&
			read(BinaryInputStream& stream,Uint32 lengthinbytes) = 0;

	virtual void setOutputEncoding(
			TransferSyntax *transfersyntax,
			unsigned short bytesinword,
			unsigned short bitsallocated,
			unsigned short bitsstored,
			unsigned short highbit,
			Uint32 length=0xffffffff);

	virtual	bool validateVR(TextOutputStream& stream,
			SpecificCharacterSetInfo *specificCharacterSetInfo = NULL,
			ElementDictionary *dict=0) const
		{ (void)stream; (void)dict; return true; }

	bool validateRetired(TextOutputStream& stream,
			ElementDictionary *dict=0) const;

	virtual const char *	getVR(void) const = 0;
	virtual Uint16		getVM(void) const = 0;
	virtual Uint32		getVL(void) const = 0;
		Tag		getTag(void) const	{ return tag; }
	virtual Uint16		getValueSize(void) const = 0;
		
	virtual bool	isNumeric(void) const		{ return false; }
	virtual bool	isNumericBinary(void) const	{ return false; }
	virtual bool	isNumericString(void) const	{ return false; }
	virtual bool	isString(void) const		{ return false; }
	virtual bool	isTag(void) const		{ return false; }
	virtual bool	isOtherData(void) const		{ return false; }
	virtual bool	isOtherByteNonPixel(void) const	{ return false; }
	virtual bool	isOtherWordNonPixel(void) const	{ return false; }
	virtual bool	isUnknown(void) const		{ return false; }
	virtual bool	isSequence(void) const		{ return false; }

	virtual OtherUnspecifiedLargeAttributeBase *castToOtherData(void);

	virtual bool	isEmpty(void) const			{ return getVM() == 0; }
	virtual bool	isOne(void) const			{ return getVM() == 1; }
	virtual bool	isMultiple(void) const			{ return getVM() > 1; }
	virtual bool	isEmptyOrHasAnyEmptyValue(void) const	{ return getVL() == 0 || isEmpty(); }	// overriden in string attributes
													// should be OK for SQ even if VL is 0xFFFFFFFF
	virtual bool	isEmptyOrHasAllEmptyValues(void) const	{ return getVL() == 0 || isEmpty(); }	// overriden in string attributes
													// should be OK for SQ even if VL is 0xFFFFFFFF
		
	virtual bool	getValue(unsigned,Uint16&) const;
	virtual bool	getValue(unsigned,Uint32&) const;
	virtual bool	getValue(unsigned,Int16&) const;
	virtual bool	getValue(unsigned,Int32&) const;
	virtual bool	getValue(unsigned,Float32&) const;
	virtual bool	getValue(unsigned,Float64&) const;
	virtual bool	getValue(unsigned,Tag&) const;
	virtual bool	getValue(unsigned,char * &) const;
	virtual bool	getValue(const unsigned char * & rvalue,Uint32 &rlength) const;
	virtual bool	getValue(const Uint16 * & rvalue,Uint32 &rlengthinwords) const;

	virtual void	setValue(unsigned,Uint16);
	virtual void	setValue(unsigned,Uint32);
	virtual void	setValue(unsigned,Int16);
	virtual void	setValue(unsigned,Int32);
	virtual void	setValue(unsigned,Float32);
	virtual void	setValue(unsigned,Float64);
	virtual void	setValue(unsigned,Tag);
	virtual void	setValue(unsigned,const char *);
	virtual void	setValue(const unsigned char *values,Uint32 length);
	virtual void	setValue(const Uint16 *values,Uint32 lengthinwords);

	virtual void	addValue(Uint16);
	virtual void	addValue(Uint32);
	virtual void	addValue(Int16)	;
	virtual void	addValue(Int32)	;
	virtual void	addValue(Float32);
	virtual void	addValue(Float64);
	virtual void	addValue(Tag);
	virtual void	addValue(const char *);

	virtual void	addValues(unsigned,Uint16 *);
	virtual void	addValues(unsigned,Uint32 *);
	virtual void	addValues(unsigned,Int16 *);
	virtual void	addValues(unsigned,Int32 *);
	virtual void	addValues(unsigned,Float32 *);
	virtual void	addValues(unsigned,Float64 *);
	virtual void	addValues(unsigned,Tag *);
	virtual void	addValues(unsigned,const char **);
	virtual void	addValues(const char *);

	virtual int	getLists(AttributeList ***);

	virtual bool	verifyDefinedTerms(char *(*method)(char *value),bool verbose,TextOutputStream& log,ElementDictionary *dict,int which=-1) const;
	virtual bool	verifyEnumValues(char *(*method)(char *value),bool verbose,TextOutputStream& log,ElementDictionary *dict,int which=-1) const;
	virtual bool	verifyEnumValues(char *(*method)(Uint16 value),bool verbose,TextOutputStream& log,ElementDictionary *dict,int which=-1) const;
	virtual bool	verifyBitMap(char *(*method)(Uint16 value),bool verbose,TextOutputStream& log,ElementDictionary *dict,int which=-1) const;
	virtual bool	verifyEnumValues(char *(*method)(Uint16 group,Uint16 value),bool verbose,TextOutputStream& log,ElementDictionary *dict,int which=-1) const;
	virtual bool	verifyNotZero(bool verbose,TextOutputStream& log,ElementDictionary *dict,int which=-1,bool warningNotError=true) const;

	virtual bool	verifyVR(const char *module,const char *element,TextOutputStream& log,ElementDictionary *dict) const;
	virtual bool	verifyVM(const char *module,const char *element,TextOutputStream& log,ElementDictionary *dict,Uint16 multiplicityMin=0,Uint16 multiplicityMax=0,const char *source=NULL) const;
};

#endif // __Header_attr__
