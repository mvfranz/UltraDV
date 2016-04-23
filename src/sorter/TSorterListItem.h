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

#ifndef __TSorterListItem__
#define __TSorterListItem__ 

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
		inline char 		*GetDataString(){ return m_DataString; }
		inline entry_ref 	*GetEntryRef(){ return &m_EntryRef; }
		inline SorterType 	GetType(){ return m_Type; }
		inline DataType 	GetDataType(){ return m_DataType; }
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
						
		// Member variables		
		SorterType	m_Type;
		char 		*m_DataString;
		entry_ref	m_EntryRef;
		TCueView	*m_Cue;
		DataType	m_DataType;
};		

#endif
