#ifndef __Header_elmentry__
#define __Header_elmentry__

class ElementDictionaryTableEntry {
public:
	Uint16		Group;
	Uint16		Element;
	Uint16		PrivateBlock;	// 0x0010-0x00ff valid, 0x0000=inactive
	const char *	ValueRepresentation;
	Uint16		ValueMultiplicityMinimum;
	Uint16		ValueMultiplicityMaximum;
	const char *	Owner;
	const char *	Keyword;
	const char *	Description;
	bool	Retired;
};

#endif /* __Header_elmentry__ */
