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
		
		TTimeText	*GetTransportText(){ return m_TransportText; }
		
		// Member variables	
		
	private:				
		// Member functions
		
		void 	Init();
				
		// Member variables
		TBitmapButton	*m_StartButton;				
		TBitmapButton	*m_RewindButton;
		TBitmapButton	*m_PlayButton;
		TBitmapButton	*m_PauseButton;
		TBitmapButton	*m_FFButton;		
		TBitmapButton	*m_EndButton;
		TTimeText		*m_TransportText;
};		


// Utility Functions
BBitmap *GetTransportButton(char *theResource);

#endif
