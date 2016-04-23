//---------------------------------------------------------------------
//
//	File:	TElementsSorter.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	Display a horizontal bar listing the attributes of the
//			files in the tracker view.  The attributes can be clicked
//			and the files are sorted according to the attribute.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>
#include <../posix/sys/stat.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TElementsView.h"
#include "TSorterContainer.h"
#include "TSorterList.h"
#include "TSorterListItem.h"
#include "TSorterIconListItem.h"

#include "TElementsSorter.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TElementsSorter::TElementsSorter(BRect bounds, TElementsView *parent) : 
	BView(bounds, "ElementsSorter", B_FOLLOW_ALL, B_WILL_DRAW)
{
	m_Parent = parent;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TElementsSorter::~TElementsSorter()
{

	// Clear out our sorter list
	if (m_SorterList)
	{
	    for (int32 index = 0; index < m_SorterList->CountItems(); index++){
	  	TSorterContainer * theSorter = static_cast<TSorterContainer *>(m_SorterList->ItemAt(index));
		    if (theSorter)
			delete theSorter;
		}
	}
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

const int16 kDefaultSorterWidth 	= 100;
const int16 kDefaultIconSorterWidth = 30;

void TElementsSorter::Init()
{
	// We don't need a background color
	SetViewColor(kWhite);
		
	// Create list to holder sorters
	m_SorterList = new BList();
	
			
	// Create our sorters.  We need several for the following attributes:
	// 	--	Icon
	//	--	Name
	//	--	Date
	//	--	Path
	//	--	Type
	//	--	Duration
	//	--	Audio Info
	//	--	Video Info
	
	// Set up ID.  Increment for each sorter we add
	int16 theID = 0;
	
	// Icon
	BRect iconRect 	= Bounds();
	iconRect.right 	= iconRect.left + kDefaultIconSorterWidth;
	m_IconSorter 	= new TSorterContainer(iconRect, "Icon", kIconSorter, theID);
	AddChild(m_IconSorter);
	m_SorterList->AddItem(m_IconSorter);	
	theID++;
	
	// Name
	BRect nameRect 	= Bounds();
	nameRect.left 	= iconRect.right + 1;
	nameRect.right 	= nameRect.left + kDefaultSorterWidth;
	m_NameSorter 	= new TSorterContainer(nameRect, "Name", kNameSorter, theID);
	AddChild(m_NameSorter);
	m_SorterList->AddItem(m_NameSorter);	
	theID++;
		
	// Date
	BRect dateRect 	= Bounds();
	dateRect.left 	= nameRect.right+1;
	dateRect.right 	= dateRect.left + kDefaultSorterWidth;
	m_DateSorter	= new TSorterContainer(dateRect, "Date", kDateSorter, theID);
	AddChild(m_DateSorter);
	m_SorterList->AddItem(m_DateSorter);
	theID++;
	
	// Path
	BRect pathRect 	= Bounds();
	pathRect.left 	= dateRect.right+1;
	pathRect.right 	= pathRect.left + kDefaultSorterWidth;
	m_PathSorter	= new TSorterContainer(pathRect, "Path", kPathSorter, theID);
	AddChild(m_PathSorter);
	m_SorterList->AddItem(m_PathSorter);
	theID++;

	// Type
	BRect typeRect 	= Bounds();
	typeRect.left 	= pathRect.right+1;
	typeRect.right 	= typeRect.left + kDefaultSorterWidth;
	m_TypeSorter	= new TSorterContainer(typeRect, "Type", kTypeSorter, theID);
	AddChild(m_TypeSorter);
	m_SorterList->AddItem(m_TypeSorter);
	theID++;
	
	// Duration
	BRect durationRect 	= Bounds();
	durationRect.left 	= typeRect.right+1;
	durationRect.right 	= durationRect.left + kDefaultSorterWidth;
	m_DurationSorter	= new TSorterContainer(durationRect, "Duration", kDurationSorter, theID);
	AddChild(m_DurationSorter);
	m_SorterList->AddItem(m_DurationSorter);
	theID++;
	
	// Size
	BRect sizeRect 		= Bounds();
	sizeRect.left 		= durationRect.right+1;
	sizeRect.right 		= sizeRect.left + kDefaultSorterWidth;
	m_SizeSorter		= new TSorterContainer(sizeRect, "Size", kSizeSorter, theID);
	AddChild(m_SizeSorter);
	m_SorterList->AddItem(m_SizeSorter);
	theID++;
		
	// Audio Info
	BRect audioRect 	= Bounds();
	audioRect.left 		= sizeRect.right+1;
	audioRect.right 	= audioRect.left + kDefaultSorterWidth;
	m_AudioInfoSorter	= new TSorterContainer(audioRect, "Audio Info", kAudioInfoSorter, theID);
	AddChild(m_AudioInfoSorter);
	m_SorterList->AddItem(m_AudioInfoSorter);
	theID++;
		
	// Video Info
	BRect videoRect 	= Bounds();
	videoRect.left 		= audioRect.right+1;
	videoRect.right 	= videoRect.left + kDefaultSorterWidth;
	m_VideoInfoSorter	= new TSorterContainer(videoRect, "Video Info", kVideoInfoSorter, theID);
	AddChild(m_VideoInfoSorter);
	m_SorterList->AddItem(m_VideoInfoSorter);
	theID++;
	
	// Set Name to be active
	m_LastSorterClicked = 1;
	m_NameSorter->MakeActive(true);
}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TElementsSorter::Draw(BRect updateRect)
{
	PushState();
	
	//  Draw dummy header to the right of our rightmost TSorter		
	TSorterContainer *sorter = static_cast<TSorterContainer *>( m_SorterList->ItemAt( m_SorterList->CountItems()-1) );
	if (sorter)
	{	
		BRect bounds 	= Bounds();
		bounds.bottom 	= bounds.top + kSorterHeight; 
		bounds.left 	= sorter->Frame().right;
		
		if (updateRect.Intersects(bounds) )
		{
			// Fill background
			SetHighColor(kBeGrey);
			FillRect(bounds);
			
			// Frame it
			BPoint endPt;	
			SetHighColor(kWhite);	
			MovePenTo(bounds.left, bounds.top+1);
			endPt.Set( bounds.right, bounds.top+1 );
			StrokeLine(endPt);
			
			SetHighColor(kMediumGrey);
			MovePenTo(bounds.left, bounds.bottom-1);
			endPt.Set( bounds.right, bounds.bottom-1 );
			StrokeLine(endPt);
			
			SetHighColor(kBlack);
			MovePenTo(bounds.left, bounds.top);
			endPt.Set( bounds.right, bounds.top );
			StrokeLine(endPt);	
			MovePenTo(bounds.left, bounds.bottom);
			endPt.Set( bounds.right, bounds.bottom );
			StrokeLine(endPt);
		}
	}
		
	PopState();
	
}

#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TElementsSorter::MessageReceived(BMessage *theMessage)
{
	switch(theMessage->what)
	{			
		// A sorter was clicked
		case SORTER_CLICK_MSG:
			HandleSorterClick(theMessage);
			break;
			
		// A sorter is being dragged
		case SORTER_RESIZE_MSG:
			HandleSorterResize(theMessage);
			// Tell parent so scroll bars can be adjusted
			m_Parent->MessageReceived(theMessage);
			break;
			
		// A sorter is being scrolled vertically
		case SORTER_SCROLL_V_MSG:
			HandleSorterScrollV(theMessage);
			break;
		
		//	We have received some file refs from a drag onto one of our sorters
		case B_SIMPLE_DATA:
			HandleRefsMessage(theMessage);
			break;
				
		// Add the single ref to our browser
		case ADD_REF_MSG:
			HandleRefMessage(theMessage);
			break;

		case SORTER_SELECT_MSG:
			HandleSorterSelect(theMessage);
			break;
			
		case SORTER_INVOKE_MSG:
			break;

		default:
			BView::MessageReceived(theMessage);
			break;	
	}
}


#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TElementsSorter::MouseDown(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TElementsSorter::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TElementsSorter::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}


#pragma mark -
#pragma mark === Keyboard Handling ===

//---------------------------------------------------------------------
//	DeleteItem
//---------------------------------------------------------------------
//
//	Remove this item from all of the sorters
//

void TElementsSorter::DeleteItem( int32 theItem )
{
	if (theItem >= 0)
	{
		// Confirm deletion with the user
		//
		//
		
		// Delete from all lists
		for (int32 index = 0; index < m_SorterList->CountItems(); index++)
		{
			TSorterContainer *theContainer = static_cast<TSorterContainer *>( m_SorterList->ItemAt(index));
			if (theContainer)
			{
				TSorterList *theList = theContainer->GetSorterList();
				if (theList)
				{
					// Remove itemView or iconView depending on type
					if ( theContainer->GetType() == kIconSorter )
					{
						TSorterIconListItem *listItem = static_cast<TSorterIconListItem *>(theList->RemoveItem(theItem));					
						if (listItem)
							delete listItem;					
					}	
					else
					{
						TSorterListItem *listItem = static_cast<TSorterListItem *>(theList->RemoveItem(theItem));					
						if (listItem)
							delete listItem;
					}						
				}			
			}			
		}
		
		// Select first item in list
		TSorterContainer *theContainer = static_cast<TSorterContainer *>( m_SorterList->ItemAt(0));
		if (theContainer)
		{
			TSorterList *theList = theContainer->GetSorterList();
			if (theList)
			{
				if ( theList->CountItems() > 0)
				{
					theList->Select(0);
				}
			}		
		}			
	}	
}


#pragma mark -
#pragma mark === Sorter Routines ===

//---------------------------------------------------------------------
//	HandleSorterClick
//---------------------------------------------------------------------
//
//	Handle a sorter being clicked
//

void TElementsSorter::HandleSorterClick(BMessage *theMessage)
{
	// Extract sorter ID from message
	int16 theID;
	
	if (theMessage->FindInt16("ID", &theID) == B_OK)
	{
		if (theID != m_LastSorterClicked)
		{		
			// Get sorter from list
			TSorterContainer *theSorter = static_cast<TSorterContainer *>( m_SorterList->ItemAt(theID) );
			if (theSorter)
			{
				// Activate new sorter
				theSorter->MakeActive(true);
				
				// Deactivate previously clicked sorter
				TSorterContainer *lastSorter = static_cast<TSorterContainer *>( m_SorterList->ItemAt(m_LastSorterClicked) );
				if (lastSorter)	
					lastSorter->MakeActive(false);
				
				// Syncronize contents of sorter lists
				SynchronizeLists(theSorter);
				
				// Save for next go around
				m_LastSorterClicked = theID;
			}
		}		
	}
}


//---------------------------------------------------------------------
//	HandleSorterResize
//---------------------------------------------------------------------
//
//	Handle a sorter being resized
//

void TElementsSorter::HandleSorterResize(BMessage *theMessage)
{
	// Extract sorter ID from message
	int16 theID;
	
	if (theMessage->FindInt16("ID", &theID) == B_OK)
	{	
		// Get sorter from list
		TSorterContainer *theSorter = static_cast<TSorterContainer *>( m_SorterList->ItemAt(theID) );
		if (theSorter)
		{
			// Get drag point
			BPoint dragPt;
			if (theMessage->FindPoint("ResizePoint", &dragPt) == B_OK)
			{				
				// Get current size				
				BRect sizeRect = theSorter->Bounds();
							
				// Resize
				sizeRect.right = dragPt.x;
				
				// Check for width violation
				if (sizeRect.Width() > kMinSorterWidth)
				{				
					// Resize the sorter based on drag
					theSorter->ResizeTo(sizeRect.Width(), sizeRect.Height());
					
					// Now scoot the other views over
					for (int32 index = theID+1; index < m_SorterList->CountItems(); index++ )
					{
						TSorterContainer *moveSorter = static_cast<TSorterContainer *>( m_SorterList->ItemAt(index) );
						if (moveSorter)
						{
							// Get previous item in list for MoveTo coordinates
							TSorterContainer *prevSorter = static_cast<TSorterContainer *>( m_SorterList->ItemAt(index-1) );
							if (prevSorter)
							{	
								moveSorter->MoveTo( prevSorter->Frame().right+1, moveSorter->Frame().top);
							}						
						}					
					}
					
				}
			}						
		}
	}
}


//---------------------------------------------------------------------
//	HandleSorterScrollV
//---------------------------------------------------------------------
//
//	Handle a sorter being scrolled vertically
//

void TElementsSorter::HandleSorterScrollV(BMessage *theMessage)
{
	// Inform all of our attached TSorterContainer objects
	for (int32 index = 0; index < m_SorterList->CountItems(); index++)
	{
		TSorterContainer *theSorter = static_cast<TSorterContainer *>( m_SorterList->ItemAt(index) );
		if (theSorter)
			theSorter->MessageReceived(theMessage);					
	}
}

//---------------------------------------------------------------------
//	HandleSorterSelect
//---------------------------------------------------------------------
//
//	Handle a sorter being selected
//

void TElementsSorter::HandleSorterSelect(BMessage *theMessage)
{
	TSorterList *theList;
	if ( theMessage->FindPointer("source", (void **)&theList) == B_OK)
	{		
		if ( theList->CurrentSelection() >= 0)
		{
			
			// Get sorter and determine who to select and deselect
			TSorterContainer *theSorter = theList->GetParent();
			if (theSorter)
			{
					// Select the name sorter of this row
					m_NameSorter->GetSorterList()->Select( theList->CurrentSelection() );
					
					// Deselect all others
					m_DateSorter->GetSorterList()->DeselectAll();
					m_PathSorter->GetSorterList()->DeselectAll();
					m_TypeSorter->GetSorterList()->DeselectAll();
					m_DurationSorter->GetSorterList()->DeselectAll();
					m_SizeSorter->GetSorterList()->DeselectAll();
					m_AudioInfoSorter->GetSorterList()->DeselectAll();
					m_VideoInfoSorter->GetSorterList()->DeselectAll();
			}
		}
	}
}

//---------------------------------------------------------------------
//	HandleSorterSelect
//---------------------------------------------------------------------
//
//	Handle a sorter being selected
//

void TElementsSorter::HandleSorterInvoke(BMessage *theMessage)
{
	/*
	TSorterList *theList;
	if ( theMessage->FindPointer("source", &theList) == B_OK)
	{		
		if ( theList->CurrentSelection() >= 0)
		{
			
			// Get sorter and determine who to select and deselect
			TSorterContainer *theSorter = theList->GetParent();
			if (theSorter)
			{
					// Select the name sorter of this row
					m_NameSorter->GetSorterList()->Select( theList->CurrentSelection() );
					
					// Deselect all others
					m_DateSorter->GetSorterList()->DeselectAll();
					m_PathSorter->GetSorterList()->DeselectAll();
					m_TypeSorter->GetSorterList()->DeselectAll();
					m_DurationSorter->GetSorterList()->DeselectAll();
					m_SizeSorter->GetSorterList()->DeselectAll();
					m_AudioInfoSorter->GetSorterList()->DeselectAll();
					m_VideoInfoSorter->GetSorterList()->DeselectAll();
			}
		}
	}
	*/
}

#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	GetScrollArea
//---------------------------------------------------------------------
//
//	Return the total scroll area of our Sorter
//

BRect TElementsSorter::GetScrollArea()
{
	BRect firstFrame(0,0,0,0);
	BRect lastFrame(0,0,0,0);
	BRect bounds = Bounds();
	
	// Get first sorter in list
	TSorterContainer *firstSorter = static_cast<TSorterContainer *>( m_SorterList->FirstItem() );
	firstFrame = firstSorter->Frame();
	
	// Get last sorter in list
	TSorterContainer *lastSorter = static_cast<TSorterContainer *>( m_SorterList->ItemAt( m_SorterList->CountItems() - 1) );
	if (lastSorter)
		lastFrame = lastSorter->Frame();		

	bounds.left 	= firstFrame.left;
	bounds.right 	= lastFrame.right;
	
	return bounds;
}


//---------------------------------------------------------------------
//	SynchronizeLists
//---------------------------------------------------------------------
//
//	Synchronize the contents of the lists based on list item IDs
//

void TElementsSorter::SynchronizeLists(TSorterContainer *syncSource)
{
	// use syncSource as the list that all others are sorted against
	for (int32 index = 0; index < m_SorterList->CountItems(); index++)
	{
		TSorterContainer *theContainer = static_cast<TSorterContainer *>(m_SorterList->ItemAt(index));
		if (theContainer)
		{
			// Don't sync against ourself
			if (theContainer != syncSource)
			{
				theContainer->Synchronize(syncSource);			
			}		
		}	
	}		
}

#pragma mark -
#pragma mark === Ref Handling ===


//---------------------------------------------------------------------
//	HandleRefsMessage
//---------------------------------------------------------------------
//
//	Handle files dragged into window
//

void TElementsSorter::HandleRefsMessage(BMessage *theMessage)
{
	
	uint32 		theType; 
	int32 		theCount; 
    entry_ref 	theRef;	

	theMessage->GetInfo("refs", &theType, &theCount); 
       
	if ( theType != B_REF_TYPE ) 
		return;
		
	for ( int32 i = --theCount; i >= 0; i-- ) 
	{ 
		if ( theMessage->FindRef("refs", i, &theRef) == B_OK ) 
		{ 			
			// Evaluate the ref and determine if we can deal with it
			// Currently we are only dealing with files
			EvaluateRef(theRef);
		} 
	} 
}


//---------------------------------------------------------------------
//	HandleRefMessage
//---------------------------------------------------------------------
//
//	Handle a single ref that has been sent to us in a message
//

void TElementsSorter::HandleRefMessage(BMessage *theMessage)
{
	
    entry_ref 	theRef;	

	if ( theMessage->FindRef("FileRef", &theRef) == B_OK ) 
	{ 			
		// Evaluate the ref and determine if we can deal with it
		// Currently we are only dealing with files
		EvaluateRef(theRef);
	} 
}


//---------------------------------------------------------------------
//	EvaluateRef
//---------------------------------------------------------------------
//
//	Check ref and see if it is a type we can handle
//

status_t TElementsSorter::EvaluateRef(entry_ref &ref) 
{ 
	struct stat st; 
	BEntry 		entry; 
	
	// Can we create a BEntry?
	if (entry.SetTo(&ref, false) != B_OK) 
		return B_ERROR; 
		
	// Can we get a BStatable?
	if (entry.GetStat(&st) != B_OK) 
		return B_ERROR; 
		
	// Is it a SymLink?
	if (S_ISLNK(st.st_mode)) 
		return HandleLink(ref, st);
	// How about a File?
	else if (S_ISREG(st.st_mode)) 
		return HandleFile(ref, st); 
	// A Directory?
	else if (S_ISDIR(st.st_mode)) 
	{ 
		BDirectory dir; 
		if (dir.SetTo(&ref) != B_OK) 
			return B_ERROR; 
			
		if (dir.IsRootDirectory()) 
			return HandleVolume(ref, st, dir); 
		else 
			return HandleDirectory(ref, st, dir); 
	} 
	
	// No luck
	return B_ERROR;
} 


//---------------------------------------------------------------------
//	HandleFile
//---------------------------------------------------------------------
//
//

status_t TElementsSorter::HandleLink(entry_ref &theRef, struct stat &st) 
{ 
	
	// Resolve possible symlink...
	BEntry entry(&theRef, true);
	if ( entry.InitCheck() == B_OK )
	{
		entry.GetRef(&theRef);
		
		return HandleFile(theRef, st);
	}
	
	return B_ERROR;
}


//---------------------------------------------------------------------
//	HandleFile
//---------------------------------------------------------------------
//
//

status_t TElementsSorter::HandleFile(entry_ref &theRef, struct stat &st) 
{ 
	{
		BFile theFile; 
		if ( theFile.SetTo(&theRef, B_READ_WRITE) == B_OK )
		{									
			BNodeInfo nodeInfo(&theFile);
			if (nodeInfo.InitCheck() == B_NO_ERROR)
			{
				if ( IsSupportedType(nodeInfo) )
				{
					// 	Pass the node info to all of our sorters.  They will 
					//	know what to do with it.
					BMessage *refMessage = new BMessage(SORTER_REFS_MSG);
					refMessage->AddRef("FileRef", &theRef);
				
					for (int32 index = 0; index < m_SorterList->CountItems(); index++)
					{
						TSorterContainer *sorter = static_cast<TSorterContainer *>(m_SorterList->ItemAt(index));
						if (sorter)
							sorter->MessageReceived(refMessage);					
					}
					
					delete refMessage;
					
					// Get active list, sort and sync
					for (int32 sortIndex = 0; sortIndex < m_SorterList->CountItems(); sortIndex++)
					{ 
						TSorterContainer *theContainer = static_cast<TSorterContainer *>(m_SorterList->ItemAt(sortIndex));			
						if (theContainer)
						{
							if (theContainer->IsActive())
							{
								theContainer->Sort();
								SynchronizeLists(theContainer);
								break;
							}						
						}
					}
									
					return B_NO_ERROR;
				}
				return B_ERROR;				
			}
		}				
	}

	return B_ERROR;
}


//---------------------------------------------------------------------
//	HandleDirectory
//---------------------------------------------------------------------
//
//

status_t TElementsSorter::HandleDirectory(entry_ref &ref, struct stat &st, BDirectory &dir) 
{ 
	return B_ERROR;
}


//---------------------------------------------------------------------
//	HandleVolume
//---------------------------------------------------------------------
//
//

status_t TElementsSorter::HandleVolume(entry_ref &ref, struct stat &st, BDirectory &dir) 
{ 
	return B_ERROR;
	
	/*
	BVolumeRoster vol_roster; 
	BVolume       vol; 
	BDirectory    root_dir; 
	dev_t         device; 
	
	while (vol_roster.GetNextVolume(&vol) == B_NO_ERROR) 
	{ 
		vol.GetRootDirectory(&root_dir); 
		
		if (root_dir == dir) 
	  		break; 
	} 
    */

} 
