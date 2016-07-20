//---------------------------------------------------------------------
//
//	File:	TTransportPalette.h
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

#ifndef __TTRANSPORTPALETTE_H__
#define __TTRANSPORTPALETTE_H__

//	Includes
#include <MessageRunner.h>

//	Class Definition
class TTransportPalette: public BWindow
{
	public:
		TTransportPalette(BRect bounds);
		~TTransportPalette();

		void 	MessageReceived(BMessage* message);

		bool 	QuitRequested();
		
		// Accessor Functions
		TTransportPaletteView	*GetTransportPaletteView(){ return fTransportView; }
	private:
		// Member functions
		void		Init();

		// Member variables
		TTransportPaletteView	*fTransportView;
		
		bool			fIsPlaying;
		bool			fIsStopping;

		BMessageRunner*	fRunner;
};

// Transport Buttons
//const int16 kTransportButtonWidth 	= 27;
//const int16 kTransportButtonHeight = 19;
//const int16 kTransportButtonOffset = 1;
const int16 kTransportButtonWidth 	= 36;
const int16 kTransportButtonHeight 	= 29;
const int16 kTransportButtonOffset 	= 5;

//const 
enum transportButton
{
	kStartButton,
	kRewindButton,
	kPlayButton,
	kPauseButton,
	kStopButton,
	kFFButton,
	kEndButton,
	kRecordButton,
	kTotalTransportButtons
};

#endif
