//---------------------------------------------------------------------
//
//	File:	TCueSheetWindow.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Cue sheet window
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <stdio.h>

#include <Application.h>
#include <Debug.h>
#include <MediaNode.h>
#include <MediaRoster.h>
#include <TimeCode.h>

#include "AppAlerts.h"
#include "AppConstants.h"
#include "AppMessages.h"
#include "DebugUtils.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "MuseumApp.h"

#include "TUndoEngine.h"

#include "TMediaCuePalette.h"
#include "TTimePalette.h"
#include "TTimePaletteView.h"
#include "TTransportPalette.h"
#include "TTransportPaletteView.h"
#include "TStageWindow.h"
#include "TStageView.h"
#include "TBrowserWindow.h"
#include "TCueSheetTimeView.h"
#include "TCueSheetView.h"
#include "THeaderContainerView.h"
#include "TCueSheetScrollBarH.h"
#include "TCueSheetScrollBarV.h"
#include "TCueChannel.h"
#include "TCueChannelHeader.h"
#include "TCueView.h"
#include "TMuseumMenus.h"
#include "TExportTimeView.h"
#include "TExportZone.h"
#include "TTimelineView.h"
#include "TTimeScalerView.h"
#include "TToolbar.h"
#include "TMuseumClipboard.h"
#include "TPlaybackEngine.h"
#include "TAudioEngine.h"

#include "TCueSheetWindow.h"

// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Default Constructor
//

