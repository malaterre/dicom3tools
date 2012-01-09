#include "attr.h"

Attribute::Attribute(Tag t)
{
	tag=t;
	byteoffset=0xffffffff;
	used=false;
	ie = UnknownIE;
}

Attribute::~Attribute()
{
#ifdef TRACE_DESTRUCTORS
cerr << "Attribute::~Attribute" << endl;
#endif
}

void
Attribute::setByteOffset(Uint32 offset)
{
//cerr << "Attribute::setByteOffset to " << hex << offset << dec << endl;
	byteoffset=offset;
}

Uint32
Attribute::getByteOffset(void) const
{
//cerr << "Attribute::getByteOffset is " << hex << byteoffset << dec << endl;
//	Assert(byteoffset != 0xffffffff);
	return byteoffset;
}

TextOutputStream&
Attribute::writeBase(TextOutputStream& stream,ElementDictionary *dict,bool verbose,bool showUsedAndIE)
{
	if (verbose) {
		stream << "@";
		writeZeroPaddedHexNumber(stream,byteoffset,8);
		stream << ": ";
	}
	tag.write(stream,dict);
	stream << "\t VR=<" << getVR() << ">  ";
	stream << " VL=<";
	writeZeroPaddedHexNumber(stream,getVL(),4);
	stream << ">  " << flush;
	if (showUsedAndIE) {
		stream << "Used=<" << (isUsed() ? "T" : "F") << "> ";
		stream << "IE=<" << describeInformationEntity(getInformationEntity()) << "> ";
	}
	return stream;
}

BinaryOutputStream&
Attribute::writeBase(BinaryOutputStream& stream)
{
	Assert(0);
	// this routine is probably never used, but just in case ...
	// assume default Implicit VR
	stream << tag << getVL();
	return stream;
}

DicomOutputStream&
Attribute::writeBase(DicomOutputStream& stream)
{
	// NB. Changing transfer syntaxes is caller's responsibility
	stream << tag;
	const char *vr = getVR();
	Assert(stream.getTransferSyntaxInUse());
	if (stream.getTransferSyntaxInUse()->isExplicitVR()) {
		Assert(vr);
		Assert(strlen(vr) == 2);
//cerr << "VR=" << vr << endl;
		if (vr[0]=='X') {			// we have a problem ... an internal unspecified pseudo-VR is present ... need to override it
			if (vr[1]=='S') {		// XS is choice between US or SS
cerr << "Overriding XS with US during write" << endl;
				vr="US";
			}
			else if (vr[1]=='O') {		// XO is choice between US or SS or OW
cerr << "Overriding XO with OW during write" << endl;
				vr="OW";
			}
		}
		else if (vr[0]=='O' && vr[1]=='X') {	// we have a problem ... an internal unspecified pseudo-VR is present ... need to override it
cerr << "Overriding OX with OW during write" << endl;
			vr="OW";
		}
		stream << vr;
		if (vr[0]=='O' && (vr[1]=='B' || vr[1]=='W' || vr[1]=='X' || vr[1]=='F')
	 	 || vr[0]=='S' &&  vr[1]=='Q'
	 	 || vr[0]=='U' && (vr[1]=='N' || vr[1]=='T'))
		{
			// Explicit OB,OW,OX,OF,SQ,UN,UT
			stream << Uint16(0) << getVL();
		}
		else {
			stream << (Uint16)getVL();
		}
	}
	else {	// Implicit VR
		stream << getVL();
	}
	return stream;
}

BinaryOutputStream&
Attribute::writeData(BinaryOutputStream& stream)
{
	Assert(0);
	return stream;
}

TextOutputStream&
Attribute::writeData(TextOutputStream& stream)
{
	Assert(0);
	return stream;
}

OtherUnspecifiedLargeAttributeBase *
Attribute::castToOtherData(void)
{
	Assert(0);
	return 0;
}

void
Attribute::setOutputEncoding(
		TransferSyntax *,
		unsigned short,
		unsigned short,
		unsigned short,
		unsigned short,
		Uint32)
{
	Assert(0);
}

bool
Attribute::getValue(unsigned,Uint16&) const
{
	return false;
}

bool
Attribute::getValue(unsigned,Uint32&) const
{
	return false;
}

bool
Attribute::getValue(unsigned,Int16&) const
{
	return false;
}

bool
Attribute::getValue(unsigned,Int32&) const
{
	return false;
}

bool
Attribute::getValue(unsigned,Float32&) const
{
	return false;
}

bool
Attribute::getValue(unsigned,Float64&) const
{
	return false;
}

bool
Attribute::getValue(unsigned,Tag&) const
{
	return false;
}

bool
Attribute::getValue(unsigned,char * &) const
{
	return false;
}

bool
Attribute::getValue(const unsigned char * &,Uint32 &) const
{
	return false;
}

bool
Attribute::getValue(const Uint16 * &,Uint32 &) const
{
	return false;
}

void
Attribute::setValue(unsigned,Uint16)
{
	Assert(0);
}

void
Attribute::setValue(unsigned,Uint32)
{
	Assert(0);
}

void
Attribute::setValue(unsigned,Int16)
{
	Assert(0);
}

void
Attribute::setValue(unsigned,Int32)
{
	Assert(0);
}

void
Attribute::setValue(unsigned,Float32)
{
	Assert(0);
}

void
Attribute::setValue(unsigned,Float64)
{
	Assert(0);
}

void
Attribute::setValue(unsigned,Tag)
{
	Assert(0);
}

void
Attribute::setValue(unsigned,const char *)
{
	Assert(0);
}

void
Attribute::setValue(const unsigned char *,Uint32)
{
	Assert(0);
}

void
Attribute::setValue(const Uint16 *,Uint32)
{
	Assert(0);
}

void
Attribute::addValue(Uint16)
{
	Assert(0);
}

void
Attribute::addValue(Uint32)
{
	Assert(0);
}

void
Attribute::addValue(Int16)	
{
	Assert(0);
}

void
Attribute::addValue(Int32)	
{
	Assert(0);
}

void
Attribute::addValue(Float32)
{
	Assert(0);
}

void
Attribute::addValue(Float64)
{
	Assert(0);
}

void
Attribute::addValue(Tag)
{
	Assert(0);
}

void
Attribute::addValue(const char *)
{
	Assert(0);
}


void
Attribute::addValues(unsigned number,Uint16 *vptr)
{
	while (number--) addValue(*vptr++);
}

void
Attribute::addValues(unsigned number,Uint32 *vptr)
{
	while (number--) addValue(*vptr++);
}

void
Attribute::addValues(unsigned number,Int16 *vptr)
{
	while (number--) addValue(*vptr++);
}

void
Attribute::addValues(unsigned number,Int32 *vptr)
{
	while (number--) addValue(*vptr++);
}

void
Attribute::addValues(unsigned number,Float32 *vptr)
{
	while (number--) addValue(*vptr++);
}

void
Attribute::addValues(unsigned number,Float64 *vptr)
{
	while (number--) addValue(*vptr++);
}

void
Attribute::addValues(unsigned number,Tag *vptr)
{
	while (number--) addValue(*vptr++);
}

void
Attribute::addValues(unsigned number,const char **vptr)
{
	while (number--) addValue(*vptr++);
}

void
Attribute::addValues(const char *)
{
	Assert(0);
}

int
Attribute::getLists(AttributeList ***a)
{
	Assert(0);
	*a=0;
	return 0;
}

DicomOutputStream&
DicomOutputStream::operator<<(Attribute& rhs)
{
	rhs.write(*this);
	return *this;
}

