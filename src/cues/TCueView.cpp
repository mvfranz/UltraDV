//---------------------------------------------------------------------
//
//	File:	TCueView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	This class acts as the superclass that all cues are derived
//			form.  This view represents itself visually in the cue
//			channel area.  For each CueView there is an associated
//			CueContentView that is attached to the stage.  This view
//			may or may not be visible on the stage depending on type.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MenuMessages.h"
#include "MuseumApp.h"
#include "TMuseumIcons.h"
#include "AppUtils.h"
#include "DebugUtils.h"
#include "ResourceManager.h"

#include "TCursor.h"
#include "MuseumApp.h"
#include "TMuseumClipboard.h"
#include "TCueChannel.h"
#include "TCueChannelHeader.h"
#include "TCueSheetView.h"
#include "TCueSheetWindow.h"
#include "TCueTimeText.h"
#include "TTimePalette.h"
#include "TTimePaletteView.h"
#include "TTimelineView.h"
#include "TBitmapView.h"
#include "TStageWindow.h"
#include "TStageView.h"
#include "TBrowserWindow.h"

#include "TBitmapButton.h"

#include "TPlaybackEngine.h"

#include "TCueMenu.h"
#include "TCueEffect.h"
#include "TCueEffectView.h"
#include "TTransitionMenu.h"

#include "TCueView.h"
#include "BaseCueChunk.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueView::TCueView(int16 id, TCueChannel* parent, BRect bounds, uint32 startTime, char* name) :
	BView(bounds, name, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW | B_FRAME_EVENTS | B_PULSE_NEEDED )
{
	// Get cue ID
	fID = id;

	// Save parent
	fChannel = parent;

	// Save start time and bounds
	fStartTime = startTime;

	// The BView class will set this to true when it is done
	fIsInstantiated = false;

	// Save start time for later initialization
	fInsertTime     = startTime;

	// Set these variables to NULL
	fFile             = NULL;
	fAntThread   = NULL;
	fEffectsList = NULL;

	//	Set these to false
	fEditorOpen = false;

	// Set buttons to NULL
	fLockButton                     = NULL;
	fMuteButton                     = NULL;
}

//---------------------------------------------------------------------
//	ABH - missing Constructor
//---------------------------------------------------------------------
//
//

TCueView::TCueView(int16 id, TCueChannel* parent, BRect bounds, BPoint point, uint32 startTime, char* name) :
	BView(bounds, name, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW|B_FRAME_EVENTS )
{
	// do something with scale, save it???
	fInsertPoint = point; // ???

	// Get cue ID
	fID = id;

	// Save parent
	fChannel = parent;

	// Save start time and bounds
	fStartTime = startTime;


	// The BView class will set this to true when it is done
	fIsInstantiated = false;

	// Save start time for later initialization
	fInsertTime     = startTime;

	// Set these variables to NULL
	fFile             = NULL;
	fAntThread   = NULL;
	fEffectsList = NULL;

	//	Set these to false
	fEditorOpen = false;

	// Set buttons to NULL
	fLockButton                     = NULL;
	fMuteButton                     = NULL;
}


//
// ABH new constructor
//

