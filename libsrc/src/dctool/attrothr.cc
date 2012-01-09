#include "attr.h"
#include "attrothr.h"
#include "convtype.h"
#include "tobyte.h"
#include "frombyte.h"
#include "topack.h"
#include "frompack.h"
#include "fromshft.h"
#include "toshft.h"
#include "cnvbit.h"
#include "unencap.h"

void
OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules(
	Uint16 bitsallocated,
	TransferSyntax *transfersyntax,
	Uint16& bytesinword,
	Endian& encodedendian,
	Uint32& length,
	Endian forceEndian)
{
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start" << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start rows=" << dec << rows << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start columns=" << dec << columns << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start frames=" << dec << frames << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start samples=" << dec << samplesperpixel << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start transfersyntax ptr=" << hex << transfersyntax << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start bytesinword=" << dec << bytesinword << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start encodedendian=" << dec << encodedendian << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start length=" << hex << length << dec << endl;

	Assert(transfersyntax);
	Endian wordendian = forceEndian == NoEndian ? transfersyntax->getPixelEndian() : forceEndian;

//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules start wordendian=" << dec << wordendian << endl;

	Assert(bytesinword<=2);

	if (bytesinword == 0)
		bytesinword =
			(transfersyntax->isNotEncapsulated()
			&& (transfersyntax->isImplicitVR() || bitsallocated>8))
				? 2 : 1;

	Assert(bitsallocated <= bytesinword*8u);

	encodedendian = bytesinword == 2 ? wordendian : ByteEndian;

	if (length == 0xffffffff && transfersyntax->isNotEncapsulated()) {
		// the following is a problem on < 32 bit long machines, since need largest intermediate value may be bitsallocated * 2^32
		// but factoring out the ((bitsallocated-1u)/16u+1u)*2u is not really good enough since bitsallocated may not be a multiple of 8 or 16 :(
		if (bitsallocated % 8 == 0) {
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules Using bitsallocated is modulo 8 method" << endl;
			 Uint16 bytesPerBitsAllocated = (bitsallocated-1u)/8u+1u;
			 length=Uint32(rows)*columns*frames*samplesperpixel*bytesPerBitsAllocated;
			 if (length % 2 == 1) ++length;
		}
		else {
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules Using bitsallocated not modulo 8 method" << endl;
			Assert( ((unsigned long)(rows)*columns*frames*samplesperpixel) <= ((unsigned long)(rows)*columns*frames*samplesperpixel*bitsallocated) );
			length=(((unsigned long)(rows)*columns*frames*samplesperpixel*bitsallocated-1u)/16u+1u)*2u;	// note cast to ensure (hope) that result is not truncated - 32 bits are NOT enough
		}
	}
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules ************" << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules end bytesinword=" << dec << bytesinword << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules end encodedendian=" << dec << encodedendian << endl;
//cerr << "OtherUnspecifiedLargeAttributeBase::Proposal14EncodingRules end length=" << hex << length << dec << endl;
}

Uint32
OtherUnspecifiedLargeAttributeBase::getDestinationBytesPerFrame(void) {
	Assert(dsttransfersyntax->isNotEncapsulated());
	Assert( ((Uint64)(rows)*columns*frames*samplesperpixel) < ((Uint64)(rows)*columns*frames*samplesperpixel*dstbitsallocated) );
	return (Uint32)(((Uint64)(rows)*columns*samplesperpixel*dstbitsallocated-1u)/8u+1u);
}

DicomOutputStream&
OtherUnspecifiedLargeAttributeBase::writeData(DicomOutputStream& stream)
{
	Assert(dsttransfersyntax == 0
		|| (stream.getTransferSyntaxToWriteDataSet() != 0
		   && *(stream.getTransferSyntaxToWriteDataSet())
			== *dsttransfersyntax));

	writeData((BinaryOutputStream&)stream);

	return stream;
}

