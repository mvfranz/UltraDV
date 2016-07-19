//---------------------------------------------------------------------
//
//	File:	TSorterListItem.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TSorterListItef_
#define __TSorterListItef_ 

// Forward Declarations
#include "TSorterContainer.h"

// Forward Declarations

// Class Declarations
class TSorterListItem : public BListItem
{
	public:
		// Member functions
		TSorterListItem(uint32 level, bool expanded, BRect bounds, const char *theString, SorterType theType, entry_ref &fileRef);
		~TSorterListItem();
		
		void 	DrawItem(BView *owner, BRect itemRect, bool drawEverything = false);	
		void	Update(BView *owner, const BFont *font);			
		
		// Inlines
		inline char 		*GetDataString(){ return fDataString; }
		inline entry_ref 	*GetEntryRef(){ return &fEntryRef; }
		inline SorterType 	GetType(){ return fType; }
		inline DataType 	GetDataType(){ return fDataType; }
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
						
		// Member variables		
		SorterType	fType;
		char 		*fDataString;
		entry_ref	fEntryRef;
		TCueView	*fCue;
		DataType	fDataType;
};		

#endif