TCueSheetWindow::TCueSheetWindow(BRect bounds, int32 id) : BWindow(bounds, "Untitled", B_DOCUMENT_WINDOW, B_WILL_ACCEPT_FIRST_CLICK)
{			
	// Set up ID
	m_ID = id;
	
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Create new window from a BMessage
//

TCueSheetWindow::TCueSheetWindow(BMessage *data) : BWindow (data)
{

	//
	// Rehydrate the cue from message data
	//
	
	// Init Undo Engine
	m_UndoEngine = new TUndoEngine();
	
	//	Set up variable settings
	m_File = NULL;
		
	// Dialog box state
	m_PanelOpen 	= false;

	//	Update menu pointers
	BMenuBar *badBar = (BMenuBar *)FindView("MuseumMenu");
	RemoveChild(badBar);
	delete badBar;
	
	//
	//	Create new menu...
	
	// Set up menu bar
	BRect mbarRect = Bounds();
	mbarRect.bottom = mbarRect.top+kMenuHeight;
	BMenuBar* mbar = new BMenuBar(mbarRect, "MuseumMenu");

	// Create a new TMuseumMenu object and pass the  BMenuBar object in to the constructor. 
	// Then add the individual menu items with the TMuseumMenu object
	m_AppMenu = new TMuseumMenus(mbar);	
	AddChild(mbar);
	
	
	//	Update pointers to child views
	m_StageWindow		= (TStageWindow *)FindWindow("Stage");	
	m_HeaderContainer 	= (THeaderContainerView *)FindView("HeaderContainerView");
	m_Toolbar 			= (TToolbar *)FindView("TooolbarView");
	m_TimeZone 			= (TCueSheetTimeView *)FindView("TimeZoneView");
	m_ExportTimeView 	= (TExportTimeView *)FindView("ExportTimeView");
	m_ExportZone 		= (TExportZone *)FindView("ExportZoneView");
	m_Timeline 			= (TTimelineView *)FindView("TimelineView");
	m_CueSheetView 		= (TCueSheetView *)FindView("CueSheetView");	
	if (!m_CueSheetView)
		printf("TCSW:TCSW m_CueSheetView is null!\n");
	m_HScroll 			= (TCueSheetScrollBarH *)FindView("HCueScroll");	
	m_VScroll 			= (TCueSheetScrollBarV *)FindView("VCueScroll");		
	m_TimeScaler 		= (TTimeScalerView *)FindView("TimeScalerView");
	
		
	//	Match CueChannels to CueChannelHeaders
	printf("TCSW:TCSW GetChannelList start\n");
	for(int32 index = 0; index < m_CueSheetView->GetChannelList()->CountItems(); index++)
	{
		TCueChannelHeader *header 	= (TCueChannelHeader *)m_HeaderContainer->ChildAt(index);
		TCueChannel *channel 		= (TCueChannel *)m_CueSheetView->GetChannelList()->ItemAt(index);
				
		header->SetChannel(channel);
		channel->SetHeader(header);
	}
		printf("TCSW:TCSW GetChannelList end\n");

	
	//	Set scrollbar targets
	m_HScroll->SetTarget(m_CueSheetView);
	m_VScroll->SetTarget(m_CueSheetView);
		
	// 	Adjust the scroll bars
	AdjustScrollBars();
	
	// 	Add ourself to cue sheet list
	static_cast<MuseumApp *>(be_app)->GetCueSheetList()->AddItem(this);
	
	// 	Create playbackEngine
	m_PlaybackEngine = new TPlaybackEngine(m_CueSheetView);	
	ASSERT(m_PlaybackEngine);
	
	//	Attach palettes and stage to playbackEngine
	//TTimePalette *timePalette = static_cast<MuseumApp*>(be_app)->GetTimePalette();
	//ASSERT(timePalette);
	//timePalette->GetTimePaletteView()->ConnectToPlaybackEngine(m_PlaybackEngine);
		
	//TTransportPalette *transport = static_cast<MuseumApp*>(be_app)->GetTransport();	
	//ASSERT(transport);
	//transport->GetTransportPaletteView()->ConnectToPlaybackEngine(m_PlaybackEngine);
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueSheetWindow::~TCueSheetWindow()
{
	// Kill Undo Engine
	delete m_UndoEngine;

	// Close our file
	if (m_File)
		delete m_File;
		
	// Stop sound playback
	if (m_AudioEngine)
		delete m_AudioEngine;

	// Stop playback
	if (m_PlaybackEngine)
	{
		//m_PlaybackEngine->Stop();
		delete m_PlaybackEngine;
	}
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	gzr: to do...  I am doing some fudging with the CueSheetView.. I am subtracting
//	one pixel from the right and bottom to avoid any contact with the scroll bars.
//	If the view comes close to the scroll bar they bleed into my view.  I may be doing
//	something wrong or the system may be.  Try to find out.

void TCueSheetWindow::Init()
{		
	status_t retVal;
	
	// Init Undo Engine
	m_UndoEngine = new TUndoEngine();
			
	// Member file used for saves
	m_File = NULL;
		
	// Dialog box state
	m_PanelOpen = false;
				
	// Create the stage window	
	BRect stageRect(0,0,320,240);
	m_StageWindow = new TStageWindow(stageRect, this);
	CenterWindow(m_StageWindow);
	
	// Set up menu bar
	BRect mbarRect = Bounds();
	mbarRect.bottom = mbarRect.top+kMenuHeight;
	BMenuBar* mbar = new BMenuBar(mbarRect, "MuseumMenu");
	
	// Set up application menus.
	// Create a new TMuseumMenu object and pass the  BMenuBar object in to the constructor. 
	// Then add the individual menu items with the TMuseumMenu object
	m_AppMenu = new TMuseumMenus(mbar);	
	
	AddChild(mbar);

	// 	Set the channel length to an hour duration in milliseconds
	float channelLength = TimeToPixels( 60 * 60 * 1000L, B_TIMECODE_24, 3);
	
	// 	Create Toolbar.  It is always at the top of the window, below the menu.
	BRect toolRect = Bounds();
	toolRect.Set(toolRect.left, kMenuHeight, toolRect.right, kMenuHeight+kToolbarHeight);
	m_Toolbar = new TToolbar(toolRect, this);
	AddChild(m_Toolbar);
	m_Toolbar->Show();
	
	// 	Create ExportTimeView.  It is always located below the toolbar and to the
	// 	very left of the cue sheet.  It is as wide as the cue sheet headers.
	BRect exportTimeRect;
	exportTimeRect.left		= m_Toolbar->Frame().left;
	exportTimeRect.top	 	= m_Toolbar->Frame().bottom+1;
	exportTimeRect.right  	= exportTimeRect.left + kHeaderWidth;
	exportTimeRect.bottom 	= exportTimeRect.top + kExportZoneHeight + 11;
	m_ExportTimeView = new TExportTimeView(this, exportTimeRect);
	AddChild(m_ExportTimeView);
	m_ExportTimeView->Show();
			
	// 	Create CueSheet Time Zone.  It is always located below the toolbar and to the
	// 	very left of the cue sheet.  It is as wide as the cue sheet headers.
	BRect zoneRect;
	zoneRect.left	= exportTimeRect.left;
	zoneRect.top	= exportTimeRect.bottom+1;
	zoneRect.right 	= zoneRect.left+kHeaderWidth;
	zoneRect.bottom	= zoneRect.top + kTimelineHeight - 11;
	m_TimeZone = new TCueSheetTimeView(this, zoneRect);
	AddChild(m_TimeZone);
	m_TimeZone->Show();
		
	//	Create ExportZone.  It is always located below the toolbar and to the right
	//	of the TimeZone.  This is where the user determines the span of the cue sheet
	//	to preview or export
	BRect exportRect;
	exportRect.left   	= zoneRect.right + 1;
	exportRect.top    	= m_Toolbar->Frame().bottom+1;
	exportRect.right  	= Bounds().right;
	exportRect.bottom   = exportRect.top + kExportZoneHeight;
	m_ExportZone = new TExportZone(exportRect, this);
	AddChild(m_ExportZone);
	m_ExportZone->Show();
	
	// 	Create Timeline.  It is always located below the toolbar and to the right
	//	of the TimeZone.  This is where the time is indicated and the playback head resides
	BRect timeRect;
	timeRect.left 	= exportRect.left;
	timeRect.top 	= exportRect.bottom + 1;
	timeRect.right 	= Bounds().right;
	timeRect.bottom = timeRect.top + kTimelineHeight;
	m_Timeline = new TTimelineView(timeRect, this);
	AddChild(m_Timeline);
	m_Timeline->Show();

	// Create Header Container View
	BRect containerRect  = Bounds();
	containerRect.top    = m_Timeline->Frame().bottom;
	containerRect.right  = containerRect.left + kHeaderWidth;
	containerRect.bottom -= kScrollHeight+1;
	m_HeaderContainer = new THeaderContainerView(containerRect);
	AddChild(m_HeaderContainer);
	m_HeaderContainer->SetViewColor(kMediumGrey);
	
		
	// Create cue sheet view.  Add one pixel to left to fix drawing bug with header...
	BRect bounds = m_Timeline->Frame();
	bounds.Set(bounds.left, bounds.bottom+1, Bounds().right-(kScrollWidth+1), Bounds().bottom - (kScrollHeight+1));
	m_CueSheetView = new TCueSheetView(bounds, this);
	m_CueSheetView->SetViewColor(kGrey);
    
    	
	//
	// Create scroll bars
	//
	
	// Horizontal	
	BRect scrollRect = Bounds();
	scrollRect.Set(scrollRect.left+kTimeScalerWidth, scrollRect.bottom-kScrollHeight, scrollRect.right-kScrollWidth, scrollRect.bottom);
	m_HScroll = new TCueSheetScrollBarH(this, scrollRect, NULL, 0, channelLength);
	AddChild(m_HScroll);	
	m_HScroll->SetSteps(kTickSpacing, kTickSpacing*4);
		
	// Vertical
	scrollRect = Bounds();
	scrollRect.Set(scrollRect.right-kScrollWidth, scrollRect.top + ( m_Timeline->Frame().bottom+1), scrollRect.right, scrollRect.bottom-kScrollHeight);
	m_VScroll = new TCueSheetScrollBarV(this, scrollRect, m_CueSheetView, 0, channelLength);
	AddChild(m_VScroll);	
	m_VScroll->SetSteps(kTickSpacing, kTickSpacing*4);
	
	// Create TimeScaler
	BRect scalerRect(0, Bounds().bottom-14,kTimeScalerWidth, Bounds().bottom);
	m_TimeScaler = new TTimeScalerView(scalerRect, m_CueSheetView);
	AddChild(m_TimeScaler);
		
	// Limit the window size
	SetSizeLimits( 280, 1500, 280, 1500);
		        	
	// Add CueSheetView to frame
	AddChild(m_CueSheetView);
	
	// Adjust the scroll bars
	AdjustScrollBars();
	
	// Add ourself to cue sheet list
	static_cast<MuseumApp *>(be_app)->GetCueSheetList()->AddItem(this);		
	
	//	Get reference to MediaRoster
	BMediaRoster *roster = BMediaRoster::Roster();
	
	// Create PlaybackEngine and register it
	m_PlaybackEngine = new TPlaybackEngine(m_CueSheetView);	
	ASSERT(m_PlaybackEngine);
	roster->RegisterNode(m_PlaybackEngine);
	
	//	Get pointer to timesource and set playback engine to it
	media_node timeSource = m_PlaybackEngine->GetTimeSource();
	retVal = roster->SetTimeSourceFor(m_PlaybackEngine->Node().node, timeSource.node);
	
	//
	//	Register media nodes, set their timesource and start them		
	//
	
	//	Locator
	TTimePalette *timePalette = ((MuseumApp *)be_app)->GetTimePalette();
	retVal = roster->SetTimeSourceFor(timePalette->Node().node, timeSource.node);
	retVal = roster->StartNode(timePalette->Node(), 0);

	//	Transport
	TTransportPalette *transport = ((MuseumApp *)be_app)->GetTransport();
	retVal = roster->SetTimeSourceFor(transport->Node().node, timeSource.node);
	retVal = roster->StartNode(transport->Node(), 0);
	
	//	TimelineView
	retVal = roster->SetTimeSourceFor(m_Timeline->Node().node, timeSource.node);
	retVal = roster->StartNode(m_Timeline->Node(), 0);
	
	//	Stage
	TStageView *stageView = m_StageWindow->GetStageView();
	retVal = roster->SetTimeSourceFor(stageView->Node().node, timeSource.node);
	retVal = roster->StartNode(stageView->Node(), 0);
		
	//	Create audio engine
	m_AudioEngine = new TAudioEngine();	
}

#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TCueSheetWindow::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TCueSheetWindow") ) 
		return new TCueSheetWindow(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueSheetWindow::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BWindow::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TCueSheetWindow");
		
		// Add our member variables to the archive		
				
		// Add attached views
		if (deep)
		{				
			//	Archive StageWindow
			BMessage stageArchive;
			m_StageWindow->Archive(&stageArchive, true);
			data->AddMessage("StageWindow", &stageArchive);
					
		}				
	}
	
	
	Looper()->Unlock();
	
	return myErr;
}


#pragma mark -
#pragma mark === Window Visibility ===

//---------------------------------------------------------------------
//	Hide
//---------------------------------------------------------------------
//
//	Hide our attached stage window
//

void TCueSheetWindow::Hide()
{	
	// Hide stage
	m_StageWindow->Lock();
	if (m_StageWindow->IsHidden() == false)
		m_StageWindow->Hide();
	m_StageWindow->Unlock();

	// Pass up to parent
	BWindow::Hide();				
}

//---------------------------------------------------------------------
//	Show
//---------------------------------------------------------------------
//
//	Show our attached stage window and set ourself as the 
//	current Cue Sheet
//

void TCueSheetWindow::Show()
{
	// Show stage
	m_StageWindow->Lock();
	if (m_StageWindow->IsHidden())
		m_StageWindow->Show();
	m_StageWindow->Unlock();
				
	// We are now the main Cue Sheet
	static_cast<MuseumApp *>(be_app)->SetCueSheet(this);
	
	// Pass up to parent
	BWindow::Show();
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	If window is being activated show stage.  If it is being deactivated,
//	stop any playback and hide stage
//

void TCueSheetWindow::WindowActivated( bool activate)
{	
	// Move indicator tick offscreen to hide it
	if ( activate == false )
	{
		BPoint where;
		where.x = 0;
		BMessage *message = new BMessage(UPDATE_TIMELINE_MSG);
		message->AddPoint("Where", where);
		message->AddInt32("TheTime", GetCurrentTime());		
		m_Timeline->MessageReceived(message);
		delete message;	
	}
	else
	{
		// Tell stage to end edit mode
		BMessage *theMessage = new BMessage(END_STAGE_EDIT_MSG);
		m_StageWindow->PostMessage(theMessage, m_StageWindow->GetStageView());
		delete theMessage;
	}
	
	/*
	if (activate)
	{
		// Show stage
		m_StageWindow->Lock();
		if (m_StageWindow->IsHidden())
			m_StageWindow->Show();
		m_StageWindow->Unlock();
		
		// We are now the main Cue Sheet
		if ( static_cast<MuseumApp *>(be_app)->GetCueSheet() != this )
			static_cast<MuseumApp *>(be_app)->SetCueSheet(this);
	}
	else
	{
		m_StageWindow->Lock();
		if (m_StageWindow->IsHidden() == false)
		{
			if ( IsHidden() )
				m_StageWindow->Hide();
		}
		m_StageWindow->Unlock();
	}
	*/
	
	BWindow::WindowActivated(activate);
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::FrameResized( float newWidth, float newHeight)
{	
	AdjustScrollBars();
	
	BWindow::FrameResized(newWidth, newHeight);
}


//---------------------------------------------------------------------
//	QuitRequested
//---------------------------------------------------------------------
//
//

bool TCueSheetWindow::QuitRequested()
{	
	bool retVal = true;
	bool quit 	= false;	
	
	// Ask user to save document
	if (m_CueSheetView->IsDirty())
	{
		long userVal = SaveAlert();
		
		// Check user response
		switch( userVal)
		{
			// User does not want to save
			case 0:
				m_CueSheetView->SetDirty(false);
				retVal = true;
				break;
			
			// User decided not to quit
			case 1:
				retVal = false;
				break;
			
			// User wants to save
			case 2:
				retVal = false;
				break;
				
			default:
				retVal = true;
				
		}
	}
		
	// Remove ourself from applications Cue Sheet list if window is closing
	if (retVal)
	{
		BList *theList = static_cast<MuseumApp *>(be_app)->GetCueSheetList();
		if (theList)
		{
			theList->RemoveItem(this);
								
			// If this is the last cue sheet open, tell application to quit
			if (theList->CountItems() == 0)
			{
				quit = true;
				be_app->PostMessage(B_QUIT_REQUESTED);		
			}
		}
	}
	
	// Now tell app to fix Windows menu to reflect new state
	if (quit == false)
		be_app->PostMessage(FIX_WINDOWS_MENU_MSG);
			
	// Return user response to application
	return retVal;
}


//---------------------------------------------------------------------
//	MyQuitRequested
//---------------------------------------------------------------------
//
//	Version of QuitRequested used when application is quiting.  
//

bool TCueSheetWindow::MyQuitRequested()
{	
	bool retVal = true;
	bool quit 	= false;
	
	// Ask user to save document
	if (m_CueSheetView->IsDirty())
	{
		long userVal = SaveAlert();
		
		// Check user response
		switch( userVal)
		{
			// User does not want to save
			case 0:
				m_CueSheetView->SetDirty(false);
				retVal = true;
				break;
			
			// User decided not to quit
			case 1:
				retVal = false;
				break;
			
			// User wants to save
			case 2:
				retVal = true;
				break;
				
			default:
				retVal = true;				
		}
	}
		
	// Remove ourself from list	if window is closing
	if (retVal)
	{
		BList *theList = static_cast<MuseumApp *>(be_app)->GetCueSheetList();
		if (theList)
		{
			theList->RemoveItem(this);
								
			// If this is the last cue sheet open, tell application to quit
			if (theList->CountItems() == 0)
				be_app->PostMessage(B_QUIT_REQUESTED);		
		}
	}
		
	// Now tell app to fix Windows menu to reflect new state
	if (quit == false)
		be_app->PostMessage(FIX_WINDOWS_MENU_MSG);

	// Return user response to application
	return retVal;
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::MessageReceived(BMessage* message)
{		
	switch (message->what)
	{					
		// These messages come from the transport
		case START_BUTTON_MSG:		
			{				
				m_CueSheetView->SetCurrentTime( StartTime() );
												
				// Inform Transport
				BMessage *theMessage = new BMessage(UPDATE_TIMELINE_MSG);
				theMessage->AddInt32("TheTime", GetCurrentTime());
				TTransportPalette *theTransport = static_cast<MuseumApp *>(be_app)->GetTransport();
				theTransport->PostMessage(theMessage, theTransport->GetTransportPaletteView());
				delete theMessage;				
			}
			break;
			
		case REWIND_BUTTON_MSG:
			/*{
				// Calculate new time
				uint32 increment = 1000 / ( GetFPSValue(GetCurrentTimeFormat())+1); 
				uint32 newTime = GetCurrentTime() - (increment-=2);
				
				// Don't allow underflow
				if (newTime < StartTime())
					newTime = StartTime();
									
				m_CueSheetView->SetCurrentTime(newTime);
																	
				// Inform Transport
				BMessage *theMessage = new BMessage(UPDATE_TIMELINE_MSG);
				theMessage->AddInt32("TheTime", GetCurrentTime());
				TTransportPalette *theTransport = static_cast<MuseumApp *>(be_app)->GetTransport();
				theTransport->PostMessage(theMessage, theTransport->GetTransportPaletteView());
				delete theMessage;	
				
				char textStr[256];
				TimeToString(GetCurrentTime(), GetCurrentTimeFormat(), textStr, false);
			}*/
			break;
			
		case PLAY_BUTTON_MSG:
			break;
			
		case PAUSE_BUTTON_MSG:
			break;
			
		case STOP_BUTTON_MSG:
			break;
		
		case FF_BUTTON_MSG:
			/*{
				// Calculate new time
				uint32 increment = 1000 / ( GetFPSValue(GetCurrentTimeFormat())); 
				uint32 newTime = GetCurrentTime() + (increment+=2);
				
				// Don't allow overflow
				if (newTime > StartTime() + Duration())
					newTime = StartTime() + Duration();
									
				m_CueSheetView->SetCurrentTime(newTime);
																	
				// Inform Transport
				BMessage *theMessage = new BMessage(UPDATE_TIMELINE_MSG);
				theMessage->AddInt32("TheTime", GetCurrentTime());
				TTransportPalette *theTransport = static_cast<MuseumApp *>(be_app)->GetTransport();
				theTransport->PostMessage(theMessage, theTransport->GetTransportPaletteView());
				delete theMessage;	
				
				char textStr[256];
				TimeToString(GetCurrentTime(), GetCurrentTimeFormat(), textStr, false);
			}*/
			break;
			
		case END_BUTTON_MSG:
			{
				m_CueSheetView->SetCurrentTime(StartTime() + Duration());
																	
				// Inform Transport
				BMessage *theMessage = new BMessage(UPDATE_TIMELINE_MSG);
				theMessage->AddInt32("TheTime", GetCurrentTime());
				TTransportPalette *theTransport = static_cast<MuseumApp *>(be_app)->GetTransport();
				theTransport->PostMessage(theMessage, theTransport->GetTransportPaletteView());
				delete theMessage;				
			}
			break;
		
		case RECORD_BUTTON_MSG:
			break;
					
		case RESOLUTION_CHANGE_MSG:		
			{	
				Lock();
				
				// Get start and duration in pixels
				int32 startPixels 	 = TimeToPixels( m_CueSheetView->StartTime(), GetCurrentTimeFormat(), GetCurrentResolution());
				int32 durationPixels = TimeToPixels( m_CueSheetView->Duration(), GetCurrentTimeFormat(), GetCurrentResolution());
				int32 resizePixels 	 = durationPixels - startPixels; 
				
				m_CueSheetView->ResolutionChanged(message);
				m_ExportZone->ResolutionChanged(resizePixels);
				m_Timeline->ResolutionChanged(resizePixels);
				
				// Update scroll bars								
				AdjustScrollBars();
				
				Unlock();
			}
			break;			
					
		// These messages come from the BFilePanel when saving a cue sheet
		case B_SAVE_REQUESTED:
			Save(message);
			break;

		case B_CANCEL:
			break;
			
		default:
			BWindow::MessageReceived(message);
			break;
	}
}	

#pragma mark -
#pragma mark === Menu Handling ===

//---------------------------------------------------------------------
//	MenusBeginning
//---------------------------------------------------------------------
//
//	Handle updating of menu bar items


void TCueSheetWindow::MenusBeginning()
{
	AdjustFileMenu();
	AdjustEditMenu();
	AdjustCueSheetMenu();
	AdjustCueMenu();
	AdjustStageMenu();
	AdjustWindowsMenu();
}


//---------------------------------------------------------------------
//	AdjustFileMenu
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::AdjustFileMenu()
{

	// Get application
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);

	BMenu *theMenu = m_AppMenu->GetFileMenu();
	if (theMenu)
	{	
		// Save item
		BMenuItem *saveItem = theMenu->FindItem(FILE_SAVE_MSG);
		if (saveItem)
		{
			if (m_CueSheetView->IsDirty())
				saveItem->SetEnabled(true);
			else
				saveItem->SetEnabled(false);			
		}
		
		// Revert item
		BMenuItem *revertItem = theMenu->FindItem(FILE_REVERT_MSG);
		if (saveItem)
		{
			if (m_CueSheetView->IsDirty())
				revertItem->SetEnabled(true);
			else
				revertItem->SetEnabled(false);			
		}

	}
}


//---------------------------------------------------------------------
//	AdjustEditMenu
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::AdjustEditMenu()
{

	// Get application
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);

	BMenu *theMenu = m_AppMenu->GetEditMenu();
	if (theMenu)
	{	
		// 	Undo Item	
		BMenuItem *undoItem = theMenu->FindItem(EDIT_UNDO_MSG);		
		if (undoItem)
		{
			if (m_UndoEngine->CanUndo())
				undoItem->SetEnabled(true);
			else
				undoItem->SetEnabled(false);		
		}		
		
		// Cut
		// Enable if any items are selected
		BMenuItem *cutItem = theMenu->FindItem(EDIT_CUT_MSG);
		if (cutItem)
			cutItem->SetEnabled( m_CueSheetView->ItemsSelected() );
		
		// Copy
		// Enable if any items are selected
		BMenuItem *copyItem = theMenu->FindItem(EDIT_COPY_MSG);
		if (copyItem)			
			copyItem->SetEnabled( m_CueSheetView->ItemsSelected() );
			
		// Paste
		// Enable if there is anything on the clipboard
		BMenuItem *pasteItem = theMenu->FindItem(EDIT_PASTE_MSG);
		if (pasteItem)
			pasteItem->SetEnabled( static_cast<MuseumApp *>(be_app)->m_Clipboard->HasData() );
			
		// Clear
		BMenuItem *clearItem = theMenu->FindItem(EDIT_CLEAR_MSG);
		if (clearItem)
			clearItem->SetEnabled( m_CueSheetView->ItemsSelected() );
			
		// Select All
		// Enable if there is anything to be selected
		BMenuItem *selectAllItem = theMenu->FindItem(EDIT_SELECTALL_MSG);
		if (selectAllItem)
		{
			// Enable this item if there are any cues to be selected
			selectAllItem->SetEnabled( m_CueSheetView->HasCues() );
		}
	}
}


//---------------------------------------------------------------------
//	AdjustCueSheetMenu
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::AdjustCueSheetMenu()
{

	// Get application
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);

	BMenu *theMenu = m_AppMenu->GetCueSheetMenu();
	if (theMenu)
	{	
		// 	Setup Item	
		BMenuItem *setupItem = theMenu->FindItem(CUE_SETUP_MSG);		
		if (setupItem)
			;

		// 	Time Base Item	
		BMenuItem *timeBaseItem = theMenu->FindItem(CUE_TIMEBASE_MSG);		
		if (timeBaseItem)
			;
			
		// 	Zoom In Item	
		BMenuItem *zoomInItem = theMenu->FindItem(CUE_ZOOMIN_MSG);		
		if (zoomInItem)
			;
			
		// 	Zoom Out Item	
		BMenuItem *zoomOutItem = theMenu->FindItem(CUE_ZOOMOUT_MSG);		
		if (zoomOutItem)
			;
			
		// 	Show Durations Item	
		// 	Enable if there are any cues in the cue sheet
		BMenuItem *showDurationsItem = theMenu->FindItem(CUE_SHOWDURATIONS_MSG);		
		if (showDurationsItem)		
		{
			// Enable this item if there are any cues to be selected
			showDurationsItem->SetEnabled( m_CueSheetView->HasCues() );
		}
		
		// 	Show Stop Time Item	
		BMenuItem *showStopTimeItem = theMenu->FindItem(CUE_SHOWSTOPTIMES_MSG);		
		if (showStopTimeItem)
		{
			// Enable this item if there are any cues to be selected
			showStopTimeItem->SetEnabled( m_CueSheetView->HasCues() );
		}


		// 	Insert Channels Item	
		BMenuItem *insertItem = theMenu->FindItem(CUE_INSERTCHANNELS_MSG);		
		if (insertItem)
			;
			
		// 	Delete Channels Item	
		BMenuItem *deleteItem = theMenu->FindItem(CUE_DELETECHANNELS_MSG);		
		if (deleteItem)
		{
			//  If we only have one channel, disable this item
			if (m_CueSheetView->GetTotalChannels() > 1)
				deleteItem->SetEnabled(true);
			else
				deleteItem->SetEnabled(false);
		}
			
		// 	Insert Time Item	
		BMenuItem *insertTimeItem = theMenu->FindItem(CUE_INSERTTIME_MSG);		
		if (insertTimeItem)
		{
			insertTimeItem->SetEnabled(false);
		}
			
		// 	Align Start Item	
		BMenuItem *startItem = theMenu->FindItem(CUE_ALIGNSTART_MSG);		
		if (startItem)
		{
			//  A cue in two or more distinct channels must be selected for this
			//	command to be enable
			if ( m_CueSheetView->HasMultipleCueChannelsSelected() )
				startItem->SetEnabled(true);
			else
				startItem->SetEnabled(false);
		}
		
		// 	Align End Item	
		BMenuItem *endItem = theMenu->FindItem(CUE_ALIGNEND_MSG);		
		if (endItem)
		{
			//  A cue in two or more distinct channels must be selected for this
			//	command to be enable
			if ( m_CueSheetView->HasMultipleCueChannelsSelected() )
				endItem->SetEnabled(true);
			else
				endItem->SetEnabled(false);
		}
			
		// 	Collapse Item	
		BMenuItem *collapseItem = theMenu->FindItem(CUE_COLLAPSEALL_MSG);		
		if (collapseItem)
			;
			
		// 	Jump To Item	
		BMenuItem *jumpToItem = theMenu->FindItem(CUE_JUMPTO_MSG);		
		if (jumpToItem)
			;						
		
		
	}	
}


//---------------------------------------------------------------------
//	AdjustCueMenu
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::AdjustCueMenu()
{
	// Get application
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);

	BMenu *theMenu = m_AppMenu->GetCueMenu();
	if (theMenu)
	{	
		//	Get Cue Info
		// Enable if any items are selected
		BMenuItem *cueInfoItem = theMenu->FindItem(CUESHEET_GETINFO_MSG);
		if (cueInfoItem)
			cueInfoItem->SetEnabled( m_CueSheetView->ItemsSelected() );

		//	Default Cue Setup
		// Enable if any items are selected
		//BMenuItem *cueInfoItem = theMenu->FindItem(CUESHEET_GETINFO_MSG);
		//if (cueInfoItem)
		//	cueInfoItem->SetEnabled( m_CueSheetView->ItemsSelected() );

		//	Show Cue Contents
		// Enable if any items are selected
		BMenuItem *showContentsItem = theMenu->FindItem(CUESHEET_SHOWCONTENTS_MSG);
		if (showContentsItem)
			showContentsItem->SetEnabled( m_CueSheetView->ItemsSelected() );

		//	Show All Cue Contents
		// Enable if we have cues
		BMenuItem *showAllContentsItem = theMenu->FindItem(CUESHEET_SHOWALLCONTENTS_MSG);
		if (showAllContentsItem)
			showAllContentsItem->SetEnabled( m_CueSheetView->HasCues() );

		//	Hide All Cue Contents
		// Enable if we have cues
		BMenuItem *hideAllContentsItem = theMenu->FindItem(CUESHEET_HIDEALLCONTENTS_MSG);
		if (hideAllContentsItem)
			hideAllContentsItem->SetEnabled( m_CueSheetView->HasCues() );

		//	Cue Setup
		// Enable if any items are selected
		BMenuItem *setupItem = theMenu->FindItem(CUESHEET_SETUP_MSG);
		if (setupItem)
			setupItem->SetEnabled( m_CueSheetView->ItemsSelected() );

		//	Open Cue Editor
		// Enable if any items are selected
		BMenuItem *editItem = theMenu->FindItem(CUESHEET_EDIT_MSG);
		if (editItem)
			editItem->SetEnabled( m_CueSheetView->ItemsSelected() );

		//	Add Keyframes
		// Enable if any items are selected
		BMenuItem *keyframeItem = theMenu->FindItem(CUESHEET_KEYFRAME_MSG);
		if (keyframeItem)
			keyframeItem->SetEnabled( m_CueSheetView->ItemsSelected() );
	}
}


//---------------------------------------------------------------------
//	AdjustStageMenu
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::AdjustStageMenu()
{

	// Get application
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);

	BMenu *theMenu = m_AppMenu->GetStageMenu();
	if (theMenu)
	{	
		// 	Stage Item	
		//BMenuItem *stageItem = theMenu->FindItem(WINDOWS_STAGE_MSG);		
		//if (stageItem)
		
		//	Live Drag Item
		BMenuItem *updateItem = theMenu->FindItem(STAGE_LUPDATE_MSG);
		if (updateItem)
			updateItem->SetMarked( m_CueSheetView->GetLiveUpdate() );		
	}
}


