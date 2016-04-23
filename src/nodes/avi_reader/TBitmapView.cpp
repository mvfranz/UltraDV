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
	m_Bitmap 	= bitmap; 
	m_Ownership = own;
	
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
	if (m_Bitmap)
	{
		if (m_Ownership)
			delete m_Bitmap; 
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
	if (m_Bitmap)
		DrawBitmap(m_Bitmap, m_Bitmap->Bounds(), Bounds());
};
