#include "attr.h"
#include "attrlist.h"
#include "listsort.h"

// In order for the sorted list template to know how to sort one needs
// to define a type that can order itself, hence this ...

class AttributePointer {
	Attribute *Pointer;
public:
	AttributePointer(void)				{ Pointer=0; }
	AttributePointer(Attribute *p)			{ Pointer=p; }
	AttributePointer(const AttributePointer& t)	{ Pointer=t.Pointer; }

	void operator=(Attribute *p)			{ Pointer=p; }
	void operator=(const AttributePointer& t)	{ Pointer=t.Pointer; }

	Attribute *	operator()(void)		{ return Pointer; }
	Attribute *	operator->(void)		{ return Pointer; }
	Attribute &	operator*(void)			{ return *Pointer; }

	operator Attribute*()				{ return Pointer; }

	int operator==(const AttributePointer& t2)
		{ return Pointer->getTag() == t2.Pointer->getTag(); }
	int operator<(const AttributePointer& t2)
		{ return Pointer->getTag() <  t2.Pointer->getTag(); }
	int operator<=(const AttributePointer& t2)
		{ return Pointer->getTag() <= t2.Pointer->getTag(); }
	int operator>(const AttributePointer& t2)
		{ return Pointer->getTag() >  t2.Pointer->getTag(); }
	int operator>=(const AttributePointer& t2)
		{ return Pointer->getTag() >= t2.Pointer->getTag(); }
};	

// ********************** Implementation methods ************************

class AttributeListImpl : public SortedList<AttributePointer> {
public:
	AttributeListImpl(void) : SortedList<AttributePointer>()
		{
		}

	virtual ~AttributeListImpl();

	Attribute * operator[](Tag tag);
	void	operator-=(Tag tag);

	void	operator+=(Attribute *p);
	void	operator-=(Attribute *p);

	void	operator-=(AttributeList& deletelist);
	void	operator+=(AttributeList& replacelist);

	void	operator-=(AttributeListImpl& deletelist);
	void	operator+=(AttributeListImpl& replacelist);
};

class AttributeListIteratorImpl : public SortedListIterator<AttributePointer> {
public:
	AttributeListIteratorImpl(AttributeListImpl& list)
		: SortedListIterator<AttributePointer> (list)
		{
			//cerr << "AttributeListIteratorImpl(list)" << endl;
			//cerr << "AttributeListIteratorImpl(list) "
			//     << "list.head=" << hex << (unsigned long)(list.head)
			//     << "" << endl;
		}
};

AttributeListImpl::~AttributeListImpl()
{
//#ifdef TRACE_DESTRUCTORS
//cerr << "AttributeListImpl::~AttributeListImpl" << endl;
//#endif
	AttributeListIteratorImpl listi(*this);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		delete a;
		++listi;
	}
}

inline Attribute *
AttributeListImpl::operator[](Tag tag)
{
	Attribute *p = 0;
	AttributeListIteratorImpl i(*this);
	while (!i) {
		if (i()->getTag() == tag) {
			p=i();
//cerr << "operator[]: found tag = " << tag << endl;
			break;
		}
		++i;
	}
	return p;
}

inline void
AttributeListImpl::operator-=(Attribute *p)
{
	if (p) {
//cerr << "operator-=: removing tag = " << p->getTag() << endl;
		SortedList<AttributePointer>::operator-=(p);
	}
}

inline void
AttributeListImpl::operator+=(Attribute *p)
{
	if (p) SortedList<AttributePointer>::operator+=(p);
}

inline void
AttributeListImpl::operator-=(Tag tag)
{
	Attribute *p;
	while ((p = operator[](tag)) != 0) operator-=(p);
}

inline void
AttributeListImpl::operator-=(AttributeListImpl& deletelist)
{
	AttributeListIteratorImpl i(deletelist);
	while (!i) { operator-=(i()); ++i; }
}

inline void
AttributeListImpl::operator+=(AttributeListImpl& replacelist)
{
	AttributeListIteratorImpl i(replacelist);
	while (!i) {
		Attribute *a=i();
		operator-=(a->getTag());
		operator+=(a);
		replacelist-=a;
		i.first();
	}
}

inline void
AttributeListImpl::operator-=(AttributeList& deletelist)
{
	operator-=(*deletelist.list);
}

inline void
AttributeListImpl::operator+=(AttributeList& replacelist)
{
	operator+=(*replacelist.list);
}

// ********************** Interface methods ************************

AttributeList::AttributeList(void)
{
	list=new AttributeListImpl();
	Assert(list);
	dictionary=0;
	specificCharacterSetInfo=NULL;		// populated when needed by looking for SpecificCharacterSet attribute value in list, e.g., on calling validateVR() in ManagedAttributeList
}

AttributeList::~AttributeList()
{
#ifdef TRACE_DESTRUCTORS
cerr << "AttributeList::~AttributeList" << endl;
#endif
	Assert(list);
	delete list;
	if (dictionary) delete dictionary;
	if (specificCharacterSetInfo) delete specificCharacterSetInfo;
}

Attribute * 	AttributeList::operator[](Tag tag)		{ return list->operator[](tag); }
void		AttributeList::operator-=(Tag tag)		{ list->operator-=(tag); }

void 		AttributeList::operator-=(Attribute *p)		{ list->operator-=(p); }
void		AttributeList::operator+=(Attribute *p)		{ list->operator+=(p); }

void 		AttributeList::operator-=(AttributeList& p)	{ list->operator-=(p); }
void		AttributeList::operator+=(AttributeList& p)	{ list->operator+=(p); }

AttributeListIterator::AttributeListIterator(AttributeList& list)
{
	iterator=new AttributeListIteratorImpl(*list.list);
	Assert(iterator);
}

AttributeListIterator::~AttributeListIterator()
{
#ifdef TRACE_DESTRUCTORS
cerr << "AttributeListIterator::~AttributeListIterator" << endl;
#endif
	Assert(iterator);
	delete iterator;
}

void 		AttributeListIterator::first(void)	{ iterator->first(); }
int 		AttributeListIterator::ismore(void)	{ return iterator->ismore(); }
void 		AttributeListIterator::next(void)	{ iterator->next(); }
Attribute * 	AttributeListIterator::value(void)	{ return iterator->value(); }


