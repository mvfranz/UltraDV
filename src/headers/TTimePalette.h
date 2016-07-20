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

#include <MediaNode.h>

class TTimePalette: public BWindow
{
	public:
		TTimePalette(BRect bounds);
		~TTimePalette();

		bool 	QuitRequested();
		
		// Accessor Functions
		TTimePaletteView *GetTimePaletteView(){ return fTimeView; }
		
	private:
		// Member functions
		void	Init();
		
		static status_t run_routine(void *data);
		void 			RunRoutine();

		
		// Member variables
		TTimePaletteView *fTimeView;
		
		bool			fTimeToQuit;
		bool			fIsPlaying;
		bool			fIsStopping;

		thread_id		fRunThread;
};

#endif
