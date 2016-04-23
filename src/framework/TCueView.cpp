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
#include "AppGlobals.h"
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

#include "TTimer.h"
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

TCueView::TCueView(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime, char *name) : 
		  BView(bounds, name, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW|B_FRAME_EVENTS ),
		  BMediaNode("CueNode")
{	
	// Get cue ID
	m_ID = id;
	
	// Save parent
	m_Channel = parent;
	
	// Save start time and bounds
	m_StartTime = startTime;
	
	// The BView class will set this to true when it is done
	m_IsInstantiated = false;
	
	// Save start time for later initialization
	m_InsertTime 	= startTime;
		
	// Set these variables to NULL
	m_File 		  = NULL;
	m_AntThread   = NULL;
	m_EffectsList = NULL;
	
	//	Set these to false
	m_EditorOpen = false;
	
	// Set buttons to NULL
	m_LockButton 			= NULL;
	m_MuteButton 			= NULL;
}

//---------------------------------------------------------------------
//	ABH - missing Constructor
//---------------------------------------------------------------------
//
//

TCueView::TCueView(int16 id, TCueChannel *parent, BRect bounds, BPoint point, uint32 startTime, char *name) : 
		  BView(bounds, name, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW|B_FRAME_EVENTS ),
		  BMediaNode("CueNode")
{	
	// do something with scale, save it???
	m_InsertPoint = point; // ???

	// Get cue ID
	m_ID = id;
	
	// Save parent
	m_Channel = parent;
	
	// Save start time and bounds
	m_StartTime = startTime;
	
	
	// The BView class will set this to true when it is done
	m_IsInstantiated = false;
	
	// Save start time for later initialization
	m_InsertTime 	= startTime;
		
	// Set these variables to NULL
	m_File 		  = NULL;
	m_AntThread   = NULL;
	m_EffectsList = NULL;
	
	//	Set these to false
	m_EditorOpen = false;
	
	// Set buttons to NULL
	m_LockButton 			= NULL;
	m_MuteButton 			= NULL;
}


//
// ABH new constructor
//

