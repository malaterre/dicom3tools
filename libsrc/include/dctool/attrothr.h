#ifndef __Header_attrothr__
#define __Header_attrothr__

#include "srcsink.h"

/* ********************* Large Other Attributes ********************* */

class OtherUnspecifiedLargeAttributeBase : public Attribute {
protected:
	class SourceBase<Uint16> *srcpixeldata;
	class SourceBase<unsigned char> *srcrawdata;

	Uint16 rows;
	Uint16 columns;
	Uint16 frames;
	Uint16 samplesperpixel;

	Uint16 srcbitsallocated;
	Uint16 srcbitsstored;
	Uint16 srchighbit;

	TransferSyntax *dsttransfersyntax;
	Uint16 dstbytesinword;
	Uint16 dstbitsallocated;
	Uint16 dstbitsstored;
	Uint16 dsthighbit;
	Uint32	dstlength;
	Endian dstendian;
	
	bool suppressScalingOnBitDepthConversion;

	class PointFilter<Uint16,Uint16> *pixelDataPointTransformFilter;

	void Proposal14EncodingRules(
		Uint16 bitsallocated,
		TransferSyntax *transfersyntax,
		Uint16& bytesinword,
		Endian& encodedendian,
		Uint32& length,
		Endian forceEndian);

public:
	OtherUnspecifiedLargeAttributeBase(Tag t,
		Uint16 r,Uint16 c,Uint16 f,Uint16 sperp);

	virtual ~OtherUnspecifiedLargeAttributeBase();

	virtual BinaryOutputStream& writeData(BinaryOutputStream& stream);
	virtual DicomOutputStream& writeData(DicomOutputStream& stream);
	virtual TextOutputStream& writeData(TextOutputStream& stream)
		{
			return Attribute::writeData(stream);
		}

	virtual ostream& writeRaw(ostream& stream);
	virtual ostream& writeRaw(ostream& stream,Uint32 byteOffset,Uint32 byteCount);

	virtual TextOutputStream& write(TextOutputStream& stream,
			ElementDictionary *dict=0,bool verbose=false,bool showUsedAndIE=false);

	virtual BinaryOutputStream& write(BinaryOutputStream& stream);

	virtual DicomOutputStream& write(DicomOutputStream& stream);

	virtual BinaryInputStream& read(BinaryInputStream& stream,Uint32 length);

	void setOutputEncoding(
			TransferSyntax *transfersyntax,
			Uint16 bytesinword,
			Uint16 bitsallocated,
			Uint16 bitsstored,
			Uint16 highbit,
			Uint32 length=0xffffffff);

	void setSuppressScalingOnBitDepthConversion(bool doNotConvert) { suppressScalingOnBitDepthConversion = doNotConvert; }

	const char *	getVR() const
		{
			return dstbytesinword == 1
				? "OB"
				: (dstbytesinword == 2
					? "OW"
					: "OX");
		}

	Uint32		getVL(void) const	{ return dstlength; }
	Uint16		getVM(void) const	{ return 1; }
	Uint16	getValueSize(void) const	{ return 1; }	// should never be used, since is meaningless ... difference between srcbytesinword and dstbytesinword is handled elsewhere

	bool	isOtherData(void) const	{ return true; }

	OtherUnspecifiedLargeAttributeBase *castToOtherData(void)	{ return this; }

	// these methods are not in base class ...

	virtual class SourceBase<Uint16> *get16BitSource()	{ return srcpixeldata; }

	virtual bool activateSource(void) = 0;

	virtual bool activateSourceWithoutUnpacking(void) = 0;

	void insertPixelPointTransform(class PointFilter<Uint16,Uint16> *f)
		{
			Assert(f);
			pixelDataPointTransformFilter=f;
		}
		
	Uint32 getDestinationBytesPerFrame(void);
};

