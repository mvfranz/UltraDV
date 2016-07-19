//---------------------------------------------------------------------
//
//	File:	TSorterIconListItem.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.23.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TSorterIconListItef_
#define __TSorterIconListItef_ 

// Includes
#include "TSorterContainer.h"

// Forward Declarations

// Class Declarations
class TSorterIconListItem : public BListItem
{
	public:
		// Member functions
		TSorterIconListItem(uint32 level, bool expanded, BRect bounds, TSorterIcons &theIcons, entry_ref& fileRef);
		~TSorterIconListItem();
		void 	DrawItem(BView *owner, BRect itemRect, bool drawEverything = false);
	
		void	Update(BView *owner, const BFont *font);	
		
		// Member variables	
		
		// Inlines
		BBitmap *GetMiniIcon(){ return fIcons.fMiniIcon; }
		BBitmap *GetLargeIcon(){ return fIcons.fLargeIcon; }
		
	private:				
		// Member functions
		void 	Init();
						
		// Member variables	
		SorterType		fType;
		char 			*fDataString;
		entry_ref		fEntryRef;	
		TSorterIcons	fIcons;
};		

#endif