TCueView::TCueView(BaseCueChunk* TheChunk, TCueChannel* parent, BRect bounds, char* name) :
	BView(bounds, name, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW|B_FRAME_EVENTS )
{
	// Get cue ID
//	fID = id;

	// Save parent
	fChannel = parent;

	// Save start time and bounds
	fStartTime = TheChunk->startTime;

	// The BView class will set this to true when it is done
	fIsInstantiated = false;

	// Save start time for later initialization
//	fInsertTime     = startTime;

	// Set these variables to NULL
	fFile             = NULL;
	fAntThread   = NULL;
	fEffectsList = NULL;

	//	Set these to false
	fEditorOpen = false;

	// Set buttons to NULL
	fLockButton                     = NULL;
	fMuteButton                     = NULL;
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct based on an existing cue
//

TCueView::TCueView(const TCueView* theCue) :
	BView(theCue->Bounds(), ((BView*)theCue)->Name(), B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW | B_FRAME_EVENTS )
{
	fID                             = theCue->fID;
	fChannel                        = theCue->fChannel;
	fStartTime              = theCue->fStartTime;
	fIsInstantiated         = theCue->fIsInstantiated;
	fInsertTime             = theCue->fInsertTime;
	fIsExpanded             = theCue->fIsExpanded;

	fFile                           = theCue->fFile;

	//fLockButton           = new TCueLockButton(theCue->fLockButton);
	//fMuteButton           = new TCueMuteButton(theCue->fMuteButton);
}


//---------------------------------------------------------------------
//	TCueView
//---------------------------------------------------------------------
//
//	Create new cue from a BMessage
//

TCueView::TCueView(BMessage* data) : BView (data)
{
	// Set buttons to NULL
	fLockButton     = NULL;
	fMuteButton     = NULL;
	fChannel                = NULL;
	fFile                   = NULL;
	fAntThread      = NULL;
	fEffectsList    = NULL;

	// Set default values
	fIsSelected = false;

	//
	// Rehydrate the cue from message data
	//

	// Extract our member variables from the archive
	ssize_t numBytes;
	rgb_color* color;

	data->FindData("LowColor", B_RGB_COLOR_TYPE, (const void**)&color, &numBytes );
	fLowColor = *color;
	data->FindData("HighColor", B_RGB_COLOR_TYPE, (const void**)&color, &numBytes );
	fHighColor = *color;

	data->FindInt32("DrawingMode", fDrawingMode);

	fFileTime = data->FindInt32("FileTime");
	fStartTime = data->FindInt32("StartTime");
	fPreparedTime = data->FindInt32("PreparedTime");
	fDuration = data->FindInt32("Duration");
	fStartOffset = data->FindInt32("StartOffset");

	data->FindBool("CanTransition", &fCanTransition);
	data->FindBool("CanPath", &fCanPath);

	data->FindBool("CanStretch", &fCanStretch);
	data->FindBool("CanWindow", &fCanWindow);
	data->FindBool("CanLoop", &fCanLoop);
	data->FindBool("CanEnvelope", &fCanEnvelope);
	data->FindBool("HasDuration", &fHasDuration);
	data->FindBool("IsVisible", &fIsVisible);
	data->FindBool("CanCrop", &fCanCrop);

	data->FindBool("HasEditor", &fHasEditor);
	data->FindBool("EditorOpen", &fEditorOpen);

	data->FindBool("HasExternalEditor", &fHasExternalEditor);

	data->FindInt16("ID", &fID);

	data->FindBool("IsLocked", &fIsLocked);
	data->FindBool("IsExpanded", &fIsExpanded);
	data->FindBool("IsMuted", &fIsMuted);

	data->FindRef("FileRef", &fFileRef);

	data->FindBool("ShowDuration", &fShowDuration);

	CueDisplay* displayMode;
	data->FindData("CueDisplayMode", B_ANY_TYPE, (const void**)&displayMode, &numBytes);
	fCueDisplayMode = *displayMode;

	// Create resize zones
	const BRect bounds = Bounds();
	fLResizeZone.Set(bounds.left, bounds.top, bounds.left+kResizeZoneWidth, bounds.bottom);
	fRResizeZone.Set(bounds.right-kResizeZoneWidth, bounds.top, bounds.right, bounds.bottom );

	//	Set up our child item pointers
	fStartText = (TCueTimeText*)FindView("StartTimeTextView");
	fEndText        = (TCueTimeText*)FindView("EndTimeTextView");

	fLockButton = (TBitmapButton*)FindView("CueLockButton");
	fMuteButton = (TBitmapButton*)FindView("CueMuteButton");
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueView::~TCueView()
{
	// Stop marching ants and destroy MarchingThread
	if (fAntThread) {
		delete fAntThread;
		fAntThread = NULL;
	}

	if(fIsPlaying) {
		StopCue(GetCurrentTime());
	}

	//	Empty out effects list
	if (fEffectsList) {
		for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
			//	Get the effects in the list
			TCueEffectView* effectView = (TCueEffectView*)fEffectsList->ItemAt(index);
			if (effectView)
				delete effectView;
		}

		delete fEffectsList;
	}
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TCueView::Init()
{
// ABH
	fRotation = 0;

	// We handle our own drawing
	SetViewColor(B_TRANSPARENT_32_BIT);

	//	Set up marching ants
	fCueAnts.data[0] = 0x1F;
	fCueAnts.data[1] = 0x3E;
	fCueAnts.data[2] = 0x7C;
	fCueAnts.data[3] = 0xF8;
	fCueAnts.data[4] = 0xF1;
	fCueAnts.data[5] = 0xE3;
	fCueAnts.data[6] = 0xC7;
	fCueAnts.data[7] = 0x8F;

	// We are displaying system cursor
	fAppCursorSet = true;

	//	Set up member variables
	fIsPlaying              = false;
	fIsStopping             = false;

	// Set up default settings
	fIsPrepared             = false;
	fIsPaused                       = false;
	fMouseDown              = false;
	fResizing                       = false;
	fIsMuted                        = false;
	fShowDuration           = false;
	fEffectsVisible = false;

	fDrawingMode            = B_OP_COPY;
	fLowColor                       = kWhite;
	fHighColor                      = kBlack;

	fCueDisplayMode = kCueControls;

	// Mouse tracking
	fLastClickPt.Set(0, 0);

	// Load cue icon
	LoadCueIcon();

	// Create resize zones
	BRect bounds = Bounds();
	fLResizeZone.Set(bounds.left, bounds.top, bounds.left+kResizeZoneWidth, bounds.bottom);
	fRResizeZone.Set(bounds.right-kResizeZoneWidth, bounds.top, bounds.right, bounds.bottom );

	//	Set effect tray to bogus value
	fEffectsTray.Set(-500, 5000, -1, -1);

	//
	// Create and show start and stop time indicators
	//

	char timeStr[256];

	// Start time is attached to the top left side of the cue rect
	bounds = Bounds();
	bounds.left     += kTimeTextOffset+kResizeZoneWidth;
	bounds.top      += kTimeTextOffset;
	bounds.right    =  bounds.left + kTimeTextWidth;
	bounds.bottom   =  bounds.top + kTimeTextHeight;

	fStartText = new TCueTimeText(this, START_TIME_TEXT_FOCUS_MSG, bounds, "StartTimeTextView", B_FOLLOW_LEFT | B_FOLLOW_TOP, kTextOutline);
	AddChild(fStartText);

	// Set proper time
	TimeToString(fStartTime/1000, fChannel->GetCueSheet()->GetTimeFormat(), timeStr, false);
	fStartText->SetText(timeStr);

	// End time is attached to the bottom right side of the cue rect
	if (fHasDuration) {
		bounds = Bounds();
		bounds.right    -= kTimeTextOffset+kResizeZoneWidth;
		bounds.top      =  bounds.bottom - (kTimeTextHeight+kTimeTextOffset);
		bounds.left     =  bounds.right - kTimeTextWidth;
		bounds.bottom   -= kTimeTextOffset;

		fEndText = new TCueTimeText(this, END_TIME_TEXT_FOCUS_MSG, bounds, "EndTimeTextView", B_FOLLOW_RIGHT | B_FOLLOW_TOP, kTextOutline);
		AddChild(fEndText);

		// Set proper time
		TimeToString(fStartTime+fDuration, fChannel->GetCueSheet()->GetTimeFormat(), timeStr, false);
		fEndText->SetText(timeStr);

		// Set up location of duration delta rect
		const BRect textRect = bounds;
		fDurationDeltaRect.top  = textRect.top;
		fDurationDeltaRect.bottom       = textRect.bottom;
		fDurationDeltaRect.right        = textRect.left - kDurationDeltaOffset;
		fDurationDeltaRect.left         = fDurationDeltaRect.right - fDurationDeltaRect.Height();
		fDurationDeltaRect.InsetBy(2,2);
	} else
		fEndText = NULL;

	//      Create Cue Buttons
	//

	BRect buttonRect;

	//	Cue Lock Button
	buttonRect.Set( fCueIcon->Frame().right+kCueButtonOffset, fCueIcon->Frame().top, fCueIcon->Frame().right+kCueButtonWidth+kCueButtonOffset, fCueIcon->Frame().top + kCueButtonHeight );
	fLockButton = new TBitmapButton( buttonRect, "CueLockButton", GetAppIcons()->fUnlock, GetAppIcons()->fLock,
	                                 this, new BMessage(LOCK_CUE_MSG), false, B_TWO_STATE_BUTTON);
	AddChild(fLockButton);

	// Cue Mute Button
	buttonRect.Set( fLockButton->Frame().right + kCueButtonOffset, buttonRect.top, fLockButton->Frame().right+kCueButtonWidth+kCueButtonOffset, buttonRect.bottom );
	fMuteButton = new TBitmapButton( buttonRect, "CueMuteButton", GetAppIcons()->fVisibleUp, GetAppIcons()->fVisibleDown,
	                                 this, new BMessage(MUTE_BUTTON_MSG), false, B_TWO_STATE_BUTTON);
	AddChild(fMuteButton);


	//	Create Cue Effects Channel Spinner.  Locate it just below the Cue Lock Button
	//

	buttonRect.Set( fLockButton->Frame().left,  fLockButton->Frame().bottom+kCueButtonOffset,
	                fLockButton->Frame().right, fLockButton->Frame().bottom+kCueButtonOffset+kCueButtonHeight);
	fEffectsButton = new TBitmapButton( buttonRect, "EffectsButton", GetAppIcons()->fExpand, GetAppIcons()->fContract,
	                                    this, new BMessage(CUE_EXPAND_MSG), false, B_TWO_STATE_BUTTON);
	AddChild(fEffectsButton);
	fEffectsButton->Show();

	//	Create effects list
	fEffectsList = new BList();
}

//
// missing function
//
void TCueView::Init(BaseCueChunk* TheChunk)
{

// Should probably do more, but what?

	Init();

}

#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TCueView::Instantiate(BMessage* archive)
{

	if ( validate_instantiation(archive, "TCueView") )
		return new TCueView(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueView::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TCueView");

		// Add our member variables to the archive
		data->AddData("LowColor", B_RGB_COLOR_TYPE, &fLowColor, sizeof(rgb_color) );
		data->AddData("HighColor", B_RGB_COLOR_TYPE, &fHighColor, sizeof(rgb_color) );
		data->AddInt32("DrawingMode", (int32)fDrawingMode);

		data->AddInt32("FileTime", fFileTime);
		data->AddInt32("StartTime", fStartTime);
		data->AddInt32("PreparedTime", fPreparedTime);
		data->AddInt32("Duration", fDuration);
		data->AddInt32("StartOffset", fStartOffset);

		data->AddBool("CanTransition", fCanTransition);
		data->AddBool("CanPath", fCanPath);

		data->AddBool("CanStretch", fCanStretch);
		data->AddBool("CanWindow", fCanWindow);
		data->AddBool("CanLoop", fCanLoop);
		data->AddBool("CanEnvelope", fCanEnvelope);
		data->AddBool("HasDuration", fHasDuration);
		data->AddBool("IsVisible", fIsVisible);
		data->AddBool("CanCrop", fCanCrop);

		data->AddBool("HasEditor", fHasEditor);
		data->AddBool("EditorOpen", fEditorOpen);

		data->AddBool("HasExternalEditor", fHasExternalEditor);

		data->AddInt16("ID", fID);

		data->AddBool("IsLocked", fIsLocked);
		data->AddBool("IsExpanded", fIsExpanded);
		data->AddBool("IsMuted", fIsMuted);

		data->AddRef("FileRef", &fFileRef);

		data->AddBool("ShowDuration", fShowDuration);
		data->AddData("CueDisplayMode", B_ANY_TYPE, &fCueDisplayMode, sizeof(CueDisplay) );

		// Add attached views
		if (deep) {
		}
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
//

void TCueView::Draw(BRect updateRect)
{
	// Save environment
	Looper()->Lock();
	PushState();

	const BRect bounds = Bounds();

	// Fill cue area
	SetHighColor(kBlueGrey);
	FillRect(updateRect);

	// Draw resize zones if transitions and keyframe views are off
	if ( fDuration && fCanStretch) {
		BPoint startPt, endPt;
		SetHighColor(kSteelBlue);
		FillRect(fRResizeZone);
		FillRect(fLResizeZone);

		// Draw duration delta?
		if (fShowDuration) {
			DrawDurationDelta();
		}

		// Left side highlights
		SetHighColor(kDarkSteelBlue);
		startPt.Set(fLResizeZone.left, fLResizeZone.top);
		endPt.Set(fLResizeZone.left, fLResizeZone.bottom);
		StrokeLine(startPt, endPt);
		SetHighColor(kMediumSteelBlue);
		startPt.x++;
		endPt.x++;
		StrokeLine(startPt, endPt);

		SetHighColor(kDarkSteelBlue);
		startPt.Set(fLResizeZone.right, fLResizeZone.top);
		endPt.Set(fLResizeZone.right, fLResizeZone.bottom);
		StrokeLine(startPt, endPt);
		SetHighColor(kMediumSteelBlue);
		startPt.x--;
		endPt.x--;
		StrokeLine(startPt, endPt);

		// Left Shadow
		SetHighColor(kDarkGrey);
		startPt.Set(fLResizeZone.right+1, fLResizeZone.top);
		endPt.Set(fLResizeZone.right+1, fLResizeZone.bottom);
		StrokeLine(startPt, endPt);

		// Right side highlights
		SetHighColor(kDarkSteelBlue);
		startPt.Set(fRResizeZone.right, fRResizeZone.top);
		endPt.Set(fRResizeZone.right, fRResizeZone.bottom);
		StrokeLine(startPt, endPt);
		SetHighColor(kMediumSteelBlue);
		startPt.x--;
		endPt.x--;
		StrokeLine(startPt, endPt);

		SetHighColor(kDarkSteelBlue);
		startPt.Set(fRResizeZone.left, fRResizeZone.top);
		endPt.Set(fRResizeZone.left, fRResizeZone.bottom);
		StrokeLine(startPt, endPt);
		SetHighColor(kMediumSteelBlue);
		startPt.x++;
		endPt.x++;
		StrokeLine(startPt, endPt);

		// Right Shadow
		SetHighColor(kDarkGrey);
		startPt.Set(fRResizeZone.left-1, fLResizeZone.top);
		endPt.Set(fRResizeZone.left-1, fLResizeZone.bottom);
		StrokeLine(startPt, endPt);
	}

	// Highlights
	SetHighColor(kPaleGrey);
	BPoint startPt, endPt;
	startPt.Set(bounds.left, bounds.bottom);
	endPt.Set(bounds.left, bounds.top);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.left, bounds.top);
	endPt.Set(bounds.right, bounds.top);
	StrokeLine(startPt, endPt);

	// Shadow
	SetHighColor(kDarkGrey);
	startPt.Set(bounds.right, bounds.top);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(startPt, endPt);
	startPt.Set(bounds.right, bounds.bottom);
	endPt.Set(bounds.left, bounds.bottom);
	StrokeLine(startPt, endPt);

	//	Effect tray
	DrawEffectsTray(updateRect);

	// Indicate that cue is selected
	if (fIsSelected)
		DoMarchingAnts(bounds);

	// Restore environment
	PopState();
	Looper()->Unlock();
}

// ABH missing function???

uint32 TCueView::GetDuration()
{
	printf("GetDuration called\n");
	return fDuration;
}

// ABH missing function

void TCueView::SetTransitionInID(uint32 id)
{
	fTransitionInID = id;
}

// ABH missing function

void TCueView::SetTransitionOutID(uint32 id)
{
	fTransitionOutID = id;
}

// ABH missing function

void TCueView::SetTransitionInDuration(uint32 length)
{
	fTransitionInDuration = length;
}

// ABH missing function

void TCueView::SetTransitionOutDuration(uint32 length)
{
	fTransitionOutDuration = length;
}

// ABH missing function

uint32 TCueView::GetTransitionInDuration()
{
	return fTransitionInDuration;
}

// ABH missing function

uint32 TCueView::GetTransitionOutDuration()
{
	return fTransitionOutDuration;
}
//---------------------------------------------------------------------
//	DrawDurationDelta
//---------------------------------------------------------------------
//
//	Draw duration delta mark to the left of the fEndText
//

void TCueView::DrawDurationDelta()
{
	if (fShowDuration) {
		if (fEndText) {
			fEndText->Looper()->Lock();
			const BRect textRect = fEndText->Frame();
			fEndText->Looper()->Unlock();

			Looper()->Lock();

			BPoint startPt, endPt;
			SetHighColor(kBlack);

			// Bottom right to bottom left
			startPt.Set(fDurationDeltaRect.right, fDurationDeltaRect.bottom);
			endPt.Set(fDurationDeltaRect.left, fDurationDeltaRect.bottom);
			StrokeLine(startPt, endPt);
			// Bottom left to top center
			startPt.Set(fDurationDeltaRect.left, fDurationDeltaRect.bottom);
			endPt.Set(fDurationDeltaRect.right - (fDurationDeltaRect.Width()/2), fDurationDeltaRect.top);
			StrokeLine(startPt, endPt);
			// Top center to bottom right
			startPt.Set(fDurationDeltaRect.right - (fDurationDeltaRect.Width()/2), fDurationDeltaRect.top);
			endPt.Set(fDurationDeltaRect.right, fDurationDeltaRect.bottom);
			StrokeLine(startPt, endPt);

			Looper()->Unlock();
		}
	}
}


//---------------------------------------------------------------------
//	DrawEffectsTray
//---------------------------------------------------------------------
//
//	Draw effects tray and any attached effects
//

void TCueView::DrawEffectsTray(BRect updateRect)
{
	if (fEffectsVisible) {
		BPoint startPt, endPt;

		const BRect bounds = Bounds();
		const int32 addAmount = kChannelMaxHeight - (kCueInset*2);

		SetHighColor(kHeaderGrey);
		FillRect(fEffectsTray);

		// Top border
		SetHighColor(kDarkGrey);
		startPt.Set(bounds.left+1, bounds.top+addAmount);
		endPt.Set(bounds.right-1,  bounds.top+addAmount);
		StrokeLine(startPt, endPt);
		SetHighColor(kLightGrey);
		startPt.Set(bounds.left+1, bounds.top+addAmount+1);
		endPt.Set(bounds.right-1,  bounds.top+addAmount+1);
		StrokeLine(startPt, endPt);

		// Draw hilight
		SetHighColor(kDarkGrey);
		startPt.Set(bounds.left+3, bounds.bottom-3);
		endPt.Set(bounds.left+3, bounds.top+addAmount+4);
		StrokeLine(startPt, endPt);
		endPt.Set(bounds.right-3, bounds.top+addAmount+4);
		StrokeLine(endPt);

		// Draw Shadow
		SetHighColor(kLightGrey);
		startPt.Set(bounds.right-3, bounds.top+addAmount+4);
		endPt.Set(bounds.right-3, bounds.bottom-3);
		StrokeLine(startPt, endPt);
		endPt.Set(bounds.left+3, bounds.bottom-3);
		StrokeLine(endPt);

		//	Draw divider lines
		for (int32 index = fEffectsTray.top + (8+kEffectHeight); index < bounds.bottom; index += kEffectHeight) {
			startPt.Set(bounds.left+3, index);
			endPt.Set(bounds.right-4, index);
			SetHighColor(kLightGrey);
			StrokeLine(startPt, endPt);
			startPt.y++; endPt.y++;
			SetHighColor(kDarkGrey);
			StrokeLine(startPt, endPt);
		}

		DrawEffects(updateRect);
	}
}


//---------------------------------------------------------------------
//	DrawEffects
//---------------------------------------------------------------------
//
//	Draw attached effects
//

void TCueView::DrawEffects(BRect updateRect)
{
	//	Are there any effects to be drawn?
	if (fEffectsList->CountItems() > 0) {
		for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
			//	Get the effects in the list
			TCueEffectView* effect = (TCueEffectView*)fEffectsList->ItemAt(index);
			if (effect) {
				effect->Draw(updateRect);
			}
		}
	}
}

#pragma mark -
#pragma mark === Mouse Routines ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TCueView::MouseDown(BPoint where)
{
	// Do nothing if we are playing
	if ( ((TCueSheetWindow*)fChannel->Window())->GetPlaybackEngine()->IsPlaying() )
		return;

	// Make sure we aren't locked
	if ( fChannel->IsLocked() || fIsLocked )
		return;

	// We have the focus
	MakeFocus(true);

	// Set flag
	fMouseDown = true;

	// Check to see which button is down
	uint32 buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (int32*)&buttons);

	// Is this a double click?  If so, open editor
	uint32 type;
	int32 count = 0;
	BMessage* message = Window()->CurrentMessage();

	// Determine which button has been clicked
	switch(buttons)
	{
	case B_PRIMARY_MOUSE_BUTTON:
	{
		if (B_OK == message->GetInfo("clicks", &type, &count) ) {
			int32 clickCount = message->FindInt32("clicks", count-1);
			if ( (clickCount == 2) && (where == fLastClickPt) ) {
				//	In effects tray?
				if ( EffectsClick(where) ) {
					TCueEffectView* effect = ClickedEffect(where);
					if (effect) {
						//	See if we need to deselect other effects
						if (effect->Selected() == false)
							DeselectAllEffects();

						//	Give it the double click
						effect->MouseDown(where, true);
					}
					//	Deselect all
					else
						DeselectAllEffects();
				} else {
					if(fHasEditor)
						OpenEditor();
				}
			}
			//	Handle single click
			else{
				//	Was the click in the Effects tray?
				if ( EffectsClick(where) ) {
					TCueEffectView* effect = ClickedEffect(where);
					if (effect) {
						//	See if we need to deselect other effects
						if (effect->Selected() == false)
							DeselectAllEffects();

						//	Give it the click
						effect->MouseDown(where);
					}
					//	Deselect all
					else
						DeselectAllEffects();
				}
				//	Click is in cue view
				else{
					//	Check cue display state and handle properly
					switch (fCueDisplayMode)
					{
					case kCueControls:
						MouseDownControls(where);
						break;

					case kCueContents:
						break;

					default:
						TRESPASS();
						break;
					}
				}
			}
		}
		break;

	// If the second mouse button is down, open cue menu
	case B_SECONDARY_MOUSE_BUTTON:
	{
		// Deselect all other cues
		fChannel->GetCueSheet()->DeselectAllCues();
		Select();

		//	Is the click in the cue body or the effect tray?
		if ( EffectsClick(where) ) {
			OpenTransitionMenu(where);
		} else {
			OpenCueMenu(where);
		}
	}
	break;
	}
	}

	// Wait a while for the mouse button to be realeased
	snooze(250 * 1000);

	// Is button down?  They are dragging or resizing the cue...
	// Check to see if button is down
	Window()->CurrentMessage()->FindInt32("buttons", (int32*)&buttons);

	if (buttons && fMouseDown) {
		//      If the left mouse button is down, they are doing the following:
		//	Dragging the cue either within the channel or to another channel,
		//	stretching the cue or dragging a transition duration
		if (buttons == B_PRIMARY_MOUSE_BUTTON) {
			//	In effects tray?
			if ( EffectsClick(where) ) {
				TCueEffectView* effect = ClickedEffect(where);
				if (effect)
					effect->MouseMoved(where);
			} else {
				//	Check cue display state and handle properly
				switch (fCueDisplayMode)
				{
				case kCueControls:
					MouseDragControls(where);
					break;

				case kCueContents:
					break;

				default:
					TRESPASS();
					break;
				}
			}
		}

		// Set our flag to indicate mouse up happened
		fMouseDown = false;

		// Save position for double click compare
		fLastClickPt = where;
	}
}

//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TCueView::MouseUp(BPoint where)
{
	// Make sure we aren't locked
	if ( fChannel->IsLocked() || fIsLocked )
		return;

	fMouseDown = false;
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TCueView::MouseMoved( BPoint where, uint32 code, const BMessage* a_message )
{
	// Make sure we aren't locked
	if ( fChannel->IsLocked() || fIsLocked ) {
		// Inform channel of mouse move.  We are adding the left element of
		// the frame to create the proper coordinate for the timeline
		where.x += Frame().left;
		fChannel->MouseMoved(where, code, a_message);
		return;
	}

	// Display resizing cursors if mouse button is up
	uint32 buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (int32*)&buttons);
	if (fCanStretch && !buttons) {
		switch(code)
		{

		case B_ENTERED_VIEW:
			if ( fRResizeZone.Contains(where) || fLResizeZone.Contains(where) ) {
				be_app->SetCursor(&static_cast<MuseumApp*>(be_app)->fResizeHorzCursor );
			}
			break;

		case B_INSIDE_VIEW:
			if ( fRResizeZone.Contains(where) || fLResizeZone.Contains(where) )
				be_app->SetCursor(&static_cast<MuseumApp*>(be_app)->fResizeHorzCursor );
			else{
				be_app->SetCursor(B_HAND_CURSOR);
			}
			break;

		case B_EXITED_VIEW:
			be_app->SetCursor(B_HAND_CURSOR);
			break;

		default:
			break;
		}
	}

	// Inform channel of mouse move.  We are adding the left element of
	// the frame to create the proper coordinate for the timeline
	where.x += (Frame().left + fChannel->Bounds().left);
	fChannel->MouseMoved(where, code, a_message);
}

//---------------------------------------------------------------------
//	MouseDownControls
//---------------------------------------------------------------------
//
//	Handle mouse down down in control mode
//

void TCueView::MouseDownControls(BPoint where)
{
	// If shift key is down the user is group selecting/deselecting
	if (IsShiftKeyDown()) {
		if(fIsSelected)
			Deselect();
		else
			Select();
	} else {
		// Deselect all other cues
		if (fIsSelected)
			fChannel->GetCueSheet()->DeselectAllCues(this);
		else{
			Select();
			fChannel->GetCueSheet()->DeselectAllCues(this);
		}
	}
}

//---------------------------------------------------------------------
//	MouseDragControls
//---------------------------------------------------------------------
//
//	Handle mouse down drag over control mode
//

void TCueView::MouseDragControls(BPoint where)
{
	//
	//	Check and see if they are resizing
	//	First check and see if they are in the right resize zone...
	//
	if( fRResizeZone.Contains(where) && fCanStretch ) {
		ResizeRightSide();

		// Fix drag zones
		UpdateResizeZones();
	} else if( fLResizeZone.Contains(where) && fCanStretch) {
		ResizeLeftSide();

		// Fix drag zones
		UpdateResizeZones();
	}
	//
	// They must be moving the cue...
	//
	else{
		BRect bounds = Bounds();

		BMessage message(CUE_DRAG_MSG);
		message.AddPointer("CueView", this);
		BRect frame = Frame();
		BPoint dragPt = where;
		ConvertToParent(&dragPt);
		uint32 delta = dragPt.x - (frame.left + fChannel->Bounds().left);
		message.AddInt32("CueDelta", delta);

		// Center the rect within the mouse for a better drag appearence
		static_cast<MuseumApp*>(be_app)->fIsCueDrag = true;
		static_cast<MuseumApp*>(be_app)->fCueDragDelta = delta;

		DragMessage(&message, bounds);
	}
}

//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TCueView::WindowActivated(bool state)
{

}


#pragma mark -
#pragma mark === Keyboard Handling ===

//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TCueView::KeyDown(const char* bytes, int32 numBytes)
{
	//	Igonore if we are locked
	if ( fChannel->IsLocked() || fIsLocked )
		return;

	//	Determine display mode and perform proper keydown action
	//	Determine drawing state
	switch (fCueDisplayMode)
	{
	case kCueControls:
	{
		//	Do we have effect/effects to delete?
		if (fEffectsVisible && fEffectsList->CountItems() > 0) {
			DeleteSelectedEffects();
		}
		//	Give click to channel to sort out
		else{
			fChannel->KeyDown(bytes, numBytes);
		}
	}
	break;

	case kCueContents:
		fChannel->KeyDown(bytes, numBytes);
		break;

	default:
		TRESPASS();
		break;
	}
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TCueView::KeyUp(const char* bytes, int32 numBytes)
{
	BView::KeyUp(bytes, numBytes);
}


#pragma mark -
#pragma mark === Scroll Handling ===

//---------------------------------------------------------------------
//	ScrollBy
//---------------------------------------------------------------------
//
//

void TCueView::ScrollBy(float horz, float vert)
{
	BView::ScrollBy(horz, vert);
}


#pragma mark -
#pragma mark === Frame Handling ===

//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TCueView::FrameMoved(BPoint new_position)
{
	BView::FrameMoved(new_position);
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TCueView::FrameResized(float new_width, float new_height)
{
	//  Resize the resize zones of the cue and attached effects
	if (fCanStretch) {
		UpdateResizeZones();

		//	Update effects
		for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
			TCueEffectView* effect = (TCueEffectView*)fEffectsList->ItemAt(index);
			if(effect)
				effect->Resized();
		}
	}

	// Fix duration delta location
	if (fHasDuration) {
		if (fEndText) {
			// Set up location of duration delta rect
			const BRect textRect = fEndText->Frame();
			fDurationDeltaRect.top  = textRect.top;
			fDurationDeltaRect.bottom       = textRect.bottom;
			fDurationDeltaRect.right        = textRect.left - kDurationDeltaOffset;
			fDurationDeltaRect.left         = fDurationDeltaRect.right - fDurationDeltaRect.Height();
			fDurationDeltaRect.InsetBy(2,2);
		}
	}

	//	Force redraw
	Invalidate();
}

#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent CueChannel and are in the
//	the channels cue list.  This is intended for use when instantiating
//	cues from an archive.
//

void TCueView::AttachedToWindow()
{
	if(fChannel == NULL) {
		fChannel = (TCueChannel*)Parent();

		BList* cueList = fChannel->GetCueList();

		if ( cueList->HasItem(this) == false)
			cueList->AddItem(this);
	}

	BView::AttachedToWindow();
}

//---------------------------------------------------------------------
//	DetachedFromWindow
//---------------------------------------------------------------------
//
//	Hide all attached controls
//

void TCueView::DetachedFromWindow()
{
	BView::DetachedFromWindow();
}


//---------------------------------------------------------------------
//	Show
//---------------------------------------------------------------------
//
//	Update all cue control visibility
//

void TCueView::Show()
{
	BView::Show();
}


//---------------------------------------------------------------------
//	Hide
//---------------------------------------------------------------------
//
//	Update all cue control visibility
//

void TCueView::Hide()
{
	BView::Hide();
}


//---------------------------------------------------------------------
//	ChannelInsert
//---------------------------------------------------------------------
//
//	Cue has been inserted into a channel.  Perform needed updates
//

void TCueView::ChannelInsert()
{
	UpdateResizeZones();
	UpdateStartTimeText();
	UpdateEndTimeText();
}

#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//

void TCueView::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		if ( message->WasDropped() ) {
		// Handle both of these cases by calling the channel's
		// to the cue's channel
		case CUE_DRAG_MSG:
		case CUE_ICON_DRAG_MSG:
			fChannel->MessageReceived(message);
			break;
		}

	case LOCK_CUE_MSG:
	{
		// Ignore if channel is locked
		if (fChannel->IsLocked())
			return;

		if (fIsLocked)
			UnlockCue();
		else
			LockCue();
	}
	break;

	//	Message from cue
	case MUTE_BUTTON_MSG:
	{
		// Ignore if channel is locked
		if (fChannel->IsLocked())
			return;

		// Ignore if we are locked
		if (fIsLocked)
			return;

		//	Change our state
		if (fIsMuted)
			Unmute();
		else
			Mute();

		//	Update cues visual appearence
		if (fIsVisible) {
			BMessage* message = new BMessage(TIMELINE_DRAG_MSG);
			message->AddInt32("TheTime", GetCurrentTime());
			fChannel->GetCueSheet()->MessageReceived(message);
			delete message;
		}
	}
	break;

	case MARCH_ANTS_MSG:
	{
		if (fIsSelected)
			DoMarchingAnts(Bounds());
	}
	break;

	case SHOW_BUTTON_MSG:
		break;

	case LOCK_CUE_BUTTON_MSG:
		break;

	case PATH_BUTTON_MSG:
		break;

	//	Messages from cue menu
	//

	case CUE_LOCK_MSG:
	{
		if (fIsLocked)
			UnlockCue();
		else
			LockCue();
	}
	break;

	case CUE_CONTROLS_MSG:
		SetDisplayMode(kCueControls);
		break;

	case CUE_CONTENTS_MSG:
		SetDisplayMode(kCueContents);
		break;

	case CUE_DURATION_MSG:
		if (fHasDuration) {
			if (fShowDuration)
				HideDuration();
			else
				ShowDuration();
			break;
		}

	//      Message sent when time text loses focus.  Extract the time and adjust cue postion
	//	based on result
	case START_TIME_TEXT_FOCUS_MSG:
	{
		uint32 theTime = message->FindInt32("TheTime");
		AdjustCueStartTime(theTime);
	}
	break;

	case END_TIME_TEXT_FOCUS_MSG:
	{
		uint32 theTime = message->FindInt32("TheTime");
		AdjustCueEndTime(theTime);
	}
	break;

	case TRANSITION_IN_TEXT_FOCUS_MSG:
		break;

	case TRANSITION_OUT_TEXT_FOCUS_MSG:
		break;

	case CUE_SHOWDURATIONS_MSG:
		ShowDuration();
		break;

	case CUE_SHOWSTOPTIMES_MSG:
		HideDuration();
		break;

	//	Add a keyframe.
	case CUESHEET_KEYFRAME_MSG:
		break;

	//	Pass message to channel
	case CUE_EXPAND_MSG:
		fChannel->MessageReceived(message);
		break;

	//	Pass up to parent view
	default:
		BView::MessageReceived(message);
		break;
	}
}


