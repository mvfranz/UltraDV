//---------------------------------------------------------------------
//
//	File:	TCueSheetView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Cue sheet view
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>
#include <assert.h>

#include "AppAlerts.h"
#include "AppConstants.h"
#include "AppMessages.h"
#include "AppTypes.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"
#include "MenuMessages.h"

#include "TCueSheetWindow.h"
#include "TCueView.h"
#include "TCueChannel.h"
#include "TCueChannelHeader.h"
#include "TTimelineView.h"
#include "TMuteButton.h"
#include "TSoloButton.h"
#include "TStageWindow.h"
#include "TStageView.h"
#include "TTimePalette.h"
#include "TTimePaletteView.h"
#include "TMuseumClipboard.h"
#include "THeaderContainerView.h"
#include "TPasteCues.h"

#include "TCueSheetView.h"



// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Default Constructor
//

TCueSheetView::TCueSheetView(BRect bounds, TCueSheetWindow* parent) :
	BView(bounds, "CueSheetView", B_FOLLOW_ALL, B_WILL_DRAW|B_DRAW_ON_CHILDREN)
{
	fParent = parent;
	ASSERT(parent);

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from message
//

TCueSheetView::TCueSheetView(BMessage* data) : BView(data)
{
	//
	// Rehydrate the cue from message data
	//

	fParent = NULL;

	ssize_t numBytes;
	rgb_color* color;
	timecode_type* timeCode;

	data->FindData("StageColor", B_RGB_COLOR_TYPE, (const void**)&color, &numBytes );
	fStageColor = *color;

	data->FindData("TimeFormat", B_ANY_TYPE, (const void**)&timeCode, &numBytes );
	fTimeFormat = *timeCode;

	// Extract our member variables from the archive
	data->FindInt16("Resolution", &fResolution);

	fStartTime      = data->FindInt32("StartTime");
	fDuration               = data->FindInt32("Duration");
	fCurrentTime    = data->FindInt32("CurrentTime");
	fExportStartTime = data->FindInt32("ExportStartTime");
	fExportStopTime  = data->FindInt32("ExportStopTime");

	data->FindBool("IsDirty", fIsDirty);

	data->FindBool("StageToFront", &fStageToFront);
	data->FindBool("SnapToGrid", &fSnapToGrid);
	data->FindBool("ShowDuration", &fShowDuration);
	data->FindBool("HideMenu", &fHideMenu);
	data->FindBool("StageGridOn", &fStageGridOn);
	data->FindBool("CenterStage", &fCenterStage);
	data->FindBool("IsCompressedView", &fIsCompressedView);
	data->FindBool("HideCursor", &fHideCursor);

	// Create channel list
	fChannelList = new BList();
	ASSERT(fChannelList);
	if (!fChannelList)
		printf("TCSV::TCSV: channelList null!\n");
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueSheetView::~TCueSheetView()
{
	FreeChannelList();
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TCueSheetView::Init()
{

	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);

	// Ask user to save when quiting
	fIsDirty = true;

	//	Live dragging updates
	fLiveUpdate = true;

	// We need to get the width of the timeline and channels based on the default length
	// and time base and resoltuion of the cue sheet;
	//float channelLength = TimeToPixels(1000L * 60 * 60, B_24_FPS, 3);
	float channelLength = Bounds().Width();

	// Create channel list
	fChannelList = new BList();
	ASSERT(fChannelList);

	// Set default start time, duration and resolution
	fStartTime      = 0;
	fDuration               = 1000L * 60 * 60;  // One hour, in microseconds
	fResolution     = 3;
	fTimeFormat     = B_TIMECODE_24;

	//	Set up our export start and stop times
	fExportStartTime        = fStartTime;
	fExportStopTime = fStartTime + fDuration;

	// Create the default channels.  For now, lets just assume eight...
	// Set to default width and height.  We may want to allow user to
	// specify this at a later time.
	// gzr: Add this to preferences
	int32 startChannels = 8;

	// Set channel size.  To do this we set the channel width to the proper pixel width based
	// on the default time length and the current resolution
	BRect bounds = Bounds();
	bounds.Set(bounds.left, bounds.top, bounds.left+channelLength, kChannelMinHeight);

	for( short i = 0; i < startChannels; i++) {
		bounds.Set( bounds.left, ((kChannelMinHeight*i)), bounds.left + channelLength, ((kChannelMinHeight*i)) + kChannelMinHeight);
		TCueChannel* newChannel = new TCueChannel(bounds, this, i+1);
		AddChild(newChannel);
		newChannel->Show();

		// Add it to the channel list
		fChannelList->AddItem(newChannel);
	}
	SetCurrentTime(0);
}

#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TCueSheetView::Instantiate(BMessage* archive)
{

	if ( validate_instantiation(archive, "TCueSheetView") )
		return new TCueSheetView(archive);
	else {
		printf("TCSV:Instantiate failed!\n");
	}
	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueSheetView::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TCueSheetView");

		// Add our member variables to the archive
		data->AddInt16("Resolution", fResolution);

		data->AddInt32("StartTime", fStartTime);
		data->AddInt32("Duration", fDuration);
		data->AddInt32("CurrentTime", fCurrentTime);

		data->AddInt32("ExportStartTime", fExportStartTime);
		data->AddInt32("ExportStopTime", fExportStopTime);

		data->AddBool("IsDirty", fIsDirty);

		data->AddBool("StageToFront", fStageToFront);
		data->AddBool("SnapToGrid", fSnapToGrid);
		data->AddBool("ShowDuration", fShowDuration);
		data->AddBool("HideMenu", fHideMenu);
		data->AddBool("StageGridOn", fStageGridOn);
		data->AddBool("CenterStage", fCenterStage);
		data->AddBool("IsCompressedView", fIsCompressedView);
		data->AddBool("HideCursor", fHideCursor);

		data->AddData("StageColor", B_RGB_COLOR_TYPE, &fStageColor, sizeof(rgb_color) );
		data->AddData("TimeFormat", B_ANY_TYPE, &fTimeFormat, sizeof(timecode_type) );

		// Add attached views
		if (deep) {

		}
	}

	Looper()->Unlock();

	return myErr;
}

/*
   //---------------------------------------------------------------------
   //	CreateDefaultChannels
   //---------------------------------------------------------------------
   //
   //

   void TCueSheetView::CreateDefaultChannels()
   {
        // Create the default channels.  For now, lets just assume six...
        // Set to default width and height.  We may want to allow user to
        // specify this at a later time.
        // gzr: Add this to preferences
        int32 startChannels = 8;

        // We need to get the width of the timeline and channels based on the default length
        // and time base and resoltuion of the cue sheet;
        long channelLength = TimeToPixels(1000L * 60 * 60, B_24_FPS, 3);

        // Set channel size.  To do this we set the channel width to the proper pixel width based
        // on the default time length and the current resolution
        BRect bounds = Bounds();
        bounds.Set(bounds.left, bounds.top, bounds.left+channelLength, kChannelMinHeight);

        for( short i = 0; i < startChannels; i++)
        {
                bounds.Set( bounds.left, ((kChannelMinHeight*i)), bounds.left + channelLength, ((kChannelMinHeight*i)) + kChannelMinHeight);
                TCueChannel *newChannel = new TCueChannel(bounds, this, i+1);
                AddChild(newChannel);
                newChannel->Show();

                // Add it to the channel list
                fChannelList->AddItem(newChannel);
        }
   }
 */

#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TCueSheetView::Draw(BRect updateRect)
{
	// Save settings
	PushState();

	pattern squares = { 0xF0, 0xF0, 0xF0, 0xF0,
		            0x0F, 0x0F, 0x0F, 0x0F };

	// Fill with background color
	SetHighColor(kSteelGrey);
	FillRect(updateRect, squares);

	// Resore settings
	PopState();
}

//---------------------------------------------------------------------
//	DrawAfterChildren
//---------------------------------------------------------------------
//
//

void TCueSheetView::DrawAfterChildren(BRect updateRect)
{
	// Save settings
	PushState();

	//	Draw indicator line
	//SetHighColor(kRed);


	// Resore settings
	PopState();

	BView::DrawAfterChildren(updateRect);
}


//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TCueSheetView::MouseDown(BPoint where)
{
	Window()->Activate(true);
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TCueSheetView::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TCueSheetView::MouseMoved( BPoint where, uint32 code, const BMessage* a_message )
{
	switch(code)
	{
	case B_ENTERED_VIEW:
		break;

	case B_INSIDE_VIEW:
		break;

	case B_EXITED_VIEW:
		break;

	default:
		break;
	}
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TCueSheetView::MessageReceived(BMessage* message)
{
	switch (message->what)
	{

	// Handle time format change
	case TIMEFORMAT_CHANGED_MSG:
		UpdateTimeFormat(message);
		break;

	// Tell cue channels to draw playback indicator
	case UPDATE_INDICATOR_MSG:
	{
		BPoint drawPt;
		message->FindPoint("IndicatorPoint", &drawPt);
		for (int32 index = 0; index < fChannelList->CountItems(); index++) {
			TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);
			theChannel->DrawIndicatorTick(drawPt);
		}
	}
	break;

	//	Inform all current cues to update their stage appearance based on timeline drag
	case TIMELINE_DRAG_MSG:
	case UPDATE_TIMELINE_MSG:
	{
		if (fLiveUpdate) {
			// Get StageView
			TStageView* stageView = fParent->GetStage()->GetStageView();

			if (stageView) {
				//	Extract time from message
				uint32 theTime = message->FindInt32("TheTime");

				//	Create list of cues at this time
				for (int32 index = 0; index < fChannelList->CountItems(); index++) {
					TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);
					if (theChannel) {
						TCueView* theCue = theChannel->GetCueAtTime(theTime);
						if (theCue) {
							if (!theCue->IsMuted())
								theCue->HandlePlayback(theTime);
						}
					}
				}

				//	Update data to stage
				if (stageView->LockLooper()) {
					stageView->UpdateStage(stageView->Bounds(), theTime);
					stageView->Invalidate();
					stageView->UnlockLooper();
				}
			}
		}
	}
	break;

	// Expand All Channels
	case EXPAND_ALL_BUTTON_MSG:
		ExpandAllChannels();
		break;

	// Contract All Channels
	case CONTRACT_ALL_BUTTON_MSG:
		ContractAllChannels();
		break;

	// Insert a channel
	case INSERT_CHANNEL_MSG:
		InsertChannel(message);
		break;

	// Delete a channel
	case DELETE_CHANNEL_MSG:
		DeleteChannel(message);
		break;

	// Accept pasted cues
	case CUE_PASTE_MSG:
		HandleCuePaste(message);
		break;

	case CUE_SETUP_MSG:
	case CUE_TIMEBASE_MSG:
	case CUE_ZOOMIN_MSG:
	case CUE_ZOOMOUT_MSG:
	case CUE_SHOWDURATIONS_MSG:
	case CUE_SHOWSTOPTIMES_MSG:
		SendMessageToAllChannels(message);
		break;

	//	Add a keyframe.
	case CUESHEET_KEYFRAME_MSG:
	{
		//	Make sure we have cues
		if (HasCues()) {
			//	Make sure we have some selected cues
			if ( ItemsSelected() ) {
				// Start with channels
				for (int32 index = 0; index < fChannelList->CountItems(); index++) {
					TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);

					// We have a channel
					if (theChannel) {
						//	Pass message to selected cues in all channels
						theChannel->SendMessageToAllSelectedCues(message);
					}
				}
			}
		}
	}
	break;

	default:
		BView::MessageReceived(message);
		break;
	}
}


