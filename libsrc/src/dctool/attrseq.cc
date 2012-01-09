#include "attr.h"
#include "attrlist.h"
#include "attrmxls.h"
#include "attrseq.h"
#include "elmconst.h"
#include "elmtype.h"
#include "mesgtext.h"

// The various items and delimiters have no VR and always have
// implicit VR style value length fields

SequenceAttribute::~SequenceAttribute()
{
	FifoListIterator<AttributeList *> itemsi(listoflists);
	while (!itemsi) {
		AttributeListIterator listi(*itemsi());
		while (!listi) {
			Attribute *a=listi();
			Assert(a);
			delete a;
			++listi;
		}
		++itemsi;
	}
}

DicomOutputStream&
SequenceAttribute::write(DicomOutputStream& stream)
{
	// The various items and delimiters have no VR and always have
	// implicit VR style value length fields

	Attribute::writeBase(stream);
	FifoListIterator<AttributeList *> itemsi(listoflists);
	while (!itemsi) {
		stream << TagFromName(Item)
		       << Uint32(0xffffffff);	// undefined VL
		AttributeListIterator listi(*itemsi());
		while (!listi) {
			stream << *listi();
			++listi;
		}
		stream << TagFromName(ItemDelimitationItem) << Uint32(0);
		++itemsi;
	}
	stream << TagFromName(SequenceDelimitationItem) << Uint32(0);
	return stream;
}

TextOutputStream&
SequenceAttribute::write(TextOutputStream& stream,ElementDictionary *dict,bool verbose,bool showUsedAndIE)
{
	Attribute::writeBase(stream,dict,verbose,showUsedAndIE);
	stream << "\n";
	FifoListIterator<AttributeList *> itemsi(listoflists);
	while (!itemsi) {
		ElementDictionary *itemdict=((ReadableAttributeList *)itemsi())->getDictionary();
		AttributeListIterator listi(*itemsi());
		stream << "  ----:\n";
		while (!listi) {
			stream << "    > ";
			//Assert(itemdict);
			listi()->write(stream,itemdict ? itemdict : dict,verbose);
			stream << "\n";
			++listi;
		}
		++itemsi;
	}
	return stream;
}

int
SequenceAttribute::getLists(AttributeList ***a)
{
	// returns number of AttributeList pointers
	// creates an array of these pointers in a
	// it is the callers responsibility to destroy
	// the array (but NOT the objects pointed to) later

	FifoListIterator<AttributeList *> itemsi(listoflists);

	int n=0;
	while (!itemsi) {
		++n;
		++itemsi;
	}
	if (n) {
		*a=new AttributeList * [n];
		int i;
		for (i=0,itemsi.first(); i<n && !itemsi; ++i,++itemsi) {
			(*a)[i]=itemsi();
		}
	}
	else
		*a=0;

	return n;
}

Uint32
SequenceAttribute::getNumberOfItems (void) const
{
	Uint32 nItems=0;
	SimpleListEntry<AttributeList *> *ptr = listoflists.head;
	while (ptr) {
		++nItems;
		ptr=ptr->next;
	}
	return nItems;
}


bool
SequenceAttribute::verifyVM(const char *module,const char *element,
	TextOutputStream& log,
	ElementDictionary *dict,
	Uint16 multiplicityMin,Uint16 multiplicityMax,const char *specifiedSource) const
{
	// Sequences always have a VM of 1, by definition
	// Here we are checking the number of items, not the VM per se.
	Assert(dict);
	Tag tag=getTag();
	Uint32 nItems = getNumberOfItems();
//log << "SequenceAttribute::verifyVM(): nItems = " << nItems << endl;
	Uint16 errmin,errmax;
	int err=0;
	const char *source;
	if (multiplicityMin != 0 && multiplicityMax != 0) {	// else don't check
		if (nItems < multiplicityMin || nItems >multiplicityMax) {
			err=1;
			errmin=multiplicityMin;
			errmax=multiplicityMax;
			source= specifiedSource ? specifiedSource : MMsgDC(ModuleDefinition);
		}
	}
	if (err) {
		log << EMsgDC(BadSequenceNumberOfItems)
		    <<  " " << dec << nItems << " (" << errmin;
		if (errmin != errmax)
			if (errmax == VMUNLIMITED)
				log << "-n";
			else
				log << "-" << errmax;
		log << " " << MMsgDC(RequiredBy) << " " << source << ")";
		if (element) log << " " << MMsgDC(Element) << "=<" << element << ">";
		if (module)  log << " " << MMsgDC(Module)  << "=<" << module  << ">";
		log << endl;
		return false;
	}
	else {
		return true;
	}
}