//---------------------------------------------------------------------
//	AdjustWindowsMenu
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::AdjustWindowsMenu()
{

	//	Set up string buffers
	bool	retVal;
	char	*menuStr   = (char *)malloc( sizeof(char) * 256);
	char 	*accelStr  = (char *)malloc( sizeof(char));

	// Get application
	MuseumApp *theApp = static_cast<MuseumApp *>(be_app);

	BMenu *theMenu = m_AppMenu->GetWindowsMenu();
	if (theMenu)
	{	
		// 	Stage Item	
		BMenuItem *stageItem = theMenu->FindItem(WINDOWS_STAGE_MSG);		
		if (stageItem)
		{
			TStageWindow *stage = m_StageWindow;
			if (stage)
			{
				stage->Lock();
				if (stage->IsHidden())
					retVal = GetString("WindowsMenuStrings", kWindowsShowStageItem, menuStr);	
				else	
					retVal = GetString("WindowsMenuStrings", kWindowsHideStageItem, menuStr);	
				stage->Unlock();
					
				stageItem->SetLabel(menuStr);				
			}
			else
			{
				retVal = GetString("WindowsMenuStrings", kWindowsShowStageItem, menuStr);	
				stageItem->SetLabel(menuStr);
			}
		}
		
		// Browser Item
		BMenuItem *browserItem = theMenu->FindItem(WINDOWS_BROWSER_MSG);		
		if (stageItem)
		{
			TBrowserWindow *browser = theApp->GetBrowser();
			if (browser)
			{
				browser->Lock();
				if (browser->IsHidden())
					retVal = GetString("WindowsMenuStrings", kWindowsShowBrowserItem, menuStr);	
				else	
					retVal = GetString("WindowsMenuStrings", kWindowsHideBrowserItem, menuStr);	
				browser->Unlock();
				
				browserItem->SetLabel(menuStr);
			}
			else
			{
				retVal = GetString("WindowsMenuStrings", kWindowsShowBrowserItem, menuStr);	
				browserItem->SetLabel(menuStr);
			}
		}
				
		// 	Transport Item	
		BMenuItem *transportItem = theMenu->FindItem(WINDOWS_TRANSPORT_MSG);		
		if (transportItem)
		{
			TTransportPalette *transport = theApp->GetTransport();
			if (transport)
			{
				transport->Lock();
				if (transport->IsHidden())
					retVal = GetString("WindowsMenuStrings", kWindowsShowTransportItem, menuStr);			
				else	
					retVal = GetString("WindowsMenuStrings", kWindowsHideTransportItem, menuStr);			
				transport->Unlock();
			
				transportItem->SetLabel(menuStr);
			}
			else
			{
				retVal = GetString("WindowsMenuStrings", kWindowsShowTransportItem, menuStr);			
				transportItem->SetLabel(menuStr);
			}
		}
		
		// 	Locator Item	
		BMenuItem *counterItem = theMenu->FindItem(WINDOWS_COUNTER_MSG);		
		if (counterItem)
		{
			TTimePalette *time = theApp->GetTimePalette();
			if (time)
			{
				time->Lock();
				if (time->IsHidden())
					retVal = GetString("WindowsMenuStrings", kWindowsShowLocatorItem, menuStr);
				else	
					retVal = GetString("WindowsMenuStrings", kWindowsHideLocatorItem, menuStr);
				time->Unlock();
				
				counterItem->SetLabel(menuStr);
			}
			else
			{
				retVal = GetString("WindowsMenuStrings", kWindowsShowLocatorItem, menuStr);
				counterItem->SetLabel(menuStr);
			}
		}
			
		// 	Media Cue Item	
		BMenuItem *mediaItem = theMenu->FindItem(WINDOWS_MEDIA_MSG);		
		if (mediaItem)
		{
			TMediaCuePalette *mediaPalette = theApp->GetMediaPalette();
			if (mediaPalette)
			{
				mediaPalette->Lock();
				if (mediaPalette->IsHidden())
					retVal = GetString("WindowsMenuStrings", kWindowsShowMediaItem, menuStr);
				else	
					retVal = GetString("WindowsMenuStrings", kWindowsHideMediaItem, menuStr);
				mediaPalette->Unlock();
				
				mediaItem->SetLabel(menuStr);
			}
			else
			{
				retVal = GetString("WindowsMenuStrings", kWindowsShowMediaItem, menuStr);
				mediaItem->SetLabel(menuStr);
			}
		}	
	}
}