#pragma mark -

//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TCueSheetView::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TCueSheetView::KeyDown(const char* bytes, int32 numBytes)
{
	if (numBytes == 1) {
		switch(bytes[0])
		{
		// Delete/Backspace key
		case B_DELETE:
		case B_BACKSPACE:
			DeleteSelectedCues();
			break;

		// If option key is down, align cue starts, else nudge selected cues
		case B_LEFT_ARROW:
			if ( IsOptionKeyDown() )
				AlignCueStartTimes();
			else
				NudgeSelectedCues(bytes);
			break;

		// If option key is down, align cue ends, else nudge selected cues
		case B_RIGHT_ARROW:
			if ( IsOptionKeyDown() )
				AlignCueEndTimes();
			else
				NudgeSelectedCues(bytes);
			break;

		}
	}
}

#pragma mark -
#pragma mark === List Handling Routine ===

//---------------------------------------------------------------------
//	FreeChannelList
//---------------------------------------------------------------------
//
//	Deallocate channel list
//
#pragma warn_possunwant off

void TCueSheetView::FreeChannelList()
{
	void* anItem;

	for ( long i = fChannelList->CountItems(); anItem = fChannelList->ItemAt(i); i++ ) {
		delete anItem;
	}

	delete fChannelList;

}
#pragma warn_possunwant reset

#pragma mark -
#pragma mark === Selection Routines ====


//---------------------------------------------------------------------
//	HasCues
//---------------------------------------------------------------------
//
//	Check to see if channel has any cues in it
//

bool TCueSheetView::HasCues()
{
	for(int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);

		if(theChannel) {
			if ( theChannel->HasCues() )
				return true;
		}
	}

	return false;
}


//---------------------------------------------------------------------
//	ItemsSelected
//---------------------------------------------------------------------
//
//	Check and see if anything in cue sheet is selected
//

