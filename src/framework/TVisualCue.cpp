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

TVisualCue::TVisualCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime, char *cueName) : 
	TCueView(id, parent, bounds, startTime, cueName)
{	
	// Init member variables 
	m_File				= NULL;
	m_Bitmap 			= NULL;
	m_TransformBitmap	= NULL;
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from an entry_ref

TVisualCue::TVisualCue(entry_ref &theRef, int16 id,  TCueChannel *parent, BRect bounds, uint32 startTime, char *cueName) : 
	TCueView(id, parent, bounds, startTime, cueName)
{	
	// Init member variables 
	m_Bitmap 			= NULL;
	m_TransformBitmap	= NULL;		
}

		
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a BMessage
//

TVisualCue::TVisualCue(BMessage *theMessage) : TCueView(theMessage)
{	
	// Init member varibles
	m_Bitmap 			= NULL;
	m_TransformBitmap	= NULL;
		
	ssize_t 		numBytes;
	DisplayQuality	*quality;	
	theMessage->FindData("DisplayQuality", B_ANY_TYPE, (void **)&quality, &numBytes );
	m_DisplayQuality = *quality;
										
	// 	Set the file to none.  The bitmap data was passed in to us,
	//	not read from a file.
	m_File = NULL;
				
	// We are now fully instantiated
	m_IsInstantiated = true;	
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVisualCue::~TVisualCue()
{
	//	Delete render semaphore
	delete_sem(m_RenderSem);

	if (m_Bitmap)
	{
		delete	m_Bitmap;
		m_Bitmap = NULL;
	}
	
	if (m_TransformBitmap)
	{
		delete m_TransformBitmap; 
		m_TransformBitmap = NULL;
	}
	
	if (m_CuePosition)
		delete m_CuePosition;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVisualCue::Init()
{								
	// 	Set up default duration in milliseconds
	m_Duration 			= 1000;
	
	// 	Set up default settings
	m_IsLocked 			= false;	
	m_IsSelected 		= false;	
	m_LowColor 			= kWhite;	
	m_HighColor 		= kBlack;	
	m_IsPrepared 		= false;
	m_IsPlaying 		= false;	
	m_HasDuration 		= true;	
	m_CanStretch		= true;		
	m_CanEnvelope		= false;
	m_IsVisible 		= true;
	m_CanCrop			= true;	
	m_CanTransition		= true;	
	m_CanPath			= true;	
	m_HasEditor			= true;	
	m_CanWindow			= false;
	m_CanLoop			= false;
		
	m_DisplayQuality	= kPreviewQuality;
	m_Transparency = 0;	// ABH
// ABH should init	m_Scale 
	m_Rotation = 0;
	
	// Default initialization
	TCueView::Init();
		
	//	Set up TCuePosition
	m_CuePosition = new TCuePosition(m_Bitmap->Bounds());
	
	//	Set up registration
	float regX = m_Bitmap->Bounds().Width()  / 2;
	float regY = m_Bitmap->Bounds().Height() / 2;
	m_CuePosition->Registration(BPoint(regX, regY));
	
	//--------------
	
	//
	//	Create temp wipe effect
	//
	
	//	Set time in local cue time, not global time
	TWipeRightOut *effect = new TWipeRightOut();	
	effect->EndTime(m_Duration);
	effect->Duration(m_Duration - 100);
	
	BRect effectRect = Bounds();
	const int32 addAmount = kChannelMaxHeight - (kCueInset*2);	
	effectRect.Set(Bounds().left+4, Bounds().top+addAmount, Bounds().right-4, Bounds().bottom-4);		
	
	//	We know we are the first effect so we can do this cheap calculation
	effectRect.top 	  += 6;
	effectRect.bottom = effectRect.top;
	effectRect.top	  = effectRect.bottom;
	effectRect.bottom = effectRect.top + kEffectHeight;

	//--------------
		
	//	Create effect view and add to list
	TTransitionView *effectView = new TTransitionView(effectRect, this, effect);
	m_EffectsList->AddItem(effectView);

	//	Create render semaphore
	m_RenderSem = create_sem(1, "VisualCueRenderSem");	
}

#pragma mark -
#pragma mark === Archiving Functions ===

// ABH missing functions 

bool TVisualCue::HasTransitionIn(){
	return m_HasTransitionIn;	// Have to initialize this somewhere!
}

bool TVisualCue::HasTransitionOut(){
	return m_HasTransitionOut;	// Have to initialize this somewhere!
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

	if (val > 0){
		m_Transparency = val;
	}
}

float TVisualCue::GetTransparency(){
	printf("GetDrawArea called\n");
	return m_Transparency;
}

BRect TVisualCue::GetDrawArea(){
	printf("GetDrawArea called\n");
}

float TVisualCue::GetRotation(){
	printf("GetRotation called\n");
	return m_Rotation;
}

void TVisualCue::SetRotation(float val){
	printf("SetRotation called\n");
	if (val >= 0){
		m_Rotation = val;
	}
}

BRect TVisualCue::GetArea(){
	printf("GetArea called - fix this!!!\n");
	return (BRect)(1.0,1.0);	//ABH fix this !!!
}
BRect TVisualCue::GetScaledArea(){
	printf("GetScaledAra called - fix this!!!\n");
	return (BRect)(1.0,1.0); 	//ABH fix this !!!
}
BPoint TVisualCue::GetScale(){
	printf("GetScale called\n");
	return m_Scale ;
}

void TVisualCue::SetScale(BPoint val){
	printf("SetScale called\n");
	
		m_Scale = val;

}

int TVisualCue::GetDuration(){
	printf("GetDuration called - fix this!!!\n");
	return 1; 		// ABH fix this!	
}

BRect TVisualCue::GetCroppedArea(){
	BRect area = (0,0);
	printf("GetCroppedArea called - fix this!!!\n");
	return (BRect)(1.0,1.0); // fix this !!!
}
//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TVisualCue::Instantiate(BMessage *archive) 
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

status_t TVisualCue::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = TCueView::Archive(data, deep);
	
	if (myErr == B_OK)
	{				
		// Add ourselves to the archive
		data->AddString("class", "TVisualCue");	
				
		// Add our member variables to the archive		
		data->AddData("DisplayQuality", B_ANY_TYPE, &m_DisplayQuality, sizeof(DisplayQuality) );
				
		//	Archive out deep data
		if (deep)
		{
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
	if (!m_IsInstantiated)
		return;

	PushState();
	
	//	Determine drawing state	
	switch (m_CueDisplayMode)
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
				if ( updateRect.right > Bounds().right - m_LResizeZone.Width())
					updateRect.right = Bounds().right - m_LResizeZone.Width();
					
				//	Clip out m_EffectsTray
				if (m_EffectsVisible)
				{
					if ( updateRect.bottom > m_EffectsTray.top)
						updateRect.bottom = m_EffectsTray.top;
				}
				
				//	Install clip region
				clipRegion.Set(updateRect);
				ConstrainClippingRegion( &clipRegion );
				
				// Set up display rect
				BRect pictRect = Bounds();
				if (m_EffectsVisible)
					pictRect.bottom = m_EffectsTray.top;
				pictRect.left = m_LResizeZone.right+1;
				pictRect.right = m_LResizeZone.right+pictRect.Height();
				pictRect.InsetBy(1,1);		
							
				// Fill cue area with bitmap
				if (m_Bitmap)
				{
					for (int32 index = 0; pictRect.right < updateRect.right; index++)
					{
						pictRect.left 	= ( pictRect.Height()* index ) + m_LResizeZone.right+1; 
						pictRect.right 	= pictRect.left + pictRect.Height(); 
						DrawBitmap(m_Bitmap, pictRect);
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
	if (m_IsMuted)
		return;
	
	//	Acquire render sem
	//status_t err = acquire_sem_etc(fCaptureSem, 1, B_TIMEOUT, timeout);
	acquire_sem(m_RenderSem);
	
	//	Render bitmap data internally
	RenderBitmapData();
	
	//	All done
	release_sem(m_RenderSem);
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
	
	const BRect timeBounds 	= Bounds();
	const int32 halfY 		= Bounds().Height() / 2;
	const int32 tickHeight 	= kTickHeight / 2;
	
	int32 fpsValue 	= GetFPSValue( GetCurrentTimeFormat() );
	
	int32 numTicks;	
	switch( GetCurrentResolution() )
	{

		// One Frame
		case 0:
			numTicks = 0;
			break;
		
		// 1/4 Second
		case 1:
			numTicks = kTickSpacing/ ( (fpsValue-1) / 4 ); 
			break;
			
		// Half Second
		case 2:						
			numTicks = kTickSpacing/ ( (fpsValue-1)/2 ); 
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
	
	if (numTicks > 0)
	{
		for (int32 lastIndex = index; lastIndex < (index+kTickSpacing); lastIndex+=numTicks)
		{
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

void TVisualCue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{		
	
		// 	Transition has been changed.  Update cue if in kCueTransitions mode
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
		//	m_OpacityDialog = NULL;
		//	break;
						
		//	Show scale dialog
		//case STAGE_CUE_SCALE_MSG:
		//	ShowScaleDialog();
		//	break;
			
		//	Scale dialog has been closed
		//case SCALE_CLOSE_MSG:
		//	m_ScaleDialog = NULL;
		//	break;
			
		//	Show rotate dialog
		//case STAGE_CUE_ROTATE_MSG:
		//	ShowRotateDialog();
		//	break;
			
		//	Rotate dialog has been closed
		//case ROTATE_CLOSE_MSG:
		//	m_RotateDialog = NULL;
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
	if(m_Panel)
	{
		// Clean up any RefFilters
		TRefFilter *theFilter = static_cast<TRefFilter *>( m_Panel->RefFilter() );
		if (theFilter)
			delete theFilter;
			
		delete m_Panel;
		m_Panel = NULL;				
	}
	
	TCueView::HidePanel();
}


#pragma mark -
#pragma mark === Sizing Routines ====


//---------------------------------------------------------------------
//	Expand
//---------------------------------------------------------------------
//
// 	Show cue controls
//

void TVisualCue::Expand(bool force)
{
	if (force == true)
		m_IsExpanded = false;

	// Do nothing if we are already expanded
	if (m_IsExpanded) 
		return;
		
	m_IsExpanded = true;
	
	// We show certain controls based on what cue display mode we are in
	//
	
	switch (m_CueDisplayMode)
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
// 	Hide cue controls
//

void TVisualCue::Contract(bool force)
{
	if (force == true)
		m_IsExpanded = true;
	
	// Do nothing if we are already contracted
	if (m_IsExpanded == false)
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
	m_CueDisplayMode = kCueContents;
			
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
	if (theMode == m_CueDisplayMode)
		return;
		
	//	Clean up old mode
	switch(m_CueDisplayMode)
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
	m_CueDisplayMode = theMode;
}


//---------------------------------------------------------------------
//	SetQualityMode
//---------------------------------------------------------------------
//
//	Toggle current cue quality mode
//

void TVisualCue::SetQualityMode(DisplayQuality theMode)
{
	/*
	//	Do nothing if we are alreay set to requested mode
	if (theMode == m_DisplayQuality)
		return;
		
	//	Set new mode variable
	m_DisplayQuality = theMode;

	//	Set new mode
	switch(m_DisplayQuality)
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
	}*/		
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

BBitmap *TVisualCue::GetBitmap(uint32 theTime)
{
	return m_Bitmap;
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
	
	if (theTime >= m_StartTime && theTime <= m_StartTime + m_Duration)
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
	/*
	if (IsOnStage())
	{
		TStageWindow *theStage	= static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage();
		TStageView 	 *stageView = theStage->GetStageView();

		theStage->Lock();		
		stageView->StageDraw( GetDrawArea(), GetCurrentTime());
		stageView->Draw( GetDrawArea());
		theStage->Unlock();
	}
	*/
}
	