OtherUnspecifiedLargeAttributeBase::OtherUnspecifiedLargeAttributeBase(Tag t,Uint16 r,Uint16 c,Uint16 f,Uint16 sperp)
		: Attribute(t)
{
	srcpixeldata=0;

	rows=r;
	columns=c;
	frames=f;
	samplesperpixel=sperp;
	//Assert(rows);					// have encountered objects with zero length Pixel Data that are standard extended and not described by Rows, etc. :(
	//Assert(columns);
	//Assert(frames);
	//Assert(samplesperpixel);

	dsttransfersyntax=0;
	dstbytesinword=0;
	dstbitsallocated=0;
	dstbitsstored=0;
	dsthighbit=0;
	dstlength=0xffffffff;
	dstendian=NoEndian;

	srcbitsallocated=0;
	srcbitsstored=0;
	srchighbit=0;

	srcpixeldata=0;
	srcrawdata=0;

	pixelDataPointTransformFilter=0;
	
	suppressScalingOnBitDepthConversion = false;		// the default behavior when copying is to move the pixels up if ddstbitsstored > srcbitsstored, i.e., to scale, which may not always be desirable
}

OtherUnspecifiedLargeAttributeBase::~OtherUnspecifiedLargeAttributeBase()
{
	// NB. destruction of srcpixeldata is handled by derived classes prn.
}

BinaryOutputStream&
OtherUnspecifiedLargeAttributeBase::writeData(BinaryOutputStream& stream)
{
	Assert(dstbitsallocated);
	Assert(dstendian != NoEndian);

// Comment out this in case PixelEndian != stream.getEndian() ...
//	Assert(dstendian == stream.getEndian() || (dstendian == ByteEndian && dstbitsallocated == 8));

	if (!activateSource()) Assert(0);
	Assert(srcpixeldata);

	class SourceBase<Uint16> *srcpixeldatatouse;

	if (pixelDataPointTransformFilter) {
		if (pixelDataPointTransformFilter->isOffsetIndependent()) {
			srcpixeldatatouse=new ConvertSourceToSinkWithFilterIndependentOfOffset<Uint16,Uint16>(*srcpixeldata,
						(PointFilterIndependentOfOffset<Uint16,Uint16> *)pixelDataPointTransformFilter);
		}
		else {
			srcpixeldatatouse=new ConvertSourceToSinkWithFilterDependentOnOffset<Uint16,Uint16>(*srcpixeldata,
						(PointFilterDependentOnOffset<Uint16,Uint16> *)pixelDataPointTransformFilter);
		}
		Assert(srcpixeldatatouse);
	}
	else {
		srcpixeldatatouse=srcpixeldata;
	}

	ConvertBitUint16 *cnvbit;
	ShiftUint16 *toshift;
	if (!suppressScalingOnBitDepthConversion && srcbitsstored != dstbitsstored) {
//cerr << "OtherUnspecifiedLargeAttributeBase::writeData scaling on bit depth conversion from srcbitsstored=" << srcbitsstored << " to dstbitsstored=" << dstbitsstored << endl;
		cnvbit = new ConvertBitUint16(*srcpixeldatatouse,srcbitsstored,dstbitsstored);
		toshift = new ShiftUint16(*cnvbit,dstbitsallocated,dstbitsstored,dsthighbit);
	}
	else {
//cerr << "OtherUnspecifiedLargeAttributeBase::writeData NOT scaling on bit depth conversion - supressed or the same bit depth; from srcbitsstored=" << srcbitsstored << " to dstbitsstored=" << dstbitsstored << endl;
		cnvbit = NULL;
		toshift = new ShiftUint16(*srcpixeldatatouse,dstbitsallocated,dstbitsstored,dsthighbit);
	}
	if (dstbytesinword == 1) {
//cerr << "OtherUnspecifiedLargeAttributeBase::writeData dstbytesinword == 1" << endl;
		Assert(dstbitsallocated == 8);	// No PackByte() yet !
		ConvertUint16ToByte   tobyte   (*toshift,dstendian);
		Sink<unsigned char>   output   (stream,tobyte);
		output.write(dstlength);
	}
	else {
//cerr << "OtherUnspecifiedLargeAttributeBase::writeData dstbytesinword != 1" << endl;
		PackUint16            topack   (*toshift,dstbitsallocated);
		ConvertUint16ToByte   tobyte   (topack,dstendian);
		Sink<unsigned char>   output   (stream,tobyte);
		output.write(dstlength);
	}

	if (pixelDataPointTransformFilter) {
		Assert(srcpixeldatatouse);
		delete srcpixeldatatouse;
	}

	if (toshift) {
		delete toshift;
	}

	if (cnvbit) {
		delete cnvbit;
	}

	return stream;
}

