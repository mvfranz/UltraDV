//---------------------------------------------------------------------
//
//	File:	TSettingsTabView.cpp
//
//	Author:	Mike Ost
//
//	Date:	08.11.98
//
//	Desc:	Sub-class of TElementsView. Handles the tab labelled 'Media'
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TSettingsTabView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TSettingsTabView::TSettingsTabView(BRect bounds) :
	BView(bounds, "Settings", B_FOLLOW_ALL | B_FRAME_EVENTS, B_WILL_DRAW)
{
	// TODO: for internationalization, pull the title from the resource
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TSettingsTabView::~TSettingsTabView()
{
}
