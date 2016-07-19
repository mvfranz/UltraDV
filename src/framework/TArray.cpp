//---------------------------------------------------------------------
//
//	File:	TArray.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.24.98
//
//	Desc:	Implements a dynamically sized array. Elements may be of any
//			size, but all elements must be the same size.	
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include <alloca.h>

#include "TArray.h"


// Macros for calculating item offsets and addresses.	
// They are all one-based indices						
#define ASSERT_INDEX(index)	ASSERT( (index > 0)&&(index <= fNumItems))
#define ITEM_OFFSET(index) ((index-1) * fElementSize)
//#define ITEM_PTR(index) (&((char *)*fItems)[ ITEM_OFFSET(index) ])
#define ITEM_PTR(index) (&((char *)fItems)[ ITEM_OFFSET(index) ])

// Macro for obtaining a pointer to temporary storage for one 		
// element The temporary storage is an extra slot kept at the		
// end of the item handle.											
//#define TMP_ITEM_PTR	(&((char*)*fItems)[ ITEM_OFFSET(fNumItems+1) ])
#define TMP_ITEM_PTR	(&((char*)fItems)[ ITEM_OFFSET(fNumItems+1) ])




//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Initialize an array object, specifying the element size. The
//	initialized array is empty.
//

TArray::TArray( int32 anElementSize) : TCollection()
{	
	fBlockSize 		= 3;
	fElementSize 		= anElementSize;						
	fSlots 			= 0;
	fLockChanges 		= false;
	fUsingTemporary 	= false;
		
	//fItems = malloc( fElementSize );
	fItems = malloc( sizeof(fElementSize) * fElementSize );
	ASSERT( fItems);
				
}
	

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//


TArray::~TArray()
{
	// Free item memory	
	free(fItems);				

}


//---------------------------------------------------------------------
//	SetBlockSize
//---------------------------------------------------------------------
//
//	Set the fBlockSize which specifies the number of empty fSlots to
//	allocate when more space is needed.
//

void	TArray::SetBlockSize(short aBlockSize)
{
	fBlockSize = aBlockSize;				
	
}	
	

//---------------------------------------------------------------------
//	SetArrayItem
//---------------------------------------------------------------------
//
//	Set the value of the element at index. Index must be within the array.
//	Sends dependents an arrayElementChanged message.
//

void TArray::SetArrayItem( void *itemPtr, int32 index)
{
	ASSERT_INDEX( index);
	
	Store( itemPtr, index);
}


//---------------------------------------------------------------------
//	Store
//---------------------------------------------------------------------
//
//	Protected method to set an array element.
//


void TArray::Store( const void *itemPtr, int32 index)
{	
	//BlockMoveData( itemPtr, ITEM_PTR( index), fElementSize);		
	memcpy( ITEM_PTR(index), itemPtr, fElementSize);		
}


//---------------------------------------------------------------------
//	GetArrayItem
//---------------------------------------------------------------------
//
//	Retrieve the value of the array element. Index must be within the array.
//	itemPtr must point to storage of sufficient size to hold one element.
//

void TArray::GetArrayItem( void *itemPtr, int32 index)
{
	ASSERT_INDEX( index);
	
	Retrieve( itemPtr, index);
	
}


//---------------------------------------------------------------------
//	Retrieve
//---------------------------------------------------------------------
//
//	Protected method to retrieve an array element.
//

void TArray::Retrieve( void *itemPtr, int32 index)
{	
	//BlockMoveData( ITEM_PTR( index), itemPtr, fElementSize);
	memcpy( itemPtr, ITEM_PTR( index),  fElementSize);
}


//---------------------------------------------------------------------
//	InsertAtIndex
//---------------------------------------------------------------------
//
//	Insert an item into the array at the specified index. Any items
//	at or above index are moved down. If index is any value greater
//	than the current size, then the item is appended to the end
//	of the array.
//

