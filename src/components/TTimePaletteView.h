//---------------------------------------------------------------------
//
//	File:	TTimePaletteView.h
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

#ifndef __TTimePaletteView__
#define __TTimePaletteView__

// Consts

// Class Declarations
class TTimePaletteView: public BView
{
	public:
		// Member functions
		TTimePaletteView(BRect bounds);
		~TTimePaletteView();
		
		void 	Draw(BRect updateRect);
		void 	MessageReceived(BMessage* message);
		void 	Pulse();

		TTimeText	*GetTimeText(){ return fTimeText; }
				
		// Member variables	
		
	private:				
		// Member functions		
		void 	Init();
		
		// Member variables
		TTimeText	*fTimeText;
		TBitmapView	*fLocatorBitmap;
		
};		

#endif