TCueView::TCueView(BaseCueChunk *TheChunk, TCueChannel *parent, BRect bounds, char *name) : 
		  BView(bounds, name, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW|B_FRAME_EVENTS ),
		  BMediaNode("CueNode")
{	
	// Get cue ID
//	m_ID = id;
	
	// Save parent
	m_Channel = parent;
	
	// Save start time and bounds
	m_StartTime = TheChunk->startTime;
	
	// The BView class will set this to true when it is done
	m_IsInstantiated = false;
	
	// Save start time for later initialization
//	m_InsertTime 	= startTime;
		
	// Set these variables to NULL
	m_File 		  = NULL;
	m_AntThread   = NULL;
	m_EffectsList = NULL;
	
	//	Set these to false
	m_EditorOpen = false;
	
	// Set buttons to NULL
	m_LockButton 			= NULL;
	m_MuteButton 			= NULL;
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct based on an existing cue
//

TCueView::TCueView(const TCueView *theCue) : 
		  BView(theCue->Bounds(), ((BView *)theCue)->Name(), B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW | B_FRAME_EVENTS ),
		  BMediaNode("CueNode")
{	
	m_ID 				= theCue->m_ID;
	m_Channel 			= theCue->m_Channel;
	m_StartTime 		= theCue->m_StartTime;
	m_IsInstantiated 	= theCue->m_IsInstantiated;
	m_InsertTime 		= theCue->m_InsertTime;
	m_IsExpanded 		= theCue->m_IsExpanded;
	
	m_File 				= theCue->m_File;
	
	//m_LockButton 		= new TCueLockButton(theCue->m_LockButton);
	//m_MuteButton 		= new TCueMuteButton(theCue->m_MuteButton);
}


//---------------------------------------------------------------------
//	TCueView
//---------------------------------------------------------------------
//
//	Create new cue from a BMessage
//

TCueView::TCueView(BMessage *data) : BView (data), BMediaNode("CueNode")
{	
	// Set buttons to NULL
	m_LockButton 	= NULL;
	m_MuteButton 	= NULL;	
	m_Channel 		= NULL;
	m_File 		 	= NULL;
	m_AntThread  	= NULL;
	m_EffectsList 	= NULL;
	
	// Set default values
	m_IsSelected = false;	
	
	//
	// Rehydrate the cue from message data
	//
	
	// Extract our member variables from the archive
	ssize_t 	numBytes;
	rgb_color	*color;
	
	data->FindData("LowColor", B_RGB_COLOR_TYPE, (void **)&color, &numBytes );
	m_LowColor = *color;
	data->FindData("HighColor", B_RGB_COLOR_TYPE, (void **)&color, &numBytes );
	m_HighColor = *color;

	data->FindInt32("DrawingMode", m_DrawingMode);
	
	m_FileTime = data->FindInt32("FileTime");
	m_StartTime = data->FindInt32("StartTime");
	m_PreparedTime = data->FindInt32("PreparedTime");
	m_Duration = data->FindInt32("Duration");
	m_StartOffset = data->FindInt32("StartOffset");
	
	data->FindBool("CanTransition", &m_CanTransition);
	data->FindBool("CanPath", &m_CanPath);
			
	data->FindBool("CanStretch", &m_CanStretch);
	data->FindBool("CanWindow", &m_CanWindow);
	data->FindBool("CanLoop", &m_CanLoop);
	data->FindBool("CanEnvelope", &m_CanEnvelope);
	data->FindBool("HasDuration", &m_HasDuration);
	data->FindBool("IsVisible", &m_IsVisible);
	data->FindBool("CanCrop", &m_CanCrop);	
		
	data->FindBool("HasEditor", &m_HasEditor);
	data->FindBool("EditorOpen", &m_EditorOpen);
	
	data->FindBool("HasExternalEditor", &m_HasExternalEditor);
		
	data->FindInt16("ID", &m_ID);
	
	data->FindBool("IsLocked", &m_IsLocked);
	data->FindBool("IsExpanded", &m_IsExpanded);
	data->FindBool("IsMuted", &m_IsMuted);	
	
	data->FindRef("FileRef", &m_FileRef);
	
	data->FindBool("ShowDuration", &m_ShowDuration);
	
	CueDisplay *displayMode;
	data->FindData("CueDisplayMode", B_ANY_TYPE, (void **)&displayMode, &numBytes);
	m_CueDisplayMode = *displayMode;

	// Create resize zones
	const BRect bounds = Bounds();
	m_LResizeZone.Set(bounds.left, bounds.top, bounds.left+kResizeZoneWidth, bounds.bottom);
	m_RResizeZone.Set(bounds.right-kResizeZoneWidth, bounds.top, bounds.right, bounds.bottom );

	//	Set up our child item pointers
	m_StartText = (TCueTimeText *)FindView("StartTimeTextView");
	m_EndText 	= (TCueTimeText *)FindView("EndTimeTextView");

	m_LockButton = (TBitmapButton *)FindView("CueLockButton");
	m_MuteButton = (TBitmapButton *)FindView("CueMuteButton");					
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueView::~TCueView()
{
	// Stop marching ants and destroy MarchingThread
	if (m_AntThread)
	{
		delete m_AntThread;
		m_AntThread = NULL;
	}
	
	if(m_IsPlaying)
	{
		StopCue(GetCurrentTime());
	}
	
	//	Signal threads to quit
	m_TimeToQuit = true;
	
	//	Quit service thread
	if (write_port_etc(m_Port, 0x60000000, NULL, 0, B_TIMEOUT, DEFAULT_TIMEOUT))
		kill_thread(m_ServiceThread);
	
	status_t result;
	wait_for_thread(m_ServiceThread, &result);

	//	Wait for Run thread
	wait_for_thread(m_RunThread, &result);
	
	//	Empty out effects list
	if (m_EffectsList)	
	{
		for (int32 index = 0; index < m_EffectsList->CountItems(); index++)
		{
			//	Get the effects in the list
			TCueEffectView *effectView = (TCueEffectView *)m_EffectsList->ItemAt(index);
			if (effectView)
				delete effectView;
		}

		delete m_EffectsList;
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
	m_Rotation = 0;

	// We handle our own drawing
	SetViewColor(B_TRANSPARENT_32_BIT);

	//	Set up marching ants
	m_CueAnts.data[0] = 0x1F;
	m_CueAnts.data[1] = 0x3E;
	m_CueAnts.data[2] = 0x7C;
	m_CueAnts.data[3] = 0xF8;
	m_CueAnts.data[4] = 0xF1;
	m_CueAnts.data[5] = 0xE3; 
	m_CueAnts.data[6] = 0xC7;
	m_CueAnts.data[7] = 0x8F;

	// We are displaying system cursor
	m_AppCursorSet = true;
	
	//	Set up member variables
	m_TimeToQuit		= false;
	m_IsPlaying 		= false;
	m_IsStopping		= false;

	// Set up default settings
	m_IsPrepared		= false;
	m_IsPaused 			= false;
	m_MouseDown 		= false;
	m_Resizing			= false;	
	m_IsMuted			= false;
	m_ShowDuration		= false;
	m_EffectsVisible	= false;
		
	m_DrawingMode		= B_OP_COPY;
	m_LowColor			= kWhite;
	m_HighColor			= kBlack;
	
	m_CueDisplayMode	= kCueControls;
			
	// Mouse tracking
	m_LastClickPt.Set(0, 0);
		
	// Load cue icon
	LoadCueIcon();
	
	// Create resize zones
	BRect bounds = Bounds();
	m_LResizeZone.Set(bounds.left, bounds.top, bounds.left+kResizeZoneWidth, bounds.bottom);
	m_RResizeZone.Set(bounds.right-kResizeZoneWidth, bounds.top, bounds.right, bounds.bottom );
	
	//	Set effect tray to bogus value
	m_EffectsTray.Set(-500, 5000, -1, -1);
				
	//
	// Create and show start and stop time indicators
	//
	
	char timeStr[256];
	
	// Start time is attached to the top left side of the cue rect
	bounds = Bounds();
	bounds.left 	+= kTimeTextOffset+kResizeZoneWidth;
	bounds.top  	+= kTimeTextOffset; 
	bounds.right  	=  bounds.left + kTimeTextWidth; 
	bounds.bottom	=  bounds.top + kTimeTextHeight; 
			
	m_StartText = new TCueTimeText(this, START_TIME_TEXT_FOCUS_MSG, bounds, "StartTimeTextView", B_FOLLOW_LEFT | B_FOLLOW_TOP, kTextOutline);
	AddChild(m_StartText);
	
	// Set proper time	
	TimeToString(m_StartTime/1000, m_Channel->GetCueSheet()->GetTimeFormat(), timeStr, false);
	m_StartText->SetText(timeStr);
	
	// End time is attached to the bottom right side of the cue rect
	if (m_HasDuration)
	{
		bounds = Bounds();
		bounds.right 	-= kTimeTextOffset+kResizeZoneWidth;
		bounds.top  	=  bounds.bottom - (kTimeTextHeight+kTimeTextOffset); 
		bounds.left  	=  bounds.right - kTimeTextWidth; 
		bounds.bottom	-= kTimeTextOffset; 

		m_EndText = new TCueTimeText(this, END_TIME_TEXT_FOCUS_MSG, bounds, "EndTimeTextView", B_FOLLOW_RIGHT | B_FOLLOW_TOP, kTextOutline);
		AddChild(m_EndText);
				
		// Set proper time	
		TimeToString(m_StartTime+m_Duration, m_Channel->GetCueSheet()->GetTimeFormat(), timeStr, false);
		m_EndText->SetText(timeStr);		
		
		// Set up location of duration delta rect
		const BRect textRect = bounds;							
		m_DurationDeltaRect.top 	= textRect.top;
		m_DurationDeltaRect.bottom 	= textRect.bottom;
		m_DurationDeltaRect.right 	= textRect.left - kDurationDeltaOffset;
		m_DurationDeltaRect.left 	= m_DurationDeltaRect.right - m_DurationDeltaRect.Height();
		m_DurationDeltaRect.InsetBy(2,2);		
	}
	else
		m_EndText = NULL;
		
	// 	Create Cue Buttons
	//
	
	BRect buttonRect;
	
	//	Cue Lock Button
	buttonRect.Set( m_CueIcon->Frame().right+kCueButtonOffset, m_CueIcon->Frame().top, m_CueIcon->Frame().right+kCueButtonWidth+kCueButtonOffset, m_CueIcon->Frame().top + kCueButtonHeight );
	m_LockButton = new TBitmapButton( buttonRect, "CueLockButton", GetAppIcons()->m_Unlock, GetAppIcons()->m_Lock, 
									  this, new BMessage(LOCK_CUE_MSG), false, B_TWO_STATE_BUTTON); 
	AddChild(m_LockButton);
		
	// Cue Mute Button	
	buttonRect.Set( m_LockButton->Frame().right + kCueButtonOffset, buttonRect.top, m_LockButton->Frame().right+kCueButtonWidth+kCueButtonOffset, buttonRect.bottom );
	m_MuteButton = new TBitmapButton( buttonRect, "CueMuteButton", GetAppIcons()->m_VisibleUp, GetAppIcons()->m_VisibleDown, 
									  this, new BMessage(MUTE_BUTTON_MSG), false, B_TWO_STATE_BUTTON); 
	AddChild(m_MuteButton);

		
	//	Create Cue Effects Channel Spinner.  Locate it just below the Cue Lock Button
	//
	
	buttonRect.Set( m_LockButton->Frame().left,  m_LockButton->Frame().bottom+kCueButtonOffset,
					m_LockButton->Frame().right, m_LockButton->Frame().bottom+kCueButtonOffset+kCueButtonHeight);	
	m_EffectsButton = new TBitmapButton( buttonRect, "EffectsButton", GetAppIcons()->m_Expand, GetAppIcons()->m_Contract, 
										 this, new BMessage(CUE_EXPAND_MSG), false, B_TWO_STATE_BUTTON); 
	AddChild(m_EffectsButton);
	m_EffectsButton->Show();
	
	//	Create effects list
	m_EffectsList = new BList();
		
	//	Create our port
	m_Port = create_port(10, "CuePort");
			
	//	Create port service thread
	m_ServiceThread = spawn_thread(service_routine, "CueView:Service", B_NORMAL_PRIORITY, this);
	resume_thread(m_ServiceThread);	
	
	//	Create run thread
	m_RunThread = spawn_thread(run_routine, "CueView::Run", B_NORMAL_PRIORITY, this);	
	resume_thread(m_RunThread);
	
	//	Get pointer to timesource and register node
	status_t retVal = B_ERROR;
	media_node timeSource = m_Channel->GetCueSheet()->GetParent()->GetPlaybackEngine()->GetTimeSource();
	retVal = BMediaRoster::Roster()->SetTimeSourceFor(Node().node, timeSource.node);
	retVal = BMediaRoster::Roster()->StartNode(Node(), 0);
}

//
// missing function
//
void TCueView::Init(BaseCueChunk *TheChunk){

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

BArchivable *TCueView::Instantiate(BMessage *archive) 
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

status_t TCueView::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{				
		// Add our class name to the archive
		data->AddString("class", "TCueView");
		
		// Add our member variables to the archive
		data->AddData("LowColor", B_RGB_COLOR_TYPE, &m_LowColor, sizeof(rgb_color) );
		data->AddData("HighColor", B_RGB_COLOR_TYPE, &m_HighColor, sizeof(rgb_color) );		
		data->AddInt32("DrawingMode", (int32)m_DrawingMode);
	
		data->AddInt32("FileTime", m_FileTime);
		data->AddInt32("StartTime", m_StartTime);
		data->AddInt32("PreparedTime", m_PreparedTime);
		data->AddInt32("Duration", m_Duration);
		data->AddInt32("StartOffset", m_StartOffset);
		
		data->AddBool("CanTransition", m_CanTransition);
		data->AddBool("CanPath", m_CanPath);
						
		data->AddBool("CanStretch", m_CanStretch);
		data->AddBool("CanWindow", m_CanWindow);
		data->AddBool("CanLoop", m_CanLoop);
		data->AddBool("CanEnvelope", m_CanEnvelope);
		data->AddBool("HasDuration", m_HasDuration);
		data->AddBool("IsVisible", m_IsVisible);
		data->AddBool("CanCrop", m_CanCrop);	

		data->AddBool("HasEditor", m_HasEditor);
		data->AddBool("EditorOpen", m_EditorOpen);

		data->AddBool("HasExternalEditor", m_HasExternalEditor);
				
		data->AddInt16("ID", m_ID);
		
		data->AddBool("IsLocked", m_IsLocked);
		data->AddBool("IsExpanded", m_IsExpanded);
		data->AddBool("IsMuted", m_IsMuted);	
	
		data->AddRef("FileRef", &m_FileRef);
		
		data->AddBool("ShowDuration", m_ShowDuration);
		data->AddData("CueDisplayMode", B_ANY_TYPE, &m_CueDisplayMode, sizeof(CueDisplay) );
		
		// Add attached views
		if (deep)
		{			              		
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
	if ( m_Duration && m_CanStretch)
	{		
		BPoint startPt, endPt;
		SetHighColor(kSteelBlue);
		FillRect(m_RResizeZone);
		FillRect(m_LResizeZone);
		
		// Draw duration delta?
		if (m_ShowDuration)
		{
			DrawDurationDelta();
		}
			
		// Left side highlights
		SetHighColor(kDarkSteelBlue);
		startPt.Set(m_LResizeZone.left, m_LResizeZone.top);		
		endPt.Set(m_LResizeZone.left, m_LResizeZone.bottom);
		StrokeLine(startPt, endPt);	
		SetHighColor(kMediumSteelBlue);
		startPt.x++;
		endPt.x++;
		StrokeLine(startPt, endPt);	
		
		SetHighColor(kDarkSteelBlue);
		startPt.Set(m_LResizeZone.right, m_LResizeZone.top);		
		endPt.Set(m_LResizeZone.right, m_LResizeZone.bottom);
		StrokeLine(startPt, endPt);	
		SetHighColor(kMediumSteelBlue);
		startPt.x--;
		endPt.x--;
		StrokeLine(startPt, endPt);	
		
		// Left Shadow
		SetHighColor(kDarkGrey);	
		startPt.Set(m_LResizeZone.right+1, m_LResizeZone.top);		
		endPt.Set(m_LResizeZone.right+1, m_LResizeZone.bottom);
		StrokeLine(startPt, endPt);		

		// Right side highlights
		SetHighColor(kDarkSteelBlue);
		startPt.Set(m_RResizeZone.right, m_RResizeZone.top);		
		endPt.Set(m_RResizeZone.right, m_RResizeZone.bottom);
		StrokeLine(startPt, endPt);	
		SetHighColor(kMediumSteelBlue);
		startPt.x--;
		endPt.x--;
		StrokeLine(startPt, endPt);	
		
		SetHighColor(kDarkSteelBlue);
		startPt.Set(m_RResizeZone.left, m_RResizeZone.top);		
		endPt.Set(m_RResizeZone.left, m_RResizeZone.bottom);
		StrokeLine(startPt, endPt);	
		SetHighColor(kMediumSteelBlue);
		startPt.x++;
		endPt.x++;
		StrokeLine(startPt, endPt);		
		
		// Right Shadow
		SetHighColor(kDarkGrey);	
		startPt.Set(m_RResizeZone.left-1, m_LResizeZone.top);		
		endPt.Set(m_RResizeZone.left-1, m_LResizeZone.bottom);
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
	if (m_IsSelected)
		DoMarchingAnts(bounds);

	// Restore environment
	PopState();
	Looper()->Unlock();
}

// ABH missing function???

uint32 TCueView::GetDuration(){
	printf("GetDuration called\n");
	return m_Duration;
}

// ABH missing function

void TCueView::SetTransitionInID(uint32 id){
	m_TransitionInID = id;
}

// ABH missing function

void TCueView::SetTransitionOutID(uint32 id){
	m_TransitionOutID = id;
}

// ABH missing function

void TCueView::SetTransitionInDuration(uint32 length){
	m_TransitionInDuration = length;
}

// ABH missing function

void TCueView::SetTransitionOutDuration(uint32 length){
	m_TransitionOutDuration = length;
}

// ABH missing function

uint32 TCueView::GetTransitionInDuration(){
	return m_TransitionInDuration;
}

// ABH missing function

uint32 TCueView::GetTransitionOutDuration(){
	return m_TransitionOutDuration;
}
//---------------------------------------------------------------------
//	DrawDurationDelta
//---------------------------------------------------------------------
//
//	Draw duration delta mark to the left of the m_EndText
//

void TCueView::DrawDurationDelta()
{
	if (m_ShowDuration)
	{
		if (m_EndText)
		{				
			m_EndText->Looper()->Lock();
			const BRect textRect = m_EndText->Frame();
			m_EndText->Looper()->Unlock();
			
			Looper()->Lock();
			
			BPoint startPt, endPt;
			SetHighColor(kBlack);
			
			// Bottom right to bottom left
			startPt.Set(m_DurationDeltaRect.right, m_DurationDeltaRect.bottom);
			endPt.Set(m_DurationDeltaRect.left, m_DurationDeltaRect.bottom);
			StrokeLine(startPt, endPt);
			// Bottom left to top center
			startPt.Set(m_DurationDeltaRect.left, m_DurationDeltaRect.bottom);
			endPt.Set(m_DurationDeltaRect.right - (m_DurationDeltaRect.Width()/2), m_DurationDeltaRect.top);
			StrokeLine(startPt, endPt);
			// Top center to bottom right
			startPt.Set(m_DurationDeltaRect.right - (m_DurationDeltaRect.Width()/2), m_DurationDeltaRect.top);
			endPt.Set(m_DurationDeltaRect.right, m_DurationDeltaRect.bottom);
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
	if (m_EffectsVisible)
	{
		BPoint startPt, endPt;
		
		const BRect bounds = Bounds();		
		const int32 addAmount = kChannelMaxHeight - (kCueInset*2);
		
		SetHighColor(kHeaderGrey);
		FillRect(m_EffectsTray);
		
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
		for (int32 index = m_EffectsTray.top + (8+kEffectHeight); index < bounds.bottom; index += kEffectHeight)
		{
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
	if (m_EffectsList->CountItems() > 0)
	{
		for (int32 index = 0; index < m_EffectsList->CountItems(); index++)
		{
			//	Get the effects in the list
			TCueEffectView *effect = (TCueEffectView *)m_EffectsList->ItemAt(index);
			if (effect)
			{
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
	if ( ((TCueSheetWindow *)m_Channel->Window())->GetPlaybackEngine()->IsPlaying() )
		return;
		
	// Make sure we aren't locked
	if ( m_Channel->IsLocked() || m_IsLocked )
		return;
		
	// We have the focus
	MakeFocus(true);
	
	// Set flag
	m_MouseDown = true;
		
	// Check to see which button is down
	uint32 	buttons = 0;				
	Window()->CurrentMessage()->FindInt32("buttons", (int32 *)&buttons);
	
	// Is this a double click?  If so, open editor
	uint32 	type;
	int32	count = 0;
	BMessage *message = Window()->CurrentMessage();	
	
	// Determine which button has been clicked
	switch(buttons)
	{
		case B_PRIMARY_MOUSE_BUTTON:
			{
				if (B_OK == message->GetInfo("clicks", &type, &count) )
				{
					int32 clickCount = message->FindInt32("clicks", count-1);
					if ( (clickCount == 2) && (where == m_LastClickPt) )
					{
						//	In effects tray?
						if ( EffectsClick(where) )
						{
							TCueEffectView *effect = ClickedEffect(where);
							if (effect)
							{
								//	See if we need to deselect other effects
								if (effect->Selected() == false)
									DeselectAllEffects();

								//	Give it the double click
								effect->MouseDown(where, true);	
							}
							//	Deselect all
							else							
								DeselectAllEffects();
						}
						else
						{
							if(m_HasEditor)
								OpenEditor();
						}
					}
					//	Handle single click
					else
					{
						//	Was the click in the Effects tray?
						if ( EffectsClick(where) )
						{						
							TCueEffectView *effect = ClickedEffect(where);
							if (effect)
							{
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
						else
						{
							//	Check cue display state and handle properly
							switch (m_CueDisplayMode)
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
					m_Channel->GetCueSheet()->DeselectAllCues();
					Select();	
					
					//	Is the click in the cue body or the effect tray?
					if ( EffectsClick(where) )
					{
						OpenTransitionMenu(where);
					}
					else
					{
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
	Window()->CurrentMessage()->FindInt32("buttons", (int32 *)&buttons);
	
	if (buttons && m_MouseDown)
	{
		// 	If the left mouse button is down, they are doing the following:
		//	Dragging the cue either within the channel or to another channel, 
		//	stretching the cue or dragging a transition duration
		if (buttons == B_PRIMARY_MOUSE_BUTTON)
		{
			//	In effects tray?
			if ( EffectsClick(where) )
			{
				TCueEffectView *effect = ClickedEffect(where);
				if (effect)
					effect->MouseMoved(where);	
			}
			else
			{
				//	Check cue display state and handle properly
				switch (m_CueDisplayMode)
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
		m_MouseDown = false; 	
		
		// Save position for double click compare
		m_LastClickPt = where;
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
	// Do nothing if we are playing
	if ( TimeSource()->IsRunning() )
		return;
		
	// Make sure we aren't locked
	if ( m_Channel->IsLocked() || m_IsLocked )
		return;

	m_MouseDown = false;
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TCueView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{	
	
	// Do nothing if we are playing
	if ( TimeSource()->IsRunning() )
		return;
		
	// Make sure we aren't locked
	if ( m_Channel->IsLocked() || m_IsLocked )
	{
		// Inform channel of mouse move.  We are adding the left element of
		// the frame to create the proper coordinate for the timeline
		where.x += Frame().left;
		m_Channel->MouseMoved(where, code, a_message);
		return;	
	}
			
	// Display resizing cursors if mouse button is up
	uint32 	buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (int32 *)&buttons);
	if (m_CanStretch && !buttons)
	{
		switch(code)
		{
			
			case B_ENTERED_VIEW:
				if ( m_RResizeZone.Contains(where) || m_LResizeZone.Contains(where) )				
				{
					be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->m_ResizeHorzCursor );
				}
				break;
		
			case B_INSIDE_VIEW:		
				if ( m_RResizeZone.Contains(where) || m_LResizeZone.Contains(where) )
					be_app->SetCursor(&static_cast<MuseumApp *>(be_app)->m_ResizeHorzCursor );				
				else
				{
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
	where.x += (Frame().left + m_Channel->Bounds().left);
	m_Channel->MouseMoved(where, code, a_message);
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
	if (IsShiftKeyDown())
	{
		if(m_IsSelected)
			Deselect();
		else
			Select();
	}
	else
	{			
		// Deselect all other cues
		if (m_IsSelected)
			m_Channel->GetCueSheet()->DeselectAllCues(this);
		else
		{
			Select();
			m_Channel->GetCueSheet()->DeselectAllCues(this);
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
	if( m_RResizeZone.Contains(where) && m_CanStretch )
	{						
		ResizeRightSide();
		
		// Fix drag zones
		UpdateResizeZones();																
	}
	else if( m_LResizeZone.Contains(where) && m_CanStretch)
	{
		ResizeLeftSide();
		
		// Fix drag zones
		UpdateResizeZones();																
	}	
	//		
	// They must be moving the cue...
	//
	else
	{
		BRect bounds = Bounds();
		
		BMessage message(CUE_DRAG_MSG);
		message.AddPointer("CueView", this);
		BRect frame = Frame();
		BPoint dragPt = where;
		ConvertToParent(&dragPt);
		uint32 delta = dragPt.x - (frame.left + m_Channel->Bounds().left);
		message.AddInt32("CueDelta", delta);
					
		// Center the rect within the mouse for a better drag appearence
		static_cast<MuseumApp *>(be_app)->m_IsCueDrag = true;
		static_cast<MuseumApp *>(be_app)->m_CueDragDelta = delta;
		
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

void TCueView::KeyDown(const char *bytes, int32 numBytes)
{
	// Do nothing if we are playing
	if ( TimeSource()->IsRunning() )
		return;
	
	//	Igonore if we are locked
	if ( m_Channel->IsLocked() || m_IsLocked )
		return;
		
	//	Determine display mode and perform proper keydown action
	//	Determine drawing state	
	switch (m_CueDisplayMode)
	{
		case kCueControls:
			{
				//	Do we have effect/effects to delete?
				if (m_EffectsVisible && m_EffectsList->CountItems() > 0)
				{
					DeleteSelectedEffects();
				}
				//	Give click to channel to sort out
				else
				{
					m_Channel->KeyDown(bytes, numBytes);
				}
			}
			break;
			
		case kCueContents:
			m_Channel->KeyDown(bytes, numBytes);
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

void TCueView::KeyUp(const char *bytes, int32 numBytes)
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
	if (m_CanStretch)
	{		
		UpdateResizeZones();
		
		//	Update effects
		for (int32 index = 0; index < m_EffectsList->CountItems(); index++)
		{
			TCueEffectView *effect = (TCueEffectView *)m_EffectsList->ItemAt(index);
			if(effect)
				effect->Resized();		
		}
	}
	
	// Fix duration delta location
	if (m_HasDuration)
	{
		if (m_EndText)
		{
			// Set up location of duration delta rect
			const BRect textRect = m_EndText->Frame();							
			m_DurationDeltaRect.top 	= textRect.top;
			m_DurationDeltaRect.bottom 	= textRect.bottom;
			m_DurationDeltaRect.right 	= textRect.left - kDurationDeltaOffset;
			m_DurationDeltaRect.left 	= m_DurationDeltaRect.right - m_DurationDeltaRect.Height();
			m_DurationDeltaRect.InsetBy(2,2);		
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
	if(m_Channel == NULL)
	{
		m_Channel = (TCueChannel *)Parent();
		
		BList *cueList = m_Channel->GetCueList();
		
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

void TCueView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		if ( message->WasDropped() )
		{
			// Handle both of these cases by calling the channel's
			// to the cue's channel			
			case CUE_DRAG_MSG:
			case CUE_ICON_DRAG_MSG:	
				m_Channel->MessageReceived(message);
				break;											
		}
		
		case LOCK_CUE_MSG:
			{
				// Ignore if channel is locked
				if (m_Channel->IsLocked())
					return;
					
				if (m_IsLocked)
					UnlockCue();
				else
					LockCue();
			}
			break;
		
		//	Message from cue
		case MUTE_BUTTON_MSG:
			{
				// Ignore if channel is locked
				if (m_Channel->IsLocked())
					return;
			
				// Ignore if we are locked
				if (m_IsLocked)
					return;
				
				//	Change our state
				if (m_IsMuted)
					Unmute();
				else
					Mute();
					
				//	Update cues visual appearence
				if (m_IsVisible)
				{
					BMessage *message = new BMessage(TIMELINE_DRAG_MSG);
					message->AddInt32("TheTime", GetCurrentTime());
					m_Channel->GetCueSheet()->MessageReceived(message);
					delete message;
				}		
			}
			break;
					
		case MARCH_ANTS_MSG:
			{				
				if (m_IsSelected)
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
				if (m_IsLocked)
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
			if (m_HasDuration)
			{
				if (m_ShowDuration)
					HideDuration();
				else
					ShowDuration();
				break;			
			}

		// 	Message sent when time text loses focus.  Extract the time and adjust cue postion
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
			m_Channel->MessageReceived(message);
			break;
			
		//	Pass up to parent view
		default:
			BView::MessageReceived(message);						
			break;			
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
	TimeToString(m_StartTime, m_Channel->GetCueSheet()->GetTimeFormat(), timeStr, false);
	m_StartText->SetText(timeStr);	
}

//---------------------------------------------------------------------
//	UpdateEndTimeText
//---------------------------------------------------------------------
//
//	Someone has modified the end time.  Update the end time text box
//

void TCueView::UpdateEndTimeText()
{
	if (m_HasDuration)
	{
		// Update text depending on mode
		char timeStr[256];
		
		if (m_ShowDuration)
			TimeToString(m_Duration, m_Channel->GetCueSheet()->GetTimeFormat(), timeStr, false);
		else
			TimeToString(m_StartTime+m_Duration, m_Channel->GetCueSheet()->GetTimeFormat(), timeStr, false);
		
		m_EndText->SetText(timeStr);		
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
	if (theTime != m_StartTime)
	{
		// Check for collisions with other cues
		if (m_Channel->CanInsertCue(this, theTime, true))
		{													
			// Remove cue from old location
			m_Channel->RemoveCue(this);
			
			// Insert cue into channel at new location						
			m_Channel->InsertCue(this, theTime);
		}
		else
		{
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

	int64 endTime = m_StartTime + m_Duration;

	if (theTime != endTime)
	{
		uint32 newStartTime = theTime - m_Duration;
					
		// Check for collisions with other cues
		if (m_Channel->CanInsertCue(this, newStartTime, true))
		{													
			// Remove cue from old location
			m_Channel->RemoveCue(this);
			
			// Insert cue into channel at new location						
			m_Channel->InsertCue(this, newStartTime);
		}
		else
		{
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
	int32 startPixels 	 = TimeToPixels( m_StartTime, theFormat, theResolution);
	int32 durationPixels = TimeToPixels( m_Duration, theFormat, theResolution);
	
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
	if (!m_IsLocked && !m_IsSelected)
	{				
		m_IsSelected = true;
		
		if (Looper()->Lock())
		{
			MakeFocus(true);				
			Looper()->Unlock();
		}
		
		// Fire up marching ant thread
		m_AntThread = new TTimer(this, new BMessage(MARCH_ANTS_MSG), 120000);		
	}
}

//---------------------------------------------------------------------
//	Deselect
//---------------------------------------------------------------------
//
//

void TCueView::Deselect()
{
	if (m_IsSelected)
	{
				
		m_IsSelected = false;
		
		if ( Looper()->Lock() )
		{
			Draw(Bounds());
			Looper()->Unlock();
		}
		
		if (m_AntThread)
		{
			delete m_AntThread;
			m_AntThread = NULL;
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
	if (LockLooper())
	{
		PushState();
		
		SetHighColor(kBlack);
		
		int16	index;
		uchar	lastPart;
		
		lastPart = m_CueAnts.data[7];
		
		for (index = 7; index > 0; --index)
			m_CueAnts.data[index] = m_CueAnts.data[index-1];
	
		m_CueAnts.data[0] = lastPart;
			
		StrokeRect(antsRect, m_CueAnts);
		
		PopState();
		UnlockLooper();
	};
}


//---------------------------------------------------------------------
//	SelectAllEffects
//---------------------------------------------------------------------
//
//

void TCueView::SelectAllEffects()
{
	LockLooper();
	
	for (int32 index = 0; index < m_EffectsList->CountItems(); index++)
	{
		//	Get the effects in the list
		TCueEffectView *effect = (TCueEffectView *)m_EffectsList->ItemAt(index);
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
	
	for (int32 index = 0; index < m_EffectsList->CountItems(); index++)
	{
		//	Get the effects in the list
		TCueEffectView *effect = (TCueEffectView *)m_EffectsList->ItemAt(index);
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
	
	for (int32 index = 0; index < m_EffectsList->CountItems(); index++)
	{
		//	Get the effects in the list
		TCueEffectView *effect = (TCueEffectView *)m_EffectsList->ItemAt(index);
		if (effect)
		{
			//	Delete if selected
			if (effect->Selected())
			{
				m_EffectsList->RemoveItem(effect);
				delete effect;
				index--;				
			}
		}
	}
	
	//	Redraw
	Invalidate(m_EffectsTray);
	
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
	m_IsMuted = true;
}


//---------------------------------------------------------------------
//	Unmute
//---------------------------------------------------------------------
//
//

void TCueView::Unmute()
{
	m_IsMuted = false;
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
	m_IsLocked = true;
	
	// Disable text fields
	if (m_StartText)
		m_StartText->LockText();
	
	if (m_EndText)
		m_EndText->LockText();	
		
	// Redraw lock button
	if (m_LockButton)
		m_LockButton->Invalidate();
			
	Deselect();
}


//---------------------------------------------------------------------
//	Unlock
//---------------------------------------------------------------------
//
//

void TCueView::UnlockCue()
{
	m_IsLocked = false;
	
	// Enable text fields
	if (m_StartText)
		m_StartText->UnlockText();
	
	// Only unlock end text if it is in end time mode
	if (m_EndText)
	{
		if (m_ShowDuration == false)
			m_EndText->UnlockText();	
	}
	
	// Redraw lock button
	if (m_LockButton)
		m_LockButton->Invalidate();

}


#pragma mark -
#pragma mark === Resizing Routines ===

//---------------------------------------------------------------------
//	UpdateResizeZones
//---------------------------------------------------------------------
//
// 	The cue has been resized.  Update the mouse over zones that allow
//	horizontal resizing
//

void TCueView::UpdateResizeZones()
{	
	BRect bounds = Bounds();
	
	const int32 addAmount = kChannelMaxHeight - (kCueInset*2);
	 
	//	Clip height of resize zones
	if (bounds.bottom > addAmount)
		bounds.bottom = addAmount;
		
	m_LResizeZone.Set(bounds.left, bounds.top, bounds.left+kResizeZoneWidth, bounds.bottom);
	m_RResizeZone.Set(bounds.right-kResizeZoneWidth, bounds.top, bounds.right, bounds.bottom );
	
	//	Reset size of m_Effects tray
	if (m_EffectsVisible)
		m_EffectsTray.Set(Bounds().left+4, Bounds().top+addAmount, Bounds().right-4, Bounds().bottom-4);
		
	//	Adjust effects location
	for (int32 index = 0; index < m_EffectsList->CountItems(); index++)
	{
		TCueEffectView *effect = (TCueEffectView *)m_EffectsList->ItemAt(index);
		if(effect)
			effect->Resized();		
	}			
}



//---------------------------------------------------------------------
//	ResizeRightSide
//---------------------------------------------------------------------
//
// 	Stretch the cue to the right side
//

void TCueView::ResizeRightSide()
{
	BPoint 	oldPt, resizePt;
	bool	wasResized = false;
			
	// Set up environment
	PushState();
	m_Channel->PushState();
	
	drawing_mode saveMode = m_Channel->DrawingMode();
	m_Channel->SetDrawingMode(B_OP_INVERT);
	
	m_Channel->SetPenSize(2.0);
	m_Channel->SetHighColor(kBlack);
	
	//	Get channel bounds
	const BRect channelBounds = m_Channel->Bounds();

	// Get right side limit point.  This will either be another cue or the physical end
	// of the cue channel.  We cannot allow a drag over another cues end time.	
	float 	limitX;
	BPoint	limitPt;
	if ( m_Channel->GetNeighborRightPoint(this, &limitPt) )
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
	uint32 	buttons = 0;
	GetMouse(&mousePt, &buttons, true);	
	
	while(buttons)
	{						
		// Convert the view to the parents coordinates for proper
		// mouse tracking and rect resizing
		resizePt = ConvertToParent(mousePt);
			
		// Idle if in same mouse location
		if (resizePt != oldPt)
		{			
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
			m_Channel->Looper()->Lock();
			m_Channel->StrokeRect(oldFrame);	
			m_Channel->StrokeRect(frame);
			m_Channel->Looper()->Unlock();	
												
			// Save point and frame for future compare
			oldPt 		= resizePt;	
			oldFrame 	= frame;	
			
			// Notify timeline of mouse location
			BMessage *message = new BMessage(UPDATE_TIMELINE_MSG);
			message->AddPoint("Where", resizePt);
			message->AddInt32("TheTime", GetCurrentTime());
			static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetTimeline()->MessageReceived(message);
			delete message;									
		}																				
										
		// Let other events through
		snooze(20 * 1000);
		
		// Get new mouse location and button state
		GetMouse(&mousePt, &buttons, true);							
	}
		
	// Only resize
	if( wasResized )
	{		
		wasResized = false;
		
		// Clean up last drag rect
		m_Channel->StrokeRect(oldFrame);
											
		// Adjust cue size
		float resizeBy = frame.right - (Frame().right + channelBounds.left);
		ResizeBy(resizeBy, 0);

		// Update the duration and end times					
		uint32 endTime = PixelsToTime( frame.right, GetCurrentTimeFormat(), GetCurrentResolution());	
		
		m_Duration = endTime - m_StartTime;
													
		UpdateStartTimeText();
		UpdateEndTimeText();
		
		Invalidate();
		
		// Update stage appearence
		BMessage *theMessage = new BMessage(UPDATE_TIMELINE_MSG);
		theMessage->AddInt32("TheTime", GetCurrentTime());
		static_cast<MuseumApp *>(be_app)->GetCueSheet()->PostMessage(theMessage, static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView());
		delete theMessage;		
	}

	// Set our flag to indicate mouse up happened
	m_MouseDown = false; 	

	// Restore environment
	m_Channel->SetDrawingMode(saveMode);

	PopState();
	m_Channel->PopState();
}



//---------------------------------------------------------------------
//	ResizeLeftSide
//---------------------------------------------------------------------
//
// 	Stretch the cue to the left side
//

void TCueView::ResizeLeftSide()
{
	BPoint 	oldPt, resizePt;
	bool	wasResized = false;
					
	// Set up environment
	PushState();
	m_Channel->PushState();
	
	drawing_mode saveMode = m_Channel->DrawingMode();
	m_Channel->SetDrawingMode(B_OP_INVERT);

	m_Channel->SetPenSize(2.0);
	m_Channel->SetHighColor(kBlack);
	
	//	Get channel bounds
	const BRect channelBounds = m_Channel->Bounds();
			
	// Get left side limit point.  This will either be another cue or the physical start
	// of the cue channel.  We cannot allow a drag over another cues end time.	
	float 	limitX;
	BPoint	limitPt;
	if ( m_Channel->GetNeighborLeftPoint(this, &limitPt) )
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
	uint32 	buttons = 0;
	GetMouse(&mousePt, &buttons, true);	
	
	while(buttons)
	{						
		// Convert the view to the parents coordinates for proper
		// mouse tracking and rect resizing
		resizePt = ConvertToParent(mousePt);
			
		// Idle if in same mouse location
		if (resizePt != oldPt)
		{			
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
			m_Channel->StrokeRect(oldFrame);	
			m_Channel->StrokeRect(frame);	
												
			// Save point and frame for future compare
			oldPt 		= resizePt;	
			oldFrame 	= frame;	
			
			// Notify timeline of mouse location
			BMessage *message = new BMessage(UPDATE_TIMELINE_MSG);
			message->AddPoint("Where", resizePt);
			message->AddInt32("TheTime", GetCurrentTime());
			static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetTimeline()->MessageReceived(message);
			delete message;							
		}																				
										
		// Let other events through
		snooze(20 * 1000);
		
		// Get new mouse location and button state
		GetMouse(&mousePt, &buttons, true);							
	}
		
	// Only resize
	if( wasResized )
	{		
		wasResized = false;
		
		// Clean up last drag rect
		m_Channel->StrokeRect(oldFrame);	
											
		// Adjust cue size		
		frame.left -= channelBounds.left;
		MoveTo(frame.left, kCueInset);
		float resizeBy = frame.right - (Frame().right + channelBounds.left);
		ResizeBy(resizeBy, 0);
		
		// Update the duration and end times												
		m_StartTime = PixelsToTime( frame.left, GetCurrentTimeFormat(), GetCurrentResolution());	
		uint32 endTime = PixelsToTime( frame.right, GetCurrentTimeFormat(), GetCurrentResolution());

		// Adjust cue duration
		m_Duration = endTime - m_StartTime;
			
		UpdateStartTimeText();
		UpdateEndTimeText();
		
		Invalidate();
				
		// Update stage appearence
		BMessage *theMessage = new BMessage(UPDATE_TIMELINE_MSG);
		theMessage->AddInt32("TheTime", GetCurrentTime());
		static_cast<MuseumApp *>(be_app)->GetCueSheet()->PostMessage(theMessage, static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetCueSheetView());
		delete theMessage;
	}
	
	// Set our flag to indicate mouse up happened
	m_MouseDown = false; 	
					
	// Restore environment
	m_Channel->SetDrawingMode(saveMode);

	PopState();
	m_Channel->PopState();
					
}



//---------------------------------------------------------------------
//	Contract
//---------------------------------------------------------------------
//
// 	Hide cue controls
//

void TCueView::Contract(bool force)
{
	if (force == true)
		m_IsExpanded = true;
		
	// Do nothing if we are already contracted
	if (m_IsExpanded == false)
		return;

	if (m_EffectsVisible == true)
		HideEffects();
	
	m_IsExpanded = false;
	
	if (m_CueIcon)
		m_CueIcon->MoveTo(kItemOffscreenValue, kItemOffscreenValue);

	if (m_LockButton)
		m_LockButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);

	if (m_MuteButton)
		m_MuteButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);
}



//---------------------------------------------------------------------
//	Expand
//---------------------------------------------------------------------
//
// 	Show cue controls
//

void TCueView::Expand(bool force)
{
	if (force == true)
		m_IsExpanded = false;
		
	// Do nothing if we are already expanded
	if (m_IsExpanded == true)
		return;
		
	m_IsExpanded = true;
	
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
	if (m_EffectsVisible == true)
	{
		//	Effects are now visible
		m_EffectsVisible = false;
															
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
	if (m_EffectsVisible == false)
	{
		//	Effects are now visible
		m_EffectsVisible = true;

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
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);
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
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);
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
	return m_Channel->m_ID;
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
	BMenuItem 	*selected;
	
	// Create the menu and mark the current transition
	TCueMenu *theMenu = new TCueMenu(this);
	
	// Set menu location point		
	ConvertToScreen(&menuPt);
	selected = theMenu->Go(menuPt);
	
	if (selected)
	{
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
	BMenuItem *selected;
	
	// Create the menu and mark the current transition
	TTransitionMenu *theMenu = new TTransitionMenu(this, true);
	
	// Set menu location point		
	ConvertToScreen(&menuPt);
	selected = theMenu->Go(menuPt);
	
	if (selected)
	{
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
	m_CueDisplayMode = kCueContents;
		
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
	
	if (m_CueIcon)
	{
		rect.Set(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		rect.OffsetBy(kResizeZoneWidth+5, 0);
		m_CueIcon->MoveTo(rect.left, rect.top);
	}
			
	if (m_LockButton)
	{		
		rect.Set( m_CueIcon->Frame().right+kCueButtonOffset, m_CueIcon->Frame().top, m_CueIcon->Frame().right+kCueButtonWidth+kCueButtonOffset, m_CueIcon->Frame().top + kCueButtonHeight );
		m_LockButton->MoveTo(rect.left, rect.top);
	}
			
	if (m_MuteButton)
	{
		rect.Set( m_LockButton->Frame().right+kCueButtonOffset, rect.top, m_LockButton->Frame().right+kCueButtonWidth+kCueButtonOffset, rect.bottom );
		m_MuteButton->MoveTo(rect.left, rect.top);
	}
	
	if (m_EffectsButton)
	{
		rect.Set( m_LockButton->Frame().left, m_LockButton->Frame().bottom+kCueButtonOffset, m_LockButton->Frame().right, m_LockButton->Frame().bottom+kCueButtonOffset+kCueButtonHeight);
		m_EffectsButton->MoveTo(rect.left, rect.top);
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
	if (m_CueIcon)
		m_CueIcon->MoveTo(kItemOffscreenValue, kItemOffscreenValue);   
	
	if (m_LockButton)
		m_LockButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);   
	
	if (m_MuteButton)
		m_MuteButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);
		
	if (m_EffectsButton)
		m_EffectsButton->MoveTo(kItemOffscreenValue, kItemOffscreenValue);   

}


//---------------------------------------------------------------------
//	ShowDuration
//---------------------------------------------------------------------
//
//	Show cue duration.  Draw delta symbol and display duration
//	in the m_EndText
//

void TCueView::ShowDuration()
{		
	if (m_HasDuration)
	{
		// Get duration
		char timeStr[256];
		TimeToString(m_Duration, m_Channel->GetCueSheet()->GetTimeFormat(), timeStr, false);
		m_EndText->SetText(timeStr);
				
		// Lock
		m_EndText->LockText();

		// Set flag to true
		m_ShowDuration = true;
		
		// Draw duration delta
		DrawDurationDelta();
	}	
}


//---------------------------------------------------------------------
//	HideDuration
//---------------------------------------------------------------------
//
//	Hide cue duration.  Erase delta symbol and display end time
//	in the m_EndText
//

void TCueView::HideDuration()
{
		
	if (m_HasDuration)
	{
		// Get end time
		char timeStr[256];
		TimeToString(m_StartTime+m_Duration, m_Channel->GetCueSheet()->GetTimeFormat(), timeStr, false);
		m_EndText->SetText(timeStr);
				
		// Unlock
		m_EndText->UnlockText();
		
		// Set flag to false
		m_ShowDuration = false;	
		
		// Erase delta
		Looper()->Lock();
		Invalidate(m_DurationDeltaRect);
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
	
	//	Set new cue mode
	m_CueDisplayMode = theMode;
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
	m_IsPlaying = true;
}

//---------------------------------------------------------------------
//	StopCue
//---------------------------------------------------------------------
//
//	Tell cue we are stopping
//

void TCueView::StopCue(uint32 theTime)
{
	m_IsPlaying = false;
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
	if (!m_IsExpanded || m_CueDisplayMode != kCueControls)
		m_CueIcon->MoveTo(kItemOffscreenValue, kItemOffscreenValue);
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
	BMessage *theMessage = new BMessage(ADD_REF_MSG);
	theMessage->AddRef("FileRef", &m_FileRef);
	TBrowserWindow * theBrowser = static_cast<MuseumApp *>(be_app)->GetBrowser();
	if (theBrowser)
	{
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
	if ( m_EffectsVisible && m_EffectsTray.Contains(where))
	{
		return true;
	}
	else
	{
		return false;
	}
}		


//---------------------------------------------------------------------
//	ClickedEffect
//---------------------------------------------------------------------
//
//	Return pointer to TCueEffectView clicked or NULL
//

TCueEffectView *TCueView::ClickedEffect(BPoint where)
{
	//	Was the click in the Effects tray?
	if ( m_EffectsVisible && m_EffectsTray.Contains(where))
	{
		//	Pass click to effects cue
		if (m_EffectsList->CountItems() > 0)
		{
			for (int32 index = 0; index < m_EffectsList->CountItems(); index++)
			{
				//	Get the effects in the list
				TCueEffectView *effect = (TCueEffectView *)m_EffectsList->ItemAt(index);
				
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

BMessage *TCueView::Copy()
{
	// Create a message for the new cue to be archived in
	BMessage *theMessage = new BMessage();
	
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
	m_Duration = theDuration;
}


//---------------------------------------------------------------------
//	SetStartTime
//---------------------------------------------------------------------
//
//	Set the cues startTime
//

void TCueView::SetStartTime(uint32 theTime)
{
	m_StartTime = theTime;
}

//---------------------------------------------------------------------
//	SetInstantiated
//---------------------------------------------------------------------
//
//	Set m_Resolution to theResolution value
//

void TCueView::SetInstantiated( bool theVal)
{
	m_IsInstantiated = theVal;
}


//---------------------------------------------------------------------
//	SetEditorOpen
//---------------------------------------------------------------------
//
//	Set m_Editor to the value
//

void TCueView::SetEditorOpen( bool theVal)
{
	m_EditorOpen = theVal;
}

//---------------------------------------------------------------------
//	SetChannel
//---------------------------------------------------------------------
//
//	Set m_Channel to the channel
//

void TCueView::SetChannel( TCueChannel *channel)
{
	m_Channel = channel;
}


#pragma mark -
#pragma mark === BMediaNode Routines ===

//---------------------------------------------------------------------
//	ControlPort
//---------------------------------------------------------------------
//
//	Return our port ID
//

port_id TCueView::ControlPort() const
{
	return m_Port;
}


//---------------------------------------------------------------------
//	AddOn
//---------------------------------------------------------------------
//
//	We are not an add0n,  Return NULL
//


BMediaAddOn* TCueView::AddOn( int32 *internal_id) const
{
	return NULL;
}


#pragma mark -
#pragma mark === Thread Functions ===

//-------------------------------------------------------------------
//	service_routine
//-------------------------------------------------------------------
//
//	Static service thread function
//

status_t TCueView::service_routine(void * data)
{
	((TCueView *)data)->ServiceRoutine();
	
	return 0;
}


//-------------------------------------------------------------------
//	ServiceRoutine
//-------------------------------------------------------------------
//
//	Service thread function
//

void TCueView::ServiceRoutine()
{
	while (!m_TimeToQuit)
	{
		//	Read message
		status_t 		err  = 0;
		int32 			code = 0;
		char 			msg[B_MEDIA_MESSAGE_SIZE];
		
		err = read_port_etc(m_Port, &code, &msg, sizeof(msg), B_TIMEOUT, 10000);		
		
		if (err == B_TIMED_OUT) 
			continue;
		
		if (err < B_OK)
		{
			printf("TTimePalette::ServiceRoutine: Unexpected error in read_port(): %x\n", err);
			continue;
		}
		
		// dispatch message
		if (code == 0x60000000)
			break;		
		
		if ( (BMediaNode::HandleMessage(code, msg, err)) )
		{				
			BMediaNode::HandleBadMessage(code, msg, err); 
		}

	}
}


//-------------------------------------------------------------------
//	run_routine
//-------------------------------------------------------------------
//
//	Static run thread function
//

status_t TCueView::run_routine(void *data)
{
	((TCueView *)data)->RunRoutine();	
	return 0;
}



//-------------------------------------------------------------------
//	RunRoutine
//-------------------------------------------------------------------
//
//	Run thread function
//

void TCueView::RunRoutine()
{
	while(!m_TimeToQuit)
	{
		snooze(20000);
	
		//	Update media_server
		if (TimeSource()->IsRunning())
		{
			//	Do nothing if we have been muted
			if (!m_IsMuted)
			{
				const uint32 curTime = GetCurrentTime();
				
				//	Check and see if we need to start internal playback
				if ( (m_IsPlaying == false) && (curTime >= m_StartTime) )
				{
					PlayCue(m_StartTime);
				}
				
				//	Handle current playback
				if (m_IsPlaying == true)
				{				
					//	Handle playback at current time
					if (curTime <= (m_StartTime + m_Duration))
						HandlePlayback(curTime);
					//	Time to stop
					else
						StopCue(curTime);								
				}
			}
		}
		//	Stop cue playback
		else
		{
			if (m_IsPlaying == true)
				StopCue(GetCurrentTime());	
		}
	}
}

