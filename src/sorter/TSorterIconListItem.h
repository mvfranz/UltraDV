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

#ifndef __TSorterIconListItem__
#define __TSorterIconListItem__ 

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
		BBitmap *GetMiniIcon(){ return m_Icons.m_MiniIcon; }
		BBitmap *GetLargeIcon(){ return m_Icons.m_LargeIcon; }
		
	private:				
		// Member functions
		void 	Init();
						
		// Member variables	
		SorterType		m_Type;
		char 			*m_DataString;
		entry_ref		m_EntryRef;	
		TSorterIcons	m_Icons;
};		

#endif
