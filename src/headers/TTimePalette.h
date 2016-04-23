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

class TTimePalette: public BWindow, public BMediaNode
{
	public:
		TTimePalette(BRect bounds);
		~TTimePalette();
		
		void	MessageReceived(BMessage* message);	
		
		bool 	QuitRequested();
		
		//	MediaNode Functions
		port_id 	 ControlPort() const;
		BMediaAddOn	 *AddOn( int32 * internal_id) const;

		// Accessor Functions
		TTimePaletteView *GetTimePaletteView(){ return m_TimeView; }
		
	private:
		// Member functions
		void	Init();
		
		static status_t service_routine(void *data);
		void 			ServiceRoutine();
		static status_t run_routine(void *data);
		void 			RunRoutine();

		
		// Member variables
		TTimePaletteView *m_TimeView;
		
		bool			m_TimeToQuit;
		bool			m_IsPlaying;
		bool			m_IsStopping;

		port_id 		m_Port;
		
		thread_id		m_ServiceThread;
		thread_id		m_RunThread;		
};

#endif
