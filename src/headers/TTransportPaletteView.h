//---------------------------------------------------------------------
//
//	File:	TTransportPaletteView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	MediaCue sheet view
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TTransportPaletteView__
#define __TTransportPaletteView__ 


// Constants
const int32 kTransportFontSize = 12;

//	Includes
#include "TBitmapView.h"

// Class Declarations
class TTransportPaletteView: public TBitmapView
{
	public:
		// Member functions
		TTransportPaletteView(BRect bounds, BBitmap *bitmap);
		~TTransportPaletteView();
		
		void 	Draw(BRect updateRect);		
		void	MessageReceived(BMessage *message);

		void 	Pulse();

		TTimeText	*GetTransportText(){ return fTransportText; }
		
		// Member variables	
		
	private:				
		// Member functions
		
		void 	Init();
				
		// Member variables
		TBitmapButton	*fStartButton;				
		TBitmapButton	*fRewindButton;
		TBitmapButton	*fPlayButton;
		TBitmapButton	*fPauseButton;
		TBitmapButton	*fFFButton;		
		TBitmapButton	*fEndButton;
		TTimeText		*fTransportText;
};		


// Utility Functions
BBitmap *GetTransportButton(char *theResource);

#endif
