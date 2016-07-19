//-------------------------------------------------------------------
//
//	File:	TBitmapView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	11.02.97
//
//-------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <interface/StringView.h>

#include "TBitmapView.h"



//-------------------------------------------------------------------
//
//	Function:	Constructor
//
//	Desc:		
//
//-------------------------------------------------------------------
//
//

TBitmapView::TBitmapView(BRect area, char *name, BBitmap *bitmap, bool own,  uint32 resizeFlags) : BView(area, name, resizeFlags, B_WILL_DRAW) 
{ 
	fBitmap 	= bitmap; 
	fOwnership = own;
	
	// We don't need a background color
	SetViewColor(B_TRANSPARENT_32_BIT);
}

//-------------------------------------------------------------------
//	~TBitmapView
//-------------------------------------------------------------------
//
//

TBitmapView::~TBitmapView() 
{
	if (fBitmap)
	{
		if (fOwnership)
			delete fBitmap; 
	}
}

//-------------------------------------------------------------------
//
//	Function:	Draw
//
//	Desc:		Draws the bitmap into the view	
//
//-------------------------------------------------------------------
//
//

void	TBitmapView::Draw( BRect area)
{
	if (fBitmap)
		DrawBitmap(fBitmap, fBitmap->Bounds(), Bounds());
};
