//---------------------------------------------------------------------
//
//	File:	AVIWindow.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	AVIWindow
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

//	Includes
#include <Debug.h>

#include "BuildApp.h"

#include "TBitmapView.h"
#include "TRIFFReader.h"

#include "AppUtils.h"
#include "DebugUtils.h"
#include "AVIView.h"
#include "AVIApp.h"

#include "AVIWindow.h"



//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

AVIWindow::AVIWindow(BRect bounds, char* name) : BWindow(bounds, name, B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_H_RESIZABLE|B_NOT_V_RESIZABLE|B_NOT_ZOOMABLE|B_WILL_ACCEPT_FIRST_CLICK)
{
	//	Create AVIView
	m_AVIView = new AVIView(bounds, "AVIView", B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_FRAME_EVENTS|B_WILL_DRAW);

	//	Check for proper inititalization
	if (m_AVIView)
		AddChild(m_AVIView);
	else{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}

	//	Create bitmap for bitmap view
	BBitmap* bitmap = new BBitmap(bounds, B_RGB32);
	ASSERT(bitmap);

	//	Add a bitmap view
	m_BitmapView = new TBitmapView(Bounds(), "AVIView", bitmap, true,  B_FOLLOW_LEFT | B_FOLLOW_TOP);
	m_AVIView->AddChild(m_BitmapView);

	//	Inform AVIView
	m_AVIView->SetBitmapView(m_BitmapView);
	m_BitmapView->MakeFocus(true);

	//	Show window
// ABH CenterWindow not defined
//	CenterWindow(this);
	Show();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//


AVIWindow::~AVIWindow()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
}
