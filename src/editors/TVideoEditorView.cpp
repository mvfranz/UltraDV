//---------------------------------------------------------------------
//
//	File:	TVideoEditorView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Video preview area
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"

#include "TVideoEditor.h"

#include "TVideoEditorView.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoEditorView::TVideoEditorView(TVideoEditor *editor, BMessage *message) : BView(message)
{	
	m_Editor = editor;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoEditorView::~TVideoEditorView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoEditorView::Init()
{
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
	
	m_DrawPoint.Set(0, 0);	
}


#pragma mark -
#pragma mark === Message Handling ===


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TVideoEditorView::Draw(BRect updateRect)
{
	PushState();
	
	// Refresh display with current frame
	if ( m_Editor->IsPlaying() == false)
	{
		//	Fill background with black
		SetHighColor(kBlack);
		FillRect(updateRect);

		//	Refresh with last rendered frame
		//	Look into centering the bitmap
		BBitmap *vidBitmap = m_Editor->GetDisplayBitmap();
		if (vidBitmap)
		{
			m_DrawPoint.x = (Bounds().Width() / 2) - (vidBitmap->Bounds().Width() / 2);
			m_DrawPoint.y = (Bounds().Height() / 2) - (vidBitmap->Bounds().Height() / 2);

			DrawBitmap(vidBitmap, m_DrawPoint);
		}
	}
	
	PopState();
}


//---------------------------------------------------------------------
//	DisplayFrame
//---------------------------------------------------------------------
//
//	Draw frame centered on screen
//

void TVideoEditorView::DisplayFrame(BBitmap *frame)
{		
	LockLooper();
	DrawBitmap(frame, m_DrawPoint);
	UnlockLooper();
}


