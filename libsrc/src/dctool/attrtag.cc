#include "attrtag.h"
#include "elmdict.h"

Tag
Tag::getRepeatingBase(void) const
{
	Uint16 g=group;
	Uint16 e=element;

	Uint16 gMASKEDff00=g&0xff00;
	Uint16 eMASKEDff00=e&0xff00;
	Uint16 eMASKEDff0f=e&0xff0f;

	// Transformations must match those in dictionary ... see elmdict.awk,elmdict.tpl,attrtag.cc

	// Note that some are from element 0xxx00 and others from 0xxx10

	if      (g == 0x0020) {
		if      (eMASKEDff00 == 0x3100) e=0x3100;			// 0x31xx - PS 300 - Source Image IDs
	}
	else if (g == 0x0028) {
		if      (eMASKEDff0f == 0x0400 && e != 0x0400) e=0x0410;	// 0x04x0 - PS 2 - RowsForNthOrderCoefficients
		else if (eMASKEDff0f == 0x0401 && e != 0x0401) e=0x0411;	// 0x04x1 - PS 2 - ColumnsForNthOrderCoefficients
		else if (eMASKEDff0f == 0x0402 && e != 0x0402) e=0x0412;	// 0x04x2 - PS 2 - CoefficientCoding
		else if (eMASKEDff0f == 0x0403 && e != 0x0403) e=0x0413;	// 0x04x3 - PS 2 - CoefficientCodingPointers

		else if (eMASKEDff0f == 0x0800) e=0x0800;			// 0x08x0 - PS 2 - CodeLabel
		else if (eMASKEDff0f == 0x0802) e=0x0802;			// 0x08x2 - PS 2 - NumberOfTables
		else if (eMASKEDff0f == 0x0803) e=0x0803;			// 0x08x3 - PS 2 - CodeTableLocation
		else if (eMASKEDff0f == 0x0804) e=0x0804;			// 0x08x4 - PS 2 - BitsForCodeWord
		else if (eMASKEDff0f == 0x0808) e=0x0808;			// 0x08x8 - PS 2 - ImageDataLocation
	}
	else if (g == 0x1000) {
		if      (eMASKEDff0f == 0x0000 && e != 0x0000) e=0x0010;	// 0x00x0 - PS 2 - Escape Triplet
		else if (eMASKEDff0f == 0x0001 && e != 0x0001) e=0x0011;	// 0x00x1 - PS 2 - Run Length Triplet
		else if (eMASKEDff0f == 0x0002 && e != 0x0002) e=0x0012;	// 0x00x2 - PS 2 - Huffman Table Size
		else if (eMASKEDff0f == 0x0003 && e != 0x0003) e=0x0013;	// 0x00x3 - PS 2 - Huffman Table Triplet
		else if (eMASKEDff0f == 0x0004 && e != 0x0004) e=0x0014;	// 0x00x4 - PS 2 - Shift Table Size
		else if (eMASKEDff0f == 0x0005 && e != 0x0005) e=0x0015;	// 0x00x5 - PS 2 - Shift Table Triplet
	}
	else if (g == 0x1010) {
		if (e >= 0x0004 && e <= 0xfffe) e=0x0004;			// PS 2 - Zonal Map
	}
	else if (gMASKEDff00 == 0x5000) g=(g%2) ? 0x5001: 0x5000;		// 0x50xx - PS 3 - Curve stuff
	else if (gMASKEDff00 == 0x6000) g=(g%2) ? 0x6001: 0x6000;		// 0x60xx - PS 3 and earlier - Overlay stuff
	else if (gMASKEDff00 == 0x7000) g=(g%2) ? 0x7001: 0x7000;		// 0x70xx - Private DLX TextAnnotation etc.
	else if (gMASKEDff00 == 0x7f00 && g != 0x7fe0) g=0x7f00;		// 0x7Fxx - PS 2 - VariablePixelData

	return Tag(g,e);
}


TextOutputStream&
Tag::write(TextOutputStream& stream,ElementDictionary *dict) const
{
	stream << "(";
	writeZeroPaddedHexNumber(stream,group,4);
	stream << ",";
	writeZeroPaddedHexNumber(stream,element,4);
	stream << ") " << flush;

	const char *vr=0;
	const char *desc=0;

	if (isPrivateGroup() && isPrivateOwner()) {
		vr="LO";		// PS3.5-7.8.1
		desc="PrivateCreator";
	}
	else if (dict) {
		vr=dict->getValueRepresentation(*this);
		desc=dict->getDescription(*this);
	}

	stream << (vr ? vr : "") << " ";
	stream << (desc ? desc : "?") << " " << flush;

	return stream;
}

BinaryOutputStream&
Tag::write(BinaryOutputStream& stream) const
{
	stream << group << element;
	return stream;
}

DicomOutputStream&
Tag::write(DicomOutputStream& stream) const
{
	// NB. Changing transfer syntaxes is caller's responsibility
	write((BinaryOutputStream&)stream);
	return stream;
}

BinaryInputStream&
Tag::read(BinaryInputStream& stream)
{
	stream >> group;
	stream >> element;
	return stream;
}

DicomInputStream&
Tag::read(DicomInputStream& stream)
{
	// NB. Changing transfer syntaxes is caller's responsibility
	read((BinaryInputStream&)stream);
	return stream;
}

BinaryInputStream&
BinaryInputStream::operator>>(Tag& rhs)
{
	rhs.read(*this);
	return *this;
}

DicomInputStream&
DicomInputStream::operator>>(Tag& rhs)
{
	rhs.read(*this);
	return *this;
}

BinaryOutputStream&
BinaryOutputStream::operator<<(Tag rhs)
{
	rhs.write(*this);
	return *this;
}

DicomOutputStream&
DicomOutputStream::operator<<(Tag rhs)
{
	rhs.write(*this);
	return *this;
}