ostream&
OtherUnspecifiedLargeAttributeBase::writeRaw(ostream& stream)
{
	if (!activateSource()) Assert(0);
	Assert(srcrawdata);

	Sink<unsigned char>output(stream,*srcrawdata);
	output.write(0xffffffff);

	return stream;
}

ostream&
OtherUnspecifiedLargeAttributeBase::writeRaw(ostream& stream,Uint32 byteOffset,Uint32 byteCount)
{
	if (!activateSource()) Assert(0);
	Assert(srcrawdata);

	Sink<unsigned char>output(stream,*srcrawdata);
	output.write(byteOffset,byteCount);

	return stream;
}

TextOutputStream&
OtherUnspecifiedLargeAttributeBase::write(TextOutputStream& stream,
			ElementDictionary *dict,bool verbose,bool showUsedAndIE)
{
	Attribute::writeBase(stream,dict,verbose,showUsedAndIE);
	return stream;
}

BinaryOutputStream&
OtherUnspecifiedLargeAttributeBase::write(BinaryOutputStream& stream)
{
	// not in use
	Assert(0);
	return stream;
}

DicomOutputStream&
OtherUnspecifiedLargeAttributeBase::write(DicomOutputStream& stream)
{
	Attribute::writeBase(stream);
	writeData(stream);
	return stream;
}

BinaryInputStream&
OtherUnspecifiedLargeAttributeBase::read(BinaryInputStream& stream,Uint32 length)
{
	// read is handled elsewhere ... see attrmxrd.h
	(void)length;
	Assert(0);
	return stream;
}

void
OtherUnspecifiedLargeAttributeBase::setOutputEncoding(
		TransferSyntax *transfersyntax,
		Uint16 bytesinword,
		Uint16 bitsallocated,
		Uint16 bitsstored,
		Uint16 highbit,
		Uint32 length)
{
	Assert(transfersyntax);

	dsttransfersyntax=transfersyntax;
	dstbytesinword=bytesinword;
	dstbitsallocated=bitsallocated;
	dstbitsstored=bitsstored;
	dsthighbit=highbit;
	dstlength=length;

	Assert(dstbitsstored <= dstbitsallocated);
	Assert(dsthighbit < dstbitsstored);

	Proposal14EncodingRules(
		dstbitsallocated,
		transfersyntax,
		dstbytesinword,
		dstendian,
		dstlength,
		NoEndian);
}

OtherUnspecifiedLargeAttribute::OtherUnspecifiedLargeAttribute(Tag t,
		SourceBase<Uint16> *src,
		Uint16 r,Uint16 c,Uint16 f,Uint16 sperp)
	: OtherUnspecifiedLargeAttributeBase(t,r,c,f,sperp)
{
	Assert(0);	// otherwise how to set srcbitsallocated, etc. ?
	(void)src;
	//Assert(src!=0);
	//srcpixeldata=src;
}

OtherUnspecifiedLargeAttribute::OtherUnspecifiedLargeAttribute(Tag t,
		SourceBase<Uint16> *src,
		Uint16 r,Uint16 c,Uint16 f,Uint16 sperp,
		TransferSyntax *transfersyntax,
		Uint16 bytesinword,
		Uint16 bitsallocated,
		Uint16 bitsstored,
		Uint16 highbit,
		Uint32 length)
	: OtherUnspecifiedLargeAttributeBase(t,r,c,f,sperp)
{
	Assert(src!=0);
	srcpixeldata=src;

	srcbitsallocated=bitsallocated;
	srcbitsstored=bitsstored;
	srchighbit=highbit;

	Assert(transfersyntax);
	setOutputEncoding(
		transfersyntax,
		bytesinword,bitsallocated,
		bitsstored,highbit,length);
}

OtherUnspecifiedLargeAttribute::~OtherUnspecifiedLargeAttribute()
{
	// NB. destruction does NOT delete srcpixeldata
}

bool
OtherUnspecifiedLargeAttribute::activateSource(void)
{
	return true;
}

bool
OtherUnspecifiedLargeAttribute::activateSourceWithoutUnpacking(void)
{
	Assert(0);
	return true;
}