void
TCueView::Pulse()
{
	//	Do nothing if we have been muted
	if (!fIsMuted) {
		const uint32 curTime = GetCurrentTime();

		//	Check and see if we need to start internal playback
		if ( (fIsPlaying == false) && (curTime >= fStartTime) ) {
			PlayCue(fStartTime);
		}

		//	Handle current playback
		if (fIsPlaying == true) {
			//	Handle playback at current time
			if (curTime <= (fStartTime + fDuration))
				HandlePlayback(curTime);
				//	Time to stop
			else
				StopCue(curTime);
		}
	} else {
		//	Stop cue playback
		if (fIsPlaying == true)
			StopCue(GetCurrentTime());
	}
}

#pragma mark -
#pragma mark === Cue Duration Routines ===

//---------------------------------------------------------------------
//	UpdateStartTimeText
//---------------------------------------------------------------------
//
//	Someone has modified the start time.  Update the start time text box
//

void TCueView::UpdateStartTimeText()
{
	char timeStr[256];
	TimeToString(fStartTime, fChannel->GetCueSheet()->GetTimeFormat(), timeStr, false);
	fStartText->SetText(timeStr);
}

//---------------------------------------------------------------------
//	UpdateEndTimeText
//---------------------------------------------------------------------
//
//	Someone has modified the end time.  Update the end time text box
//