void TArray::InsertAtIndex( void *itemPtr, int32 index)
{	
	ASSERT(index > 0);
	ASSERT( fLockChanges == false);
	
	if (fLockChanges) return;
			
	if (fNumItems >= fSlots)				// Check if we need more space		
		MoreSlots();

	if (index <= fNumItems) 
	{									// Move items at position >= index	
										//   down one slot, unless it is	
										//   the last item					
		//BlockMoveData( ITEM_PTR( index), ITEM_PTR( index + 1), (fNumItems - index + 1) * fElementSize);
		memcpy( ITEM_PTR( index + 1), ITEM_PTR( index), (fNumItems - index + 1) * fElementSize);
	}
	else
		index = fNumItems + 1;
		
	fNumItems++;							// There's another item in the list	

										// Stick new object in the empty slot
	Store( itemPtr, index);

}	


//---------------------------------------------------------------------
//	DeleteItem
//---------------------------------------------------------------------
//
//	Delete an item from the Array. Index must be within the array.
//	Sends dependents an arrayDeleteElement message.
//

void TArray::DeleteItem( int32 index)
{
	ASSERT_INDEX( index);
	ASSERT( fLockChanges == false);

	if (fLockChanges) return;

	// We're gonna get rid of an item	
	fNumItems--;							
	
	// Shift items following the object	
	if (index <= fNumItems) 
	{			
		//  to remove up one slot, thereby	overwriting it					
		// BlockMoveData( ITEM_PTR( index+1), ITEM_PTR( index), (fNumItems - index + 1) * fElementSize);
		memcpy( ITEM_PTR( index), ITEM_PTR( index+1), (fNumItems - index + 1) * fElementSize);
	}
		
	// The number of free fSlots is greater than the fBlockSize.	
	// Reduce the size of the items handle.						
	if (fSlots > fNumItems + fBlockSize) 
	{
		Resize( fSlots-fBlockSize);
	}		
}


//---------------------------------------------------------------------
//	Resize
//---------------------------------------------------------------------
//
//	Resize an array to the desired number of fSlots. fNumItems is not affected.
//

void TArray::Resize( int32 numSlots)
{
	// Be sure to take the scratch element storage at the beginning 
	// of the buffer into account when resizing	the item buffer														
	realloc( fItems,  (numSlots+1L) * fElementSize);	
	fSlots = numSlots;
}



//---------------------------------------------------------------------
//	MoreSlots
//---------------------------------------------------------------------
//
//	Grows the array by fBlockSize fSlots.
//

void TArray::MoreSlots()
{
	Resize( fSlots + fBlockSize);	
}	



//---------------------------------------------------------------------
//	MoveItemToIndex
//---------------------------------------------------------------------
//
//	Move an element to a specified position in the array, shifting
//	other element in the list appropriately.
//	Sends dependents an arrayMoveElement message.
//

void TArray::MoveItemToIndex( int32 currentIndex, int32 newIndex)
{
	tMovedElementInfo moveInfo;
	
	ASSERT_INDEX( currentIndex);
	ASSERT_INDEX( newIndex);
	
	if (currentIndex == newIndex) return;
	
	/* In order to move the item, we must first make a copy of it.	*/
	/* Since the items could be of any size, we keep storage for	*/
	/* exactly one item at the end of the items handle. In order	*/
	/* to guard against the two methods simultaneously using the	*/
	/* temporary buffer, we mark it in use by setting the			*/
	/* fUsingTemporary instance variable.							*/
	
	CopyToTemporary( currentIndex);
	
	if (currentIndex < newIndex) 
	{									
		// Element is before target location. Shift items between current	
		//   and target locations down one											
		//BlockMoveData( ITEM_PTR( currentIndex+1), ITEM_PTR( currentIndex), (newIndex - currentIndex) * fElementSize);
		memcpy( ITEM_PTR( currentIndex), ITEM_PTR( currentIndex+1), (newIndex - currentIndex) * fElementSize);
									
	}
	else if (currentIndex > newIndex)
	{									
		// Element is after target location.
		// Shift items between target and current locations up one	
		//BlockMoveData( ITEM_PTR( newIndex), ITEM_PTR( newIndex+1), (currentIndex - newIndex) * fElementSize);
		memcpy( ITEM_PTR( newIndex+1), ITEM_PTR( newIndex), (currentIndex - newIndex) * fElementSize);							
	}
	
	// copy element into new position 	
	CopyFromTemporary( newIndex);
	
	moveInfo.originalIndex = currentIndex;
	moveInfo.newIndex = newIndex;

	
}


//---------------------------------------------------------------------
//	SetLockChanges
//---------------------------------------------------------------------
//
// Set the fLockChanges instance variable and return its old value. Setting
// fLockChanges to true prevents the InsertAtIndex and Delete methods
// from operating.
//


