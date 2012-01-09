#include "attr.h"
#include "listval.h"
#include "attrtypt.h"

BinaryOutputStream&
TagAttribute::writeValues(BinaryOutputStream& stream)
{
	ValueListIterator<Tag> i(values);
	while (!i) {
		stream << i();
		 ++i;
	}
	return stream;
}

TextOutputStream&
TagAttribute::writeData(TextOutputStream& stream)
{
	ValueListIterator<Tag> i(values);
	while (!i) {
		stream << "(";
		writeZeroPaddedHexNumber(stream,i().getGroup(),4);
		stream << ",";
		writeZeroPaddedHexNumber(stream,i().getElement(),4);
		stream << ")";
		++i;
		if (!i) stream << ",";
	}
	return stream;
}

BinaryInputStream&
TagAttribute::read(BinaryInputStream& stream,Uint32 length)
{
	Assert(length%4 == 0);
	while (length && stream.good()) {
		Uint16 g;
		Uint16 e;
		stream >> g;
		stream >> e;
		addValue(Tag(g,e));
		length-=4;
	}
	return stream;
}

bool
TagAttribute::getValue(unsigned index,Uint32& vp) const
{
	Tag value;
	bool success=values.getValue(index,value);
	if (success) vp=(Uint32)
		   (((Uint32)value.getGroup()<<16)
		    |(Uint32)value.getElement());
	return success;
}

bool
TagAttribute::getValue(unsigned index,Int32& vp) const
{
	Tag value;
	bool success=values.getValue(index,value);
	if (success) vp=(Int32)
		   (((Uint32)value.getGroup()<<16)
		    |(Uint32)value.getElement());
	return success;
}

bool
TagAttribute::getValue(unsigned index,Tag& value) const
{
	return values.getValue(index,value);
}

void
TagAttribute::setValue(unsigned index,Uint32 value)
{
	Tag tag((Uint16)((value>>16)&0xffff),
		(Uint16)(value&0xffff));
	values.setValue(index,tag);
}

void
TagAttribute::setValue(unsigned index,Int32 value)
{
	Tag tag((Uint16)((value>>16)&0xffff),
		(Uint16)(value&0xffff));
	values.setValue(index,tag);
}

void
TagAttribute::setValue(unsigned index,Tag value)
{
	values.setValue(index,value);
}

void
TagAttribute::addValue(Uint32 value)
{
	Tag tag((Uint16)((value>>16)&0xffff),
		(Uint16)(value&0xffff));
	values.addValue(tag);
}

void
TagAttribute::addValue(Int32 value)
{
	Tag tag((Uint16)((value>>16)&0xffff),
		(Uint16)(value&0xffff));
	values.addValue(tag);
}

void
TagAttribute::addValue(Tag value)
{
	values.addValue(value);
}