void TCueView::UpdateEndTimeText()
{
	if (fHasDuration) {
		// Update text depending on mode
		char timeStr[256];

		if (fShowDuration)
			TimeToString(fDuration, fChannel->GetCueSheet()->GetTimeFormat(), timeStr, false);
		else
			TimeToString(fStartTime+fDuration, fChannel->GetCueSheet()->GetTimeFormat(), timeStr, false);

		fEndText->SetText(timeStr);
	}
}


//---------------------------------------------------------------------
//	AdjustCueStartTime
//---------------------------------------------------------------------
//
//	Someone has modified the end time.  Update the end time text box
//

void TCueView::AdjustCueStartTime(uint32 theTime)
{
	if (theTime != fStartTime) {
		// Check for collisions with other cues
		if (fChannel->CanInsertCue(this, theTime, true)) {
			// Remove cue from old location
			fChannel->RemoveCue(this);

			// Insert cue into channel at new location
			fChannel->InsertCue(this, theTime);
		} else {
			UpdateStartTimeText();
		}
	}
}


//---------------------------------------------------------------------
//	AdjustCueEndTime
//---------------------------------------------------------------------
//
//	Someone has modified the end time.  Update the end time text box
//

void TCueView::AdjustCueEndTime(uint32 theTime)
{

	int64 endTime = fStartTime + fDuration;

	if (theTime != endTime) {
		uint32 newStartTime = theTime - fDuration;

		// Check for collisions with other cues
		if (fChannel->CanInsertCue(this, newStartTime, true)) {
			// Remove cue from old location
			fChannel->RemoveCue(this);

			// Insert cue into channel at new location
			fChannel->InsertCue(this, newStartTime);
		} else {
			UpdateStartTimeText();
			UpdateEndTimeText();
		}
	}
}

