//---------------------------------------------------------------------
//
//	File:	TVideoEditorButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.13.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TVideoEditorButton__
#define __TVideoEditorButton__  

// Forward Declarations
#include "TBitmapButton.h"

// Class Declarations
class TVideoEditorButton: public TBitmapButton
{
	public:
		// Member functions
		TVideoEditorButton( BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler, BMessage *message);
		
		// Member variables	
		
	private:				
		// Member functions
		
		// Member variables
				

};		

#endif