OtherUnspecifiedLargeAttributeCopied::OtherUnspecifiedLargeAttributeCopied(Tag t,Uint16 r,Uint16 c,Uint16 f,Uint16 sperp)
	: OtherUnspecifiedLargeAttributeBase(t,r,c,f,sperp)
{
	srcstream=0;
	srcpos=OurStreamPos(-1);
	srclength=0;
	srcbytesinword=0;
	srcbitsallocated=0;
	srcbitsstored=0;
	srchighbit=0;
	srcendian=NoEndian;

	srcinput     = 0;
	srcfrombyte  = 0;
	srcfrompack  = 0;
	srcfromshift = 0;
}

OtherUnspecifiedLargeAttributeCopied::OtherUnspecifiedLargeAttributeCopied(Tag t,
		DicomInputStream &stream,OurStreamPos pos,
		Uint16 r,Uint16 c,Uint16 f,Uint16 sperp,
		Uint16 bytesinword,
		Uint16 bitsallocated,
		Uint16 bitsstored,
		Uint16 highbit,
		Uint32 length,
		Endian forceEndian)
	: OtherUnspecifiedLargeAttributeBase(t,r,c,f,sperp)
{
//cerr << "OtherUnspecifiedLargeAttributeCopied::OtherUnspecifiedLargeAttributeCopied length=" << hex << length << dec << endl;
	srcstream=&stream;
	srcpos=pos;
	srcbytesinword=bytesinword;
	srcbitsallocated=bitsallocated;
	srcbitsstored=bitsstored;
	srchighbit=highbit;
	srclength=length;

	srcinput    = 0;
	srcfrombyte = 0;
	srcfrompack = 0;
	srcfromshift = 0;

	Assert(srcstream);
	Assert(srcbitsallocated);

//cerr << "OtherUnspecifiedLargeAttributeCopied::OtherUnspecifiedLargeAttributeCopied srclength=" << hex << srclength << dec << endl;
	Proposal14EncodingRules(
		srcbitsallocated,
		srcstream->getTransferSyntaxToReadDataSet(),
		srcbytesinword,
		srcendian,
		srclength,
		forceEndian);

	// Default is to write same as read
	// until otherwise set by setOutputEncoding()

	dsttransfersyntax=
		srcstream->getTransferSyntaxToReadDataSet();
	dstendian=srcendian;
	dstbytesinword=srcbytesinword;
	dstbitsallocated=srcbitsallocated;
	dstbitsstored=srcbitsstored;
	dsthighbit=srchighbit;
	dstlength=srclength;
}

OtherUnspecifiedLargeAttributeCopied::~OtherUnspecifiedLargeAttributeCopied()
{
	if (srcinput) delete srcinput;
	if (srcfrombyte) delete srcfrombyte;
	if (srcfrompack) delete srcfrompack;
	if (srcfromshift) delete srcfromshift;
	// Not srcpixeldata because it was a copy of srcfrombyte/srcfrompack/srcfromshift
	// Not srcrawdata because it was a copy of srcinput
}

bool
OtherUnspecifiedLargeAttributeCopied::activateSource(void)
{
	srcstream->clear();
	srcstream->seekg(srcpos);
	if (srcstream->good()) {
		if (srcinput) delete srcinput;
		Assert(srclength != 0xffffffff);
		// want the buffer size a multiple of various things to
		// prevent them traversing buffer boundaries ...
		srcinput=new Source<unsigned char>(*srcstream,
			(size_t)columns*srcbitsallocated*samplesperpixel,srclength);
		Assert(srcinput);
		srcrawdata=srcinput;

		if (srcfrombyte) delete srcfrombyte;
		srcfrombyte = new ConvertByteToUint16(*srcinput,srcendian);
		Assert(srcfrombyte);

		if (srcbytesinword == 1) {
//cerr << "OtherUnspecifiedLargeAttributeCopied::activateSource srcbytesinword == 1" << endl;
			Assert(srcbitsallocated == 8);	// No UnpackByte() yet !
			Assert(!srcfrompack);
			srcpixeldata=srcfrombyte;
		}
		else {
//cerr << "OtherUnspecifiedLargeAttributeCopied::activateSource srcbytesinword != 1" << endl;
			if (srcfrompack) delete srcfrompack;
			srcfrompack = new UnpackUint16(*srcfrombyte,srcbitsallocated);
			Assert(srcfrompack);
			if (srcfromshift) delete srcfromshift;
			srcfromshift = new UnshiftUint16(*srcfrompack,srcbitsallocated,srcbitsstored,srchighbit);
			Assert(srcfromshift);
			srcpixeldata=srcfromshift;
		}

		return true;
	}
	else
		return false;
}