#pragma mark -
#pragma mark === Time Format and Resolution Handling ===


//---------------------------------------------------------------------
//	ResolutionChanged
//---------------------------------------------------------------------
//
//

void TCueView::ResolutionChanged( short theFormat, short theResolution)
{
	// Get start and duration in pixels
	int32 startPixels        = TimeToPixels( fStartTime, theFormat, theResolution);
	int32 durationPixels = TimeToPixels( fDuration, theFormat, theResolution);

	Looper()->Lock();

	Hide();

	// Move to proper start time
	MoveTo( startPixels, Frame().top );

	// Resize right side of cue to startPixels + durationPixels
	ResizeTo( durationPixels, Bounds().Height() );

	// Update resize zones
	UpdateResizeZones();

	Show();

	// Force redraw
	Invalidate();

	Looper()->Unlock();
}


#pragma mark -
#pragma mark === Selection Routines ===

//---------------------------------------------------------------------
//	Select
//---------------------------------------------------------------------
//
//

void TCueView::Select()
{
	if (!fIsLocked && !fIsSelected) {
		fIsSelected = true;

		if (Looper()->Lock()) {
			MakeFocus(true);
			Looper()->Unlock();
		}

		// Fire up marching ant thread
		fAntThread = new BMessageRunner(this, new BMessage(MARCH_ANTS_MSG), 120000);
	}
}

//---------------------------------------------------------------------
//	Deselect
//---------------------------------------------------------------------
//
//

void TCueView::Deselect()
{
	if (fIsSelected) {

		fIsSelected = false;

		if ( Looper()->Lock() ) {
			Draw(Bounds());
			Looper()->Unlock();
		}

		if (fAntThread) {
			delete fAntThread;
			fAntThread = NULL;
		}
	}
}

/*-----------------------------------------------------------------------------------

        DoMarchingAnts

        This routine is used to do the actual marching of the ants (animate  the
        selection rectangle). It should be called periodically, like in the main
        event loop of the application.

        The routine first makes a few checks. It makes sure that we have the
        animation turned on, that we have a front window, that an adequate time
        has passed since the last time we animated the selection, and that there
        is indeed a selection rectangle for the window. If all those condition
        hold, we animate the selection rectangle. This is done by shifting all of
        the horizontal lines of the pattern downwards by a single line. The line
        at the bottom is moved to the top of the pattern. This gives the following
        transformation:

                                        Start				Result
                                        ========			========
                                        11111000 ---\		01111100 <--+
                                        11110001	 \---->	11111000	|
                                        11100011 ---\		11110001	|
                                        11000111	 \---->	11100011	|
                                        10001111 ---\		11000111	|
                                        00011111	 \---->	10001111	|
                                        00111110 ---\		00011111	|
                                        01111100 -\	 \---->	00111110	|
 \					|
 \-------------------+

        Once we have the new pattern, we redraw the selection rectangle with it.

   ----------------------------------------------------------------------------------*/

