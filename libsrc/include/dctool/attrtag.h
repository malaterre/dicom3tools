#ifndef __Header_attrtag__
#define __Header_attrtag__

#include "basetype.h"
#include "txstream.h"
#include "dcstream.h"

class ElementDictionary;

class Tag {
private:
	Uint16 group;
	Uint16 element;
protected:
public:
	Tag(void)			{ group=0; element=0; }
	Tag(Uint16 g,Uint16 e)		{ group=g; element=e; }

	// allows initialization from 0 ...
	Tag(Uint32 u)	{ group=(Uint16)(u>>16); element=(Uint16)(u&0xffff); }

	Tag(const Tag& t)		{ group=t.group; element=t.element; }
	void operator=(const Tag& t)	{ group=t.group; element=t.element; }

	bool operator==(const Tag& t2) const
		{ return group == t2.group && element == t2.element; }
	bool operator<(const Tag& t2) const
		{ return group < t2.group
		      || (group == t2.group && element < t2.element); }
	bool operator<=(const Tag& t2) const
		{ return group < t2.group
		      || (group == t2.group && element <= t2.element); }
	bool operator>(const Tag& t2) const
		{ return group > t2.group
		      || (group == t2.group && element > t2.element); }
	bool operator>=(const Tag& t2) const
		{ return group > t2.group
		      || (group == t2.group && element >= t2.element); }

	operator const Uint32(void) const
		{ return (Uint32)group<<16|element; }

	Uint16	getElement(void) const	{ return element; }
	Uint16	getGroup(void) const	{ return group; }

	// Repeating groups and elements

	Tag	getRepeatingBase(void) const;

	// Private groups ... see PS3.5-1993-7.1

	bool	isPrivateGroup(void) const
		{
			return (group % 2) == 1
		     	     //&& group > 0x0007
		     	     && ((group&0xff00) != 0x7f00 || group>= 0x7fd1/*0x7fe0*/)
					// Siemens uses odd groups for variable data in PS2 compression, so not private
					// But Philips PMS uses 0x7fe1, which is private :(
					// and Elscint uses 0x7fdf, which is private :(
					// and GE uses 0x7fd1, which is private :(
					// so assume that everything above 0x7fd1 is private ... this may fail
					// if we encounter too large a number of variable data groups
					// (and PS 3.2 6.0 specifically allows 0x7f00 thru 0x7fff8 :( )
					// can't see a way around this without looking at other elements :(
			     && group != 0xffff;
		}

	bool	isPrivateOwner(void) const
		{
			//return element >= 0x0010 && element <= 0x00ff;		// what the standard says in PS 3.5 7.8.1
			return element >= 0x0001 && element <= 0x00ff;			// Sectra in mammo images illegally uses 0x0001
		}

	bool	isPrivateTag(void) const
		{
			return element >= 0x1000
			    /*&& element <= 0xffff*/;
		}

	bool	isRepeatingGroup(void) const
		{
			return (group >= 0x5000 
			     && group <= 0x501e)
			    || (group >= 0x6000 
			     && group <= 0x601e);
		}

	bool	isMetaheaderGroup(void)	const
		{
			return group == 0x0002;
		}

	bool	isLengthElement(void)	const
		{
			return element == 0x0000;
		}

	bool	isLengthElementOrLengthToEnd(void)	const
		{
			return element == 0x0000
			    || (group == 0x0008 && element == 0x0001);
		}

	bool	isPixelDataElement(void)	const
		{
			return (group == 0x7fe0 && element == 0x0010)
			    || (group == 0x7fe1 && element > 0x00ff && (element&0x00ff) == 0x0010);
				// includes the usual (0x7fe0,0x0010)
				// includes private pixel data (0x7fe1,0x1010) (but not owner) as used by Philips PMS
				// doesn't includes variable pixel data (0x7fxx,0x0010) ? if this is good
		}

	TextOutputStream&	write(TextOutputStream& stream,
					ElementDictionary *dict=0) const;
	BinaryOutputStream&	write(BinaryOutputStream& stream) const;
	DicomOutputStream&	write(DicomOutputStream& stream) const;
	BinaryInputStream&	read(BinaryInputStream& stream);
	DicomInputStream&	read(DicomInputStream& stream);
};

#define TagFromName(name)	\
		Tag(name##_GROUP,name##_ELEMENT)

#endif /* __Header_attrtag__ */


