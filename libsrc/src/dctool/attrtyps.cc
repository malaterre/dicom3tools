#include <ctype.h>	// for isspace()

#include "attr.h"
#include "listval.h"
#include "attrtyps.h"

TextOutputStream&
StringAttribute::writePaddedValues(TextOutputStream& stream,char pad)
{
//cerr << "StringAttribute::writePaddedValues" << endl;
	Uint32 length = 0;
#ifndef CRAP
	ValueListIterator<char *> i(values);
	while (!i) {
		const char *string=i();
		Uint32 l = strlen(string);
		length+=l;
		if (l) stream << string;
		++i;
		if (!i) {
			stream << "\\";
			++length;
		}
	}
#else
	// use as test of getValue()
	if (getVM()) {
		int i=0;
		while (1) {
			char *string;
			//bool success=values.getValue(i,string);
			bool success=getValue(i,string);
			Assert(success);
			Assert(string);
			Uint32 l = strlen(string);
			length+=l;
			if (l) stream << string;
			if (string) delete[] string;
			++i;
			if (i<getVM()) {
				stream << "\\";
				++length;
			}
			else
				break;
		}
	}
#endif
	if (length%2 && pad) stream << pad;
	return stream;
}

BinaryOutputStream&
StringAttribute::writePaddedValues(BinaryOutputStream& stream,char pad)
{
	ValueListIterator<char *> i(values);
	Uint32 length = 0;
	while (!i) {
		const char *string=i();
		Uint32 l = strlen(string);
		length+=l;
		if (l) stream << string;
		++i;
		if (!i) {
			stream << "\\";
			++length;
		}
	}
	if (length%2) stream << pad;
	return stream;
}

// ********** these methods call addValue() or setValue() ************

BinaryInputStream&
StringAttribute::read(BinaryInputStream& stream,Uint32 length)
{
	Assert(length<0xffffffff);
	if (length) {
		char *buffer=new char[length+1];
		stream.read(buffer,(int)length);
		buffer[length]=0;
		bool foundNonNullByte = false;
		const char *p = buffer + length - 1;
		while (p >= buffer) {
			if (*p-- == 0) {
				if (foundNonNullByte) {
					embeddedNullByte = true;	// used in attrtypv.cc
				}
				else {
					trailingNullByte = true;	// used in attrtypv.cc
				}
			}
			else {
				foundNonNullByte = true;
			}
		}
		if (isspace(buffer[length-1])) {
			trailingSpace = true;
		}
		if (stream.good()) addValues(buffer);
		if (buffer) delete[] buffer;
	}
	return stream;
}

Uint32
StringAttribute::getVL(void) const
{
	ValueListIterator<char *> i(values);
	Uint32 length = 0;
	while (!i) {
		const char *string=i();
		length+=strlen(string);
		++i;
		if (!i) {
			++length;	// delimiters
		}
	}
	if (length%2) ++length;		// pad to even length
	return length;
}


bool
StringAttribute::isEmptyOrHasAnyEmptyValue(void) const
{
	if (getVM() == 0) {
		return true;
	}
	ValueListIterator<char *> i(values);
	while (!i) {
		const char *string=i();
		Uint32 thisValueLength=strlen(string);
		const char *ptr=string;
		while (thisValueLength > 0 && *ptr++ == ' ') --thisValueLength;
		if (thisValueLength == 0) {
			return true;
		}
		++i;
	}
	return false;
}

bool
StringAttribute::isEmptyOrHasAllEmptyValues(void) const
{
	if (getVM() == 0) {
		return true;
	}
	bool allEmpty = true;
	ValueListIterator<char *> i(values);
	while (!i) {
		const char *string=i();
		Uint32 thisValueLength=strlen(string);
		const char *ptr=string;
		while (thisValueLength > 0 && *ptr++ == ' ') --thisValueLength;
		if (thisValueLength > 0) {
			allEmpty = false;
			break;
		}
		++i;
	}
	return allEmpty;
}

bool
StringAttribute::getValue(unsigned index,Uint16& vp) const
{
	char * value;
	bool success=values.getValue(index,value);
	if (success) vp=(Uint16)atof(value);	// use atof not atol to handle exponential notation
	return success;
}

bool
StringAttribute::getValue(unsigned index,Uint32& vp) const
{
	char * value;
	bool success=values.getValue(index,value);
	if (success) vp=(Uint32)atof(value);	// use atof not atol to handle exponential notation
	return success;
}

bool
StringAttribute::getValue(unsigned index,Int16& vp) const
{
	char * value;
	bool success=values.getValue(index,value);
	if (success) vp=(Int16)atof(value);	// use atof not atol to handle exponential notation
	return success;
}

bool
StringAttribute::getValue(unsigned index,Int32& vp) const
{
	char * value;
	bool success=values.getValue(index,value);
	if (success) vp=(Int32)atof(value);	// use atof not atol to handle exponential notation
	return success;
}

bool
StringAttribute::getValue(unsigned index,Float32& vp) const
{
	char * value;
	bool success=values.getValue(index,value);
	if (success) vp=(Float32)atof(value);
	return success;
}

bool
StringAttribute::getValue(unsigned index,Float64& vp) const
{
	char * value;
	bool success=values.getValue(index,value);
	if (success) vp=(Float64)atof(value);
	return success;
}

bool
StringAttribute::getValue(unsigned index,char * & rvalue) const
{
	char *value;
	bool success=values.getValue(index,value);
	if (success) {
//cerr << "StringAttribute::getValue(): got value =<" << value << ">" << endl;
		rvalue=StrDup(value);
//cerr << "StringAttribute::getValue(): dup value =<" << rvalue << ">" << endl;
	}
	return success;
}