void TCueView::DoMarchingAnts(BRect antsRect)
{
	if (LockLooper()) {
		PushState();

		SetHighColor(kBlack);

		int16 index;
		uchar lastPart;

		lastPart = fCueAnts.data[7];

		for (index = 7; index > 0; --index)
			fCueAnts.data[index] = fCueAnts.data[index-1];

		fCueAnts.data[0] = lastPart;

		StrokeRect(antsRect, fCueAnts);

		PopState();
		UnlockLooper();
	}
	;
}


//---------------------------------------------------------------------
//	SelectAllEffects
//---------------------------------------------------------------------
//
//

void TCueView::SelectAllEffects()
{
	LockLooper();

	for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
		//	Get the effects in the list
		TCueEffectView* effect = (TCueEffectView*)fEffectsList->ItemAt(index);
		if (effect)
			effect->Select();
	}

	UnlockLooper();
}


//---------------------------------------------------------------------
//	DeselectAllEffects
//---------------------------------------------------------------------
//
//

void TCueView::DeselectAllEffects()
{
	LockLooper();

	for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
		//	Get the effects in the list
		TCueEffectView* effect = (TCueEffectView*)fEffectsList->ItemAt(index);
		if (effect)
			effect->Deselect();
	}

	UnlockLooper();
}


//---------------------------------------------------------------------
//	DeleteSelectedEffects
//---------------------------------------------------------------------
//
//	Delete and remove selected effects from cue
//

void TCueView::DeleteSelectedEffects()
{
	LockLooper();

	for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
		//	Get the effects in the list
		TCueEffectView* effect = (TCueEffectView*)fEffectsList->ItemAt(index);
		if (effect) {
			//	Delete if selected
			if (effect->Selected()) {
				fEffectsList->RemoveItem(effect);
				delete effect;
				index--;
			}
		}
	}

	//	Redraw
	Invalidate(fEffectsTray);

	UnlockLooper();
}



#pragma mark -
#pragma mark === Muting Routines ===

//---------------------------------------------------------------------
//	Mute
//---------------------------------------------------------------------
//
//

void TCueView::Mute()
{
	fIsMuted = true;
}


//---------------------------------------------------------------------
//	Unmute
//---------------------------------------------------------------------
//
//

void TCueView::Unmute()
{
	fIsMuted = false;
}


#pragma mark -
#pragma mark === Locking Routines ===

//---------------------------------------------------------------------
//	LockCue
//---------------------------------------------------------------------
//
//

void TCueView::LockCue()
{
	fIsLocked = true;

	// Disable text fields
	if (fStartText)
		fStartText->LockText();

	if (fEndText)
		fEndText->LockText();

	// Redraw lock button
	if (fLockButton)
		fLockButton->Invalidate();

	Deselect();
}


//---------------------------------------------------------------------
//	Unlock
//---------------------------------------------------------------------
//
//

void TCueView::UnlockCue()
{
	fIsLocked = false;

	// Enable text fields
	if (fStartText)
		fStartText->UnlockText();

	// Only unlock end text if it is in end time mode
	if (fEndText) {
		if (fShowDuration == false)
			fEndText->UnlockText();
	}

	// Redraw lock button
	if (fLockButton)
		fLockButton->Invalidate();

}


#pragma mark -
#pragma mark === Resizing Routines ===

//---------------------------------------------------------------------
//	UpdateResizeZones
//---------------------------------------------------------------------
//
//      The cue has been resized.  Update the mouse over zones that allow
//	horizontal resizing
//

void TCueView::UpdateResizeZones()
{
	BRect bounds = Bounds();

	const int32 addAmount = kChannelMaxHeight - (kCueInset*2);

	//	Clip height of resize zones
	if (bounds.bottom > addAmount)
		bounds.bottom = addAmount;

	fLResizeZone.Set(bounds.left, bounds.top, bounds.left+kResizeZoneWidth, bounds.bottom);
	fRResizeZone.Set(bounds.right-kResizeZoneWidth, bounds.top, bounds.right, bounds.bottom );

	//	Reset size of fEffects tray
	if (fEffectsVisible)
		fEffectsTray.Set(Bounds().left+4, Bounds().top+addAmount, Bounds().right-4, Bounds().bottom-4);

	//	Adjust effects location
	for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
		TCueEffectView* effect = (TCueEffectView*)fEffectsList->ItemAt(index);
		if(effect)
			effect->Resized();
	}
}



//---------------------------------------------------------------------
//	ResizeRightSide
//---------------------------------------------------------------------
//
//      Stretch the cue to the right side
//

void TCueView::ResizeRightSide()
{
	BPoint oldPt, resizePt;
	bool wasResized = false;

	// Set up environment
	PushState();
	fChannel->PushState();

	drawing_mode saveMode = fChannel->DrawingMode();
	fChannel->SetDrawingMode(B_OP_INVERT);

	fChannel->SetPenSize(2.0);
	fChannel->SetHighColor(kBlack);

	//	Get channel bounds
	const BRect channelBounds = fChannel->Bounds();

	// Get right side limit point.  This will either be another cue or the physical end
	// of the cue channel.  We cannot allow a drag over another cues end time.
	float limitX;
	BPoint limitPt;
	if ( fChannel->GetNeighborRightPoint(this, &limitPt) )
		limitX = limitPt.x;
	else
		limitX = channelBounds.right;

	// Get frame and bounds area
	BRect frame = Frame();
	BRect oldFrame(0,0,-1,-1);
	BRect cueBounds = Bounds();

	//	Adjust frame left side to compensate for scrolled channel
	frame.left  += channelBounds.left;
	frame.right += channelBounds.left;

	// Check to see if button is down
	BPoint mousePt;
	uint32 buttons = 0;
	GetMouse(&mousePt, &buttons, true);

	while(buttons) {
		// Convert the view to the parents coordinates for proper
		// mouse tracking and rect resizing
		resizePt = ConvertToParent(mousePt);

		// Idle if in same mouse location
		if (resizePt != oldPt) {
			wasResized = true;

			frame.right = resizePt.x;

			// Check for minimum bounds violation
			if (frame.Width() < 1)
				frame.right = frame.left + 1;

			// Now check for a intrusion upon another cue or the channel header
			// If there is an overlap, bump it back to the right a pixel
			if (frame.right > limitX)
				frame.right = limitX-1;

			// Erase old rect and draw new one.  Save new frame
			// to erase on next pass through.
			fChannel->Looper()->Lock();
			fChannel->StrokeRect(oldFrame);
			fChannel->StrokeRect(frame);
			fChannel->Looper()->Unlock();

			// Save point and frame for future compare
			oldPt           = resizePt;
			oldFrame        = frame;

			// Notify timeline of mouse location
			BMessage* message = new BMessage(UPDATE_TIMELINE_MSG);
			message->AddPoint("Where", resizePt);
			message->AddInt32("TheTime", GetCurrentTime());
			static_cast<MuseumApp*>(be_app)->GetCueSheet()->GetTimeline()->MessageReceived(message);
			delete message;
		}

		// Let other events through
		snooze(20 * 1000);

		// Get new mouse location and button state
		GetMouse(&mousePt, &buttons, true);
	}

	// Only resize
	if( wasResized ) {
		wasResized = false;

		// Clean up last drag rect
		fChannel->StrokeRect(oldFrame);

		// Adjust cue size
		float resizeBy = frame.right - (Frame().right + channelBounds.left);
		ResizeBy(resizeBy, 0);

		// Update the duration and end times
		uint32 endTime = PixelsToTime( frame.right, GetCurrentTimeFormat(), GetCurrentResolution());

		fDuration = endTime - fStartTime;

		UpdateStartTimeText();
		UpdateEndTimeText();

		Invalidate();

		// Update stage appearence
		BMessage* theMessage = new BMessage(UPDATE_TIMELINE_MSG);
		theMessage->AddInt32("TheTime", GetCurrentTime());
		static_cast<MuseumApp*>(be_app)->GetCueSheet()->PostMessage(theMessage, static_cast<MuseumApp*>(be_app)->GetCueSheet()->GetCueSheetView());
		delete theMessage;
	}

	// Set our flag to indicate mouse up happened
	fMouseDown = false;

	// Restore environment
	fChannel->SetDrawingMode(saveMode);

	PopState();
	fChannel->PopState();
}



//---------------------------------------------------------------------
//	ResizeLeftSide
//---------------------------------------------------------------------
//
//      Stretch the cue to the left side
//