bool TCueSheetView::ItemsSelected()
{
	//
	// Check and see if anything is selected
	//

	// Start with channels
	for (int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);

		// We have a channel
		if (theChannel) {
			// There are selected cues in the channel
			if (theChannel->HasSelectedCues())
				return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------
//	SelectAllCues
//---------------------------------------------------------------------
//
//	Select all cues in all channels of the cue sheet
//

void TCueSheetView::SelectAllCues()
{

	TCueChannel* theChannel;

	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		theChannel = (TCueChannel*)fChannelList->ItemAt(index);

		if(theChannel) {
			theChannel->SelectAllCues();
		}
	}
}


//---------------------------------------------------------------------
//	DeselectAllCues
//---------------------------------------------------------------------
//
//	Deselect all cues in all channels of the cue sheet
//

void TCueSheetView::DeselectAllCues()
{
	TCueChannel* theChannel;

	for( long i = 0; i < fChannelList->CountItems(); i++) {
		theChannel = (TCueChannel*)fChannelList->ItemAt(i);

		if(theChannel) {
			theChannel->DeselectAllCues();
		}
	}
}

//---------------------------------------------------------------------
//	DeselectAllCues
//---------------------------------------------------------------------
//
//	Deselect all cues in all channels of the cue sheet, except the
//	cue passed in
//

void TCueSheetView::DeselectAllCues(TCueView* theCue)
{
	TCueChannel* theChannel;

	for( long i = 0; i < fChannelList->CountItems(); i++) {
		theChannel = (TCueChannel*)fChannelList->ItemAt(i);

		if(theChannel) {
			theChannel->DeselectAllCues(theCue);
		}
	}
}


//---------------------------------------------------------------------
//	HasMultipleCueChannelsSelected
//---------------------------------------------------------------------
//
//	Check and see if we have cues selected in multiple channels.
//	We use this routine to assist in determining to enable or
//	disable the "Align Start/End Times" menu items
//

bool TCueSheetView::HasMultipleCueChannelsSelected()
{
	int32 counter = 0;

	TCueChannel* theChannel;

	for( long i = 0; i < fChannelList->CountItems(); i++) {
		theChannel = (TCueChannel*)fChannelList->ItemAt(i);

		if(theChannel) {
			if ( theChannel->HasSelectedCues() ) {
				counter++;
				if (counter > 1)
					return true;
			}
		}
	}

	if ( counter > 1)
		return true;
	else
		return false;
}

#pragma mark -
#pragma mark === Cue Alignment Routines ====

//---------------------------------------------------------------------
//	AlignCueStartTimes
//---------------------------------------------------------------------
//
//	Align start times of selected cues in multiple channels
//

void TCueSheetView::AlignCueStartTimes()
{
	// Make sure we can do this first...
	if ( HasMultipleCueChannelsSelected() ) {
		//  Construct list of channels with selected cells
		BList channelList;

		for( int32 index = 0; index < GetTotalChannels(); index++) {
			TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);
			if (theChannel) {
				if (theChannel->HasSelectedCues()) {
					channelList.AddItem(theChannel);
				}

			}
		}

		//	We now have a list of channels with cues selected.  We want to align the cue start
		//	times with the cue with the earliest start time.  First, make a list with just the first
		//	selected cues in a channel.  Then find the cue with the earliest start time.
		BList selectedCues;

		for( int32 index = 0; index < channelList.CountItems(); index++) {
			// Look at all channels in cue sheet
			TCueChannel* theChannel = (TCueChannel*)channelList.ItemAt(index);
			if (theChannel) {
				// Get channels selected cue list
				BList* cueList = theChannel->GetSelectedCues();
				if (cueList) {
					// Add the first selected cue in the channel to our selected cue list
					selectedCues.AddItem( cueList->ItemAt(0) );
					// We have to free list returned by GetSelectedCues
					delete cueList;
				}
			}
		}

		//	Look through our list of selected cues and find the earliest start time.
		int32 startTime, tmpTime;
		for( int32 index = 0; index < selectedCues.CountItems(); index++) {
			TCueView* theCue = (TCueView*)selectedCues.ItemAt(index);
			if (theCue) {
				// Set startTime on first element in array
				if (index == 0)
					startTime = theCue->StartTime();
				else{
					tmpTime = theCue->StartTime();

					if (tmpTime < startTime)
						startTime = tmpTime;
				}
			}
		}

		// Now set all cues in list to start at the startTime
		for( int32 index = 0; index < selectedCues.CountItems(); index++) {
			TCueView* theCue = (TCueView*)selectedCues.ItemAt(index);

			if (theCue) {
				TCueChannel* cueChannel = theCue->GetChannel();

				// Check for collisions with other cues
				if (cueChannel->CanInsertCue(theCue, startTime, false)) {
					// Remove cue from old location
					cueChannel->RemoveCue(theCue);

					// Insert cue into channel at new location
					cueChannel->InsertCue(theCue, startTime);
				}
			}
		}
		//DeselectAllCues();
	}
}

//---------------------------------------------------------------------
//	AlignCueEndTimes
//---------------------------------------------------------------------
//
//	Align end times of selected cues in multiple channels
//

void TCueSheetView::AlignCueEndTimes()
{
	// Make sure we can do this first...
	if ( HasMultipleCueChannelsSelected() ) {
		//  Construct list of channels with selected cells
		BList channelList;

		for( int32 index = 0; index < GetTotalChannels(); index++) {
			TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);
			if (theChannel) {
				if (theChannel->HasSelectedCues()) {
					channelList.AddItem(theChannel);
				}

			}
		}

		//	We now have a list of channels with cues selected.  We want to align the cue end
		//	times with the cue with the latest end time.  First, make a list with just the last
		//	selected cues in a channel.  Then find the cue with the latest end time.
		BList selectedCues;

		for( int32 index = 0; index < channelList.CountItems(); index++) {
			// Look at all channels in cue sheet
			TCueChannel* theChannel = (TCueChannel*)channelList.ItemAt(index);
			if (theChannel) {
				// Get channels selected cue list
				BList* cueList = theChannel->GetSelectedCues();
				if (cueList) {
					// Add the last selected cue in the channel to our selected cue list
					int32 lastItem = cueList->CountItems();
					if (lastItem > 0)
						lastItem--;
					selectedCues.AddItem( cueList->ItemAt(lastItem) );
					// We have to free list returned by GetSelectedCues
					delete cueList;
				}
			}
		}

		//	Look through our list of selected cues and find the latest end time.
		int32 endTime, tmpTime;
		for( int32 index = 0; index < selectedCues.CountItems(); index++) {
			TCueView* theCue = (TCueView*)selectedCues.ItemAt(index);
			if (theCue) {
				// Set startTime on first element in array
				if (index == 0)
					endTime = theCue->StartTime() + theCue->Duration();
				else{
					tmpTime = theCue->StartTime() + theCue->Duration();

					if (tmpTime > endTime)
						endTime = tmpTime;
				}
			}
		}

		// Now set all cues in list to end at the endTime
		for( int32 index = 0; index < selectedCues.CountItems(); index++) {
			TCueView* theCue = (TCueView*)selectedCues.ItemAt(index);

			if (theCue) {
				TCueChannel* cueChannel = theCue->GetChannel();

				int32 startTime = endTime - theCue->Duration();

				// Check for collisions with other cues
				if (cueChannel->CanInsertCue(theCue, startTime, true)) {
					// Remove cue from old location
					cueChannel->RemoveCue(theCue);

					// Insert cue into channel at new location
					cueChannel->InsertCue(theCue, startTime);
				}
			}
		}
		//DeselectAllCues();
	}
}