void
StringAttribute::setValue(unsigned index,Uint16 value)
{
	ostrstream ost;
	ost << dec << value << ends;
	char *str = ost.str();
	setValue(index,str);
}

void
StringAttribute::setValue(unsigned index,Uint32 value)
{
	ostrstream ost;
	ost << dec << value << ends;
	char *str = ost.str();
	setValue(index,str);
}

void
StringAttribute::setValue(unsigned index,Int16 value)
{
	ostrstream ost;
	ost << dec << value << ends;
	char *str = ost.str();
	setValue(index,str);
}

void
StringAttribute::setValue(unsigned index,Int32 value)
{
	ostrstream ost;
	ost << dec << value << ends;
	char *str = ost.str();
	setValue(index,str);
}

void
StringAttribute::setValue(unsigned index,Float32 value)
{
	ostrstream ost;
	ost	<< resetiosflags(ios::scientific|ios::fixed|ios::uppercase)
		<< setiosflags(ios::showpoint)
		<< setprecision(6)
		<< value << ends;
	char *str = ost.str();
	setValue(index,str);
}

void
StringAttribute::setValue(unsigned index,Float64 value)
{
	ostrstream ost;
	ost	<< resetiosflags(ios::scientific|ios::fixed|ios::uppercase)
		<< setiosflags(ios::showpoint)
		<< setprecision(6)
		<< setprecision(6)
		<< value << ends;
	char *str = ost.str();
	setValue(index,str);
}

void
StringAttribute::addValue(Uint16 value)
{
	ostrstream ost;
	ost << dec << value << ends;
	char *str = ost.str();
	addValue(str);
}

void
StringAttribute::addValue(Uint32 value)
{
	ostrstream ost;
	ost << dec << value << ends;
	char *str = ost.str();
	addValue(str);
}

void
StringAttribute::addValue(Int16 value)
{
	ostrstream ost;
	ost << dec << value << ends;
	char *str = ost.str();
	addValue(str);
}

void
StringAttribute::addValue(Int32 value)
{
	ostrstream ost;
	ost << dec << value << ends;
	char *str = ost.str();
	addValue(str);
}

void
StringAttribute::addValue(Float32 value)
{
	ostrstream ost;
	ost	<< resetiosflags(ios::scientific|ios::fixed|ios::uppercase)
		<< setiosflags(ios::showpoint)
		<< setprecision(6)
		<< setprecision(6)
		<< value << ends;
	char *str = ost.str();
	addValue(str);
}

void
StringAttribute::addValue(Float64 value)
{
	ostrstream ost;
	ost	<< resetiosflags(ios::scientific|ios::fixed|ios::uppercase)
		<< setiosflags(ios::showpoint)
		<< setprecision(6)
		<< setprecision(6)
		<< value << ends;
	char *str = ost.str();
	addValue(str);
}

void
StringAttribute::addValues(const char *vptr)
{
	Assert(vptr);
	char *str=StrDup(vptr);
	char *p=str;
	while (p) {
		char *delim=strchr(p,'\\');
		if (delim) *delim='\0';
		addValue(StrDup(p));
		p=delim ? delim+1 : 0;
	}
	if (str) delete[] str;
}

// ********** these methods actually do the insertion ************

// NB. pay attention to spaces in both addValue() and setValue()

// all other methods should use addValue() or setValue() and hence
// can forget about handling spaces

static const char*
removeLeadingSpaces(const char *from)
{
	if (from) while (*from && isspace(*from)) ++from;
	return from;
}

static const char*
removeTrailingSpaces(char *from)
{
	if (from) {
		unsigned length=strlen(from);
		const char *last=&from[length-1];
		while (length && isspace(*last--)) --length;
		from[length]=0;
	}
	return from;
}

void
StringAttribute::setValue(unsigned index,const char *value)
{
	char *str=StrDup(removeLeadingSpaces(value));
	removeTrailingSpaces(str);
	values.setValue(index,str);
}

void
StringAttribute::addValue(const char *value)
{
	Assert(value);
	char *str=StrDup(removeLeadingSpaces(value));
	removeTrailingSpaces(str);
	values.addValue(str);
}

// leading (but not trailing) spaces are significant in text

void
TextAttribute::setValue(unsigned index,const char *value)
{
	Assert(value);
	char *str=StrDup(value);
	removeTrailingSpaces(str);
	values.setValue(index,str);
}

void
TextAttribute::addValue(const char *value)
{
	Assert(value);
	char *str=StrDup(value);
	removeTrailingSpaces(str);
	values.addValue(str);
}

// Long Text doesn't use backslash delimiter which is a valid character
// and may never have a VM of more than 1

void
LongTextAttributeBase::setValue(unsigned index,const char *value)
{
	Assert(value);
	Assert(index == 0);
	TextAttribute::setValue(index,value);
}

void
LongTextAttributeBase::addValue(const char *value)
{
	Assert(value);
	Assert(getVM() == 0);
	TextAttribute::addValue(value);
}

void
LongTextAttributeBase::addValues(const char *vptr)
{
	Assert(vptr);
	addValue(vptr);
}

// CodeStringFileComponent needs to separate components of path
// into separate values

void
CodeStringFileComponentAttributeBase::addValues(const char *vptr)
{
	Assert(vptr);
	char *str=StrDup(vptr);
	char *p=str;
	while (p) {
		char *delim=strchr(p,'\\');		// DOS
		if (!delim) delim=strchr(p,'/');	// Unix
		if (!delim) delim=strchr(p,':');	// Mac
		if (delim) *delim='\0';
		addValue(StrDup(p));
		p=delim ? delim+1 : 0;
	}
	if (str) delete[] str;

}

