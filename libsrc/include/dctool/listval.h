#ifndef __Header_listval__
#define __Header_listval__

/* ************************* Value Lists ************************ */

template<class T> class ValueList;
template<class T> class ValueListIterator;

template<class T>
class ValueListEntry {
private:
	//friend class ValueList<T>;
	//friend class ValueListIterator<T>;
public:		// blech :(
	T item;
	ValueListEntry *next;
};

template<class T>
class ValueList {
private:
	//friend class ValueListIterator<T>;
	//ValueListEntry<T> *head;
	
	unsigned count;

	ValueListEntry<T> *findLast(void)
		{
			ValueListEntry<T> *last=0;
			ValueListEntry<T> *ptr=head;
			while (ptr) {
				last=ptr;
				ptr=ptr->next;
			}
			return last;
		}

	ValueListEntry<T> *findEntry(unsigned index)
		{
			if (index >= count || !head) return 0;
			ValueListEntry<T> *ptr=head;
			unsigned i=0;
			while (i++ < index) {
				ptr=ptr->next;
			}
			return ptr;
		}
public:
	ValueListEntry<T> *head;		// blech :(

	ValueList(void)	{ head=0; count=0; }
	virtual ~ValueList(void)
		{
			ValueListEntry<T> *ptr=head;
			while (ptr) {
				ValueListEntry<T> *next=ptr->next;
				delete ptr;
				ptr=next;
			}
		}

	bool	getValue(unsigned index,T& value) const
		{
			if (index >= count || !head) return false;
			const ValueListEntry<T> *ptr=head;
			unsigned i=0;
			while (i++ < index) {
				ptr=ptr->next;
			}
			value=ptr->item;
			return true;
		}

	void	setValue(unsigned index,T value)
		{
			T dummy = 0;
			while (index >= count) addValue(dummy);
			ValueListEntry<T> *ptr = findEntry(index);
			if (ptr) ptr->item=value;
			// else internal error :(
		}

	void	addValue(T value)
		{
			ValueListEntry<T> *ptr = 
				new ValueListEntry<T>;
			ptr->item=value;
			ptr->next=0;
			ValueListEntry<T> *last=findLast();
			if (last) last->next=ptr;
			else head=ptr;
			++count;
		}

	void	addValues(unsigned number,T *vptr)
		{
			while (number--) addValue(*vptr++);
		}

	Uint16	getVM(void) const	{ return count; }
};

template<class T>
class ValueListIterator {
private:
	ValueListEntry<T> *head;
	ValueListEntry<T> *ptr;
public:
	ValueListIterator(void)
				{ head=0; ptr=0;}

	ValueListIterator(const ValueList<T>& list)
				{ head=list.head; ptr=head; }

	void set(ValueList<T>& list)
				{ head=list.head; ptr=head; }
	void first(void)	{ ptr=head; }
	int ismore(void)	{ return ptr != 0; }
	void next(void)		{ if (ptr) ptr=ptr->next; }
	T value(void)	        { if (ptr) return ptr->item;
				  else { T r = 0; return r; } }

	int operator!()		{ return ismore(); }
	void operator++()	{ next(); }		// prefix  ++i
	void operator++(int)	{ next(); }		// postfix i++
	T operator()()		{ return value(); }
};

#endif /* __Header_listval__ */
