//---------------------------------------------------------------------
//
//	File:	TPictureCueEditorView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.26.98
//
//	Desc:	Picture Editor View
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


#include "TPictureCueEditorView.h"



//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//


TPictureCueEditorView::TPictureCueEditorView( BBitmap* bitmap ) : BView(bitmap->Bounds(), "EditorView", B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW|B_FRAME_EVENTS)
{
	fBitmap = bitmap;

	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
}


void TPictureCueEditorView::Draw(BRect area)
{
	if (!fBitmap)
		return;

	BRect frame = fBitmap->Bounds();
	area = area & frame;

	if ((area.right >= area.left) && (area.bottom >= area.top))
		DrawBitmap(fBitmap, area, area);

}
