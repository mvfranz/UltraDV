//---------------------------------------------------------------------
//
//	File:	TSorterList.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	A SorterList is single vertical display of an element.  It has
//			a toolbar at the top that can be clicked, resized and moved.
//			These actions send a message to all other Sorters in the
//			SorterContainer.
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "DebugUtils.h"
#include "ResourceManager.h"

#include "TElementsSorter.h"
#include "TSorterContainer.h"
#include "TSorterListItem.h"
#include "TSorterIconListItem.h"

#include "TSorterList.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TSorterList::TSorterList(BRect bounds, TSorterContainer* parent, SorterType theType) : BListView(bounds, "SorterList", B_SINGLE_SELECTION_LIST, B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP_BOTTOM)
{
	fParent         = parent;
	fType           = theType;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TSorterList::~TSorterList()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TSorterList::Init()
{
	SetViewColor(kWhite);

	// Set up font and size
	SetFont(be_plain_font);
	SetFontSize(kSorterFontSize);

	// We are inactive
	fIsActive = false;
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Handle messages
//

void TSorterList::MessageReceived(BMessage* theMessage)
{
	switch(theMessage->what)
	{
	case SORTER_REFS_MSG:
		HandleRefsMessage(theMessage);
		break;

	default:
		BListView::MessageReceived(theMessage);
		break;
	}
}

#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TSorterList::Draw(BRect updateRect)
{

	//PushState();

	//PopState();

	BListView::Draw(updateRect);
}

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TSorterList::MouseDown(BPoint where)
{
	// Check to see which button is down
	uint32 buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (long*)&buttons);

	// Is this a double click?  If so, open editor
	uint32 type;
	int32 count = 0;
	BMessage* message = Window()->CurrentMessage();

	// Determine which button has been clicked
	switch(buttons)
	{
	case B_PRIMARY_MOUSE_BUTTON:
		if (B_OK == message->GetInfo("clicks", &type, &count) ) {
			int32 clickCount = message->FindInt32("clicks", count-1);
			if (clickCount == 2) {
				// Open editor for item type
				OpenEditor();
			} else {
				// If shift key is down the user is group selecting/deselecting
				if (IsShiftKeyDown()) {
				}
				// Otherwise, select the item
				else{
					BListView::MouseDown(where);
				}
			}
		}
		break;

	// If the second mouse button is down, open cue menu
	case B_SECONDARY_MOUSE_BUTTON:
		break;

	default:
		break;

	}
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TSorterList::MouseUp(BPoint where)
{
	BListView::MouseUp(where);
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TSorterList::MouseMoved( BPoint where, uint32 code, const BMessage* a_message )
{
	BListView::MouseMoved(where, code, a_message);
}


#pragma mark -
#pragma mark === Keyboard Handling ===

//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Check and see if an item is selected.  If so, inform parent and
//      it will handle removal of the item from the list.
//

void TSorterList::KeyDown(const char* bytes, int32 numBytes)
{
	if (numBytes == 1) {
		switch(bytes[0])
		{
		// Delete/Backspace key
		case B_DELETE:
		case B_BACKSPACE:
		{
			if ( CurrentSelection() >= 0)
				fParent->DeleteItem( CurrentSelection() );
		}
		break;

		default:
			BListView::KeyDown(bytes, numBytes);
			break;
		}
	}
}


#pragma mark -
#pragma mark === Dragging Routines ===

//---------------------------------------------------------------------
//	InitiateDrag
//---------------------------------------------------------------------
//
//

bool TSorterList::InitiateDrag(BPoint point, int32 index, bool wasSelected)
{
	// Exit if item not selected
	if (wasSelected == false)
		return false;

	// Select the item
	//Select(index);

	TSorterListItem* theItem = static_cast<TSorterListItem*>(ItemAt(index));
	if (theItem) {
		if (theItem->GetType() != kIconSorter) {
			//      Get bitmap based on item type.  Get this from the IconSorter, which is the
			//	first sorter in our SorterList
			BList* sorterList =  static_cast<TElementsSorter*>(fParent->Parent())->GetSorterList();
			if (sorterList) {
				TSorterContainer* theContainer =  static_cast<TSorterContainer*>(sorterList->ItemAt(0));
				if (theContainer) {
					TSorterList* iconList = theContainer->GetSorterList();
					if (iconList) {
						TSorterIconListItem* iconItem = static_cast<TSorterIconListItem*>(iconList->ItemAt(index));
						if (iconItem) {
							// Create a copy of the large icon for dragging
							BBitmap* iconBitmap = iconItem->GetLargeIcon();
							BBitmap* dragBitmap = new BBitmap( iconBitmap->Bounds(), iconBitmap->ColorSpace() );
							dragBitmap->SetBits( iconBitmap->Bits(), iconBitmap->BitsLength(), 0, iconBitmap->ColorSpace() );

							// Center mouse over icon
							BPoint centerPt( iconBitmap->Bounds().Width() / 2, iconBitmap->Bounds().Height() / 2 );

							// Create drag message
							BMessage message(BROWSER_ICON_DRAG_MSG);
							message.AddRef("FileRef", theItem->GetEntryRef());

							DragMessage(&message, dragBitmap, centerPt);

							return true;
						}
					}
				}
			}
		}
		// This is the icon sorter.  It is easier for us to get the drag bitmap icon
		else{
			TSorterIconListItem* iconItem = static_cast<TSorterIconListItem*>(ItemAt(index));
			if (iconItem) {
				// Create a copy of the large icon for dragging
				BBitmap* iconBitmap = iconItem->GetLargeIcon();
				BBitmap* dragBitmap = new BBitmap( iconBitmap->Bounds(), iconBitmap->ColorSpace() );
				dragBitmap->SetBits( iconBitmap->Bits(), iconBitmap->BitsLength(), 0, iconBitmap->ColorSpace() );

				// Center mouse over icon
				BPoint centerPt( iconBitmap->Bounds().Width() / 2, iconBitmap->Bounds().Height() / 2 );

				// Create drag message
				BMessage message(BROWSER_ICON_DRAG_MSG);
				message.AddRef("FileRef", theItem->GetEntryRef());

				DragMessage(&message, dragBitmap, centerPt);

				return true;
			}
		}
	}
	return false;
}


#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	MakeActive
//---------------------------------------------------------------------
//
//

void TSorterList::MakeActive(bool theState)
{
	fIsActive = theState;

	// Force redraw
	Invalidate();
}


//---------------------------------------------------------------------
//	SetListItemDimensions
//---------------------------------------------------------------------
//
//	Set up out defualt list item dimensions
//

void TSorterList::SetListItemDimensions(TSorterListItem* listItem, BRect theRect)
{
	// Set width and height
	listItem->SetHeight(theRect.Height());
	listItem->SetWidth(theRect.Width());

	//      This is a cheap hack to get the height of the listItem set
	//	properly.  We aren't actually using a font this large.
	BFont theFont;
	GetFont(&theFont);
	theFont.SetSize(theRect.Height());
	listItem->Update(this, &theFont);
}

//---------------------------------------------------------------------
//	SetListItemIconDimensions
//---------------------------------------------------------------------
//
//	Set up our default icon list item dimensions
//

void TSorterList::SetListItemIconDimensions(TSorterIconListItem* listItem, BRect theRect)
{
	// Set width and height
	listItem->SetHeight(theRect.Height());
	listItem->SetWidth(theRect.Width());

	//      This is a cheap hack to get the height of the listItem set
	//	properly.  We aren't actually using a font this large.
	BFont theFont;
	GetFont(&theFont);
	theFont.SetSize(theRect.Height());
	listItem->Update(this, &theFont);
}

//---------------------------------------------------------------------
//	GetOccurences
//---------------------------------------------------------------------
//
//	Return the number of times this file is already in the list.
//	Do not use this function with an icon list.
//

int32 TSorterList::GetOccurences(entry_ref &fileRef)
{
	int32 numItems = 0;

	for( int32 index = 0; index < CountItems(); index++) {
		TSorterListItem* theItem = static_cast<TSorterListItem*>(ItemAt(index) );
		if (theItem) {
			if (fileRef == *theItem->GetEntryRef() )
				numItems++;
		}
	}

	return numItems;
}


//---------------------------------------------------------------------
//	Synchronize
//---------------------------------------------------------------------
//
//	Sync to source based on list item IDs.
//	gzr: to do ...
//		There is some skanky casting going on here to cast a
//		TSorterIconListItem into a TSorterListItem.
//

void TSorterList::Synchronize( TSorterList* syncSource)
{

	// Step through each item in the syncSource
	for (int32 syncIndex = 0; syncIndex < syncSource->CountItems(); syncIndex++) {
		TSorterListItem* syncItem = (TSorterListItem*)syncSource->ItemAt(syncIndex);
		if (syncItem) {
			// Find a match in the list
			for (int32 matchIndex = 0; matchIndex < CountItems(); matchIndex++) {
				TSorterListItem* matchItem = (TSorterListItem*)ItemAt(matchIndex);
				if (matchItem) {
					entry_ref matchRef, syncRef;

					matchRef        = *matchItem->GetEntryRef();
					syncRef         = *syncItem->GetEntryRef();

					//      Do we have a match?  If so reorder the list to align
					//	the entries.
					if (matchRef == syncRef) {
						RemoveItem(matchItem);
						AddItem(matchItem, syncIndex);
						// Increment our way out of the loop
						matchIndex = CountItems();
					}
				}
			}
		}
	}
}

#pragma mark -
#pragma mark === Data List Handling ===

//---------------------------------------------------------------------
//	HandleRefsMessage
//---------------------------------------------------------------------
//
//	Handle files dragged into window
//

void TSorterList::HandleRefsMessage(BMessage* theMessage)
{
	entry_ref theRef;
	if ( theMessage->FindRef("FileRef", &theRef ) == B_OK) {
		BFile theFile;
		if ( theFile.SetTo(&theRef, B_READ_WRITE) == B_OK ) {
			// Create BEntry
			BEntry theEntry(&theRef, true);

			// Create node
			BNodeInfo nodeInfo(&theFile);
			if (nodeInfo.InitCheck() == B_NO_ERROR) {
				// Get info based on sorter type
				switch (fType)
				{
				case kIconSorter:
					AddIconsToDataList(&theEntry, nodeInfo);
					break;

				case kNameSorter:
					AddNameToDataList(&theEntry);
					break;

				case kDateSorter:
					AddDateToDataList(&theEntry);
					break;

				case kPathSorter:
					AddPathToDataList(&theEntry);
					break;

				case kTypeSorter:
					AddTypeToDataList(&theEntry, &nodeInfo);
					break;

				case kDurationSorter:
					AddDurationToDataList(&theEntry);
					break;

				case kSizeSorter:
					AddSizeToDataList(&theEntry);
					break;

				case kAudioInfoSorter:
					AddAudioInfoToDataList(&theEntry);
					break;

				case kVideoInfoSorter:
					AddVideoInfoToDataList(&theEntry);
					break;

				// Default to name
				default:
					AddNameToDataList(&theEntry);
					break;
				}
			}
		}
	}
}


//---------------------------------------------------------------------
//	AddNameToDataList
//---------------------------------------------------------------------
//
//	Handle files dragged into window
//

void TSorterList::AddNameToDataList(BEntry* theEntry)
{
	// Get an entry_ref for ListItem constructor
	entry_ref fileRef;
	theEntry->GetRef(&fileRef);

	// Setup rect
	BRect textRect  = Bounds();
	textRect.bottom = textRect.top + kSorterFontSize + 6;

	//      Determine the number of occurences of this file in the list.
	//      We do this to set the index number by the file name
	int32 numItems = GetOccurences(fileRef);
	numItems++;
	char itemStr[256];
	sprintf(itemStr, " [%d]", numItems);

	// Get file name
	char theName[B_FILE_NAME_LENGTH];
	theEntry->GetName(theName);

	// Add the number of items to the file name
	strcat(theName, itemStr);

	// Create our TSorterListItem
	float width = StringWidth(theName);
	textRect.right = textRect.left + width;
	TSorterListItem* listItem = new TSorterListItem(0, true, textRect, theName, kNameSorter, fileRef);
	AddItem(listItem);

	// Set up default size
	SetListItemDimensions(listItem, textRect);

	// Force it to draw
	Invalidate();
}


//---------------------------------------------------------------------
//	AddPathToDataList
//---------------------------------------------------------------------
//
//	Handle files dragged into window
//

void TSorterList::AddPathToDataList(BEntry* theEntry)
{
	// Get an entry_ref for ListItem constructor
	entry_ref fileRef;
	theEntry->GetRef(&fileRef);

	// Setup rect
	BRect textRect  = Bounds();
	textRect.bottom = textRect.top + kSorterFontSize + 6;

	// Create our TSorterListItem
	BPath thePath;
	theEntry->GetPath(&thePath);
	float width = StringWidth(thePath.Path());
	textRect.right = textRect.left + width;
	TSorterListItem* listItem = new TSorterListItem(0, true, textRect, thePath.Path(), kPathSorter, fileRef);
	AddItem(listItem);

	// Set up default size
	SetListItemDimensions(listItem, textRect);

	// Force it to draw
	Invalidate();
}


//---------------------------------------------------------------------
//	AddSizeToDataList
//---------------------------------------------------------------------
//
//	Get file size and it to the list
//

void TSorterList::AddSizeToDataList(BEntry* theEntry)
{
	// Get an entry_ref for ListItem constructor
	entry_ref fileRef;
	theEntry->GetRef(&fileRef);

	// Setup rect
	BRect textRect  = Bounds();
	textRect.bottom = textRect.top + kSorterFontSize + 6;

	// Create our TSorterListItem
	off_t theSize;
	theEntry->GetSize(&theSize);

	int32 kBytes = theSize / 1024;
	char sizeStr[65];
	sprintf(sizeStr, "%dK", kBytes);
	float width = StringWidth(sizeStr);
	textRect.right = textRect.left + width;
	TSorterListItem* listItem = new TSorterListItem(0, true, textRect, sizeStr, kSizeSorter, fileRef);
	AddItem(listItem);

	// Set up default size
	SetListItemDimensions(listItem, textRect);

	// Force it to draw
	Invalidate();
}


//---------------------------------------------------------------------
//	AddDateToDataList
//---------------------------------------------------------------------
//
//	Get file modification date and it to the list
//

void TSorterList::AddDateToDataList(BEntry* theEntry)
{
	// Get an entry_ref for ListItem constructor
	entry_ref fileRef;
	theEntry->GetRef(&fileRef);

	// Setup rect
	BRect textRect  = Bounds();
	textRect.bottom = textRect.top + kSorterFontSize + 6;

	// Create our TSorterListItem
	time_t theTime;
	struct  tm* localTime;
	char timeStr[255];

	theEntry->GetModificationTime(&theTime);

	localTime = localtime(&theTime);

	strftime(timeStr, sizeof(timeStr), "%A, %B %d, %Y, %H:%M:%S %p", localTime);

	float width = StringWidth(timeStr);
	textRect.right = textRect.left + width;
	TSorterListItem* listItem = new TSorterListItem(0, true, textRect, timeStr, kDateSorter, fileRef);
	AddItem(listItem);

	// Set up default size
	SetListItemDimensions(listItem, textRect);

	// Force it to draw
	Invalidate();
}


//---------------------------------------------------------------------
//	AddTypeToDataList
//---------------------------------------------------------------------
//
//	Get file type and add it to the list
//

void TSorterList::AddTypeToDataList(BEntry* theEntry, BNodeInfo* nodeInfo)
{

	// Get an entry_ref for ListItem constructor
	entry_ref fileRef;
	theEntry->GetRef(&fileRef);

	TSorterListItem* listItem;
	float width;

	// Setup rect
	BRect textRect  = Bounds();
	textRect.bottom = textRect.top + kSorterFontSize + 6;

	// Create our TSorterListItem
	char mimeStr[B_MIME_TYPE_LENGTH];
	if ( nodeInfo->GetType(mimeStr) != B_OK)
		strcpy(mimeStr, "unknown");

	/*
	   // Create a BMimeType to analyze to mime string
	   BMimeType mimeType(mimeStr);
	   if (mimeType.InitCheck() == B_NO_ERROR)
	   {
	        // Get the type description
	        char descStr[B_MIME_TYPE_LENGTH];
	        mimeType.GetShortDescription(descStr);
	        width = StringWidth(descStr);
	        textRect.right = textRect.left + width;
	        listItem = new TSorterListItem(0, true, textRect, descStr, kTypeSorter);
	   }
	   else
	   {
	        width = StringWidth(mimeStr);
	        textRect.right = textRect.left + width;
	        listItem = new TSorterListItem(0, true, textRect, mimeStr, kTypeSorter);
	   }
	 */

	width = StringWidth(mimeStr);
	textRect.right = textRect.left + width;
	listItem = new TSorterListItem(0, true, textRect, mimeStr, kTypeSorter, fileRef);
	AddItem(listItem);

	// Set up default size
	SetListItemDimensions(listItem, textRect);

	// Force it to draw
	Invalidate();
}


//---------------------------------------------------------------------
//	AddDurationToDataList
//---------------------------------------------------------------------
//
//	Get file duration and add it to the list
//

void TSorterList::AddDurationToDataList(BEntry* theEntry)
{
	// Get an entry_ref for ListItem constructor
	entry_ref fileRef;
	theEntry->GetRef(&fileRef);

	// Setup rect
	BRect textRect  = Bounds();
	textRect.bottom = textRect.top + kSorterFontSize + 6;

	// Create our TSorterListItem
	char mimeStr[2];
	strcpy(mimeStr, "-");

	float width = StringWidth(mimeStr);
	textRect.right = textRect.left + width;
	TSorterListItem* listItem = new TSorterListItem(0, true, textRect, mimeStr, kDurationSorter, fileRef);
	AddItem(listItem);

	// Set up default size
	SetListItemDimensions(listItem, textRect);

	// Force it to draw
	Invalidate();
}


//---------------------------------------------------------------------
//	AddAudioInfoToDataList
//---------------------------------------------------------------------
//
//	Get audio info and add it to the list
//

void TSorterList::AddAudioInfoToDataList(BEntry* theEntry)
{
	// Get an entry_ref for ListItem constructor
	entry_ref fileRef;
	theEntry->GetRef(&fileRef);

	// Setup rect
	BRect textRect  = Bounds();
	textRect.bottom = textRect.top + kSorterFontSize + 6;

	// Create our TSorterListItem
	char mimeStr[2];
	strcpy(mimeStr, "-");

	float width = StringWidth(mimeStr);
	textRect.right = textRect.left + width;
	TSorterListItem* listItem = new TSorterListItem(0, true, textRect, mimeStr, kAudioInfoSorter, fileRef);
	AddItem(listItem);

	// Set up default size
	SetListItemDimensions(listItem, textRect);

	// Force it to draw
	Invalidate();
}


//---------------------------------------------------------------------
//	AddVideoInfoToDataList
//---------------------------------------------------------------------
//
//	Get video info and add it to the list
//

void TSorterList::AddVideoInfoToDataList(BEntry* theEntry)
{
	// Get an entry_ref for ListItem constructor
	entry_ref fileRef;
	theEntry->GetRef(&fileRef);

	// Setup rect
	BRect textRect  = Bounds();
	textRect.bottom = textRect.top + kSorterFontSize + 6;

	// Create our TSorterListItem
	char mimeStr[2];
	strcpy(mimeStr, "-");

	float width = StringWidth(mimeStr);
	textRect.right = textRect.left + width;
	TSorterListItem* listItem = new TSorterListItem(0, true, textRect, mimeStr, kVideoInfoSorter, fileRef);
	AddItem(listItem);

	// Set up default size
	SetListItemDimensions(listItem, textRect);

	// Force it to draw
	Invalidate();
}


//---------------------------------------------------------------------
//	AddIconsToDataList
//---------------------------------------------------------------------
//
//	Get icons and add tem to the list
//

void TSorterList::AddIconsToDataList(BEntry* theEntry, BNodeInfo &nodeInfo)
{
	TSorterIcons sorterIcons;

	// Get an entry_ref for ListItem constructor
	entry_ref fileRef;
	theEntry->GetRef(&fileRef);

	// Load icons based upon file type
	if (IsAudio(nodeInfo)) {
		sorterIcons.fMiniIcon   = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fAudioIcon16;
		sorterIcons.fLargeIcon  = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fAudioIcon32;
	} else if (IsImage(nodeInfo)) {
		sorterIcons.fMiniIcon   = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fPictureIcon16;
		sorterIcons.fLargeIcon  = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fPictureIcon32;
	} else if (IsText(nodeInfo)) {
		sorterIcons.fMiniIcon   = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fTextIcon16;
		sorterIcons.fLargeIcon  = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fTextIcon32;
	} else if (IsVideo(nodeInfo)) {
		sorterIcons.fMiniIcon   = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fMovieIcon16;
		sorterIcons.fLargeIcon  = static_cast<MuseumApp*>(be_app)->fMuseumIcons->fMovieIcon32;
	}

	// Setup rect
	BRect textRect  = Bounds();
	textRect.bottom = textRect.top + kSorterFontSize + 6;

	// Create our TSorterIconListItem
	TSorterIconListItem* listItem = new TSorterIconListItem(0, true, textRect, sorterIcons, fileRef);
	AddItem(listItem);

	// Set up default size
	SetListItemIconDimensions(listItem, textRect);

	// Force it to draw
	Invalidate();
}


#pragma mark -
#pragma mark === Sorting Routines ===

//---------------------------------------------------------------------
//	Sort
//---------------------------------------------------------------------
//
//	Sort the list according to its type
//

void TSorterList::Sort()
{
	switch (fType)
	{
	case kNameSorter:
		SortItems( *SortByName );
		break;

	case kDateSorter:
		SortItems( *SortByDate);
		break;

	case kPathSorter:
		SortItems( *SortByPath);
		break;

	case kTypeSorter:
		SortItems( *SortByType);
		break;

	case kDurationSorter:
		SortItems( *SortByDuration);
		break;

	case kSizeSorter:
		SortItems( *SortBySize);
		break;

	case kAudioInfoSorter:
		SortItems( *SoryByAudioInfo);
		break;

	case kVideoInfoSorter:
		SortItems( *SortByVideoInfo);
		break;

	case kIconSorter:
		break;

	default:
		break;

	}
}

//---------------------------------------------------------------------
//	SortByName
//---------------------------------------------------------------------
//
//	Sort the list according to its name
//

int SortByName( const void* itemOne, const void* itemTwo)
{
	TSorterListItem** firstItem  = (TSorterListItem**)(itemOne);
	TSorterListItem** secondItem = (TSorterListItem**)(itemTwo);

	char* strOne = (*firstItem)->GetDataString();
	char* strTwo = (*secondItem)->GetDataString();

	// Create proxy strings for lower case conversion
	int16 strLenOne = strlen((*firstItem)->GetDataString());
	int16 strLenTwo = strlen((*secondItem)->GetDataString());

	char* lowerStrOne = new char[strLenOne];
	char* lowerStrTwo = new char[strLenTwo];

	// Convert strings
	for (int16 index = 0; index < strLenOne; index++)
		lowerStrOne[index] = tolower(strOne[index]);

	for (int16 index = 0; index < strLenTwo; index++)
		lowerStrTwo[index] = tolower(strTwo[index]);

	//int retVal = strcmp( (*firstItem)->GetDataString(), (*secondItem)->GetDataString() );
	int retVal = strcmp( lowerStrOne, lowerStrTwo);

	delete lowerStrOne;
	delete lowerStrTwo;

	if ( retVal < 0)
		return -1;
	else if (retVal > 0)
		return 1;
	else
		return 0;
}

//---------------------------------------------------------------------
//	SortByDate
//---------------------------------------------------------------------
//
//	Sort the list according to its date
//

int SortByDate( const void* itemOne, const void* itemTwo)
{
	TSorterListItem** firstItem  = (TSorterListItem**)(itemOne);
	TSorterListItem** secondItem = (TSorterListItem**)(itemTwo);

	char* sizeStrOne = (*firstItem)->GetDataString();
	char* sizeStrTwo = (*secondItem)->GetDataString();

	int sizeOne = atoi(sizeStrOne);
	int sizeTwo = atoi(sizeStrTwo);

	if (sizeOne < sizeTwo)
		return -1;
	else if (sizeOne > sizeTwo)
		return 1;
	else
		return 0;
}


//---------------------------------------------------------------------
//	SortByPath
//---------------------------------------------------------------------
//
//	Sort the list according to its path
//

int SortByPath( const void* itemOne, const void* itemTwo)
{
	TSorterListItem** firstItem  = (TSorterListItem**)(itemOne);
	TSorterListItem** secondItem = (TSorterListItem**)(itemTwo);

	int retVal = strcmp( (*firstItem)->GetDataString(), (*secondItem)->GetDataString() );

	if ( retVal < 0)
		return -1;
	else if (retVal > 0)
		return 1;
	else
		return 0;
}



//---------------------------------------------------------------------
//	SortByType
//---------------------------------------------------------------------
//
//	Sort the list according to its type
//

int SortByType( const void* itemOne, const void* itemTwo)
{
	TSorterListItem** firstItem  = (TSorterListItem**)(itemOne);
	TSorterListItem** secondItem = (TSorterListItem**)(itemTwo);

	int retVal = strcmp( (*firstItem)->GetDataString(), (*secondItem)->GetDataString() );

	if ( retVal < 0)
		return -1;
	else if (retVal > 0)
		return 1;
	else
		return 0;
}


//---------------------------------------------------------------------
//	SortByDuration
//---------------------------------------------------------------------
//
//	Sort the list according to its time duration
//

int SortByDuration( const void* itemOne, const void* itemTwo)
{
	TSorterListItem** firstItem  = (TSorterListItem**)(itemOne);
	TSorterListItem** secondItem = (TSorterListItem**)(itemTwo);

	int retVal = strcmp( (*firstItem)->GetDataString(), (*secondItem)->GetDataString() );

	if ( retVal < 0)
		return -1;
	else if (retVal > 0)
		return 1;
	else
		return 0;
}



//---------------------------------------------------------------------
//	SortBySize
//---------------------------------------------------------------------
//
//	Sort the list according to its file size
//

int SortBySize( const void* itemOne, const void* itemTwo)
{
	TSorterListItem** firstItem  = (TSorterListItem**)(itemOne);
	TSorterListItem** secondItem = (TSorterListItem**)(itemTwo);

	char* sizeStrOne = (*firstItem)->GetDataString();
	char* sizeStrTwo = (*secondItem)->GetDataString();

	int sizeOne = atoi(sizeStrOne);
	int sizeTwo = atoi(sizeStrTwo);

	if (sizeOne > sizeTwo)
		return -1;
	else if (sizeOne < sizeTwo)
		return 1;
	else
		return 0;
}


//---------------------------------------------------------------------
//	SoryByAudioInfo
//---------------------------------------------------------------------
//
//	Sort the list according to its audio length
//

int SoryByAudioInfo( const void* itemOne, const void* itemTwo)
{
	TSorterListItem** firstItem  = (TSorterListItem**)(itemOne);
	TSorterListItem** secondItem = (TSorterListItem**)(itemTwo);

	int retVal = strcmp( (*firstItem)->GetDataString(), (*secondItem)->GetDataString() );

	if ( retVal < 0)
		return -1;
	else if (retVal > 0)
		return 1;
	else
		return 0;
}


//---------------------------------------------------------------------
//	SortByVideoInfo
//---------------------------------------------------------------------
//
//	Sort the list according to its video frame size
//

int SortByVideoInfo( const void* itemOne, const void* itemTwo)
{
	TSorterListItem** firstItem  = (TSorterListItem**)(itemOne);
	TSorterListItem** secondItem = (TSorterListItem**)(itemTwo);

	int retVal = strcmp( (*firstItem)->GetDataString(), (*secondItem)->GetDataString() );

	if ( retVal < 0)
		return -1;
	else if (retVal > 0)
		return 1;
	else
		return 0;
}


#pragma mark -
#pragma mark === Editing Routines ===

//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open item editor based on content type
//

void TSorterList::OpenEditor()
{
	// Get data type
	TSorterListItem* theItem = static_cast<TSorterListItem*>(ItemAt( CurrentSelection()) );
	if (theItem) {
		switch(theItem->GetDataType())
		{
		case kAudioType:
			//DebugAlert("AudioType");
			break;

		case kPictureType:
			//DebugAlert("PictureType");
			break;

		case kTextType:
			//DebugAlert("TextType");
			break;

		case kVideoType:
			//DebugAlert("VideoType");
			break;

		default:
			break;
		}
	}


}