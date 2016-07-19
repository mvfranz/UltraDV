//---------------------------------------------------------------------
//
//	File:	TSorterList.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TSORTERLIST_H__
#define __TSORTERLIST_H__ 

// Includes
#include "TSorterContainer.h"

// Forward Declarations
class TSorterListItem;
class TSorterIconListItem;

// Class Declarations
class TSorterList : public BListView //public BView
{
	public:
		// Member functions
		TSorterList(BRect bounds, TSorterContainer *parent, SorterType theType);
		~TSorterList();
		void 	MessageReceived(BMessage *theMessage);
		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		
		void 	KeyDown(const char *bytes, int32 numBytes);
		
		// Dragging Routines
		bool 	InitiateDrag(BPoint point, int32 index, bool wasSelected);
				
		// Utility Routines
		void 	MakeActive(bool theState);
		void	Sort();
		void 	Synchronize( TSorterList *syncSource);
		
		// Member variables	
		
		// Inlines
		inline	SorterType		 	GetType(){ return fType; }
		inline 	TSorterContainer	*GetParent(){ return fParent; }
		
	private:				
		// Member functions
		void 	Init();
				
		// Utility Routines
		void 	SetListItemDimensions(TSorterListItem *listItem, BRect theRect);
		void 	SetListItemIconDimensions(TSorterIconListItem *listItem, BRect theRect);
		int32 	GetOccurences(entry_ref &fileRef);
		
		void 	HandleRefsMessage(BMessage *theMessage);
		void 	AddNameToDataList(BEntry *theEntry);
		void 	AddPathToDataList(BEntry *theEntry);
		void 	AddSizeToDataList(BEntry *theEntry);
		void 	AddDateToDataList(BEntry *theEntry);
		void 	AddTypeToDataList(BEntry *theEntry, BNodeInfo *nodeInfo);
		void 	AddDurationToDataList(BEntry *theEntry);
		void 	AddAudioInfoToDataList(BEntry *theEntry);
		void 	AddVideoInfoToDataList(BEntry *theEntry);
		void 	AddIconsToDataList(BEntry *theEntry, BNodeInfo &nodeInfo);
		
		// Editor Rotuines
		void 	OpenEditor();
		
		// Member variables		
		SorterType			fType;
		TSorterContainer	*fParent;
		bool				fIsActive;
};		


// Sorting Routines
int SortByName( const void *itemOne, const void *itemTwo);
int SortByDate( const void *itemOne, const void *itemTwo);
int SortByPath( const void *itemOne, const void *itemTwo);
int SortByType( const void *itemOne, const void *itemTwo);
int SortByDuration( const void *itemOne, const void *itemTwo);
int SortBySize( const void *itemOne, const void *itemTwo);
int SoryByAudioInfo( const void *itemOne, const void *itemTwo);			
int SortByVideoInfo( const void *itemOne, const void *itemTwo);

#endif
