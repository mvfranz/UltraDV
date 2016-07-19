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
		inline TVideoPreviewView *PreviewView(){ return fPreviewView; }
		inline BParameterWeb 	 *ParameterWeb(){ return fParameterWeb; }
		
		// Member Variables
		TVideoSettings			fTempVideoSettings;
						
	private:	
		// Member Functions	
		void		Init();	
		
		// Member Variables
		TVideoSettingsView		*fBGView;
		TVideoSettingsTabView 	*fTabView;
		TVideoPreviewView 		*fPreviewView;
				
		BParameterWeb 			*fParameterWeb;

		media_node	fTimeSource;
		media_node 	fVideoSource;
		
		media_output 	fFrom;
		media_input 	fTo;				
};

#endif