#pragma mark -
#pragma mark === Cue Handling ====

//---------------------------------------------------------------------
//	DeleteSelectedCues
//---------------------------------------------------------------------
//
//	Inform each channel to delete selected cues
//

void TCueSheetView::DeleteSelectedCues()
{
	for (int32 index = 0; index < GetTotalChannels(); index++) {
		TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);

		if (theChannel)
			theChannel->DeleteSelectedCues();
	}
}


//---------------------------------------------------------------------
//	NudgeSelectedCues
//---------------------------------------------------------------------
//
//	Nudge selected cue in direction of arrow
//

void TCueSheetView::NudgeSelectedCues(const char* bytes)
{
	for (int32 index = 0; index < GetTotalChannels(); index++) {
		TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);

		if (theChannel) {
			switch (bytes[0])
			{
			case B_LEFT_ARROW:
				theChannel->NudgeSelectedCues(true);
				break;

			case B_RIGHT_ARROW:
				theChannel->NudgeSelectedCues(false);
				break;
			}
		}
	}
}


#pragma mark -
#pragma mark === Channel Handling ====

//---------------------------------------------------------------------
//	SendMessageToAllChannels
//---------------------------------------------------------------------
//
//	Show message to all channels
//

void TCueSheetView::SendMessageToAllChannels(BMessage* theMessage)
{
	// Go through of list, infroming all channels
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->MessageReceived(theMessage);
	}
}


//---------------------------------------------------------------------
//	AddChannel
//---------------------------------------------------------------------
//
//	Add a channel at the end of channel list, moving and reordering the
//	other channel views as neccessary.
//
//		First, we change the ordering in the linked list.
//		Next, we exchange the cues Frame rectangles to affect a visual
//		change for the user.
//

bool TCueSheetView::AddChannel()
{
	//Window()->BeginViewTransaction();

	BRect bounds = Bounds();

	// Get a pointer to last channel in fChannelList
	TCueChannel* afterChannel = (TCueChannel*)fChannelList->ItemAt( fChannelList->CountItems() - 1 );

	// Get length of channel
	int32 startPixels       = TimeToPixels( fStartTime, fTimeFormat, fResolution);
	int32 durationPixels = TimeToPixels(fDuration, fTimeFormat, fResolution);
	int32 sizePixels        = durationPixels - startPixels;

	bounds.Set( bounds.left, bounds.top, bounds.left+sizePixels, bounds.top + kChannelMinHeight);
	TCueChannel* newChannel = new TCueChannel(bounds, this, fChannelList->CountItems()+1);
	AddChild(newChannel);
	newChannel->Show();

	// Add to channel list at proper position
	fChannelList->AddItem(newChannel);

	// Clean up appearence
	AdjustChannelPositions(afterChannel);

	//Window()->EndViewTransaction();

	return true;
}



//---------------------------------------------------------------------
//	InsertChannel
//---------------------------------------------------------------------
//
//	Insert a channel into the channel list, moving and reordering the
//	other channel views as neccessary.
//
//		First, we change the ordering in the linked list.
//		Next, we exchange the cues Frame rectangles to affect a visual
//		change for the user.
//

void TCueSheetView::InsertChannel(BMessage* message)
{
	//Window()->BeginViewTransaction();

	int32 totalChannels = GetTotalChannels();

	BRect bounds = Bounds();

	// Get total number of channels to be inserted
	int32 numChannels;
	message->FindInt32("NumChannels", &numChannels);

	// Find after which channel we are doing the insertion
	int32 afterIndex;
	message->FindInt32("AfterChannel", &afterIndex);

	// Get a pointer to it
	TCueChannel* afterChannel = (TCueChannel*)fChannelList->ItemAt(afterIndex-1);

	// Get length of channel
	int32 startPixels        = TimeToPixels( fStartTime, fTimeFormat, fResolution);
	int32 durationPixels = TimeToPixels( fDuration, fTimeFormat, fResolution);
	int32 sizePixels         = durationPixels - startPixels;

	// Create new channels
	for (int32 index = 0; index < numChannels; index++) {
		bounds.Set( bounds.left, bounds.top, bounds.left+sizePixels, bounds.top + kChannelMinHeight);
		TCueChannel* newChannel = new TCueChannel(bounds, this, index+1+afterIndex);
		AddChild(newChannel);
		newChannel->Show();

		// Add to channel list at proper position
		fChannelList->AddItem(newChannel, afterIndex+index);
	}

	// Clean up appearence
	AdjustChannelPositions(afterChannel);

	// Adjust scrollbars
	fParent->AdjustScrollBars();

	// Update channels IDS and header text
	UpdateChannelIDS();
	for (int32 index = 0; index < GetTotalChannels(); index++) {
		TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);
		if (theChannel)
			theChannel->GetChannelHeader()->UpdateChannelName();
	}

	//Window()->EndViewTransaction();

	// Update stage
	TStageWindow* theStage  = fParent->GetStage();
	TStageView* stageView = theStage->GetStageView();
	if (theStage->Lock()) {
		stageView->UpdateStage( stageView->Bounds(), GetCurrentTime() );
		stageView->Invalidate();
		theStage->Unlock();
	}
}


//---------------------------------------------------------------------
//	DeleteChannel
//---------------------------------------------------------------------
//
//	Delete a channel from the channel list, moving and reordering the
//	other channel views as neccessary.
//