#pragma mark -
#pragma mark === Scrollbar Handling ===

//---------------------------------------------------------------------
//	AdjustScrollBars
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::AdjustScrollBars()
{	
	const BRect scrollRect = Bounds();
	const BRect scrollArea = m_CueSheetView->Bounds();
	
	int16 timeFormat = m_CueSheetView->GetTimeFormat();
	int16 resolution = m_CueSheetView->GetResolution();
	
	// Get start and duration in pixels
	int32 startPixels 	 = TimeToPixels( m_CueSheetView->StartTime(), m_CueSheetView->GetTimeFormat(), m_CueSheetView->GetResolution());
	int32 durationPixels = TimeToPixels( m_CueSheetView->Duration(),  m_CueSheetView->GetTimeFormat(), m_CueSheetView->GetResolution());
	int32 resizePixels 	 = durationPixels - startPixels; 

	// Adjust horizontal scrollbar
	AdjustScrollBar( m_HScroll, scrollRect.Width(), kTickSpacing, resizePixels+kTimeScalerWidth+1, scrollRect.left);
	
	// 	Vertical scrollbar area is total combined height of all channels
	//	Iterate through all of the channels and calculate height
	int32 totalHeight = 0;
	for (int32 index = 0; index < m_CueSheetView->GetTotalChannels(); index++)
	{
		TCueChannel *theChannel = (TCueChannel *)m_CueSheetView->GetChannelList()->ItemAt(index);			
		if (theChannel)
			totalHeight += theChannel->Bounds().Height();
	}
		
	// Adjust vertical scrollbar
	AdjustScrollBar( m_VScroll, scrollRect.Height()-(kTimelineHeight+kToolbarHeight+kMenuHeight+10), kTickSpacing, totalHeight+kTimelineHeight+kToolbarHeight+kMenuHeight+1, scrollRect.top);
}

