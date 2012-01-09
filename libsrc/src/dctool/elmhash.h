#ifndef __Header_ElmHash__
#define __Header_ElmHash__

#include "hash.h"
#include "elmtype.h"

// ********************* stuff for IndexByString *********************

class IndexEntryString {
	HashKeyString key;
	unsigned long index;
public:
	IndexEntryString(void)		{}
	IndexEntryString(const char *s,unsigned long i)
					{ key=HashKeyString(s); index=i; }
	IndexEntryString(IndexEntryString *e)
					{ key=e->key; index=e->index; }

	HashKeyString	getKey(void) const	{ return key; }
	unsigned long	getIndex(void) const	{ return index; }

	bool operator==(IndexEntryString e)
					{ return key == e.getKey(); }
};

class IndexEntryStringList : public SimpleList<IndexEntryString>
{
public:
	~IndexEntryStringList() {}	// only because buggy g++ 2.7.0 freaks
};

class IndexEntryStringListIterator : public SimpleListIterator<IndexEntryString>
{
public:
	IndexEntryStringListIterator(void)
		: SimpleListIterator<IndexEntryString>() {}
	IndexEntryStringListIterator(IndexEntryStringList& list)
		: SimpleListIterator<IndexEntryString>(list) {}
};

class IndexByString : public OpenHashTable <IndexEntryString,
					HashKeyString,
					IndexEntryStringList,
					IndexEntryStringListIterator>
{
public:
	IndexByString(unsigned long size)
		: OpenHashTable<IndexEntryString,
				HashKeyString,
				IndexEntryStringList,
				IndexEntryStringListIterator>(size)
		{}

	// supply virtual functions for OpenHashTable ...

	unsigned long	hash(const HashKeyString &key,unsigned long size)
		{
			const unsigned nchars = 10;
			const unsigned shift  = 4;

			unsigned n=nchars;
			unsigned long value=0;
			const char *s=key.getString();
			while (n-- && *s) {
				value=(value<<shift)|*s++;
			}
			return value%size; 
		}

	HashKeyString key(const IndexEntryString &e)	{ return e.getKey(); }
};

// ********************* stuff for IndexByTag *********************

class HashKeyTag {			// Use as class K
	Tag tag;
public:
	HashKeyTag(void)		{ }
	HashKeyTag(Tag t)		{ tag=t; }

	Tag getTag(void) const		{ return tag; }

	bool operator==(HashKeyTag k)	{ return tag == k.getTag(); }
};

class IndexEntryTag {
	HashKeyTag key;
	unsigned long index;
public:
	IndexEntryTag(void)		{}
	IndexEntryTag(Tag t,unsigned long i)
					{ key=HashKeyTag(t); index=i; }
	IndexEntryTag(IndexEntryTag *e)	{ key=e->key; index=e->index; }

	HashKeyTag	getKey(void) const	{ return key; }
	unsigned long	getIndex(void) const	{ return index; }

	bool operator==(IndexEntryTag e)
					{ return key == e.getKey(); }
};

class IndexEntryTagList : public SimpleList<IndexEntryTag>
{
public:
	~IndexEntryTagList() {}	// only because buggy g++ 2.7.0 freaks
};

class IndexEntryTagListIterator : public SimpleListIterator<IndexEntryTag>
{
public:
	IndexEntryTagListIterator(void)
		: SimpleListIterator<IndexEntryTag>() {}
	IndexEntryTagListIterator(IndexEntryTagList& list)
		: SimpleListIterator<IndexEntryTag>(list) {}
};

class IndexByTag : public OpenHashTable <IndexEntryTag,
					HashKeyTag,
					IndexEntryTagList,
					IndexEntryTagListIterator>
{
public:
	IndexByTag(unsigned long size)
		: OpenHashTable<IndexEntryTag,
				HashKeyTag,
				IndexEntryTagList,
				IndexEntryTagListIterator>(size)
		{}

	// supply virtual functions for OpenHashTable ...

	unsigned long	hash(const HashKeyTag &key,unsigned long size)
		{
			const unsigned shift  = 4;

			unsigned short group   = key.getTag().getGroup();
			unsigned short element = key.getTag().getElement();

			unsigned long value = (group<<shift)|element;
			return value%size; 
		}

	HashKeyTag key(const IndexEntryTag &e)	{ return e.getKey(); }
};

// ********************* stuff for IndexByTagAndOwnerAndBlock *********************

class IndexEntryTagAndOwnerAndBlock {
	TagAndOwnerAndBlock key;
	unsigned long index;
public:
	IndexEntryTagAndOwnerAndBlock(void)		{}
	IndexEntryTagAndOwnerAndBlock(TagAndOwnerAndBlock to,unsigned long i)
			{ key=to; index=i; }
	IndexEntryTagAndOwnerAndBlock(IndexEntryTagAndOwnerAndBlock *e)
			{ key=e->getKey(); index=e->getIndex(); }

	TagAndOwnerAndBlock	getKey(void) const	{ return key; }
	unsigned long		getIndex(void) const	{ return index; }

	bool operator==(IndexEntryTagAndOwnerAndBlock& e) const
					{ return key == e.getKey(); }
};

class IndexEntryTagAndOwnerAndBlockList : public SimpleList<IndexEntryTagAndOwnerAndBlock>
{
public:
	~IndexEntryTagAndOwnerAndBlockList() {}	// only because buggy g++ 2.7.0 freaks
};

class IndexEntryTagAndOwnerAndBlockListIterator : public SimpleListIterator<IndexEntryTagAndOwnerAndBlock>
{
public:
	IndexEntryTagAndOwnerAndBlockListIterator(void)
		: SimpleListIterator<IndexEntryTagAndOwnerAndBlock>() {}
	IndexEntryTagAndOwnerAndBlockListIterator(IndexEntryTagAndOwnerAndBlockList& list)
		: SimpleListIterator<IndexEntryTagAndOwnerAndBlock>(list) {}
};

class IndexByTagAndOwnerAndBlock : public OpenHashTable <IndexEntryTagAndOwnerAndBlock,
					TagAndOwnerAndBlock,
					IndexEntryTagAndOwnerAndBlockList,
					IndexEntryTagAndOwnerAndBlockListIterator>
{
public:
	IndexByTagAndOwnerAndBlock(unsigned long size)
		: OpenHashTable<IndexEntryTagAndOwnerAndBlock,
				TagAndOwnerAndBlock,
				IndexEntryTagAndOwnerAndBlockList,
				IndexEntryTagAndOwnerAndBlockListIterator>(size)
		{}

	// supply virtual functions for OpenHashTable ...

	unsigned long	hash(const TagAndOwnerAndBlock &key,unsigned long size)
		{
			const unsigned shiftt  = 4;

			unsigned short group   = key.getTag().getGroup();
			unsigned short element = key.getTag().getElement();

			unsigned long valuet = (group<<shiftt)|element;

			const unsigned nchars = 10;
			const unsigned shifts  = 4;

			unsigned n=nchars;
			unsigned long values=0;
			const char *s=key.getOwner();

			if (s) {
				while (n-- && *s) {
					values=(values<<shifts)|*s++;
				}
			}
			return (valuet+values+key.getBlock())%size; 
		}

	TagAndOwnerAndBlock key(const IndexEntryTagAndOwnerAndBlock &e)
		{
			// Split these up because behaved wierdly with SC 4.0 (zeroed out Tag) :(
			TagAndOwnerAndBlock k=e.getKey();
			return k;
		}
};

#endif /* __Header_ElmHash__ */

