//---------------------------------------------------------------------
//
//	File:	TCollection.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.24.98
//
//	Desc:	Abstract class for a collection of objects arranged in some kind of
//			data structure. The command ground for collections is that they
//			contain zero or more items (objects).
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TCollection.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCollection::TCollection()
{
	// Start off with no items
	fNumItems = 0L;
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCollection::~TCollection()
{
}


//---------------------------------------------------------------------
//	GetNumItems
//---------------------------------------------------------------------
//
//	Return the number of items in the collection
//

long TCollection::GetNumItems()
{
	return( fNumItems);
}



//---------------------------------------------------------------------
//	IsEmpty
//---------------------------------------------------------------------
//
//	Check whether the collection contains any items
//

bool TCollection::IsEmpty()
{
	return( fNumItems == 0L );
}