void TCueView::ResizeLeftSide()
{
	BPoint oldPt, resizePt;
	bool wasResized = false;

	// Set up environment
	PushState();
	fChannel->PushState();

	drawing_mode saveMode = fChannel->DrawingMode();
	fChannel->SetDrawingMode(B_OP_INVERT);

	fChannel->SetPenSize(2.0);
	fChannel->SetHighColor(kBlack);

	//	Get channel bounds
	const BRect channelBounds = fChannel->Bounds();

	// Get left side limit point.  This will either be another cue or the physical start
	// of the cue channel.  We cannot allow a drag over another cues end time.
	float limitX;
	BPoint limitPt;
	if ( fChannel->GetNeighborLeftPoint(this, &limitPt) )
		limitX = limitPt.x;
	else
		limitX = channelBounds.left;

	// Get frame and bounds area
	BRect frame = Frame();
	BRect oldFrame(0,0,-1,-1);
	BRect cueBounds = Bounds();

	//	Adjust frame left side to compensate for scrolled channel
	frame.left  += channelBounds.left;
	frame.right += channelBounds.left;

	// Check to see if button is down
	BPoint mousePt;
	uint32 buttons = 0;
	GetMouse(&mousePt, &buttons, true);

	while(buttons) {
		// Convert the view to the parents coordinates for proper
		// mouse tracking and rect resizing
		resizePt = ConvertToParent(mousePt);

		// Idle if in same mouse location
		if (resizePt != oldPt) {
			wasResized = true;

			frame.left = resizePt.x;

			// Check for minimum bounds violation and correct if one has occurred
			if (frame.Width() < 1)
				frame.left = frame.right - 1;

			// Now check for a intrusion upon another cue or the channel header
			// If there is an overlap, bump it back to the right a pixel
			if (frame.left < limitX)
				frame.left = limitX+1;

			// Erase old rect and draw new one.  Save new frame
			// to erase on next pass through.
			fChannel->StrokeRect(oldFrame);
			fChannel->StrokeRect(frame);

			// Save point and frame for future compare
			oldPt           = resizePt;
			oldFrame        = frame;

			// Notify timeline of mouse location
			BMessage* message = new BMessage(UPDATE_TIMELINE_MSG);
			message->AddPoint("Where", resizePt);
			message->AddInt32("TheTime", GetCurrentTime());
			static_cast<MuseumApp*>(be_app)->GetCueSheet()->GetTimeline()->MessageReceived(message);
			delete message;
		}

		// Let other events through
		snooze(20 * 1000);

		// Get new mouse location and button state
		GetMouse(&mousePt, &buttons, true);
	}

	// Only resize
	if( wasResized ) {
		wasResized = false;

		// Clean up last drag rect
		fChannel->StrokeRect(oldFrame);

		// Adjust cue size
		frame.left -= channelBounds.left;
		MoveTo(frame.left, kCueInset);
		float resizeBy = frame.right - (Frame().right + channelBounds.left);
		ResizeBy(resizeBy, 0);

		// Update the duration and end times
		fStartTime = PixelsToTime( frame.left, GetCurrentTimeFormat(), GetCurrentResolution());
		uint32 endTime = PixelsToTime( frame.right, GetCurrentTimeFormat(), GetCurrentResolution());

		// Adjust cue duration
		fDuration = endTime - fStartTime;

		UpdateStartTimeText();
		UpdateEndTimeText();

		Invalidate();

		// Update stage appearence
		BMessage* theMessage = new BMessage(UPDATE_TIMELINE_MSG);
		theMessage->AddInt32("TheTime", GetCurrentTime());
		static_cast<MuseumApp*>(be_app)->GetCueSheet()->PostMessage(theMessage, static_cast<MuseumApp*>(be_app)->GetCueSheet()->GetCueSheetView());
		delete theMessage;
	}

	// Set our flag to indicate mouse up happened
	fMouseDown = false;

	// Restore environment
	fChannel->SetDrawingMode(saveMode);

	PopState();
	fChannel->PopState();

}



//---------------------------------------------------------------------
//	Contract
//---------------------------------------------------------------------
//
//      Hide cue controls
//

void TCueView::Contract(bool force)
{
	if (force == true)
		fIsExpanded = true;

	// Do nothing if we are already contracted
	if (fIsExpanded == false)
		return;

	if (fEffectsVisible == true)
		HideEffects();

	fIsExpanded = false;

	if (fCueIcon)
		fCueIcon->MoveTo(kItemOffscreenValue, kItemOffscreenValue);

	if (fLockButton)
		fLockButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);

	if (fMuteButton)
		fMuteButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);
}



//---------------------------------------------------------------------
//	Expand
//---------------------------------------------------------------------
//
//      Show cue controls
//

void TCueView::Expand(bool force)
{
	if (force == true)
		fIsExpanded = false;

	// Do nothing if we are already expanded
	if (fIsExpanded == true)
		return;

	fIsExpanded = true;

	//	Show all controls
	ShowControls();
}


//---------------------------------------------------------------------
//	HideEffects
//---------------------------------------------------------------------
//
//	Hide effects tray
//

void TCueView::HideEffects()
{
	//	Hide effects tray
	if (fEffectsVisible == true) {
		//	Effects are now visible
		fEffectsVisible = false;

		//	Reset the drag zones
		UpdateResizeZones();

		//	Force a redraw
		Invalidate();
	}
}

//---------------------------------------------------------------------
//	ShowEffects
//---------------------------------------------------------------------
//
//	Show effects tray
//

void TCueView::ShowEffects()
{
	//	Show effects tray
	if (fEffectsVisible == false) {
		//	Effects are now visible
		fEffectsVisible = true;

		//	Reset the drag zones
		UpdateResizeZones();

		//	Force a redraw
		Invalidate();
	}
}

#pragma mark -
#pragma mark === File Handling ===


//---------------------------------------------------------------------
//	ShowPanel
//---------------------------------------------------------------------
//
//	Show file panel
//

void TCueView::ShowPanel()
{
	// Deactivate the active cue sheet so as to move timeline tick off screen
	MuseumApp* theApp = static_cast<MuseumApp*>(be_app);
	theApp->GetCueSheet()->SetPanelOpen(true);
}



//---------------------------------------------------------------------
//	HidePanel
//---------------------------------------------------------------------
//
//	Hide and delete file panel
//

void TCueView::HidePanel()
{
	// Deactivate the active cue sheet so as to move timeline tick off screen
	MuseumApp* theApp = static_cast<MuseumApp*>(be_app);
	theApp->GetCueSheet()->SetPanelOpen(false);
}


#pragma mark -
#pragma mark === Layer Routines ===


//---------------------------------------------------------------------
//	GetChannelID
//---------------------------------------------------------------------
//
//	Return cues channel ID
//

int32 TCueView::GetChannelID()
{
	return fChannel->fID;
}


#pragma mark -
#pragma mark === Cue Button Handling ===

//---------------------------------------------------------------------
//	OpenCueMenu
//---------------------------------------------------------------------
//
//	Open cue pop up menu
//

void TCueView::OpenCueMenu(BPoint menuPt)
{
	BMenuItem* selected;

	// Create the menu and mark the current transition
	TCueMenu* theMenu = new TCueMenu(this);

	// Set menu location point
	ConvertToScreen(&menuPt);
	selected = theMenu->Go(menuPt);

	if (selected) {
		if ( selected->Message() )
			MessageReceived( selected->Message() );
	}

	// Clean up
	delete theMenu;
}


//---------------------------------------------------------------------
//	OpenTransitionMenu
//---------------------------------------------------------------------
//
//	Open transition pop up menu
//

void TCueView::OpenTransitionMenu(BPoint menuPt)
{
	BMenuItem* selected;

	// Create the menu and mark the current transition
	TTransitionMenu* theMenu = new TTransitionMenu(this, true);

	// Set menu location point
	ConvertToScreen(&menuPt);
	selected = theMenu->Go(menuPt);

	if (selected) {
		if ( selected->Message() )
			MessageReceived( selected->Message() );
	}

	// Clean up
	delete theMenu;
}


#pragma mark -
#pragma mark === Cue Contents

//---------------------------------------------------------------------
//	ShowContents
//---------------------------------------------------------------------
//
//	Display cue data within cue frame.  Hide cue controls.
//

void TCueView::ShowContents()
{
	fCueDisplayMode = kCueContents;

	// Hide all of the cue controls
	HideControls();

	// Show cue contents in cue area

	// Force update
	Invalidate();
}


//---------------------------------------------------------------------
//	HideContents
//---------------------------------------------------------------------
//
//	Hide cue data within cue frame.  Show cue controls.
//

void TCueView::HideContents()
{
	// Show all of the cue controls
	ShowControls();

	// Force update
	Invalidate();
}


//---------------------------------------------------------------------
//	ShowControls
//---------------------------------------------------------------------
//
//	Show cue controls
//