#pragma mark -
#pragma mark === File Saving ===

//---------------------------------------------------------------------
//	Save
//---------------------------------------------------------------------
//
//

void TCueSheetWindow::Save(BMessage *message)
{
	status_t	myErr;
	entry_ref 	theRef;
	char		*theString = NULL;
					
	// Get directory to save data to
	message->FindRef("directory", &theRef);
	
	// Get name of file to be saved as
	message->FindString("name", (const char **)&theString);
	
	// Set window title to new filename
	SetTitle(theString);		

	// Create a BDirectory object
	BDirectory saveDir(&theRef);
												
	// Now create the file.  Create new file if m_File is unitialized.
	if (m_File)
		delete m_File;
	
	m_File = new BFile();
	
	myErr = saveDir.CreateFile(theString, m_File, false);
	
	if (myErr != B_OK)
		return;
	
	//	Write out file header
	//WriteHeader(m_File);
	
	BMessage archive;
	
	//	Archive CueSheetWindow and write to file
	BMessage cueSheetArchive;
	Archive(&cueSheetArchive, true);
	
	//	Flatten and write out to file
	status_t numBytes;
	cueSheetArchive.Flatten(m_File, &numBytes); 
	
	// Set file type
	BNodeInfo info(m_File);
	if ( info.InitCheck() )
		return;
	
	myErr = info.SetType("application/x-mediapede-cuesheet");
	
	// Set icons
	BBitmap *smallIcon = GetMICNFromResource("application/x-mediapede-cuesheet");
	info.SetIcon( smallIcon, B_MINI_ICON);
	
	BBitmap *largeIcon = GetICONFromResource("application/x-mediapede-cuesheet");				
	info.SetIcon( largeIcon, B_LARGE_ICON);
	
	delete smallIcon;
	delete largeIcon;
	
	// Update our entry in the Windows menu to reflect possible new filename
	BMenuItem *theItem = m_AppMenu->GetWindowsMenu()->FindItem( Title());
	if (theItem)
		theItem->SetLabel(theString);
	
	// We aren't dirty now
	m_CueSheetView->SetDirty(false);
}


