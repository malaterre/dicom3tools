#ifndef __Header_transynd__
#define __Header_transynd__

#include "transyne.h"

class TransferSyntaxDictionary {
	TransferSyntaxDictionaryEntry *ptr;
public:
	TransferSyntaxDictionary(void)	{ first(); }

	const TransferSyntaxDictionaryEntry *operator[](const char *uid);

	void first(void);
	int ismore(void);
	void next(void);
	const TransferSyntaxDictionaryEntry *value(void);

	int operator!()		{ return ismore(); }
	void operator++()	{ next(); }		// prefix  ++i
	void operator++(int)	{ next(); }		// postfix i++
	const TransferSyntaxDictionaryEntry *
		operator()()
				{ return value(); }
};

#endif /* __Header_transynd__ */