void TCueView::ShowControls()
{

	BRect rect;

	if (fCueIcon) {
		rect.Set(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		rect.OffsetBy(kResizeZoneWidth+5, 0);
		fCueIcon->MoveTo(rect.left, rect.top);
	}

	if (fLockButton) {
		rect.Set( fCueIcon->Frame().right+kCueButtonOffset, fCueIcon->Frame().top, fCueIcon->Frame().right+kCueButtonWidth+kCueButtonOffset, fCueIcon->Frame().top + kCueButtonHeight );
		fLockButton->MoveTo(rect.left, rect.top);
	}

	if (fMuteButton) {
		rect.Set( fLockButton->Frame().right+kCueButtonOffset, rect.top, fLockButton->Frame().right+kCueButtonWidth+kCueButtonOffset, rect.bottom );
		fMuteButton->MoveTo(rect.left, rect.top);
	}

	if (fEffectsButton) {
		rect.Set( fLockButton->Frame().left, fLockButton->Frame().bottom+kCueButtonOffset, fLockButton->Frame().right, fLockButton->Frame().bottom+kCueButtonOffset+kCueButtonHeight);
		fEffectsButton->MoveTo(rect.left, rect.top);
	}

}


//---------------------------------------------------------------------
//	HideControls
//---------------------------------------------------------------------
//
//	Hide cue controls
//

void TCueView::HideControls()
{
	if (fCueIcon)
		fCueIcon->MoveTo(kItemOffscreenValue, kItemOffscreenValue);

	if (fLockButton)
		fLockButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);

	if (fMuteButton)
		fMuteButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);

	if (fEffectsButton)
		fEffectsButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);

}


//---------------------------------------------------------------------
//	ShowDuration
//---------------------------------------------------------------------
//
//	Show cue duration.  Draw delta symbol and display duration
//	in the fEndText
//

void TCueView::ShowDuration()
{
	if (fHasDuration) {
		// Get duration
		char timeStr[256];
		TimeToString(fDuration, fChannel->GetCueSheet()->GetTimeFormat(), timeStr, false);
		fEndText->SetText(timeStr);

		// Lock
		fEndText->LockText();

		// Set flag to true
		fShowDuration = true;

		// Draw duration delta
		DrawDurationDelta();
	}
}


//---------------------------------------------------------------------
//	HideDuration
//---------------------------------------------------------------------
//
//	Hide cue duration.  Erase delta symbol and display end time
//	in the fEndText
//

void TCueView::HideDuration()
{

	if (fHasDuration) {
		// Get end time
		char timeStr[256];
		TimeToString(fStartTime+fDuration, fChannel->GetCueSheet()->GetTimeFormat(), timeStr, false);
		fEndText->SetText(timeStr);

		// Unlock
		fEndText->UnlockText();

		// Set flag to false
		fShowDuration = false;

		// Erase delta
		Looper()->Lock();
		Invalidate(fDurationDeltaRect);
		Looper()->Unlock();
	}

}


//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Display cue editor
//

void TCueView::OpenEditor()
{
}


//---------------------------------------------------------------------
//	SetDisplayMode
//---------------------------------------------------------------------
//
//	Toggle current cue display mode
//

void TCueView::SetDisplayMode(CueDisplay theMode)
{
	//	Do nothing if we are alreay set to requested mode
	if (theMode == fCueDisplayMode)
		return;

	//	Clean up old mode
	switch(fCueDisplayMode)
	{
	case kCueControls:
		break;

	case kCueContents:
		HideContents();
		break;

	default:
		TRESPASS();
		break;
	}

	//	Set new mode
	switch(theMode)
	{
	case kCueControls:
		break;

	case kCueContents:
		ShowContents();
		break;

	default:
		TRESPASS();
		break;
	}

	//	Set new cue mode
	fCueDisplayMode = theMode;
}


#pragma mark -
#pragma mark === Playback Routines ===


//---------------------------------------------------------------------
//	Preroll
//---------------------------------------------------------------------
//
//	Prepare cue for playback
//

void TCueView::PrerollCue(uint32 theTime)
{

}

//---------------------------------------------------------------------
//	PlayCue
//---------------------------------------------------------------------
//
//	Tell cue to start playback at millisecond time
//

void TCueView::PlayCue(uint32 theTime)
{
	fIsPlaying = true;
}

//---------------------------------------------------------------------
//	StopCue
//---------------------------------------------------------------------
//
//	Tell cue we are stopping
//

void TCueView::StopCue(uint32 theTime)
{
	fIsPlaying = false;
}


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//	Tell cue we are pausing
//

void TCueView::PauseCue()
{
}


//---------------------------------------------------------------------
//	Resume
//---------------------------------------------------------------------
//
//	Tell cue we are resuming playback
//

void TCueView::ResumeCue()
{
}

//---------------------------------------------------------------------
//	HandlePlayback
//---------------------------------------------------------------------
//
//	Process tick received from playback engine.  We receive a tick each
//	millisecond.
//

void TCueView::HandlePlayback(uint32 theTime)
{

}

#pragma mark -
#pragma mark === Utility Routines ===


//---------------------------------------------------------------------
//	LoadCueIcon
//---------------------------------------------------------------------
//
//	Load proper cue icon for cue type
//

void TCueView::LoadCueIcon()
{
	//	Move cue icon to proper offscreen position if cue is minimized
	if (!fIsExpanded || fCueDisplayMode != kCueControls)
		fCueIcon->MoveTo(kItemOffscreenValue, kItemOffscreenValue);
}

//---------------------------------------------------------------------
//	AddToBrowser
//---------------------------------------------------------------------
//
//	Add file to Browser
//

void TCueView::AddToBrowser()
{
	//  Add file to browser
	BMessage* theMessage = new BMessage(ADD_REF_MSG);
	theMessage->AddRef("FileRef", &fFileRef);
	TBrowserWindow* theBrowser = static_cast<MuseumApp*>(be_app)->GetBrowser();
	if (theBrowser) {
		theBrowser->PostMessage(theMessage, NULL);
	}
	delete theMessage;
}

//---------------------------------------------------------------------
//	EffectsClick
//---------------------------------------------------------------------
//
//	Return true if click occured in effects tray
//

bool TCueView::EffectsClick(BPoint where)
{

	//	Was the click in the Effects tray?
	if ( fEffectsVisible && fEffectsTray.Contains(where)) {
		return true;
	} else {
		return false;
	}
}


//---------------------------------------------------------------------
//	ClickedEffect
//---------------------------------------------------------------------
//
//	Return pointer to TCueEffectView clicked or NULL
//

TCueEffectView* TCueView::ClickedEffect(BPoint where)
{
	//	Was the click in the Effects tray?
	if ( fEffectsVisible && fEffectsTray.Contains(where)) {
		//	Pass click to effects cue
		if (fEffectsList->CountItems() > 0) {
			for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
				//	Get the effects in the list
				TCueEffectView* effect = (TCueEffectView*)fEffectsList->ItemAt(index);

				//	Does it contain the click?
				if (effect->Bounds().Contains(where))
					return effect;
			}
		}
	}

	return NULL;
}


#pragma mark -
#pragma mark === Clipboard Routines ===

//---------------------------------------------------------------------
//	Copy
//---------------------------------------------------------------------
//
//	Create a copy of the cue and return it as BMessage
//

BMessage* TCueView::Copy()
{
	// Create a message for the new cue to be archived in
	BMessage* theMessage = new BMessage();

	status_t myErr = Archive(theMessage, true);

	if (myErr == B_OK)
		return theMessage;
	else
		return NULL;
}


//---------------------------------------------------------------------
//	PasteComplete
//---------------------------------------------------------------------
//
//	Cue has been notified that a paste operation on it is now complete
//

void TCueView::PasteComplete()
{

}

#pragma mark -
#pragma mark === Accessor Functions ===


//---------------------------------------------------------------------
//	SetDuration
//---------------------------------------------------------------------
//
//	Set the cues duration
//

void TCueView::SetDuration(uint32 theDuration)
{
	fDuration = theDuration;
}


//---------------------------------------------------------------------
//	SetStartTime
//---------------------------------------------------------------------
//
//	Set the cues startTime
//

void TCueView::SetStartTime(uint32 theTime)
{
	fStartTime = theTime;
}

//---------------------------------------------------------------------
//	SetInstantiated
//---------------------------------------------------------------------
//
//	Set fResolution to theResolution value
//

void TCueView::SetInstantiated( bool theVal)
{
	fIsInstantiated = theVal;
}


//---------------------------------------------------------------------
//	SetEditorOpen
//---------------------------------------------------------------------
//
//	Set fEditor to the value
//

void TCueView::SetEditorOpen( bool theVal)
{
	fEditorOpen = theVal;
}

//---------------------------------------------------------------------
//	SetChannel
//---------------------------------------------------------------------
//
//	Set fChannel to the channel
//

void TCueView::SetChannel( TCueChannel* channel)
{
	fChannel = channel;
}
