#include "elmtype.h"
#include "elmpriv.h"

const char *
PrivateOwners::operator[](Tag tag)
{
	SimpleListIterator<TagAndOwnerAndBlock *> i(*this);
	while (!i) {
		// ignore the block
		if (tag.getGroup() == i()->getTag().getGroup()
		&& (tag.getElement()&0xff00)
			== i()->getTag().getElement()
				) return i()->getOwner();
		++i;
	}
	return 0;
}

void
PrivateOwners::operator+=(const TagAndOwnerAndBlock &to)
{
	// elements      0x0010-0x00ff
	// define blocks 0x1000-0xff00

	const char *owner = to.getOwner();
	Assert(owner);
	SimpleList<TagAndOwnerAndBlock *>::operator+=(new TagAndOwnerAndBlock(
		Tag(to.getTag().getGroup(),to.getTag().getElement()<<8),
		owner,0)); // ignore the block field
}