void TCueSheetView::DeleteChannel(BMessage* message)
{
	//Window()->BeginViewTransaction();

	int32 totalChannels = GetTotalChannels();

	BRect bounds = Bounds();

	// Get total number of channels to be inserted
	int32 numChannels = message->FindInt32("NumChannels");

	// Find after which channel we are doing the insertion
	int32 afterIndex = message->FindInt32("AfterChannel");

	//      Do some checks on these values.  We can't delete any tracks after the last track, as
	//	there aren't any.
	if (afterIndex >= totalChannels)
		return;

	//  If there is only one channel, abort
	if (totalChannels == 1)
		return;

	//  A user may try to delete more channels after a certain channel then exist.
	//	Clip the value to be valid
	if ( numChannels > totalChannels - afterIndex)
		numChannels = totalChannels - afterIndex;

	// Start removing the channels from the list and the view
	//for (int32 index = afterIndex; index <= numChannels; index++)
	for (int32 index = 0; index < numChannels; index++) {
		// Remove item from channel list
		TCueChannel* deletedChannel = (TCueChannel*)fChannelList->RemoveItem(afterIndex);

		// Detach it's view and delete
		if (deletedChannel) {
			TCueChannelHeader* theHeader =  deletedChannel->GetChannelHeader();
			if (theHeader)
				theHeader->Hide();
			deletedChannel->Hide();
			bool retVal = RemoveChild(deletedChannel);
			theHeader->RemoveSelf();
			delete deletedChannel;
			delete theHeader;
		}
	}

	// Clean up appearence
	TCueChannel* afterChannel = (TCueChannel*)fChannelList->ItemAt( afterIndex-1 );
	if (afterChannel)
		AdjustChannelPositions(afterChannel);

	// Adjust scrollbars
	fParent->AdjustScrollBars();

	// Update channels IDs and header text
	UpdateChannelIDS();
	for (int32 index = 0; index < GetTotalChannels(); index++) {
		TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);
		if (theChannel)
			theChannel->GetChannelHeader()->UpdateChannelName();
	}

	//Window()->EndViewTransaction();
}



//---------------------------------------------------------------------
//	DragInsertChannel
//---------------------------------------------------------------------
//
//	Insert a channel into the channel list, moving and reordering the
//	other channel views as neccessary.  The dragged channel gets placed
//	after the insert channel.
//
//		First, we change the ordering in the linked list.
//		Next, we exchange the cues Frame rectangles to affect a visual
//		change for the user.
//

void TCueSheetView::DragInsertChannel(TCueChannel* insertChannel, TCueChannel* afterChannel)
{
	//Window()->BeginViewTransaction();

	if (fChannelList->HasItem(insertChannel) ) {
		// Find the item to be inserted in the list, get its index and remove
		// it from the list
		int32 insertIndex = fChannelList->IndexOf(insertChannel);
		int32 afterIndex = fChannelList->IndexOf(afterChannel);

		// Remove insert item from list and reinsert it at afterIndex.  The afterIndex item
		// has now slipped down one ID in the list
		TCueChannel* insertedChannel = (TCueChannel*)fChannelList->RemoveItem(insertIndex);

		if ( fChannelList->AddItem(insertChannel, afterIndex) ) {
			BRect insertFrame;
			BRect insertHeaderFrame;
			BRect afterFrame;
			BRect afterHeaderFrame;
			TCueChannel* nextChannel, * prevChannel;

			// We are successful in reordering the list.  Now change the visual positioning
			// of the cues. All cues after the inserted cue need to shift their position down
			// the cue sheet.

			// If the channel being inserted formerly was at the top and it is being dropped after
			// the second channel, we just swap places...
			if ( (insertIndex == 0 && afterIndex == 1) ) {
				insertFrame             = insertedChannel->Frame();
				insertHeaderFrame       = insertedChannel->GetChannelHeader()->Frame();
				afterChannel->MoveTo(insertFrame.left, insertFrame.top);
				afterChannel->GetChannelHeader()->MoveTo(insertHeaderFrame.left, insertHeaderFrame.top);

				afterFrame                      = afterChannel->Frame();
				afterHeaderFrame        = afterChannel->GetChannelHeader()->Frame();
				insertedChannel->MoveTo(afterFrame.left, afterFrame.bottom);
				insertedChannel->GetChannelHeader()->MoveTo(afterHeaderFrame.left, afterHeaderFrame.bottom);

				// Update the channel ID's.  We add one to the channel due to 0 offset
				insertedChannel->SetChannelID( fChannelList->IndexOf(insertChannel) + 1);
				afterChannel->SetChannelID( fChannelList->IndexOf(afterChannel) + 1);
			}
			// If the second item in the list is getting dropped on the first,
			// swap places also...
			else if( insertIndex == 1 && afterIndex == 0 ) {

				afterFrame              =  afterChannel->Frame();
				afterHeaderFrame = afterChannel->GetChannelHeader()->Frame();
				insertedChannel->MoveTo(afterFrame.left, afterFrame.top);
				insertedChannel->GetChannelHeader()->MoveTo(afterHeaderFrame.left, afterHeaderFrame.top);

				insertFrame = insertedChannel->Frame();
				insertHeaderFrame = insertedChannel->GetChannelHeader()->Frame();
				afterChannel->MoveTo(insertFrame.left, insertFrame.bottom);
				afterChannel->GetChannelHeader()->MoveTo(insertHeaderFrame.left, insertHeaderFrame.bottom);

				// Update the channel ID's.  We add one to the channel due to 0 offset
				insertedChannel->SetChannelID( fChannelList->IndexOf(insertChannel) + 1);
				afterChannel->SetChannelID( fChannelList->IndexOf(afterChannel) + 1);
			}
			// A channel is being dragged up the list
			else if( insertIndex > 1 && insertIndex > afterIndex ) {
				// Set inserted channel to new location
				insertFrame = afterChannel->Frame();
				insertHeaderFrame = afterChannel->GetChannelHeader()->Frame();
				insertedChannel->MoveTo(insertFrame.left, insertFrame.top);
				insertedChannel->GetChannelHeader()->MoveTo(insertHeaderFrame.left, insertHeaderFrame.top);

				// Update the channel ID's.  We add one to the channel due to 0 offset
				insertedChannel->SetChannelID( fChannelList->IndexOf(insertedChannel) + 1);

				// Bubble channels down
				for( int32 index = afterIndex; index < insertIndex; index++) {
					// Get channels
					prevChannel = (TCueChannel*)fChannelList->ItemAt(index);
					nextChannel = (TCueChannel*)fChannelList->ItemAt(index+1);

					insertFrame             = prevChannel->Frame();
					insertHeaderFrame       = prevChannel->GetChannelHeader()->Frame();
					nextChannel->MoveTo(insertFrame.left, insertFrame.bottom);
					nextChannel->GetChannelHeader()->MoveTo(insertHeaderFrame.left, insertHeaderFrame.bottom);

					// Update the channel ID's.  We add one to the channel due to 0 offset
					nextChannel->SetChannelID( fChannelList->IndexOf(nextChannel) + 1);
				}
			}
			//      If any other channel is being dropped on the top of the list
			//      we need to the channels down until we get to the previous
			//	location of the new channel.
			else{
				// Now push all channels up...
				for( int32 index = insertIndex; index <= afterIndex; index++) {
					// Get channels
					prevChannel = (TCueChannel*)fChannelList->ItemAt(index-1);
					nextChannel = (TCueChannel*)fChannelList->ItemAt(index);

					// If this value is null, we are moving the top channel
					if (!prevChannel) {
						//insertFrame.Set( Bounds().left, Bounds().top+kTimelineHeight, Bounds().right,  Bounds().top+kTimelineHeight);
						//insertHeaderFrame.Set( 0, kMenuHeight+kTimelineHeight, kHeaderWidth, kMenuHeight+kTimelineHeight);
						insertFrame.Set( Bounds().left, Bounds().top, Bounds().right,  Bounds().top);
						insertHeaderFrame.Set( 0, Bounds().top+1, kHeaderWidth, Bounds().top+1);
					} else {
						insertFrame             = prevChannel->Frame();
						insertHeaderFrame       = prevChannel->GetChannelHeader()->Frame();
					}

					nextChannel->MoveTo(insertFrame.left, insertFrame.bottom);
					nextChannel->GetChannelHeader()->MoveTo(insertHeaderFrame.left, insertHeaderFrame.bottom);

					// Update the channel ID's.  We add one to the channel due to 0 offset
					nextChannel->SetChannelID( fChannelList->IndexOf(nextChannel) + 1);
				}
			}
		}
		// Update stage
		TStageWindow* theStage  = fParent->GetStage();
		TStageView* stageView = theStage->GetStageView();
		theStage->Lock();
		stageView->UpdateStage( stageView->Bounds(), GetCurrentTime() );
		stageView->Invalidate();
		theStage->Unlock();
	}

	//Window()->EndViewTransaction();
}


