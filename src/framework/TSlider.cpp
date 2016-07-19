//-------------------------------------------------------------------
//
//	File:	TSlider.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.13.98
//
//	Desc:	Derivative of BSlider for live updates
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TSlider.h"


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Default constructor
//

TSlider::TSlider( BRect frame, const char *name, const char *label, BMessage *message, int32 minValue,
                  int32 maxValue, thumb_style thumbType, uint32 resizingMode, uint32 flags) :
	BSlider(frame, name, label, message, minValue, maxValue, thumbType, resizingMode, flags)
{


}



//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Construct from message
//

TSlider::TSlider(BMessage *data) : BSlider(data)
{
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TSlider::~TSlider()
{
}



//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

void TSlider::SetValue(int32 value)
{
	// Pass to parent
	BSlider::SetValue(value);
}

