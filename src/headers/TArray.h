//---------------------------------------------------------------------
//
//	File:	TArray.h
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

#ifndef __TARRAY_H__
#define __TARRAY_H__

#include "TCollection.h"

typedef	int (*CompareFunc)( void*, void*);	 // array element comparison function

class TArray : public TCollection
{

	public:
		// Member Variables
		int16		m_BlockSize;			// Number of m_Slots to allocate when 
											//  more space is needed			
		int32		m_Slots;				// Total number of m_Slots allocated	
		void		*m_Items;				// Items in the array				
				
		int32 		m_ElementSize;			// size of each element in bytes	
		bool		m_LockChanges;			// can't insert or delete if locked 
		bool		m_UsingTemporary;		// TRUE if temporary element storage
											// buffer is in use					
			
		// Member Functions
		TArray( int32 elementSize);
		virtual ~TArray();

		virtual void	SetBlockSize( int16 aBlockSize);	
		virtual void	InsertAtIndex( void *itemPtr, int32 index);
		virtual void	DeleteItem( int32 index);
		virtual void	MoveItemToIndex( int32 currentIndex, int32 newIndex);
		virtual void	SetArrayItem( void *itemPtr, int32 index);
		virtual void	GetArrayItem( void *itemPtr, int32 index);
		virtual void	Swap( int32 index1, int32 index2);
		virtual int32	Search( void *itemPtr, CompareFunc compare);
		virtual bool 	SetLockChanges( bool fLockChanges);
		//virtual CObject	*Copy(void);
	
		virtual void	AddEnd(void *itemPtr);
	
		virtual void	Resize( int32 numSlots);
		virtual void	MoreSlots( void);
		virtual void	Store( const void *itemPtr, int32 index);
		virtual void	Retrieve( void *itemPtr, int32 index);
		virtual void	CopyToTemporary( int32 index);
		virtual void	CopyFromTemporary( int32 index);
		virtual int32	ItemOffset( int32 itemIndex);

	private:

};


//
//	A pointer to a tMovedElementInfo structure is passed as the info
//	parameter to BroadCastChange when MoveItemToIndex has completed
//

typedef struct tMovedElementInfo
{
	int32	originalIndex;	// item's original index
	int32	newIndex;		// item's new index			
	
} tMovedElementInfo;

// Change protocol for Array class 
enum									// info parameter usage		
{
	arrayInsertElement = 1,				// index of new element		
	arrayDeleteElement,					// index of deleted element	
	arrayMoveElement,					// pointer to tMovedElementInfo, see above 
	arrayElementChanged,				// index of changed element	
	
	arrayLastChange = arrayElementChanged
};

#define		BAD_INDEX		-1L			//  Flag indicating a failed search	

#endif
