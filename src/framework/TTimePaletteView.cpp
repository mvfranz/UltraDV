//---------------------------------------------------------------------
//
//	File:	TTimePaletteView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.31.98
//
//	Desc:	Transport Palette View
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
#include "MuseumApp.h"
#include "AppUtils.h"

#include "TTimeText.h"
#include "TBitmapView.h"
#include "TPlaybackEngine.h"
#include "TCueSheetWindow.h"

#include "TTimePaletteView.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTimePaletteView::TTimePaletteView(BRect bounds) : BView(bounds, "TransportView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTimePaletteView::~TTimePaletteView()
{
	// Clean up TBitmapView
	if (m_LocatorBitmap)
		delete m_LocatorBitmap;		
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TTimePaletteView::Init()
{      		
	// Set view background color
	SetViewColor(kBlack);
	BFont saveFont;
	GetFont(&saveFont);
	
				
	//
	// Create bitmap view
	//
	
	// Get application info
	app_info info;
	
	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);
	if (file.InitCheck())
		return;
	
	size_t 		size;
	BBitmap 	*data;
	
	BResources res(&file);
	data = (BBitmap *)res.FindResource('bits', "Locator", &size);
	if (!data)			
		return;
	
	// Load bitmap
	BRect bitmapRect = Bounds();
	BBitmap *bitmap = new BBitmap(bitmapRect, B_COLOR_8_BIT);
	ASSERT(bitmap);
	bitmap->SetBits(data, size, 0, B_COLOR_8_BIT);
	m_LocatorBitmap = new TBitmapView( bitmapRect, "LocatorBitmap", bitmap, true, B_FOLLOW_ALL);	
	ASSERT(m_LocatorBitmap);	
	AddChild(m_LocatorBitmap);
	m_LocatorBitmap->Show();
	
	// Set text size temporarily to the text view size to get proper 
	// string length of default start time "00:00:00:00"
	SetFont(be_bold_font);
	SetFontSize(kTimeFontSize);
	float strWidth = StringWidth("00:00:00:00");
	
	// Create text display
	BRect bounds  = Bounds();	
	bounds.right = bounds.left + strWidth + 10;
	bounds.bottom = bounds.top + kTimeFontSize; 
	
	bounds.OffsetBy(22, 28);
				
	// Set up font
	BFont timeFont(be_bold_font);
	timeFont.SetSize(kTimeFontSize);
	
	// Create TTimeText View
	m_TimeText = new TTimeText(NULL, 0, bounds, "TimeTextView", B_FOLLOW_ALL);
	m_LocatorBitmap->AddChild(m_TimeText);
	m_TimeText->SetFontAndColor(&timeFont, 0, kGreen);
	m_TimeText->Show(); 
			
	// Restore settings
	SetFont(&saveFont);
}


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TTimePaletteView::Draw(BRect updateRect)
{
	m_TimeText->Draw(updateRect);	
}



//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TTimePaletteView::MouseDown(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TTimePaletteView::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TTimePaletteView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TTimePaletteView::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TTimePaletteView::KeyDown(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TTimePaletteView::KeyUp(const char *bytes, int32 numBytes)
{
}



//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TTimePaletteView::FrameMoved(BPoint new_position)
{
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TTimePaletteView::FrameResized(float new_width, float new_height)
{
}



#pragma mark -
#pragma mark === Message Handling ===


//------------------------------------------------------------------
//	MessageReceived
//------------------------------------------------------------------
//
//

void TTimePaletteView::MessageReceived(BMessage* message)
{			
	switch (message->what)
	{
		// Pass this down to time text
		case NEW_TIME_MSG:
			m_TimeText->MessageReceived(message);
			break;
		
		// The time is being update due to a mouse move over the cue sheet
		case UPDATE_TIMELINE_MSG:
			m_TimeText->MessageReceived(message);
			break;
			
		default:
			BView::MessageReceived(message);						
			break;			
	}	
}	
