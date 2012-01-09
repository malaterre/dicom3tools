#ifndef __Header_elmtype__
#define __Header_elmtype__

#include "attrtag.h"

const Uint16 VMUNLIMITED = (Uint16)-1;
const Uint16 VMNONE = 0;

#define DATADICTIONARYINDEXSIZE	791

class TagAndOwnerAndBlock {
	Tag tag;
	char *owner;
	Uint16 block;
	TagAndOwnerAndBlock& constructor(Tag t,const char *o,Uint16 b)
		{
			tag=t;
			Assert(o);
			owner=new char[strlen(o)+1];
			strcpy(owner,o);
			block=b;
			return *this;
		}
public:
	TagAndOwnerAndBlock(void)			{}
	TagAndOwnerAndBlock(Tag t,const char *o,Uint16 b)
		{
			constructor(t,o,b);
		}
	TagAndOwnerAndBlock(const TagAndOwnerAndBlock &to)
		{
			constructor(to.getTag(),to.getOwner(),to.getBlock());
		}
	TagAndOwnerAndBlock& operator=(const TagAndOwnerAndBlock &to)
		{
			return constructor(to.getTag(),to.getOwner(),to.getBlock());
		}
	~TagAndOwnerAndBlock()
		{
			Assert(owner);
			delete[] owner;
		}

	Tag		getTag(void)   const	{ return tag; }
	const char *	getOwner(void) const	{ return owner; }
	Uint16		getBlock(void) const	{ return block; }

	bool operator==(const TagAndOwnerAndBlock& to) const
		{
//if (tag == to.getTag() && owner && to.getOwner() && block == to.getBlock()) {
//	cerr	<< "TagAndOwnerAndBlock: operator==(): comparing <"
//		<< (owner ? owner : "-null-") << "> with <"
//		<< (to.getOwner() ? to.getOwner() : "-null-") << "> returns "
//		<< (strcmp(owner,to.getOwner()) ? "not equal" : "equal")
//		<< endl;
//}
			return tag == to.getTag()
			    && owner && to.getOwner()
			    && strcmp(owner,to.getOwner()) == 0
			    && block == to.getBlock();
		}
};

#endif /* __Header_elmtype__ */
