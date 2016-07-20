//---------------------------------------------------------------------
//
//	File:	TTimePalette.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Header
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TTIMEPALETTE_H__
#define __TTIMEPALETTE_H__

#include <MessageRunner.h>

class TTimePalette: public BWindow
{
	public:
		TTimePalette(BRect bounds);
		~TTimePalette();

		void 	MessageReceived(BMessage* message);
		bool 	QuitRequested();

		// Accessor Functions
		TTimePaletteView *GetTimePaletteView(){ return fTimeView; }
		
	private:
		// Member functions
		void	Init();
	
		// Member variables
		TTimePaletteView *fTimeView;
		
		bool			fIsPlaying;
		bool			fIsStopping;

		BMessageRunner*	fRunner;
};

#endif
