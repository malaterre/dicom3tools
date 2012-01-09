#ifndef __Header_hash__
#define __Header_hash__

//#include "generic/basetype.h"
//#include "generic/listsimp.h"
#include "basetype.h"
#include "listsimp.h"

template <class E,class K,class L,class LI>
class OpenHashTable {
	L **table;
	unsigned long size;

	// This wierd return value stuff and splitting out
	// the while from the constructor is to keep SunPro
	// C++ happy :(

	OpenHashTable& clear(void)
		{
			unsigned long s=size;
			L **p=table;
			while (s--) *p++=0;
			return *this;
		}
public:
	OpenHashTable(unsigned long s)
		{
			//typedef L *OpenHashTableEntry;
			table=new L *[s];
			size=s;
			clear();
		}

	// need a destructor to free the table and ?? delete
	// what is in it ?? :(

	OpenHashTable&	operator+=(E entry)
		{
			unsigned long index=hash(key(entry),size);
			L *list=table[index];
			if (!list) {
				list=new L;
				table[index]=list;
			}
			*list+=entry;
			return *this;
		}

	OpenHashTable&	operator-=(E entry)
		{
			unsigned long index=hash(key(entry),size);
			L *list=table[index];
			if (list) {
				*list-=entry;
			}
			return *this;
		}

	E *		operator[](K k)
		{
			unsigned long index=hash(k,size);
			L *list=table[index];
			if (list) {
				LI li(*list);
				while (!li) {
					E e=li();
					if (key(e) == k) {
						E *ep = new E;
						*ep=e;
						return ep;
					}
					++li;
				}
			}
			return 0;
		}

	// virtual functions supplied by derived class ...

	virtual	unsigned long	hash(const K &key,unsigned long size)	= 0;
	virtual	K 		key (const E &entry)	= 0;

};

// ************** Useful classes for using OpenHashTable **************

class HashKeyString {			// Use as class K
	const char *string;
public:
	HashKeyString(void)		{ string=0; }
	HashKeyString(const char *s)	{ string=s; }

	const char *getString(void) const	{ return string; }

	bool operator==(const HashKeyString& k) const
		{
			return string && k.getString()
				   && strcmp(string,k.getString()) == 0;
		}
};

#endif /* __Header_hash__ */

