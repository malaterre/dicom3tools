#ifndef __Header_listsimp__
#define __Header_listsimp__

template<class T> class SimpleList;
template<class T> class SimpleListIterator;

template <class T>
class SimpleListEntry {
public:		// blech :(
	T item;
	SimpleListEntry* next;
};

template<class T>
class SimpleList {
protected:
	//friend class SimpleListIterator<T>;
	//SimpleListEntry<T> *head;
public:
	SimpleListEntry<T> *head;	// blech :(
	
	SimpleList(void)
		{
			//cerr << "SimpleList<T>::SimpleList()\n" << flush;
			head=0;
		}

	virtual ~SimpleList(void)
		{
			SimpleListEntry<T> *ptr=head;
			while (ptr) {
				SimpleListEntry<T> *next=ptr->next;
				delete ptr;
				ptr=next;
			}
		}

	void operator+=(T value)
		{
			//cerr << "SimpleList<T>::operator+=\n" << flush;
			SimpleListEntry<T> *ptr = new SimpleListEntry<T>;
			ptr->item=value;
			// Insert at head of list
			ptr->next=head;
			head=ptr;
		}

	void operator-=(T value)
		{
			//cerr << "SimpleList<T>::operator-=() start\n"
			//     << flush;
			//cerr << "SimpleList<T>::operator-=() "
			//     << "head=" << hex << (unsigned long)head
			//     << "\n" << flush;
			SimpleListEntry<T> *ptr = head;
			SimpleListEntry<T> *last = 0;
			while (ptr) {
			//cerr << "SimpleList<T>::operator-=() looping\n"
			//     << flush;
			//cerr << "SimpleList<T>::operator-=() "
			//     << "head=" << hex << (unsigned long)head
			//     << "\n" << flush;
			//cerr << "SimpleList<T>::operator-=() "
			//     << "last=" << hex << (unsigned long)last
			//     << "\n" << flush;
			//cerr << "SimpleList<T>::operator-=() "
			//     << "ptr=" << hex << (unsigned long)ptr
			//     << "\n" << flush;
			//cerr << "SimpleList<T>::operator-=() "
			//     << "ptr->next=" << hex
			//     << (unsigned long)ptr->next
			//     << "\n" << flush;
				if (ptr->item == value) {
					if (last) last->next=ptr->next;
					else head=ptr->next;
					SimpleListEntry<T> *old = ptr;
					ptr=ptr->next;
					delete old;
				}
				else {
					last=ptr;
					ptr=ptr->next;
				}
			}
			//cerr << "SimpleList<T>::operator-=() done\n" << flush;
			//cerr << "SimpleList<T>::operator-=() "
			//    << "head=" << hex << (unsigned long)head
			//     << "\n" << flush;
			//dump();
		}

	void	dump(void)
		{
			cerr << "SimpleList<T>::dump()\n" << flush;
			cerr << "SimpleList<T>::dump() "
			     << "head=" << hex << (unsigned long)(head)
			     << "\n" << flush;
			SimpleListEntry<T> *ptr=head;
			while (ptr) {
				cerr << "SimpleList<T>::dump() "
				     << "ptr=" << hex << (unsigned long)(ptr)
				     << "\n" << flush;
				ptr=ptr->next;
			}
		}

	bool	isEmpty(void) const		{ return head == NULL; }
	bool	isOne(void) const		{ return head != NULL && head->next == NULL; }
	bool	isMultiple(void) const		{ return head != NULL && head->next != NULL; }
};

template<class T>
class SimpleListIterator {
protected:
	SimpleList<T>	   *list;
	SimpleListEntry<T> *ptr;
public:
	SimpleListIterator(void)
		{
			//cerr << "SimpleListIterator(void)\n" << flush;
			list=0; ptr=0;
		}

	SimpleListIterator(SimpleList<T>& l)
		{
			//cerr << "SimpleListIterator(list)\n" << flush;
			list=&l; ptr=list->head;
			//cerr << "SimpleListIterator(list) "
			//     << "head=" << hex << (unsigned long)(list->head)
			//     << "\n" << flush;
			//cerr << "SimpleListIterator(list) "
			//     << "ptr=" << hex << (unsigned long)ptr
			//     << "\n" << flush;
		}

	void set(SimpleList<T>& l)
		{
			list=&l; ptr=list->head;
		}

	void first(void)	{ ptr=list->head; }
	int ismore(void)
		{
			//cerr << "SimpleListIterator::ismore() "
			//     << "ptr=" << hex << (unsigned long)ptr
			//     << "\n" << flush;
			return ptr != 0;
		}
	void next(void)		{ if (ptr) ptr=ptr->next; }
	T value(void)	        { if (ptr) return ptr->item;
				  else { T r = 0; return r; } }

	int operator!()		{ return ismore(); }
	void operator++()	{ next(); }		// prefix  ++i
	void operator++(int)	{ next(); }		// postfix i++
	T operator()()		{ return value(); }
};

#endif /* __Header_listsimp__ */


