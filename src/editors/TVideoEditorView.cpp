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
	fEditor = editor;

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

	fDrawPoint.Set(0, 0);
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
	if ( fEditor->IsPlaying() == false) {
		//	Fill background with black
		SetHighColor(kBlack);
		FillRect(updateRect);

		//	Refresh with last rendered frame
		//	Look into centering the bitmap
		BBitmap *vidBitmap = fEditor->GetDisplayBitmap();
		if (vidBitmap) {
			fDrawPoint.x = (Bounds().Width() / 2) - (vidBitmap->Bounds().Width() / 2);
			fDrawPoint.y = (Bounds().Height() / 2) - (vidBitmap->Bounds().Height() / 2);

			DrawBitmap(vidBitmap, fDrawPoint);
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
	DrawBitmap(frame, fDrawPoint);
	UnlockLooper();
}