//---------------------------------------------------------------------
//	UpdateChannelIDS
//---------------------------------------------------------------------
//
// Update all channel internal IDs in accordance with their order in the
// linked list
//

void TCueSheetView::UpdateChannelIDS()
{
	for(short index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->SetID(index + 1);
	}
}

//---------------------------------------------------------------------
//	AdjustChannelPositions
//---------------------------------------------------------------------
//
//	Adjust all channels after the channel passed in.  We do this in
//	response to the user expanding, contracting or dragging a channel's
//	height down.
//

void TCueSheetView::AdjustChannelPositions(TCueChannel* theChannel)
{
	// Make sure the channel is in the list
	if (fChannelList->HasItem(theChannel)) {
		// Go through rest of list, adjusting channels...
		for( int32 index = fChannelList->IndexOf(theChannel); index < fChannelList->CountItems(); index++) {
			TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
			TCueChannel* prevChannel = (TCueChannel*)fChannelList->ItemAt(index-1);
			ASSERT(cueChannel);

			// Now move it down below the previous channel.  If prevChannel is NULL, we are
			//	adjusting the top channel and don't need to adjust it.
			if (prevChannel) {
				TCueChannelHeader* theHeader = cueChannel->GetChannelHeader();
				TCueChannelHeader* prevHeader = prevChannel->GetChannelHeader();
				cueChannel->MoveTo( cueChannel->Frame().left, prevChannel->Frame().bottom);
				theHeader->MoveTo( theHeader->Frame().left, prevHeader->Frame().bottom);
			}
		}
	}
}


//---------------------------------------------------------------------
//	ContractAllChannels
//---------------------------------------------------------------------
//
//	Contract all channels in the cue sheet channel list
//

void TCueSheetView::ContractAllChannels()
{
	//Window()->BeginViewTransaction();

	// Go through of list, contracting channels...
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->Contract();
	}

	// Adjust channel positions
	TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(0);
	ASSERT(cueChannel);
	AdjustChannelPositions(cueChannel);

	// Adjust scroll bars
	fParent->AdjustScrollBars();

	//Window()->EndViewTransaction();
}


//---------------------------------------------------------------------
//	ExpandAllChannels
//---------------------------------------------------------------------
//
//	Expand all channels in the cue sheet channel list
//

void TCueSheetView::ExpandAllChannels()
{
	//Window()->BeginViewTransaction();

	// Go through of list, contracting channels...
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->Expand();
	}

	// Adjust channel positions
	TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(0);
	ASSERT(cueChannel);
	AdjustChannelPositions(cueChannel);

	// Adjust scroll bars
	fParent->AdjustScrollBars();

	//Window()->EndViewTransaction();
}


//---------------------------------------------------------------------
//	UnlockAllChannels
//---------------------------------------------------------------------
//
//	Unlock all channels in the cue sheet channel list
//

void TCueSheetView::UnlockAllChannels()
{
	// Go through of list, contracting channels...
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->UnlockChannel();
	}
}

//---------------------------------------------------------------------
//	LockAllChannels
//---------------------------------------------------------------------
//
//	Lock all channels in the cue sheet channel list
//

void TCueSheetView::LockAllChannels()
{
	// Go through of list, contracting channels...
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->LockChannel();
	}
}



#pragma mark -
#pragma mark === Mute/Solo Handling ===

//---------------------------------------------------------------------
//	SoloAllChannels
//---------------------------------------------------------------------
//
//	Solo all channels in the cue sheet channel list
//

void TCueSheetView::SoloAllChannels()
{
	// Go through of list, soloing channels...
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->Solo();
	}

	//	Have stage redraw
	UpdateStage();
}


//---------------------------------------------------------------------
//	UnsoloAllChannels
//---------------------------------------------------------------------
//
//	Unsolo all channels in the cue sheet channel list
//

void TCueSheetView::UnsoloAllChannels()
{
	// Go through of list, unsoloing channels...
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->Unsolo();
	}
}



//---------------------------------------------------------------------
//	AreOtherChannelsSoloed
//---------------------------------------------------------------------
//
//	Check to see if any other channel in the cue sheet is soloed.
//	Exclude the channel asking the question
//

bool TCueSheetView::AreOtherChannelsSoloed( TCueChannel* theChannel)
{
	// Go through of list, looking for soloed channels
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);

		if (cueChannel != theChannel) {
			if ( cueChannel->GetSolo() == true )
				return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------
//	MuteAllChannels
//---------------------------------------------------------------------
//
//	Mute all channels in the cue sheet channel list
//

void TCueSheetView::MuteAllChannels()
{
	// Go through of list, muting channels...
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->Mute();
	}

	//	Have stage redraw
	UpdateStage();
}

//---------------------------------------------------------------------
//	MuteAllUnsoloedChannels
//---------------------------------------------------------------------
//
//	Mute all channels in the cue sheet channel list that are not soloed
//

void TCueSheetView::MuteAllUnsoloedChannels()
{
	// Go through of list, muting channels...
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		if ( !cueChannel->GetSolo() )
			cueChannel->Mute();
	}

	//	Have stage redraw
	UpdateStage();
}


//---------------------------------------------------------------------
//	UnmuteAllChannels
//---------------------------------------------------------------------
//
//	Unmute all channels in the cue sheet channel list
//

void TCueSheetView::UnmuteAllChannels()
{
	// Go through of list, unmuting channels...
	for( int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* cueChannel = (TCueChannel*)fChannelList->ItemAt(index);
		ASSERT(cueChannel);
		cueChannel->Unmute();
	}

	//	Have stage redraw
	UpdateStage();
}


