//---------------------------------------------------------------------
//
//	File:	TCueSheetScrollBarH.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	Custom Horizontal scroll bar to scroll CueSheet
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

#include "TCueSheetScrollBarH.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueChannel.h"
#include "TExportZone.h"
#include "TTimelineView.h"



// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueSheetScrollBarH::TCueSheetScrollBarH( TCueSheetWindow *parent, BRect frame, BView *target, float min, float max ) :
					BScrollBar( frame, "HCueScroll", target, min, max, B_HORIZONTAL)
{
	m_CueSheetWindow = parent;
	
	// Do default initialization
	Init();
}					


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Contruct from an archive
//

TCueSheetScrollBarH::TCueSheetScrollBarH(BMessage *data ) : BScrollBar(data)
{		
	m_CueSheetWindow = NULL;
	
	// Do default initialization
	Init();
}					


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueSheetScrollBarH::~TCueSheetScrollBarH()
{

}


#pragma mark -
#pragma mark === Archiving Routines ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TCueSheetScrollBarH::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TCueSheetScrollBarH") ) 
		return new TCueSheetScrollBarH(archive); 
		
	return NULL; 
}


//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueSheetScrollBarH::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BScrollBar::Archive(data, deep);
						
	if (myErr == B_OK)
	{					
		// Add our class name to the archive
		data->AddString("class", "TCueSheetScrollBarH");
		
		// Add our member variables to the archive		
				
		// Add attached views
		if (deep)
		{		
		
		}		
	}
	
	Looper()->Unlock();	
	
	return myErr;
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TCueSheetScrollBarH::Init()
{
	// 	Get current scroll value and save it in our tracking variable.  
	//	We use this value to scroll the HeaderZone in concert with
	//	the CueSheetView
	m_LastValue = Value();
}

//---------------------------------------------------------------------
//	ValueChanged
//---------------------------------------------------------------------
//
//

void TCueSheetScrollBarH::ValueChanged(float newValue)
{		
	// Pass message to base class
	BScrollBar::ValueChanged(newValue);
	
	// We need to offset the Export Zone, TimeLine and CueChannels...
	if (newValue != m_LastValue)
	{				
		if (newValue > m_LastValue)
		{
			m_CueSheetWindow->GetExportZone()->ScrollBy(newValue - m_LastValue, 0);
			m_CueSheetWindow->GetTimeline()->ScrollBy(newValue - m_LastValue, 0);
			
			//	Handle cue channels
			BList *channelList = m_CueSheetWindow->GetCueSheetView()->GetChannelList();
			for (int32 channelNum = 0; channelNum < channelList->CountItems(); channelNum++)
			{
				TCueChannel *theChannel = (TCueChannel *)channelList->ItemAt(channelNum);
				if (theChannel)
					theChannel->ScrollBy(newValue - m_LastValue, 0);				
			}		
		}
		else			
		{
			m_CueSheetWindow->GetExportZone()->ScrollBy(-(m_LastValue - newValue), 0);
			m_CueSheetWindow->GetTimeline()->ScrollBy(-(m_LastValue - newValue), 0);
			
			//	Handle cue channels
			BList *channelList = m_CueSheetWindow->GetCueSheetView()->GetChannelList();
			for (int32 channelNum = 0; channelNum < channelList->CountItems(); channelNum++)
			{
				TCueChannel *theChannel = (TCueChannel *)channelList->ItemAt(channelNum);
				if (theChannel)
					theChannel->ScrollBy(-(m_LastValue - newValue), 0);				
			}		

		}
			
		// Force a redraw of the items
		m_CueSheetWindow->GetExportZone()->Invalidate();		
		BRect bounds = m_CueSheetWindow->GetTimeline()->Bounds();
		m_CueSheetWindow->GetTimeline()->Invalidate(bounds);
		
		// Save value for next compare
		m_LastValue = newValue;		
	}		
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

void TCueSheetScrollBarH::AttachedToWindow()
{			
	if(m_CueSheetWindow == NULL)
	{
		m_CueSheetWindow = (TCueSheetWindow *)Window();		
	}
	
	//	Pass up to parent
	BView::AttachedToWindow();
}

#pragma mark -
#pragma mark === Accessor Functions ===

//---------------------------------------------------------------------
//	SetParent
//---------------------------------------------------------------------
//
//
void TCueSheetScrollBarH::SetParent(TCueSheetWindow *parent)
{
 	m_CueSheetWindow = parent;
}