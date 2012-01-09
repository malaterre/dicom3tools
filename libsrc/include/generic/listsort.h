/* listsort.h 1.00 95/09/21 Copyright (C) 1995. David A. Clunie. All rights reserved. */
#ifndef __Header_listsort__
#define __Header_listsort__

#include "listsimp.h"

// type T must be cabable of ordering itself, specifically "<" must work

template<class T>
class SortedList : public SimpleList<T> {
public:
	SortedList(void) : SimpleList<T>()
		{
		}

	void	operator+=(T value)
		{
			SimpleListEntry<T> *ptr = new SimpleListEntry<T>;
			ptr->item=value;
			// Insert into list in ascending order
			SimpleListEntry<T> *test,*last;
			test=SimpleList<T>::head;
			last=0;
			while (test && test->item < value) {
				last=test;
				test=test->next;
			}
			ptr->next=test;
			if (last) last->next=ptr;
			else SimpleList<T>::head=ptr;

		}
};

template<class T>
class SortedListIterator : public SimpleListIterator<T> {
public:
	SortedListIterator(SortedList<T>& list)
		: SimpleListIterator<T> (list)
		{
		}
};

#endif /* __Header_listsort__ */