#pragma mark -
#pragma mark === Update Routines ===

//---------------------------------------------------------------------
//	ResolutionChanged
//---------------------------------------------------------------------
//
//	Handle a change in the cue sheet resolution.  This is usually in response
//	to a RESOLUTION_CHANGED_MSG.  We need to do several things:
//		1. Change the fResolution memeber variable
//		2. Update the timeline
//		3. Inform all channels of the change
//

void TCueSheetView::ResolutionChanged(BMessage* message)
{
	// Unpack message and set new fTimeFormat
	//timecode_type format;

	//if ( message->FindInt32("TimeFormat", 0, &format) !=  B_NAME_NOT_FOUND)
	//	fTimeFormat = format;

	// Redraw timeline to reflect new resolution
	UpdateTimeline();

	//	Calculare resize pixels
	const int32 startPixels         = TimeToPixels( fStartTime, fTimeFormat, fResolution);
	const int32 durationPixels      = TimeToPixels( fDuration, fTimeFormat, fResolution);
	const int32 resizePixels        = durationPixels - startPixels;

	// All channels need to be informed and resized as well...
	for (int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);
		theChannel->ResolutionChanged(resizePixels);
	}
}


//---------------------------------------------------------------------
//	UpdateTimeline
//---------------------------------------------------------------------
//
//	Force timeline to redraw.  This is usually in response to a
//	RESOLUTION_CHANGED_MSG
//

void TCueSheetView::UpdateTimeline()
{
	BRect updateRect;

	// Clip out playback indicator.  We only want to redraw the time and ticks...
	updateRect = fParent->GetTimeline()->Bounds();
	updateRect.top += kIndicatorZoneHeight + kTickHeight;

	// Force redraw
	fParent->GetTimeline()->Invalidate(updateRect);
}


//---------------------------------------------------------------------
//	UpdateTimeFormat
//---------------------------------------------------------------------
//
//	Update the fTimeFormat.
//

void TCueSheetView::UpdateTimeFormat(BMessage* message)
{
	// Unpack message and set new fTimeFormat
	fTimeFormat = (timecode_type)message->FindInt16("TimeFormat");

	// Redraw timeline to reflect change
	UpdateTimeline();

	// Update time palette
	// Tell time display where we are
	//BMessage *message = new BMessage(onBitmap);
	//BPoint drawPt( fIndicatorRect.left + (fIndicatorRect.Width() / 2)+1, fIndicatorRect.top);
	//message->AddPoint("Where", drawPt);
	//static_cast<MuseumApp *>(be_app)->GetCueSheet()->PostMessage(message, NULL);
	//delete message;

}

#pragma mark -
#pragma mark === Clipboard Handling ===

//---------------------------------------------------------------------
//	Undo
//---------------------------------------------------------------------
//
//	Handle undo functionality
//

void TCueSheetView::Undo()
{
}


//---------------------------------------------------------------------
//	Cut
//---------------------------------------------------------------------
//
//	Handle clipboard cut
//

void TCueSheetView::Cut()
{
}


//---------------------------------------------------------------------
//	Copy
//---------------------------------------------------------------------
//
//	Handle clipboard copies
//

void TCueSheetView::Copy()
{
	// Create a list to hold archived cues
	BList* archivedList = new BList();

	//      Check and see what is selected.  Start with all selected cues.  Iterate thorugh
	//	all channels and ask for their selected cues
	for (int32 index = 0; index < fChannelList->CountItems(); index++) {
		TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt(index);

		// We have a channel
		if (theChannel) {
			// There are selected cues in the channel
			if (theChannel->HasSelectedCues()) {
				// Get a list of selected cues in channel
				BList* cueList = theChannel->GetSelectedCues();

				// Call the Copy method of each selected cue.  This will return
				// an archived version of the Cue
				for (int32 cueIndex = 0; cueIndex < cueList->CountItems(); cueIndex++) {
					TCueView* theCue = (TCueView*)cueList->ItemAt(cueIndex);
					if (theCue) {
						BMessage* cueMessage = theCue->Copy();

						// Add the archive to out list if it is valid
						if( cueMessage)
							archivedList->AddItem(cueMessage);
					}
				}
			}
		}
	}

	//	Now check and see if we have any items added to our list
	if (archivedList->CountItems() > 0) {
		// Add the list of archived items to the clipboard
		TMuseumClipboard* theClipboard = static_cast<MuseumApp*>(be_app)->fClipboard;

		// Create message, add out cue list to it and add it to the clipboard
		BMessage* clipMessage = new BMessage(CUE_LIST_MSG);
		clipMessage->AddPointer("CueList", archivedList);
		theClipboard->AddItem(clipMessage);
	}
}


//---------------------------------------------------------------------
//	Paste
//---------------------------------------------------------------------
//
//	Handle clipboard paste
//

void TCueSheetView::Paste()
{
	// Make sure clipboard has data relevant to us
	TMuseumClipboard* theClipboard = static_cast<MuseumApp*>(be_app)->fClipboard;

	// If we have data, get it and determine its type
	int32 clipData = theClipboard->HasData();
	if (clipData) {
		switch (clipData)
		{
		case kCueListData:
		{
			BList* cueArchiveList = theClipboard->GetCueArchiveList();

			if (cueArchiveList)
				PasteCues(cueArchiveList);
		}
		break;

		default:
			break;
		}
	}
}


//---------------------------------------------------------------------
//	PasteCues
//---------------------------------------------------------------------
//
//	Paste cues in cue list into cue sheet
//

void TCueSheetView::PasteCues(BList* cueArchiveList)
{
	// Create list to store unarchived cues
	BList* cueList = new BList();

	// Lock clipboard so list isn't deleted
	TMuseumClipboard* theClipboard = static_cast<MuseumApp*>(be_app)->fClipboard;
	if ( theClipboard->Lock() ) {
		// Go through list and extract archived cues
		for( int32 index = 0; index < cueArchiveList->CountItems(); index++) {
			// get the archived cue
			BMessage* cueMessage = (BMessage*)cueArchiveList->ItemAt(index);

			if (cueMessage) {
				BArchivable* unarchived = instantiate_object(cueMessage);
				if ( unarchived ) {
					// Add cue to our list of unarchived cues
					TCueView* view = cast_as(unarchived, TCueView);
					if (view)
						cueList->AddItem(view);
				}
			}
		}

		// Unlock the clipboard
		theClipboard->Unlock();

		//
		//	Prepare to show dialog.  We pass the list containing the cues to
		//	be pasted, the curent time and a refernce to the cue sheet to the
		//	dialog.  The dialog then sends a CUE_PASTE_MSG to the cue sheet
		//	if the user decides to proceed with the paste.
		//

		//      Display Paste Cue dialog using this time as the suggested paste time.
		//	Use the track of the first copied track as the suggested paste track.
		BMessage* theMessage = GetWindowFromResource("PasteCuesWindow");
		BWindow* pasteWindow = new TPasteCues(this, cueList, theMessage);

		// Center and show dialog
		CenterWindow(pasteWindow);
		pasteWindow->Show();

	}
}


