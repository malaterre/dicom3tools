#include "elmtype.h"
#include "elmdict.h"
#include "elmentry.h"
#include "elmtable.h"
#include "elmpriv.h"	// declare before elmhash.h to avoid old SunPro C++ bug
#include "elmhash.h"

static int ElementDictionaryReferenceCount;	// will be zero at start

static ElementDictionaryTableEntry 	*Table;
static IndexByString 			*StringIndex;
static IndexByTag   			*TagIndex;
static IndexByTagAndOwnerAndBlock   	*TagAndOwnerAndBlockIndex;

static class ElementDictionaryTableEntry GenericGroupLengthElementDictionaryTableEntry = { 0x0000,0x0000,0x0000,"UL",1,1,"","GroupLength","Group Length",true };

ElementDictionary::ElementDictionary(void)
{
	if (ElementDictionaryReferenceCount++ == 0) {
		Table=ElementDictionaryTable;
		StringIndex = new
			IndexByString(DATADICTIONARYINDEXSIZE);
		TagIndex = new
			IndexByTag(DATADICTIONARYINDEXSIZE);
		TagAndOwnerAndBlockIndex = new
			IndexByTagAndOwnerAndBlock(DATADICTIONARYINDEXSIZE);
		ElementDictionaryTableEntry *ptr=Table;
		unsigned long index=0;
		while (ptr->Keyword) {
			Tag tag(ptr->Group,ptr->Element);
			if (tag.isPrivateGroup()) {
			// if (ptr->Owner == 0) {	// same same
				TagAndOwnerAndBlock tob(tag,ptr->Owner,ptr->PrivateBlock);
				IndexEntryTagAndOwnerAndBlock ietob(tob,index);
				(*TagAndOwnerAndBlockIndex)+=ietob;
				// PrivateBlock will be zero (wildcard) except when
				// non-standard owner disambiguation needed
			}
			else {
				(*StringIndex)+=IndexEntryString(
					ptr->Keyword,index);
				(*TagIndex)+=IndexEntryTag(tag,index);
			}
			++ptr;
			++index;
		}
	}
	CurrentOwners=new PrivateOwners;
}

ElementDictionary::~ElementDictionary()
{
	Assert(ElementDictionaryReferenceCount>0);
	Assert(StringIndex);
	Assert(TagIndex);
	Assert(TagAndOwnerAndBlockIndex);
	Assert(CurrentOwners);

	if (--ElementDictionaryReferenceCount == 0) {
		if (StringIndex) delete StringIndex;
		if (TagIndex) delete TagIndex;
		if (TagAndOwnerAndBlockIndex) delete TagAndOwnerAndBlockIndex;
	}
	if (CurrentOwners) delete CurrentOwners;
}

