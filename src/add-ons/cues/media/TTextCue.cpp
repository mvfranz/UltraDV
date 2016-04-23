//---------------------------------------------------------------------
//
//	File:	TTextCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Create a user editable text field	
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <assert.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TBitmapView.h"
#include "TVisualCue.h"
#include "TCueTimeText.h"
#include "TCueChannel.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TStageWindow.h"

#include "TTextCue.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTextCue::TTextCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime) : 
			TVisualCue(id, parent, bounds, startTime, "TextCue")
{	
	// Perform default initialization
	Init();
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a BMessage
//

TTextCue::TTextCue(BMessage *theMessage) : TVisualCue(theMessage)
{	
	// Load cue icon
	LoadCueIcon();
	
	//	Pass up to parent
	TCueView::LoadCueIcon();	
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTextCue::~TTextCue()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TTextCue::Init()
{	
	// Set up default duration in microseconds
	m_Duration = 1000;
	
	// Editor is closed
	m_EditorOpen = false;
	
	// Set up default settings
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
			
	// 	Set up area rectangles.  We create an initital rect the size of the stage
	//	and then inset by ten pixels
	BRect textBounds = static_cast<MuseumApp *>(be_app)->GetCueSheet()->GetStage()->Bounds();
	textBounds.InsetBy(10, 10);
	m_Bitmap = new BBitmap(textBounds, B_RGB_32_BIT);
				
	// Default initialization
	TVisualCue::Init();
	
	// Add the cue to the cue channel
	if ( m_Channel->CanInsertCue(this, m_InsertTime, true))
	{
		m_Channel->InsertCue(this, m_InsertTime);		
		Select();
		
		// We are now fully instantiated
		m_IsInstantiated = true;
	}
	
	// Set expanded status
	if (m_Channel->IsExpanded())
	{
		m_IsExpanded = false;
		Expand();
	}
	else
	{
		m_IsExpanded = true;
		Contract();		
	}		
}


#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TTextCue::Instantiate(BMessage *archive) 
{ 	
	if ( validate_instantiation(archive, "TTextCue") ) 
		return new TTextCue(archive); 
		
	return NULL; 
}
  

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TTextCue::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = TVisualCue::Archive(data, deep);
	
	if (myErr == B_OK)
	{				
		// Add ourselves to the archive
		data->AddString("class", "TTextCue");						
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

void TTextCue::Draw(BRect updateRect)
{
	// Save colors
	rgb_color saveColor = HighColor();
		
	// Restore color
	SetHighColor(saveColor);
	
	// Pass up to parent
	TVisualCue::Draw(updateRect);

}


void TTextCue::RenderBitmapData()
{
}


#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TTextCue::MouseDown(BPoint where)
{
	// Pass up to parent
	TVisualCue::MouseDown(where);
}


//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TTextCue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{

	default:
		TVisualCue::MessageReceived(message);						
		break;
	}
}


#pragma mark -
#pragma mark === Editor ===
//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TTextCue::OpenEditor()
{

}

#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	LoadCueIcon
//---------------------------------------------------------------------
//
//	Load cues identifying icon
//

void TTextCue::LoadCueIcon()
{
	BBitmap *cueIcon = GetAppIcons()->m_TextUpIcon;

	if (cueIcon)
	{
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);		
		m_CueIcon = new TBitmapView(area, "CueIcon", cueIcon, false);
		AddChild(m_CueIcon);		
	}	
}
