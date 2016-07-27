//---------------------------------------------------------------------
//
//	File:	TVisualCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.24.98
//
//	Desc:	Cue that has visual data
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppAlerts.h"
#include "AppMessages.h"
#include "DebugUtils.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TPlaybackEngine.h"
#include "TBitmapView.h"
#include "TCueView.h"
#include "TCueChannel.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueTimeText.h"
#include "TCueMenu.h"
#include "TStageCueMenu.h"
#include "TStageWindow.h"
#include "TStageView.h"
#include "TTimelineView.h"
#include "TBrowserWindow.h"
#include "TRefFilter.h"
#include "TTransition.h"
#include "TCompositor.h"

//	Dialogs
#include "TOpacityDialog.h"
#include "TScaleDialog.h"
#include "TRotateDialog.h"

//	Transitions
#include "TTransitionView.h"
#include "TWipeRightOut.h"

#include "TVisualCue.h"


#pragma mark -
#pragma mark === Constructors ===

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVisualCue::TVisualCue(int16 id, TCueChannel* parent, BRect bounds, uint32 startTime, char* cueName) :
	TCueView(id, parent, bounds, startTime, cueName)
{
	// Init member variables
	fFile                           = NULL;
	fBitmap                         = NULL;
	fTransformBitmap        = NULL;
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from an entry_ref

TVisualCue::TVisualCue(entry_ref &theRef, int16 id,  TCueChannel* parent, BRect bounds, uint32 startTime, char* cueName) :
	TCueView(id, parent, bounds, startTime, cueName)
{
	// Init member variables
	fBitmap                         = NULL;
	fTransformBitmap        = NULL;
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a BMessage
//

TVisualCue::TVisualCue(BMessage* theMessage) : TCueView(theMessage)
{
	// Init member varibles
	fBitmap                         = NULL;
	fTransformBitmap        = NULL;

	ssize_t numBytes;
	DisplayQuality* quality;
	theMessage->FindData("DisplayQuality", B_ANY_TYPE, (const void**)&quality, &numBytes );
	fDisplayQuality = *quality;

	//      Set the file to none.  The bitmap data was passed in to us,
	//	not read from a file.
	fFile = NULL;

	// We are now fully instantiated
	fIsInstantiated = true;
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVisualCue::~TVisualCue()
{
	//	Delete render semaphore
	delete_sem(fRenderSem);

	if (fBitmap) {
		delete  fBitmap;
		fBitmap = NULL;
	}

	if (fTransformBitmap) {
		delete fTransformBitmap;
		fTransformBitmap = NULL;
	}

	if (fCuePosition)
		delete fCuePosition;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVisualCue::Init()
{
	//      Set up default duration in milliseconds
	fDuration                       = 1000;

	//      Set up default settings
	fIsLocked                       = false;
	fIsSelected             = false;
	fLowColor                       = kWhite;
	fHighColor              = kBlack;
	fIsPrepared             = false;
	fIsPlaying              = false;
	fHasDuration            = true;
	fCanStretch             = true;
	fCanEnvelope            = false;
	fIsVisible              = true;
	fCanCrop                        = true;
	fCanTransition          = true;
	fCanPath                        = true;
	fHasEditor                      = true;
	fCanWindow                      = false;
	fCanLoop                        = false;

	fDisplayQuality = kPreviewQuality;
	fTransparency = 0;      // ABH
// ABH should init	fScale
	fRotation = 0;

	// Default initialization
	TCueView::Init();

	//	Set up TCuePosition
	fCuePosition = new TCuePosition(fBitmap->Bounds());

	//	Set up registration
	float regX = fBitmap->Bounds().Width()  / 2;
	float regY = fBitmap->Bounds().Height() / 2;
	fCuePosition->Registration(BPoint(regX, regY));

	//--------------

	//
	//	Create temp wipe effect
	//

	//	Set time in local cue time, not global time
	TWipeRightOut* effect = new TWipeRightOut();
	effect->EndTime(fDuration);
	effect->Duration(fDuration - 100);

	BRect effectRect = Bounds();
	const int32 addAmount = kChannelMaxHeight - (kCueInset*2);
	effectRect.Set(Bounds().left+4, Bounds().top+addAmount, Bounds().right-4, Bounds().bottom-4);

	//	We know we are the first effect so we can do this cheap calculation
	effectRect.top    += 6;
	effectRect.bottom = effectRect.top;
	effectRect.top    = effectRect.bottom;
	effectRect.bottom = effectRect.top + kEffectHeight;

	//--------------

	//	Create effect view and add to list
	TTransitionView* effectView = new TTransitionView(effectRect, this, effect);
	fEffectsList->AddItem(effectView);

	//	Create render semaphore
	fRenderSem = create_sem(1, "VisualCueRenderSem");
}

#pragma mark -
#pragma mark === Archiving Functions ===

// ABH missing functions

bool TVisualCue::HasTransitionIn()
{
	return fHasTransitionIn;        // Have to initialize this somewhere!
}

bool TVisualCue::HasTransitionOut()
{
	return fHasTransitionOut;       // Have to initialize this somewhere!
}

// ABH called in TTransitionMenuLooper
//BHandler TVisualCue::GetCueTransitionOutButton(){
//
//}

//BHandler TVisualCue::GetCueTransitionInButton(){
//
//}

void TVisualCue::SetTransparency(float val)
{
	printf("SetTransparency called\n");

	if (val > 0) {
		fTransparency = val;
	}
}

float TVisualCue::GetTransparency()
{
	printf("GetDrawArea called\n");
	return fTransparency;
}

BRect TVisualCue::GetDrawArea()
{
	printf("GetDrawArea called\n");
}

float TVisualCue::GetRotation()
{
	printf("GetRotation called\n");
	return fRotation;
}

void TVisualCue::SetRotation(float val)
{
	printf("SetRotation called\n");
	if (val >= 0) {
		fRotation = val;
	}
}

BRect TVisualCue::GetArea()
{
	printf("GetArea called - fix this!!!\n");
	return (BRect)(1.0,1.0);        //ABH fix this !!!
}
BRect TVisualCue::GetScaledArea()
{
	printf("GetScaledAra called - fix this!!!\n");
	return (BRect)(1.0,1.0);        //ABH fix this !!!
}
BPoint TVisualCue::GetScale()
{
	printf("GetScale called\n");
	return fScale;
}

void TVisualCue::SetScale(BPoint val)
{
	printf("SetScale called\n");

	fScale = val;

}

int TVisualCue::GetDuration()
{
	printf("GetDuration called - fix this!!!\n");
	return 1;               // ABH fix this!
}

BRect TVisualCue::GetCroppedArea()
{
	BRect area = (0,0);
	printf("GetCroppedArea called - fix this!!!\n");
	return (BRect)(1.0,1.0); // fix this !!!
}
//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TVisualCue::Instantiate(BMessage* archive)
{
	if ( validate_instantiation(archive, "TVisualCue") )
		return new TVisualCue(archive);

	return NULL;
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TVisualCue::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = TCueView::Archive(data, deep);

	if (myErr == B_OK) {
		// Add ourselves to the archive
		data->AddString("class", "TVisualCue");

		// Add our member variables to the archive
		data->AddData("DisplayQuality", B_ANY_TYPE, &fDisplayQuality, sizeof(DisplayQuality) );

		//	Archive out deep data
		if (deep) {
		}
	}

	Looper()->Unlock();

	return myErr;
}


#pragma mark -
#pragma mark === Drawing Functions ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TVisualCue::Draw(BRect updateRect)
{
	// Return if were not done cooking...
	if (!fIsInstantiated)
		return;

	PushState();

	//	Determine drawing state
	switch (fCueDisplayMode)
	{
	case kCueControls:
		TCueView::Draw(updateRect);
		break;

	case kCueContents:
	{
		TCueView::Draw(updateRect);

		//	Save clipping region
		BRegion clipRegion, saveRegion;
		GetClippingRegion(&saveRegion);

		// Clip out resize rects
		if ( updateRect.right > Bounds().right - fLResizeZone.Width())
			updateRect.right = Bounds().right - fLResizeZone.Width();

		//	Clip out fEffectsTray
		if (fEffectsVisible) {
			if ( updateRect.bottom > fEffectsTray.top)
				updateRect.bottom = fEffectsTray.top;
		}

		//	Install clip region
		clipRegion.Set(updateRect);
		ConstrainClippingRegion( &clipRegion );

		// Set up display rect
		BRect pictRect = Bounds();
		if (fEffectsVisible)
			pictRect.bottom = fEffectsTray.top;
		pictRect.left = fLResizeZone.right+1;
		pictRect.right = fLResizeZone.right+pictRect.Height();
		pictRect.InsetBy(1,1);

		// Fill cue area with bitmap
		if (fBitmap) {
			for (int32 index = 0; pictRect.right < updateRect.right; index++) {
				pictRect.left   = (pictRect.Height()* index) + fLResizeZone.right+1;
				pictRect.right  = pictRect.left + pictRect.Height();
				DrawBitmap(fBitmap, pictRect);
			}
		}

		// Restore clipping
		ConstrainClippingRegion(&saveRegion);
	}
	break;

	default:
		TRESPASS();
		break;
	}

	PopState();
}


//---------------------------------------------------------------------
//	RenderData
//---------------------------------------------------------------------
//
//	Render internal transformations
//

void TVisualCue::RenderData(uint32 theTime, BRect bounds)
{
	// Exit if we are muted
	if (fIsMuted)
		return;

	//	Acquire render sem
	//status_t err = acquire_sefetc(fCaptureSem, 1, B_TIMEOUT, timeout);
	acquire_sem(fRenderSem);

	//	Render bitmap data internally
	RenderBitmapData();

	//	All done
	release_sem(fRenderSem);
}


//---------------------------------------------------------------------
//	RenderBitmapData
//---------------------------------------------------------------------
//
//	Render internal bitmap modifications
//

void TVisualCue::RenderBitmapData()
{
}


//---------------------------------------------------------------------
//	DrawSubTicks
//---------------------------------------------------------------------
//
//	Draw intermediate ticks between main tick indicators
//

void TVisualCue::DrawSubTicks(int32 index)
{
	BPoint startPt, endPt;

	const BRect timeBounds  = Bounds();
	const int32 halfY               = Bounds().Height() / 2;
	const int32 tickHeight  = kTickHeight / 2;

	int32 fpsValue  = GetFPSValue( GetCurrentTimeFormat() );

	int32 numTicks;
	switch( GetCurrentResolution() )
	{

	// One Frame
	case 0:
		numTicks = 0;
		break;

	// 1/4 Second
	case 1:
		numTicks = kTickSpacing/ ( (fpsValue-1) / 4);
		break;

	// Half Second
	case 2:
		numTicks = kTickSpacing/ ( (fpsValue-1)/2);
		break;

	// One Second
	case 3:
		numTicks = kTickSpacing/(fpsValue-1);
		break;

	// 2 Seconds
	case 4:
		numTicks = 2;
		break;

	// 5 Seconds
	case 5:
		numTicks = 5;
		break;

	// 10 Seconds
	case 6:
		numTicks = 10;
		break;

	// 30 Seconds
	case 7:
		numTicks = 30;
		break;

	// 1 Minute
	case 8:
		numTicks = 10;
		break;

	// 5 Minutes
	case 9:
		numTicks = 10;
		break;
	}

	if (numTicks > 0) {
		for (int32 lastIndex = index; lastIndex < (index+kTickSpacing); lastIndex+=numTicks) {
			//	Tick postion
			startPt.Set( lastIndex, halfY - tickHeight);
			endPt.Set( startPt.x, halfY + tickHeight);
			StrokeLine(startPt, endPt);
		}
	}
}

#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TVisualCue::MessageReceived(BMessage* message)
{
	switch(message->what)
	{

	//      Transition has been changed.  Update cue if in kCueTransitions mode
	case CHANGE_ICON_MSG:
		break;

	//	Cue menu quality messages
	case STAGE_CUE_WIREFRAME_MSG:
		SetQualityMode(kWireframeQuality);
		break;

	case STAGE_CUE_PREVIEW_MSG:
		SetQualityMode(kPreviewQuality);
		break;

	case STAGE_CUE_BETTER_MSG:
		SetQualityMode(kBetterQuality);
		break;

	case STAGE_CUE_BEST_MSG:
		SetQualityMode(kBestQuality);
		break;

	//	Show opacity dialog
	//case STAGE_CUE_OPACITY_MSG:
	//	ShowOpacityDialog();
	//	break;

	//	Opacity dialog has been closed
	//case OPACITY_CLOSE_MSG:
	//	fOpacityDialog = NULL;
	//	break;

	//	Show scale dialog
	//case STAGE_CUE_SCALE_MSG:
	//	ShowScaleDialog();
	//	break;

	//	Scale dialog has been closed
	//case SCALE_CLOSE_MSG:
	//	fScaleDialog = NULL;
	//	break;

	//	Show rotate dialog
	//case STAGE_CUE_ROTATE_MSG:
	//	ShowRotateDialog();
	//	break;

	//	Rotate dialog has been closed
	//case ROTATE_CLOSE_MSG:
	//	fRotateDialog = NULL;
	//	break;

	default:
		TCueView::MessageReceived(message);
		break;
	}
}

#pragma mark -
#pragma mark === File Handling ===

//---------------------------------------------------------------------
//	HidePanel
//---------------------------------------------------------------------
//
//	Hide and delete file panel
//

void TVisualCue::HidePanel()
{
	if(fPanel) {
		// Clean up any RefFilters
		TRefFilter* theFilter = static_cast<TRefFilter*>(fPanel->RefFilter() );
		if (theFilter)
			delete theFilter;

		delete fPanel;
		fPanel = NULL;
	}

	TCueView::HidePanel();
}


#pragma mark -
#pragma mark === Sizing Routines ====


//---------------------------------------------------------------------
//	Expand
//---------------------------------------------------------------------
//
//      Show cue controls
//

void TVisualCue::Expand(bool force)
{
	if (force == true)
		fIsExpanded = false;

	// Do nothing if we are already expanded
	if (fIsExpanded)
		return;

	fIsExpanded = true;

	// We show certain controls based on what cue display mode we are in
	//

	switch (fCueDisplayMode)
	{
	case kCueControls:
		ShowControls();
		break;

	case kCueContents:
		HideControls();
		break;

	default:
		TRESPASS();
		break;
	}
}


//---------------------------------------------------------------------
//	Contract
//---------------------------------------------------------------------
//
//      Hide cue controls
//

void TVisualCue::Contract(bool force)
{
	if (force == true)
		fIsExpanded = true;

	// Do nothing if we are already contracted
	if (fIsExpanded == false)
		return;

	TCueView::Contract();
}


#pragma mark -
#pragma mark === Cue Contents

//---------------------------------------------------------------------
//	ShowContents
//---------------------------------------------------------------------
//
//	Display cue data within cue frame.  Hide cue controls.
//

void TVisualCue::ShowContents()
{
	fCueDisplayMode = kCueContents;

	TCueView::ShowContents();
}


//---------------------------------------------------------------------
//	HideContents
//---------------------------------------------------------------------
//
//	Hide cue data within cue frame.  Show cue controls.
//

void TVisualCue::HideContents()
{
	TCueView::HideContents();
}


//---------------------------------------------------------------------
//	ShowControls
//---------------------------------------------------------------------
//
//	Show cue controls
//

void TVisualCue::ShowControls()
{

	BRect rect;

	TCueView::ShowControls();
}


//---------------------------------------------------------------------
//	HideControls
//---------------------------------------------------------------------
//
//	Hide cue controls
//

void TVisualCue::HideControls()
{

	TCueView::HideControls();
}

//---------------------------------------------------------------------
//	SetDisplayMode
//---------------------------------------------------------------------
//
//	Toggle current cue display mode
//

void TVisualCue::SetDisplayMode(CueDisplay theMode)
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

	//	Set new mode variable
	fCueDisplayMode = theMode;
}


//---------------------------------------------------------------------
//	SetQualityMode
//---------------------------------------------------------------------
//
//	Toggle current cue quality mode
//

void TVisualCue::SetQualityMode(DisplayQuality theMode)
{

	   //	Do nothing if we are alreay set to requested mode
	   if (theMode == fDisplayQuality)
	        return;

	   //	Set new mode variable
	   fDisplayQuality = theMode;

	   //	Set new mode
	   switch(fDisplayQuality)
	   {
	        case kWireframeQuality:
	                UpdateStageCue();
	                break;

	        case kPreviewQuality:
	                UpdateStageCue();
	                break;

	        case kBetterQuality:
	                UpdateStageCue();
	                break;

	        case kBestQuality:
	                UpdateStageCue();
	                break;

	        default:
	                TRESPASS();
	                break;
	   }
}

#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	GetBitmap
//---------------------------------------------------------------------
//
//	Return visual display of data at the time specified.  If theTime is
//	NULL, return the data for the current time
//

BBitmap* TVisualCue::GetBitmap(uint32 theTime)
{
	return fBitmap;
}


//---------------------------------------------------------------------
//	IsOnStage
//---------------------------------------------------------------------
//
//	Return true if the cue data is visible on stage
//

bool TVisualCue::IsOnStage()
{
	//	If we are muted, we are invisible
	if ( IsMuted() )
		return false;

	uint32 theTime = GetCurrentTime();

	if (theTime >= fStartTime && theTime <= fStartTime + fDuration)
		return true;

	return false;
}


//---------------------------------------------------------------------
//	UpdateStageCue
//---------------------------------------------------------------------
//
//	Update cues stage data
//

void TVisualCue::UpdateStageCue()
{
	   if (IsOnStage())
	   {
	        TStageWindow *theStage	= static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage();
	        TStageView       *stageView = theStage->GetStageView();

	        theStage->Lock();
	        stageView->StageDraw( GetDrawArea(), GetCurrentTime());
	        stageView->Draw( GetDrawArea());
	        theStage->Unlock();
	   }
}

