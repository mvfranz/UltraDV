//---------------------------------------------------------------------
//
//	File:	TTransitionsTabView.cpp
//
//	Author:	Mike Ost
//
//	Date:	08.11.98
//
//	Desc:	Sub-class of TElementsView. Handles the tab labelled
//			'Transitions'
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TTransitionsTabView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTransitionsTabView::TTransitionsTabView(BRect bounds) :
	BView(bounds, "Transitions", B_FOLLOW_ALL | B_FRAME_EVENTS,
	      B_WILL_DRAW)
{
	// TODO: for internationalization, pull the title from the resource
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTransitionsTabView::~TTransitionsTabView()
{
}