//---------------------------------------------------------------------
//	HandleCuePaste
//---------------------------------------------------------------------
//
//	Handle reciept of CUE_PASTE_MSG
//

void TCueSheetView::HandleCuePaste(BMessage* theMessage)
{
	BList* cueList;
	int32 pasteChannel;
	uint32 pasteTime, offsetTime, deltaStart;

	//      Extract info from message
	theMessage->FindPointer("CueList", (void**)&cueList);
	pasteTime = theMessage->FindInt32("PasteTime");
	theMessage->FindInt32("PasteChannel", &pasteChannel);

	//      Make a copy of the pasteTime for delta offsets of multiple cue paste
	offsetTime = pasteTime;

	//      Begin paste-o-matic....

	//      Get Cue
	for (int32 index = 0; index < cueList->CountItems(); index++) {
		TCueView* insertCue = (TCueView*)cueList->ItemAt(index);
		if (insertCue) {
			// Set up time and position for cue in channel
			if (index == 0)
				deltaStart = insertCue->StartTime();

			// Find relative time for cue insertion based on desired paste time.
			else{
				int32 delta;

				// Does cue start before reference cue?
				if ( insertCue->StartTime() < deltaStart) {
					delta = deltaStart - insertCue->StartTime();
					offsetTime = pasteTime - delta;
				}
				// Does cue start after reference cue?
				else if ( insertCue->StartTime() > deltaStart) {
					delta = insertCue->StartTime() - deltaStart;
					offsetTime = pasteTime + delta;
				}
				// Cue has identical startTime as refernce cue
				else
					offsetTime = pasteTime;

				// Make sure offset time is within bounds of cue sheet time frame
				if ( offsetTime < 0)
					offsetTime = 0;

				if ( (offsetTime + insertCue->Duration()) > fDuration)
					offsetTime = fDuration - insertCue->Duration();
			}

			//      Get channel
			TCueChannel* theChannel = (TCueChannel*)fChannelList->ItemAt( (pasteChannel-1) );

			if (theChannel) {
				//      Try to insert the cue in the selected channel...
				if ( theChannel->CanInsertCue(insertCue, offsetTime, false) ) {
					theChannel->InsertCue(insertCue, offsetTime);
					insertCue->PasteComplete();
				}
				//      The channel was unable to accept the cue.  Work our way down the channels
				//	until one can accept it.  If no channels can accept the cue, we may need
				//	to create a new channel.
				else{
					bool retVal = false;

					for (int32 chanIndex = pasteChannel; chanIndex < fChannelList->CountItems(); chanIndex++) {
						// Get channel
						TCueChannel* retryChannel = (TCueChannel*)fChannelList->ItemAt(chanIndex);
						if (retryChannel) {
							retVal = retryChannel->CanInsertCue(insertCue, offsetTime, false);
							if (retVal) {
								retryChannel->InsertCue(insertCue, offsetTime);
								insertCue->PasteComplete();

								// Increment chanIndex to break out of loop
								chanIndex = fChannelList->CountItems() + 1;
							}
						}
					}

					// If we are here, we need to create a new channel and insert the cue into it
					if (retVal == false) {
						if ( AddChannel() ) {
							// We wre able to add a channel.  Get a pointer to it
							TCueChannel* newChannel = (TCueChannel*)fChannelList->ItemAt( fChannelList->CountItems() - 1 );
							if (newChannel) {
								// Inset cue into new channel
								newChannel->InsertCue(insertCue, offsetTime);
							}
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------
//	Clear
//---------------------------------------------------------------------
//
//	Handle clipboard clear
//

void TCueSheetView::Clear()
{
	DeleteSelectedCues();
}


//---------------------------------------------------------------------
//	Select All
//---------------------------------------------------------------------
//
//	Handle Select All functionality
//

void TCueSheetView::SelectAll()
{
	SelectAllCues();
}


#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent CueSheetWindow.
//	This is intended for use when instantiating
//	cues from an archive.
//

void TCueSheetView::AttachedToWindow()
{
	if(fParent == NULL) {
		fParent = (TCueSheetWindow*)Window();

		/*
		   THeaderContainerView *container = fParent->GetHeaderContainer();

		   //	Match CueChannels to headers
		   for(int32 index = 0; index < fChannelList->CountItems(); index++)
		   {
		        TCueChannelHeader *header = (TCueChannelHeader *)container->ChildAt(index);
		        header->SetChannel( (TCueChannel *)fChannelList->ItemAt(index));
		   }
		 */
	}

	//	Pass up to parent
	BView::AttachedToWindow();
}


#pragma mark -
#pragma mark === Member Variable Handling ===

//---------------------------------------------------------------------
//	SetCurrentTime
//---------------------------------------------------------------------
//
//	Set fCurrentTime to theTime value
//

void TCueSheetView::SetCurrentTime(uint32 theTime)
{
	fCurrentTime = theTime;
}

//---------------------------------------------------------------------
//	SetResolution
//---------------------------------------------------------------------
//
//	Set fResolution to theResolution value
//

void TCueSheetView::SetResolution(int16 theResolution)
{
	fResolution = theResolution;
}

//---------------------------------------------------------------------
//	SetDirty
//---------------------------------------------------------------------
//
//	Set fIsDirty to the value
//

void TCueSheetView::SetDirty(bool theVal)
{
	fIsDirty = theVal;
}


//---------------------------------------------------------------------
//	SetExportStartTime
//---------------------------------------------------------------------
//
//	Set fExportStartTime to theTime value
//

void TCueSheetView::SetExportStartTime(uint32 theTime)
{
	//	Clip time
	if (theTime < fStartTime)
		theTime = fStartTime;

	fExportStartTime = theTime;
}


//---------------------------------------------------------------------
//	SetExportStopTime
//---------------------------------------------------------------------
//
//	Set fExportStopTime to theTime value
//

void TCueSheetView::SetExportStopTime(uint32 theTime)
{
	//	Clip time
	if (theTime > fStartTime + fDuration)
		theTime = fStartTime + fDuration;

	fExportStopTime = theTime;
}

//---------------------------------------------------------------------
//	SetParent
//---------------------------------------------------------------------
//
//	Set fParent to parent
//

void TCueSheetView::SetParent(TCueSheetWindow* parent)
{
	fParent = parent;
}

//---------------------------------------------------------------------
//	SetLiveUpdate
//---------------------------------------------------------------------
//
//	Set status of fLiveDrag flag
//

void TCueSheetView::SetLiveUpdate(bool update)
{
	fLiveUpdate = update;
}
