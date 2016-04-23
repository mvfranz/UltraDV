//---------------------------------------------------------------------
//
//	File:	TExportTimeView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.16.98
//
//	Desc:	Displays in and out times of ExportZone.  Also displays
//			live update checkbox	
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

#include "TCueSheetWindow.h"
#include "TCueSheetView.h"

#include "TExportTimeView.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TExportTimeView::TExportTimeView(TCueSheetWindow *parent, BRect bounds) : BView(bounds, "ExportTimeView", B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	// Save parent view
	m_CueSheetWindow = parent;
	
	// Perform default initialization
	Init();
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TExportTimeView::TExportTimeView(BMessage *data) : BView(data)
{
	m_CueSheetWindow = NULL;
	
	// Perform default initialization
	Init();		
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TExportTimeView::~TExportTimeView()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TExportTimeView::Init()
{
}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TExportTimeView::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TExportTimeView") ) 
		return new TExportTimeView(archive); 
		
	return NULL; 
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TExportTimeView::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BView::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TExportTimeView");
		
		// Add our member variables to the archive		
				
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

void TExportTimeView::Draw(BRect updateRect)
{
	// Set up environment
	PushState();
	
	const BRect bounds = Bounds();
	
	BPoint startPt, endPt;
	
	//	Fill area
	SetHighColor(kKhaki);
	FillRect(bounds);

	//	Border in black
	SetHighColor(kBlack);
	startPt.Set(bounds.right, bounds.top);
	endPt.Set(bounds.right, bounds.bottom);
	StrokeLine(startPt, endPt);
	endPt.Set(bounds.left, bounds.bottom);
	StrokeLine(endPt);
	endPt.Set(bounds.left, bounds.top);
	StrokeLine(endPt);
	
	//	Draw shadow
	SetHighColor(kDarkKhaki);
	startPt.Set(bounds.right-1, bounds.top);
	endPt.Set(bounds.right-1, bounds.bottom-1);
	StrokeLine(startPt, endPt);
	endPt.Set(bounds.left+1, bounds.bottom-1);
	StrokeLine(endPt);
	
	//	Draw highlight
	SetHighColor(kLightKhaki);
	startPt.Set(bounds.left+1, bounds.bottom-2);
	endPt.Set(bounds.left+1, bounds.top);
	StrokeLine(startPt, endPt);
	endPt.Set(bounds.right-2, bounds.top);
	StrokeLine(endPt);

	// Update text items
	DrawInText();
	DrawOutText();
	
	// Restore environment
	PopState();
}


//---------------------------------------------------------------------
//	DrawInText
//---------------------------------------------------------------------
//
//	Draw Export In Time text
//

void TExportTimeView::DrawInText()
{
	Looper()->Lock();
	
	const BRect bounds = Bounds();
	
	BRect updateRect = Bounds();
	updateRect.left  	+= 25;
	updateRect.top 	 	+= 3;
	updateRect.right 	-= 3;
	updateRect.bottom 	= updateRect.top +10;
	
	//	Clean up backgrounds
	SetHighColor(kKhaki);
	FillRect(updateRect);
	
	BPoint drawPt( bounds.left + 25, bounds.top + 12);
	
	SetHighColor(kBlack);
	SetLowColor(kKhaki);

	char textStr[13];
	TimeToString(m_CueSheetWindow->GetCueSheetView()->GetExportStartTime(), GetCurrentTimeFormat(), textStr, false);
	DrawString(textStr, drawPt);
		
	Looper()->Unlock();
}

//---------------------------------------------------------------------
//	DrawOutText
//---------------------------------------------------------------------
//
//	Draw Export Out Time text
//

void TExportTimeView::DrawOutText()
{
	Looper()->Lock();
	
	const BRect bounds = Bounds();
	
	BRect updateRect = Bounds();
	updateRect.left  	+= 25;
	updateRect.top 	 	+= 13;
	updateRect.right 	-= 3;
	updateRect.bottom 	= updateRect.top +10;
	
	//	Clean up backgrounds
	SetHighColor(kKhaki);
	FillRect(updateRect);
	
	BPoint drawPt( bounds.left + 25, bounds.top + 24);
	
	SetHighColor(kBlack);
	SetLowColor(kKhaki);
	
	int32 stopTime = m_CueSheetWindow->GetCueSheetView()->GetExportStopTime();
	
	char textStr[13];
	TimeToString(m_CueSheetWindow->GetCueSheetView()->GetExportStopTime(), GetCurrentTimeFormat(), textStr, false);
	DrawString(textStr, drawPt);
		
	Looper()->Unlock();
}


#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TExportTimeView::MouseDown(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TExportTimeView::MouseUp(BPoint where)
{
}

//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TExportTimeView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{
}

#pragma mark -
#pragma mark === Dialog Routines ===

//---------------------------------------------------------------------
//	ShowProjectSettingDialog
//---------------------------------------------------------------------
//
//	Display the project setign dialog.  This allows the user to set the 
//	time resolution, the start, duration and end times.
//
/*
void TExportTimeView::ShowProjectSettingDialog(BPoint where)
{
		
	// If we have created the dialog bring it to the front and show it
	if( m_ProjectSettings)
	{
		if (m_ProjectWindow)
		{	
			m_ProjectWindow->Show();
			m_ProjectWindow->Activate(true);
		}
	}
	// Create the Project Settings dialog from a resource archive
	else
	{
		BMessage *theMessage = GetWindowFromResource("ProjectSettingsWindow");
		m_ProjectWindow = new TProjectSettings(this, m_CueSheetWindow->GetCueSheetView(), theMessage);
							
		// Move it under the mouse
		//ConvertToScreen(&where);
		//m_ProjectWindow->MoveTo(where.x, where.y);
		CenterWindow(m_ProjectWindow);
	
		// Show the dialog
		m_ProjectWindow->Show();		
	}
	return;
}
*/

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

void TExportTimeView::AttachedToWindow()
{			
	if(m_CueSheetWindow == NULL)
	{
		m_CueSheetWindow = (TCueSheetWindow *)Window();		
	}
	
	//	PAss up to parent
	BView::AttachedToWindow();
}

#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetParent
//---------------------------------------------------------------------
//
//
void TExportTimeView::SetParent(TCueSheetWindow *parent)
{
 	m_CueSheetWindow = parent;
}