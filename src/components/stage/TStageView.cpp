//---------------------------------------------------------------------
//
//	File:	TStageView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	A stage view is a visual representation of the data contained
//			in a cue.  A cue is instantiated into the view and stored in
//			the member variable m_StageViewList.  We still need to figure out
//			layer ordering.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>
#include <assert.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "MuseumApp.h"
#include "DebugUtils.h"

#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueChannel.h"
#include "TCuePosition.h"
#include "TVisualCue.h"
#include "TStageWindow.h"
#include "TStageCue.h"
#include "TStageToolsView.h"
#include "TMouseWatcher.h"

#include "TPlaybackEngine.h"

#include "TTransportPalette.h"

#include "TStageView.h"

// Constants

#ifndef WATCH
#define WATCH(x) printf(x)
#endif

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageView::TStageView(BRect bounds, TStageWindow* parent)
	:
	BView(bounds, "StageView", B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS | B_PULSE_NEEDED)
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);

	m_Parent = parent;

	// Perform default initialization
	Init();
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageView::TStageView(BMessage* archive) : BView(archive)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TStageView::~TStageView()
{
	if (m_OffscreenBitmap) {
		m_OffscreenBitmap->Lock();
		m_OffscreenBitmap->RemoveChild(m_OffscreenView);
		delete m_OffscreenView;
		delete m_OffscreenBitmap;
	}

	// Free StageCue list
	ClearStageCueList();
	delete m_StageCueList;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TStageView::Init()
{
	// We handle our own drawing
	SetViewColor(B_TRANSPARENT_32_BIT);

	//	Set up member variables
	m_IsPlaying             = false;
	m_IsStopping            = false;

	// Create offscreen bitmap and view
	m_OffscreenBitmap       = new BBitmap(Bounds(), B_RGB_32_BIT, true);
	m_OffscreenView         = new BView( m_OffscreenBitmap->Bounds(), "OffscreenView", B_FOLLOW_ALL, B_WILL_DRAW);

	// Add child and fill with background color
	m_OffscreenBitmap->Lock();
	m_OffscreenBitmap->AddChild(m_OffscreenView);
	m_OffscreenView->SetViewColor(B_TRANSPARENT_32_BIT);
	m_OffscreenView->SetHighColor(kBlack);
	m_OffscreenView->SetDrawingMode(B_OP_OVER);
	m_OffscreenView->FillRect(m_OffscreenView->Bounds());
	m_OffscreenView->Sync();
	m_OffscreenBitmap->Unlock();

	// Init StageCue list
	m_StageCueList = new BList();

	//	We are not in edit mode
	m_SelectionMode = false;

	//	Set tool mode to move/select
	m_ToolMode = kMoveMode;
}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TStageView::Instantiate(BMessage* archive)
{

	if ( validate_instantiation(archive, "TStageView") )
		return new TStageView(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TStageView::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TStageView");
	}

	Looper()->Unlock();

	return myErr;

}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//	Draws the stage cues onto the stage view.  We need to determine
//	the cue order and draw the cues onto the stage in the proper
//	order.
//

void TStageView::Draw(BRect updateRect)
{
	//      Exit if we are playing.  The playback mechanism handles all
	//	updating of the stage
	if ( IsPlaying() )
		return;

	// Update from offscreen
	BlitOffscreen();

	// Inform any StageCues
	for (int32 index = 0; index < m_StageCueList->CountItems(); index++) {
		TStageCue* stageCue = (TStageCue*)m_StageCueList->ItemAt(index);
		if (stageCue)
			stageCue->Draw(updateRect);
	}
}


//---------------------------------------------------------------------
//	StageDraw
//---------------------------------------------------------------------
//
//	Draws cue data onto the stage view.
//

void TStageView::StageDraw(BRect updateRect, uint32 theTime)
{
	//	Clear offscreen bits
	ClearOffscreen();

	//printf("TSV::SD: start\n");
	if (!m_Parent)
		return;

	//	Get cue data at time
	TCueSheetWindow* testCueSheet = m_Parent->GetCueSheet();
	if (!testCueSheet)
		return;

	TCueSheetView* testCueSheetView = testCueSheet->GetCueSheetView();
	if (!testCueSheetView)
		return;

	BList* channelList = m_Parent->GetCueSheet()->GetCueSheetView()->GetChannelList();

	if (channelList) {
		for (int32 index = 0; index < channelList->CountItems(); index++) {
			TCueChannel* theChannel = (TCueChannel*)channelList->ItemAt(index);
			if (theChannel) {
				TVisualCue* theCue = theChannel->GetVisualCueAtTime(theTime);
				if (theCue) {
					//	Proceed only if visible
					if (!theCue->IsMuted()) {
						//	Get cues m_RenderSem
						sem_id renderSem = theCue->GetRenderSem();
						acquire_sem(renderSem);

						//	Update offscreen using transparency
						BBitmap* bitmap = theCue->GetBitmap(theTime);
						if (bitmap) {
							if (m_OffscreenView->LockLooper()) {
								BRect offsetRect = theCue->CuePosition()->Enclosure();
								offsetRect.OffsetTo(0, 0);
								m_OffscreenView->DrawBitmap(bitmap, offsetRect, theCue->CuePosition()->Enclosure());
								m_OffscreenView->UnlockLooper();
							}
						}

						//	Release semaphore
						release_sem(renderSem);
					}
				}
			}
		}
	}

	//	Update stage from offscreen
	BlitOffscreen();
}



#pragma mark -
#pragma mark === Message Handling ===


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TStageView::MessageReceived(BMessage* message)
{
	switch (message->what)
	{

	// End editing session.  Deselect all stage cues and clean up
	case END_STAGE_EDIT_MSG:
		if (m_SelectionMode) {
			//	Set selection mode to false
			m_SelectionMode = false;

			ClearStageCueList();
			if (LockLooper()) {
				Draw(Bounds());
				UnlockLooper();
			}
		}
		break;

	//	Stage tool messages
	case MOVE_TOOL_MSG:
	case ROTATE_TOOL_MSG:
	case CROP_TOOL_MSG:
	case SCALE_TOOL_MSG:
	case SHEAR_TOOL_MSG:
	case SKEW_TOOL_MSG:
	case PERSPECTIVE_TOOL_MSG:
	case MIRROR_TOOL_MSG:
	{
		SetToolMode(message->what);
		if (LockLooper()) {
			Draw(Bounds());
			UnlockLooper();
		}
	}
	break;

	//	Mouse watching messages.  Pass to selected cues...
	case MW_MOUSE_DOWN:
	case MW_MOUSE_UP:
	case MW_MOUSE_MOVED:
		SendMessageToSelectedCues(message);
		break;

	default:
		BView::MessageReceived(message);
		break;
	}

}


void
TStageView::Pulse()
{
	const uint32 curTime = GetCurrentTime();
	if (LockLooper()) {
		StageDraw(Bounds(), curTime);
		UnlockLooper();
	}
}


//---------------------------------------------------------------------
//	SendMessageToCues
//---------------------------------------------------------------------
//
//	Send message to all stage cues
//

void TStageView::SendMessageToAllCues(BMessage* message)
{
	// Determine who in cue list is selected
	for (int32 index = 0; index < m_StageCueList->CountItems(); index++) {
		TStageCue* theCue = (TStageCue*)m_StageCueList->ItemAt(index);
		if (theCue) {
			theCue->MessageReceived(message);
		}
	}
}


//---------------------------------------------------------------------
//	SendMessageToSelectedCues
//---------------------------------------------------------------------
//
//	Send message to all selected stage cues
//

void TStageView::SendMessageToSelectedCues(BMessage* message)
{
	// Determine who in cue list is selected
	for (int32 index = 0; index < m_StageCueList->CountItems(); index++) {
		TStageCue* theCue = (TStageCue*)m_StageCueList->ItemAt(index);
		if (theCue) {
			if (theCue->Selected())
				theCue->MessageReceived(message);
		}
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

void TStageView::MouseDown(BPoint where)
{
	// Do nothing if we are playing
	if ( IsPlaying() )
		return;

	// Set focus to stage window
	Window()->Activate(true);

	//      We need to find out if we are clicking in one of the stage cues.  If the m_StageCueList has
	//      items in it, we are in stage edit mode.
	if (m_StageCueList->CountItems() > 0) {
		//      Determine who got the mouse click.  If it is a cue lower in the z-order, promote it
		//	to the top of the z-order by placing it last in the list.  This is a temporary promotion
		//	and has no effect on the cues channel playback z-order
		for (int32 index = m_StageCueList->CountItems()-1; index >= 0; index--) {
			TStageCue* stageCue = (TStageCue*)m_StageCueList->ItemAt(index);

			//	We have a cue.  Does it contain the click?
			if ( stageCue->GetChannelCue()->CuePosition()->Contains(where) || stageCue->PointInResizeZones(where) ) {
				//  It does.  Check and see if it is already selected.  If it is, do nothing.
				if (stageCue->Selected()) {
					stageCue->MouseDown(where);
					return;
				}
				// It isn't.  Deselect selcted cue.  The selected cue will be last in the cue list
				//	as it is highest in the z-order.
				else{
					//  Get the selected cue
					TStageCue* deselectCue = (TStageCue*)m_StageCueList->ItemAt(m_StageCueList->CountItems()-1);
					deselectCue->Deselect();

					// Promote clicked cue in the z-order by placing it at end of list
					m_StageCueList->RemoveItem(stageCue);
					m_StageCueList->AddItem(stageCue);
					stageCue->Select();
					stageCue->MouseDown(where);
					return;
				}
			}
		}

		//	If we get this far, deselect all cues, as the click hit the stage background
		ClearStageCueList();
		if (LockLooper()) {
			Draw(Bounds());
			UnlockLooper();
			return;
		}
	}
	//      We need to find any items clicked and create the stage cue list
	else{
		//      Determine if a click occurred on one of the stage cues.
		//	If so, activate it...

		// Get a pointer to the cue sheet
		TCueSheetView* theCueSheet = static_cast<MuseumApp*>(be_app)->GetCueSheet()->GetCueSheetView();

		//      Now create a list of cues that can be drawn at this time within this rect,
		//	starting from the start channel and working our way up to the stop channel
		//

		//      Find the cues that are at the current time.
		BList* cueList = new BList();

		for ( int32 index = 0; index < theCueSheet->GetTotalChannels(); index++) {
			TCueChannel* theChannel = (TCueChannel*)theCueSheet->GetChannelList()->ItemAt(index);

			// We have a valid channel.  Get cue at this time slice...
			if (theChannel) {
				TCueView* theCue = theChannel->GetCueAtTime(GetCurrentTime());

				// Is it a visible cue and not muted?
				if (theCue && theCue->IsVisible() && theCue->IsMuted() == false) {
					// Add the cue to our list
					cueList->AddItem(theCue);
				}
			}
		}

		//  Create stage cues for each item in the cueList.  Add the new stage cues to our
		//	m_StageCueList
		for(int32 cueIndex = 0; cueIndex < cueList->CountItems(); cueIndex++) {
			TVisualCue* listCue = (TVisualCue*)cueList->ItemAt(cueIndex);
			if (listCue) {
				TStageCue* stageCue = new TStageCue(this, listCue);
				m_StageCueList->AddItem(stageCue);
			}
		}

		// Clean up cueList
		delete cueList;

		// See if we were successful in locating a cue
		if (m_StageCueList->CountItems() > 0) {
			//      Determine who got the mouse click.  If it is a cue lower in the z-order, promote it
			//	to the top of the z-order by placing it last in the list.  This is a temporary promotion
			//	and has no effect on the cues channel playback z-order
			for (int32 index = m_StageCueList->CountItems()-1; index >= 0; index--) {
				TStageCue* stageCue = (TStageCue*)m_StageCueList->ItemAt(index);

				//	We have a cue.  Does it contain the click?
				if ( stageCue->GetChannelCue()->CuePosition()->Contains(where) || stageCue->PointInResizeZones(where) ) {
					// Promote clicked cue in the z-order by placing it at end of list
					m_StageCueList->RemoveItem(stageCue);
					m_StageCueList->AddItem(stageCue);
					stageCue->Select();
					stageCue->MouseDown(where);

					//	Set selection mode to true
					m_SelectionMode = true;

					return;
				}
			}
		}
	}
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TStageView::MouseMoved( BPoint where, uint32 code, const BMessage* a_message )
{
	// Do nothing if we are playing
	if ( IsPlaying() )
		return;

	//	Determine if we have a MouseMove within a selected StageCue
	const int32 stageCues = m_StageCueList->CountItems();
	if (stageCues > 0) {
		TStageCue* stageCue = (TStageCue*)m_StageCueList->ItemAt(stageCues-1);

		//	We have a cue.  Does it contain the move?
		if (stageCue) {
			if ( stageCue->GetChannelCue()->CuePosition()->Contains(where)  || stageCue->PointInResizeZones(where) ) {
				stageCue->MouseMoved(where, code, a_message);
			}
			//	Restore cursor
			else
				be_app->SetCursor(B_HAND_CURSOR);
		}
	}
	//	Restore cursor if leaving view
	else{
		switch(code)
		{
		case B_EXITED_VIEW:
			be_app->SetCursor(B_HAND_CURSOR);
			break;

		default:
			break;
		}
	}
}


#pragma mark -
#pragma mark === Frame Handling ===

//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TStageView::FrameResized(float new_width, float new_height)
{
	// Clean u old offscreen
	if (m_OffscreenBitmap) {
		m_OffscreenBitmap->Lock();
		m_OffscreenBitmap->RemoveChild(m_OffscreenView);
		m_OffscreenBitmap->Unlock();
		delete m_OffscreenView;
		delete m_OffscreenBitmap;
	}

	// Create new offscreen at correct size
	BRect newBounds(0, 0, new_width-1, new_height-1);
	m_OffscreenBitmap       = new BBitmap(newBounds, B_RGB_32_BIT, true);
	m_OffscreenView         = new BView( m_OffscreenBitmap->Bounds(), "OffscreenView", B_FOLLOW_ALL, B_WILL_DRAW);

	// Add child and fill with background color
	m_OffscreenBitmap->Lock();
	m_OffscreenBitmap->AddChild(m_OffscreenView);
	m_OffscreenView->SetViewColor(B_TRANSPARENT_32_BIT);
	m_OffscreenView->SetHighColor(kBlack);
	m_OffscreenView->FillRect(m_OffscreenView->Bounds());
	m_OffscreenView->Sync();
	m_OffscreenBitmap->Unlock();

	Looper()->Lock();
	StageDraw(Bounds(), GetCurrentTime());
	Draw(Bounds());
	Looper()->Unlock();

	// Pass to superclass
	BView::FrameResized(new_width, new_height);
}


#pragma mark -
#pragma mark === Compositing Routines ===

//---------------------------------------------------------------------
//	ClearOffscreen
//---------------------------------------------------------------------
//
//	Clear out the offscreen by filling with background color
//

void TStageView::ClearOffscreen()
{
	// Clear out with background color
	m_OffscreenBitmap->Lock();
	m_OffscreenView->SetHighColor(kBlack);
	m_OffscreenView->FillRect(m_OffscreenView->Bounds());
	m_OffscreenView->Sync();
	m_OffscreenBitmap->Unlock();
}

//---------------------------------------------------------------------
//	BlitOffscreen
//---------------------------------------------------------------------
//
//	Draw offscreen to stage
//

void TStageView::BlitOffscreen()
{
	// Update from offscreen
	Looper()->Lock();
	DrawBitmap(m_OffscreenBitmap);
	Looper()->Unlock();
}


//---------------------------------------------------------------------
//	CreateComposite
//---------------------------------------------------------------------
//
//	Given start and stop channels, iterate through the channels and
//	create and return a composited bitmap.  Calling function needs
//	to free offscreen bitmap returned
//

BBitmap* TStageView::CreateComposite(int32 startID, int32 stopID, long theTime, BRect theRect)
{

	// Create offscreen bitmap and view.  Create the view the size of the stage
	BBitmap* offscreen = new BBitmap( theRect, B_RGB32, true, false);
	ASSERT(offscreen);
	BView* offscreenView = new BView( offscreen->Bounds(), "CompositeView", B_FOLLOW_NONE, 0);
	ASSERT(offscreenView);
	offscreen->AddChild(offscreenView);

	// Lock
	offscreenView->Looper()->Lock();

	//	Fill with background stage color
	Looper()->Lock();
	offscreenView->SetHighColor(ViewColor());
	offscreenView->FillRect(Bounds());
	Looper()->Unlock();
	offscreenView->Sync();

	// Get a pointer to the cue sheet
	TCueSheetView* theCueSheet = static_cast<MuseumApp*>(be_app)->GetCueSheet()->GetCueSheetView();

	//      Now create a list of cues that can be drawn at this time within this rect,
	//	starting from the start channel and working our way up to the stop channel
	//

	BList* cueList = new BList();

	//      Find the cues that fall with in the updateRect and within the current time.
	//	Iterate through all of the cues, check their visible bounds and determine
	printf("TStageView::CreateComposite GetChannelList\n");
	//	if they may be affected. If so, add the cue to the list
	for ( int32 index = startID; index < stopID; index++) {
		TCueChannel* theChannel = (TCueChannel*)theCueSheet->GetChannelList()->ItemAt(index-1);

		// We have a valid channel.  Get cue at this time slice...
		if (theChannel) {
			TVisualCue* theCue = (TVisualCue*)theChannel->GetCueAtTime(theTime);

			// Is it a visible cue and not muted?
			if (theCue && theCue->IsVisible() && theCue->IsMuted() == false) {
				// If there is an intersection, add the cue to our update list
				if ( theRect.Intersects( theCue->CuePosition()->Enclosure()) ) {
					cueList->AddItem(theCue);
				}
			}
		}
	}

	// Now go through the list and inform all cues that need to be redrawn to do so..
	for( int32 index = 0; index < cueList->CountItems(); index++) {
		TVisualCue* drawCue = (TVisualCue*)cueList->ItemAt(index);
		drawCue->RenderData(theTime, theRect);
	}

	// Clean up list
	delete cueList;

	// Update offscreen bitmap
	offscreenView->Sync();

	// Remove child view and delete
	offscreenView->Looper()->Unlock();
	offscreen->RemoveChild(offscreenView);
	delete offscreenView;

	// Return composited bitmap
	return offscreen;
}


//---------------------------------------------------------------------
//	CopyStageBitmap
//---------------------------------------------------------------------
//
//	Return stage bitmap data clipped by rect as a new bitmap
//
//
//

BBitmap* TStageView::CopyStageBitmap(BRect theRect)
{
	//	Center rect
	BRect viewRect = theRect;
	viewRect.OffsetTo(0, 0);

	// Create offscreen bitmap and view.  Create the view the size of the stage
	BBitmap* offscreen = new BBitmap( viewRect, B_RGB32, true, false);
	ASSERT(offscreen);
	BView* offscreenView = new BView( offscreen->Bounds(), "CompositeView", B_FOLLOW_NONE, 0);
	ASSERT(offscreenView);
	offscreen->AddChild(offscreenView);

	// Lock
	offscreenView->Looper()->Lock();

	//	Copy m_OffscreenBitmap data
	offscreenView->DrawBitmap(m_OffscreenBitmap, theRect, offscreen->Bounds());

	// Update offscreen bitmap
	offscreenView->Sync();

	// Remove child view and delete
	offscreenView->Looper()->Unlock();
	offscreen->RemoveChild(offscreenView);
	delete offscreenView;

	// Return composited bitmap
	return offscreen;
}

#pragma mark -
#pragma mark === Thread Routines ===


//---------------------------------------------------------------------
//	CreateStageOffscreen
//---------------------------------------------------------------------
//
//

thread_id TStageView::CreateStageOffscreen()
{
	thread_id StageOffscreenThread = spawn_thread(stage_offscreen, "StageOffscreen", B_NORMAL_PRIORITY, this);

	if(StageOffscreenThread != B_NO_MORE_THREADS && StageOffscreenThread != B_NO_MEMORY)
		resume_thread(StageOffscreenThread);

	return StageOffscreenThread;
}


//---------------------------------------------------------------------
//	StageOffscreen
//---------------------------------------------------------------------
//
//

int32 TStageView::StageOffscreen()
{
	// Create offscreen bitmap	and view
	m_OffscreenBitmap       = new BBitmap(Bounds(), B_RGB_32_BIT, true);
	m_OffscreenView         = new BView( m_OffscreenBitmap->Bounds(), "OffscreenView", B_FOLLOW_ALL, B_WILL_DRAW);

	// Add child and fill with background color
	if ( m_OffscreenBitmap->Lock()) {
		m_OffscreenBitmap->AddChild(m_OffscreenView);
		m_OffscreenView->SetViewColor(B_TRANSPARENT_32_BIT);
		m_OffscreenView->SetHighColor(kBlack);
		m_OffscreenView->FillRect(m_OffscreenView->Bounds());
		m_OffscreenView->Sync();
		m_OffscreenBitmap->Unlock();
	}

	// Loop
	while(true) {
		snooze(50 * 1000);
	}
}


//---------------------------------------------------------------------
//	stage_offscreen
//---------------------------------------------------------------------
//
//	Static thread function
//

int32 TStageView::stage_offscreen(void* arg)
{
	TStageView* obj = (TStageView*)arg;
	return(obj->StageOffscreen() );
}


#pragma mark -
#pragma mark === StageCue List Handling ===

//------------------------------------------------------------------
//	ClearStageCueList
//------------------------------------------------------------------
//
//	Empty list of any StageCues
//

void TStageView::ClearStageCueList()
{
	//	Deselect items in list
	for (int32 index = 0; index < m_StageCueList->CountItems(); index++) {
		TStageCue* stageCue = (TStageCue*)m_StageCueList->ItemAt(index);
		if (stageCue) {
			stageCue->Deselect();
			delete stageCue;
			stageCue = NULL;
		}
	}

	//	Now clear the list out...
	m_StageCueList->MakeEmpty();
}


#pragma mark -
#pragma mark === Stage Tools Handling ===

//---------------------------------------------------------------------
//	SetToolMode
//---------------------------------------------------------------------
//
//	Set the stage tool mode.  This determines the type of cursor
//	to display and edit to make.
//

void TStageView::SetToolMode(uint32 theTool)
{
	switch(theTool)
	{
	case MOVE_TOOL_MSG:
		m_ToolMode = kMoveMode;
		break;

	case ROTATE_TOOL_MSG:
		m_ToolMode = kRotateMode;
		break;

	case CROP_TOOL_MSG:
		m_ToolMode = kCropMode;
		break;

	case SCALE_TOOL_MSG:
		m_ToolMode = kScaleMode;
		break;

	case SHEAR_TOOL_MSG:
		m_ToolMode = kShearMode;
		break;

	case SKEW_TOOL_MSG:
		m_ToolMode = kSkewMode;
		break;

	case PERSPECTIVE_TOOL_MSG:
		m_ToolMode = kPerspectiveMode;
		break;

	case MIRROR_TOOL_MSG:
		m_ToolMode = kMirrorMode;
		break;

	default:
		m_ToolMode = kMoveMode;
		break;

	}
}
