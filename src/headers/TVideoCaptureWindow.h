//---------------------------------------------------------------------
//
//	File:	TVideoCaptureWindow.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.30.98
//
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TVIDEOCAPTUREWINDOW_H__
#define __TVIDEOCAPTUREWINDOW_H__
#include <Window.h>

// Constants

// Forward Declaration
class TVideoCaptureView;
class TVideoCaptureToolbar;
class TStatusView;

// Class Definition
class TVideoCaptureWindow: public BWindow
{
	public:
		TVideoCaptureWindow();
		~TVideoCaptureWindow();
		
		virtual void	MessageReceived(BMessage* message);	
		
		// Accessor Functions
		inline TStatusView	*GetStatusBar(){ return m_StatusBar; }
		
	private:	
		// Member Functions
		void	Init();	
		
		void	Record();
				
		// Member Variables
		TVideoCaptureToolbar 	*m_Toolbar;
		TVideoCaptureView 		*m_VidCapView;
		TStatusView				*m_StatusBar;
	
		BParameterWeb 		*m_ParameterWeb;

		media_node			m_TimeSource;
		media_node 			m_VideoSource;
		
		media_output 		m_From;
		media_input 		m_To;		
};

// Capture Messages
#define	VIDCAP_RECORD_MSG	'vcRM'
#define VIDCAP_STOP_MSG		'vcSM'

#endif

