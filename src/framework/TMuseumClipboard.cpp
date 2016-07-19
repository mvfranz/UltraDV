//---------------------------------------------------------------------
//
//	File:	TMuseumClipboard.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.05.98
//
//	Desc:	Application clipboard handling
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "AppMessages.h"
#include "TMuseumClipboard.h"
#include "TCueView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Default Constructor
//

TMuseumClipboard::TMuseumClipboard() : BClipboard("Museum", false)
{
	// Clear clipboard on start
	if ( Lock() ) {
		// Clear clipboard data
		Clear();

		// Inform clipboard to save data
		Commit();

		Unlock();
	}
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//	Default Destructor
//

TMuseumClipboard::~TMuseumClipboard()
{

}


//---------------------------------------------------------------------
//	HasData
//---------------------------------------------------------------------
//
//	Determine if there is any pertinant data for us on the clipboard
//

int16 TMuseumClipboard::HasData()
{
	int16 retVal = false;

	// Check data on clipboard
	if ( Lock() ) {
		BMessage *clipData = Data();

		// Determine type of pointer
		BMessage *cueMessage;

		if ( clipData->FindPointer( "CueList", (void **)&cueMessage) == B_OK ) {
			// Now get BList of archived cues from message on clipboard
			BList *cueList;
			if ( cueMessage->FindPointer("CueList", (void **)&cueList) == B_OK)
				retVal = kCueListData;
		}

		Unlock();
	}

	return retVal;
}



//---------------------------------------------------------------------
//	AddItem
//---------------------------------------------------------------------
//
//	Add BMessage item to the clipboard
//

bool TMuseumClipboard::AddItem( BMessage *theItem )
{
	if ( Lock() ) {
		// Clear clipboard data
		Clear();

		// Get pointer to clipboard data
		BMessage *clipData = Data();

		// Determine type of data being placed on clipboard
		switch( theItem->what)
		{
		case CUE_LIST_MSG:
		{
			clipData->AddPointer("CueList", theItem);
		}
		break;

		default:
			break;
		}

		// Inform clipboard to save data
		Commit();

		Unlock();
	}

	return true;
}


#pragma mark -
#pragma mark === Data Retrieval Functions ===

//---------------------------------------------------------------------
//	HasData
//---------------------------------------------------------------------
//
//	Determine if there is any pertinant data for us on the clipboard
//

BList *TMuseumClipboard::GetCueArchiveList()
{

	BList *cueList = NULL;

	// Get data on clipboard
	if ( Lock() ) {
		BMessage *clipData = Data();

		// Determine type of pointer
		BMessage *cueMessage;

		if ( clipData->FindPointer( "CueList", (void **)&cueMessage) == B_OK ) {
			// Now get BList of archived cues from message on clipboard
			cueMessage->FindPointer("CueList", (void **)&cueList);
		}

		Unlock();
	}

	return cueList;
}

