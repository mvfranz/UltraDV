/*	DList.h
 *	Templated type-safe version of BList, especially for the Datatypes library.
 *	Copyright © 1996-1997 Jon Watte.
 *	$Id: DList.h,v 1.1 1997/07/22 21:27:06 hplus Exp $
 */

#ifndef _DLIST_H
	#define _DLIST_H

	#ifndef _LIST_H
		#include <List.h>
	#endif
	#ifndef _DEBUG_H
		#include <Debug.h>
	#endif


/*	This template actually reduces by the optimizer to no overhead 
	over a BList (in most cases). However, it is type-safe, which 
	is a real benefit.	*/

template<class T>
class DList :
	public BList
{
private:
								DList(
									const DList &);	/*	can't clone list */
		DList					&operator=(
									const DList &);	/*	can't assign list */
public:
								DList(
									int32			blocking = 20) :
									BList(
										blocking)
								{
									ASSERT(sizeof(T) <= 4);
								}
								~DList()
								{
								}

		bool					AddItem(
									T				&item)
								{
									return BList::AddItem((void *)item);
								}
		bool					AddItem(
									T				&item,
									int32			atIndex)
								{
									return BList::AddItem((void *)item, atIndex);
								}
		bool					RemoveItem(
									T				&item)
								{
									return BList::RemoveItem((void *)item);
								}
		T						RemoveItemIndex(
									int32			index)
								{
									return (T)BList::RemoveItem(index);
								}
		int32					IndexOf(
									const T			&item) const
								{
									return BList::IndexOf((void *)item);
								}
		bool					HasItem(
									const T			&item) const
								{
									return BList::HasItem((void *)item);
								}
		T						ItemAt(
									int32			index) const
								{
									return (T)BList::ItemAt(index);
								}
		T						ItemAtFast(
									int32			index) const
								{
									return (T)BList::ItemAtFast(index);
								}
		T						FirstItem() const
								{
									return (T)BList::FirstItem();
								}
		T						LastItem() const
								{
									return (T)BList::LastItem();
								}
		void					DoForEach(
									bool			(*func)(T))
								{
									BList::DoForEach((bool(*)(void *))func);
								}
		void					DoForEach(
									bool			(*func)(T item, void *arg),
									void			*arg)
								{
									BList::DoForEach((bool(*)(void *, void*))func, arg);
								}
		void					SortItems(
									int				(*cmp)(const T a, const T b))
								{
									BList::SortItems((int(*)(const void *, const void *))cmp);
								}
};

#endif /* _DLIST_H */