const ElementDictionaryTableEntry *
ElementDictionary::operator[] (Tag tag) const
{
	ElementDictionaryTableEntry *e = 0;

	// Repeating group/element mechanism is NOT applied to private groups
	// except Papyrus 3.0 60xx annotations

	// Use isPrivateGroup() from attrtag.h to find
	// exclusions to odd group rule such as Variable Pixel Data

	if (tag.isPrivateGroup()) {
		if (CurrentOwners) {
			const char *owner=(*CurrentOwners)[tag];
			if (owner) {
				// Try first with block in case owner is ambiguous
				// (this is counter to the standard but happens)
				IndexEntryTagAndOwnerAndBlock *ei=
				    (*TagAndOwnerAndBlockIndex)[TagAndOwnerAndBlock(
					Tag(tag.getGroup(),tag.getElement()&0xff),
					owner,
					(tag.getElement()&0xff00)>>8u)];
				// If failed then use block of 0x0000 which is not a valid
				// block and hence is used as a "wildcard" ... this is
				// the "standard" way to do it and is the most common path 
				if (!ei) {
//cerr << "Tag group=" << hex << tag.getGroup() << dec << endl;
//cerr << "Tag element=" << hex << tag.getElement() << dec << endl;
//cerr << "Owner=<" << owner << ">" << endl;
					Tag testtag=tag.getRepeatingBase();
//cerr << "Test tag group=" << hex << testtag.getGroup() << dec << endl;
//cerr << "Test tag element=" << hex << testtag.getElement() << dec << endl;
					if (testtag.getGroup() == 0x6001 && strcmp(owner,"PAPYRUS 3.0") == 0) {
//cerr << "matched criteria" << endl;
//cerr << "Index tag group=" << hex << Tag(testtag.getGroup(),testtag.getElement()&0xff).getGroup() << dec << endl;
//cerr << "Index tag element=" << hex << Tag(testtag.getGroup(),testtag.getElement()&0xff).getElement() << dec << endl;
						ei=(*TagAndOwnerAndBlockIndex)[TagAndOwnerAndBlock(
							Tag(testtag.getGroup(),testtag.getElement()&0xff),
							owner,
							0)];
					}
					else {
						ei=(*TagAndOwnerAndBlockIndex)[TagAndOwnerAndBlock(
							Tag(tag.getGroup(),tag.getElement()&0xff),
							owner,
							0)];
					}
				}
				if (ei) e=Table+ei->getIndex();
			}
		}
	}
	else {
		Tag testtag=tag.getRepeatingBase();
		IndexEntryTag *ei = (*TagIndex)[testtag];
		if(ei) e=Table+ei->getIndex();
	}

	// Don't assume all elements == 0 are GroupLength
	// because in some private dictionaries they are not (!)
	// but we would have found them by now, so this is ok...

	if (!e && tag.getElement() == 0) {
		e = &GenericGroupLengthElementDictionaryTableEntry;
	}

	return e;
}

const ElementDictionaryTableEntry *
ElementDictionary::operator[] (const char *keyword) const
{
	IndexEntryString *e=(*StringIndex)[keyword];
	return e ? Table+e->getIndex() : 0;
}

const char *
ElementDictionary::getValueRepresentation(Tag t) const
{
	const ElementDictionaryTableEntry *e = (*this)[t];
	return e ? e->ValueRepresentation : 0;
}

const Uint16
ElementDictionary::getValueMultiplicityMinimum(Tag t) const
{
	const ElementDictionaryTableEntry *e = (*this)[t];
	return e ? e->ValueMultiplicityMinimum : VMNONE;
}

const Uint16
ElementDictionary::getValueMultiplicityMaximum(Tag t) const
{
	const ElementDictionaryTableEntry *e = (*this)[t];
	return e ? e->ValueMultiplicityMaximum : VMNONE;
}

const char *
ElementDictionary::getDescription(Tag t) const
{
	const ElementDictionaryTableEntry *e = (*this)[t];
	return e ? e->Description : 0;
//	return e ? e->Keyword : 0;
}

const char *
ElementDictionary::getKeyword(Tag t) const
{
	const ElementDictionaryTableEntry *e = (*this)[t];
	return e ? e->Keyword : 0;
}

bool
ElementDictionary::getTag(const char *keyword,Tag& tr) const
{
	const ElementDictionaryTableEntry *e = (*this)[keyword];
	if (e) {
		tr=Tag(e->Group,e->Element);
		return true;
	}
	else
		return false;
}

bool
ElementDictionary::isRetired(Tag& t) const
{
	const ElementDictionaryTableEntry *e = (*this)[t];
	return e && e->Retired;
}

void
ElementDictionary::addOwner(Tag t,const char *owner)
{
	Assert(CurrentOwners);
	Assert(owner);
	Assert (*owner); 
	// NB. we don't use the block in the owner list so use wildcard of 0
	(*CurrentOwners)+=TagAndOwnerAndBlock(t,owner,0);
}

bool
ElementDictionary::hasOwner(Tag& t) const
{
	return t.isPrivateGroup() && CurrentOwners && (*CurrentOwners)[t];
}