//---------------------------------------------------------------------
//	WriteHeader
//---------------------------------------------------------------------
//
//	Writes out file header
//

void TCueSheetWindow::WriteHeader(BFile *theFile)
{
	CueSheetHeaderChunk theChunk;
	
	if (B_HOST_IS_BENDIAN)
		theChunk.header.chunkID = kCueSheetDocChunkMotorolaID;
	else
		theChunk.header.chunkID	= kCueSheetDocChunkIntelID;
	
	theChunk.header.chunkSize 	= 0;
	theChunk.version 			= kCurrentFileVersion;
	
	// Rewind file to beginning
	theFile->Seek(0, SEEK_SET);
	theFile->Write(&theChunk, sizeof(theChunk));
}

#pragma mark -
#pragma mark === Clipboard Handling ===


//---------------------------------------------------------------------
//	Undo
//---------------------------------------------------------------------
//
//	Handle undo functionality
//

void TCueSheetWindow::Undo()
{
}


//---------------------------------------------------------------------
//	Cut
//---------------------------------------------------------------------
//
//	Handle clipboard cut
//

void TCueSheetWindow::Cut()
{
}


//---------------------------------------------------------------------
//	Copy
//---------------------------------------------------------------------
//
//	Handle clipboard copies
//

void TCueSheetWindow::Copy()
{
	m_CueSheetView->Copy();
}