bool
OtherUnspecifiedLargeAttributeCopied::activateSourceWithoutUnpacking(void)
{
	srcstream->clear();
	srcstream->seekg(srcpos);
	if (srcstream->good()) {
		if (srcinput) delete srcinput;
		Assert(srclength != 0xffffffff);
		// want the buffer size a multiple of various things to
		// prevent them traversing buffer boundaries ...
		srcinput=new Source<unsigned char>(*srcstream,
			(size_t)columns*srcbitsallocated*samplesperpixel,srclength);
		Assert(srcinput);
		srcrawdata=srcinput;

		if (srcfrombyte) delete srcfrombyte;
		srcfrombyte = new ConvertByteToUint16(*srcinput,srcendian);
		Assert(srcfrombyte);

		if (srcbytesinword == 1) {
//cerr << "OtherUnspecifiedLargeAttributeCopied::activateSource srcbytesinword == 1" << endl;
			Assert(srcbitsallocated == 8);	// No UnpackByte() yet !
			Assert(!srcfrompack);
			srcpixeldata=srcfrombyte;
		}
		else {
//cerr << "OtherUnspecifiedLargeAttributeCopied::activateSource srcbytesinword != 1" << endl;
			if (srcfrompack) delete srcfrompack;
			srcfrompack = new UnpackUint16(*srcfrombyte,srcbitsallocated);
			Assert(srcfrompack);
			if (srcfromshift) delete srcfromshift;
			srcfromshift = NULL;
			srcpixeldata=srcfrompack;
		}

		return true;
	}
	else
		return false;
}

OtherUnspecifiedLargeAttributeEncapsulated::OtherUnspecifiedLargeAttributeEncapsulated(Tag t,
		DicomInputStream &stream,OurStreamPos pos,
		Uint16 r,Uint16 c,Uint16 f,Uint16 sperp,
		Uint16 bitsallocated,
		Uint16 bitsstored,
		Uint16 highbit)
	: OtherUnspecifiedLargeAttributeBase(t,r,c,f,sperp)
{
//cerr << "OtherUnspecifiedLargeAttributeEncapsulated::OtherUnspecifiedLargeAttributeEncapsulated" << endl;
	srcstream=&stream;
	srcpos=pos;
	srcbitsallocated=bitsallocated;
	srcbitsstored=bitsstored;
	srchighbit=highbit;

	Assert(srcstream);
	Assert(srcbitsallocated);

	// Proposal14EncodingRules don't apply to encapsulated data

	// Default is to write same as read
	// until otherwise set by setOutputEncoding()

	dsttransfersyntax=
		srcstream->getTransferSyntaxToReadDataSet();
	dstendian=ByteEndian;
	dstbytesinword=1;
	dstbitsallocated=srcbitsallocated;
	dstbitsstored=srcbitsstored;
	dsthighbit=srchighbit;
	dstlength=((long(r)*c*f*sperp*srcbitsallocated-1u)/16u+1u)*2u;	// note cast to ensure that result is not truncated - 32 bits are NOT enough
}

OtherUnspecifiedLargeAttributeEncapsulated::~OtherUnspecifiedLargeAttributeEncapsulated()
{
	if (srcrawdata) delete srcrawdata;
	// srcpixeldata is not yet in use
}

bool
OtherUnspecifiedLargeAttributeEncapsulated::activateSource(void)
{
	srcstream->clear();
	srcstream->seekg(srcpos);
	if (srcstream->good()) {
		if (srcrawdata) delete srcrawdata;
		srcrawdata = new UnencapsulatePixelData(*srcstream);
		Assert(srcrawdata);
		srcpixeldata=0;		// decompression NYI
		return true;
	}
	else
		return false;
}

bool
OtherUnspecifiedLargeAttributeEncapsulated::activateSourceWithoutUnpacking(void)
{
	Assert(0);
	return true;
}

