#include "attrlsln.h"
#include "attrseq.h"

Uint32
lengthOfFixedPartOfAttribute(
	const TransferSyntax *ts,const Attribute *a)
{
	// returned the length in bytes of the fixed part of an element
	// ie. that part other than that returned by a->getVL()
	// obviously this is dictated by the Value Representation which
	// in turn depends on the Transfer Syntax

	Assert(a);
	Assert(ts);

	Uint32 length=4;	// length of tag itself
	// work out length of "value length" field
	if (ts->isImplicitVR())
		length+=4;
	else if (ts->isExplicitVR()) {
		const char *vru=a->getVR();
		if (vru) {	// Explicit OB,OW,SQ,UN,UT
			if (vru[0]=='O' && (vru[1]=='B' || vru[1]=='W')
			 || vru[0]=='U' && (vru[1]=='N' || vru[1]=='T')
			 || vru[0]=='S' &&  vru[1]=='Q')
				length+=6;
			else
				length+=2;
		}
		else
			Assert(0);
		length+=2;	// add length of explicit VR itself
	}
	else
		Assert(0);
	return length;
}

Uint32
lengthOfFixedPartOfAttribute(
	const TransferSyntax *metats,const TransferSyntax *datats,const Attribute *a)
{
	return lengthOfFixedPartOfAttribute(a->getTag().isMetaheaderGroup() ? metats : datats,a);
}

Uint32
lengthOfEntireAttribute(const TransferSyntax *ts,const Attribute *a)
{
	// returned the length in bytes of the entire element
	// obviously this is dictated by the Value Representation which
	// in turn depends on the Transfer Syntax

        // returns 0xffffffff if undefined length

	Assert(a);
	Assert(ts);

	Uint32 length=lengthOfFixedPartOfAttribute(ts,a);
	if (strcmp(a->getVR(),"SQ") == 0) {
		SequenceAttribute *as=(SequenceAttribute *)a;
		AttributeList **alists;
		int count=as->getLists(&alists);
		while (count--) {
			Assert(*alists);
			length+=8;	// Item
			AttributeListIterator i(**alists);
			while (!i) {
				Attribute *a=i();
				Assert(a);
                                Uint32 add=lengthOfEntireAttribute(ts,a);
                                if (add == 0xffffffff) {
                                        return 0xffffffff;
                                }
                                length+=add;
				++i;
			}
			length+=8;	// Item Delimiter
			++alists;
		}
		length+=8;		// Sequence Delimiter
	}
        else {
                Uint32 add=a->getVL();
                if (add == 0xffffffff) {
                        length=0xffffffff;      // no support for undefined lengths other than sequences (e.g. encapsulated pixel data)
                }
	        else {
		        length+=add;
                }
        }
        return length;
}


Uint32
lengthOfEntireAttribute(
	const TransferSyntax *metats,const TransferSyntax *datats,const Attribute *a)
{
	return lengthOfEntireAttribute(a->getTag().isMetaheaderGroup() ? metats : datats,a);
}