class OtherUnspecifiedLargeAttribute : public OtherUnspecifiedLargeAttributeBase {
public:
	OtherUnspecifiedLargeAttribute(Tag t,
			SourceBase<Uint16> *src,
			Uint16 r,Uint16 c,Uint16 f,Uint16 sperp);

	OtherUnspecifiedLargeAttribute(Tag t,
			SourceBase<Uint16> *src,
			Uint16 r,Uint16 c,Uint16 f,Uint16 sperp,
			TransferSyntax *transfersyntax,
			Uint16 bytesinword,
			Uint16 bitsallocated,
			Uint16 bitsstored,
			Uint16 highbit,
			Uint32 length=0xffffffff);

	virtual ~OtherUnspecifiedLargeAttribute();

	bool activateSource(void);
	
	bool activateSourceWithoutUnpacking(void);
};

class OtherUnspecifiedLargeAttributeCopied
	: public OtherUnspecifiedLargeAttributeBase {
private:
	DicomInputStream *srcstream;
	OurStreamPos srcpos;

	class Source<unsigned char> *srcinput;
	class ConvertByteToUint16   *srcfrombyte;
	class UnpackUint16          *srcfrompack;
	class UnshiftUint16         *srcfromshift;

	Uint16 srcbytesinword;
	Uint32 srclength;
	Endian srcendian;
public:
	OtherUnspecifiedLargeAttributeCopied(Tag t,
		Uint16 r,Uint16 c,Uint16 f,Uint16 sperp);

	OtherUnspecifiedLargeAttributeCopied(Tag t,
			DicomInputStream &stream,OurStreamPos pos,
			Uint16 r,Uint16 c,Uint16 f,Uint16 sperp,
			Uint16 bytesinword,
			Uint16 bitsallocated,
			Uint16 bitsstored,
			Uint16 highbit,
			Uint32 length=0xffffffff,
			Endian forceEndian=NoEndian);

	virtual ~OtherUnspecifiedLargeAttributeCopied();

	bool activateSource(void);
	
	bool activateSourceWithoutUnpacking(void);
};

class OtherUnspecifiedLargeAttributeEncapsulated
	: public OtherUnspecifiedLargeAttributeBase {
private:
	DicomInputStream *srcstream;
	OurStreamPos srcpos;
public:
	OtherUnspecifiedLargeAttributeEncapsulated(Tag t,
			DicomInputStream &stream,OurStreamPos pos,
			Uint16 r,Uint16 c,Uint16 f,Uint16 sperp,
			Uint16 bitsallocated,
			Uint16 bitsstored,
			Uint16 highbit);

	virtual ~OtherUnspecifiedLargeAttributeEncapsulated();

	bool activateSource(void);
	
	bool activateSourceWithoutUnpacking(void);
};

class SupplySourceFromAttribute : public SupplySource {
private:
	OtherUnspecifiedLargeAttributeBase *otherdata;
	bool withoutUnpacking;
public:
	SupplySourceFromAttribute(Attribute *a)
		: SupplySource()
		{
			Assert(a);
			Assert(a->isOtherData());
			otherdata=a->castToOtherData();
			Assert(otherdata);
			withoutUnpacking=false;
		}

	SupplySourceFromAttribute(Attribute *a,bool w)
		: SupplySource()
		{
			Assert(a);
			Assert(a->isOtherData());
			otherdata=a->castToOtherData();
			Assert(otherdata);
			withoutUnpacking=w;
		}

	virtual ~SupplySourceFromAttribute(void) {}

	class SourceBase<Uint16> *getSource(void)
		{
			Assert(otherdata);
			bool success=false;
			if (withoutUnpacking) {
				success = otherdata->activateSourceWithoutUnpacking();
			}
			else {
				success = otherdata->activateSource();
			}
			if (!success) {
				Assert(0);
				return 0;
			}
			class SourceBase<Uint16> *source = otherdata->get16BitSource();
			Assert(source);
			return source;
		}
};

#endif /* __Header_attrothr__ */
