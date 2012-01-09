/* listfifo.h 1.00 95/09/21 Copyright (C) 1995. David A. Clunie. All rights reserved. */
#ifndef __Header_listfifo__
#define __Header_listfifo__

#include "listsimp.h"

// First thing entered remains at head of list

template<class T>
class FifoList : public SimpleList<T> {
public:
	FifoList(void) : SimpleList<T>()
		{
		}

	void	operator+=(T value)
		{
			SimpleListEntry<T> *ptr = new SimpleListEntry<T>;
			ptr->item=value;
			// Insert into list at tail
			SimpleListEntry<T> *test,*last;
			test=SimpleList<T>::head;
			last=0;
			while (test) {
				last=test;
				test=test->next;
			}
			ptr->next=0;
			if (last) last->next=ptr;
			else SimpleList<T>::head=ptr;
		}
};

template<class T>
class FifoListIterator : public SimpleListIterator<T> {
public:
	FifoListIterator(FifoList<T>& list)
		: SimpleListIterator<T> (list)
		{
		}
};

#endif /* __Header_listfifo__ */

