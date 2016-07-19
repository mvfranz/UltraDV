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
#include <MediaNode.h>

//	Class Definition
class TTransportPalette: public BWindow, public BMediaNode
{
	public:
		TTransportPalette(BRect bounds);
		~TTransportPalette();
		
		void	MessageReceived(BMessage* message);	
		
		bool 	QuitRequested();
		
		//	MediaNode Functions
		port_id 	 ControlPort() const;
		BMediaAddOn	 *AddOn( int32 * internal_id) const;

		// Accessor Functions
		TTransportPaletteView	*GetTransportPaletteView(){ return fTransportView; }
	private:
		// Member functions
		void		Init();
		
		static status_t service_routine(void *data);
		void 			ServiceRoutine();
		static status_t run_routine(void *data);
		void 			RunRoutine();


		// Member variables
		TTransportPaletteView	*fTransportView;
		
		bool			fTimeToQuit;
		bool			fIsPlaying;
		bool			fIsStopping;

		port_id 		fPort;
		
		thread_id		fServiceThread;
		thread_id		fRunThread;		

		
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
