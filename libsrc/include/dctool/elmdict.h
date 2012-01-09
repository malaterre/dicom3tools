#ifndef __Header_elmdict__
#define __Header_elmdict__

class PrivateOwners;
class ElementDictionaryTableEntry;

class ElementDictionary {
	PrivateOwners *CurrentOwners;

	const ElementDictionaryTableEntry *operator[] (Tag tag) const;
	const ElementDictionaryTableEntry *operator[] (const char *keyword) const;
public:
	ElementDictionary(void);
	~ElementDictionary();

	const char *	getValueRepresentation(Tag t) const;
	const Uint16	getValueMultiplicityMinimum(Tag t) const;
	const Uint16	getValueMultiplicityMaximum(Tag t) const;
	const char *	getDescription(Tag t) const;
	const char *	getKeyword(Tag t) const;
	bool		getTag(const char *keyword,Tag& tr) const;
	bool		isRetired(Tag& t) const;

	void		addOwner(Tag t,const char *owner);
	bool		hasOwner(Tag& t) const;
};

#endif /* __Header_elmdict__ */
