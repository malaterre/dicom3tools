#include "misctype.h"

#include "transynd.h"
#include "transynt.h"

const TransferSyntaxDictionaryEntry *
TransferSyntaxDictionary::operator[](const char *uid)
{
	TransferSyntaxDictionaryEntry *p = TransferSyntaxDictionaryTable;
	while (p->UID) {
		if (strcmp(p->UID,uid) == 0) return p;
		++p;
	}
	return 0;
}

void
TransferSyntaxDictionary::first(void)
{
	ptr=TransferSyntaxDictionaryTable;
}

int
TransferSyntaxDictionary::ismore(void)
{
	return ptr->UID != 0;
}

void
TransferSyntaxDictionary::next(void)
{
	++ptr;
}

const TransferSyntaxDictionaryEntry *
TransferSyntaxDictionary::value(void)
{
	return ptr->UID ? ptr : 0;
}
