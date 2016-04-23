//---------------------------------------------------------------------
//
//	File:	TVideoSettingsDialog.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TVIDEOSETTINGSDIALOG_H__
#define __TVIDEOSETTINGSDIALOG_H__

// Includes
#include "TVideoSettings.h"

// Forward Declarations
class TVideoPreviewView;
class TVideoSettingsView;
class TVideoSettingsTabView;

// Class definitions
class TVideoSettingsDialog: public BWindow
{
	public:
		// Member Functions
		TVideoSettingsDialog(BRect bounds);
		~TVideoSettingsDialog();
		
		void	MessageReceived(BMessage* message);	
		
		bool 	QuitRequested();
		
		void	StartCapture();
		void	StopCapture();
		
		//	Inline
		inline TVideoPreviewView *PreviewView(){ return m_PreviewView; }
		inline BParameterWeb 	 *ParameterWeb(){ return m_ParameterWeb; }
		
		// Member Variables
		TVideoSettings			m_TempVideoSettings;
						
	private:	
		// Member Functions	
		void		Init();	
		
		// Member Variables
		TVideoSettingsView		*m_BGView;
		TVideoSettingsTabView 	*m_TabView;
		TVideoPreviewView 		*m_PreviewView;
				
		BParameterWeb 			*m_ParameterWeb;

		media_node	m_TimeSource;
		media_node 	m_VideoSource;
		
		media_output 	m_From;
		media_input 	m_To;				
};

#endif