bool TArray::SetLockChanges( bool fLockChanges)
{
	bool wasLocked = fLockChanges;
	
	fLockChanges = fLockChanges;
	
	return wasLocked;

}


//---------------------------------------------------------------------
//	Search
//---------------------------------------------------------------------
//
//	Find an array element matching the data at itemPtr. matchFunc is used 
// 	to perform the comparison and should be declared:
// 		int compare( void *item1, void *item2);
// 		
// 	and should return 0 if the items are equivalent.
// 	NOTE: this makes compare compatible with the compare function passed
// 	to qsort.
//

int32 TArray::Search( void *itemPtr, CompareFunc compare)
{
	register char	* items;
	register int32 	i;
	int32			foundIndex = BAD_INDEX;
	
	
	items = (char*) fItems;
	for (i = 0; i < fNumItems; i++)
	{
		if (compare( itemPtr, items) == 0)
		{
			foundIndex = i+1;
			break;
		}
		items += fElementSize;
	}
		
	return foundIndex;

}



//---------------------------------------------------------------------
//	Swap
//---------------------------------------------------------------------
//
//	Swap two items. Sends dependents an arrayElementChanged message for both
// 	items.
//

void TArray::Swap( int32 index1, int32 index2)
{
	ASSERT_INDEX( index1);
	ASSERT_INDEX( index2);
	
	CopyToTemporary( index1);
	//BlockMoveData( ITEM_PTR( index2), ITEM_PTR( index1), fElementSize);
	memcpy( ITEM_PTR( index2), ITEM_PTR( index1), fElementSize);
	CopyFromTemporary( index2);
	
}



//---------------------------------------------------------------------
//	CopyToTemporary
//---------------------------------------------------------------------
//
//	Place an item into temporary storage
//

void TArray::CopyToTemporary( int32 index)
{
	ASSERT( fUsingTemporary == false);
	
	fUsingTemporary = true;
	
	//BlockMoveData( ITEM_PTR( index), TMP_ITEM_PTR, fElementSize);
	memcpy( TMP_ITEM_PTR, ITEM_PTR( index), fElementSize);

}


//---------------------------------------------------------------------
//	CopyFromTemporary
//---------------------------------------------------------------------
//
//	Get an item from temporary storage
//

void TArray::CopyFromTemporary( int32 index)
{
	ASSERT( fUsingTemporary == true);

	fUsingTemporary = false;
	
	//BlockMoveData( TMP_ITEM_PTR, ITEM_PTR( index), fElementSize);
	memcpy( ITEM_PTR(index), TMP_ITEM_PTR, fElementSize);
}



//---------------------------------------------------------------------
//	ItemOffset
//---------------------------------------------------------------------
//
//	Get the offset of an item into the item data
//

int32 TArray::ItemOffset( int32 itemIndex)
{
	ASSERT_INDEX( itemIndex);
	
	return ITEM_OFFSET( itemIndex);

}


/******************************************************************************
 Copy {OVERRIDE}
 
 	Make a copy of the array, including the item handle.
******************************************************************************/
/*
CObject *TArray::Copy(void)
{
	TArray *theCopy;
	Handle	itemsCopy;
	OSErr	err;
	bool	savedAlloc;
	
	theCopy = (TArray*) CCollection::Copy();
	
	TRY
	{
		itemsCopy = theCopy->fItems;
		
		savedAlloc = SetAllocation( kAllocCanFail);
		err = HandToHand( &itemsCopy);
		SetAllocation( savedAlloc);
		
		FailOSErr( err);
		
		theCopy->fItems = itemsCopy;
	}
	CATCH
	{
		// if copying item handle failed, then we should
		// kill the copy created by CObject::Copy. We
		// clear the item handle so disposing of the
		// copy doesn't dispose of this array's item handle
		
		theCopy->fItems = NULL;
		theCopy->Dispose();
	}
	ENDTRY;
	
	return theCopy;

}
*/


//---------------------------------------------------------------------
//	AddEnd
//---------------------------------------------------------------------
//
//

void TArray::AddEnd(void *itemPtr)
{
	ASSERT(itemPtr!=NULL);
	InsertAtIndex(itemPtr, GetNumItems()+1);
}