//---------------------------------------------------------------------
//	Paste
//---------------------------------------------------------------------
//
//	Handle clipboard paste
//

void TCueSheetWindow::Paste()
{
	m_CueSheetView->Paste();
}


//---------------------------------------------------------------------
//	Clear
//---------------------------------------------------------------------
//
//	Handle clipboard clear
//

void TCueSheetWindow::Clear()
{
	m_CueSheetView->Clear();
}


//---------------------------------------------------------------------
//	Select All
//---------------------------------------------------------------------
//
//	Handle Select All functionality
//

void TCueSheetWindow::SelectAll()
{
	m_CueSheetView->SelectAll();
}


#pragma mark -
#pragma mark === Utility Routines ===

//---------------------------------------------------------------------
//	SetPanelOpen
//---------------------------------------------------------------------
//
//	Set m_PanelOpen to the value
//

void TCueSheetWindow::SetPanelOpen(bool theVal)
{
	m_PanelOpen = theVal;
	
	if (m_PanelOpen == false)
	{
		BPoint where;
		where.x = 0;
		BMessage *message = new BMessage(UPDATE_TIMELINE_MSG);
		message->AddPoint("Where", where);
		message->AddInt32("TheTime", GetCurrentTime());		
		m_Timeline->MessageReceived(message);
		delete message;		
	}
}
