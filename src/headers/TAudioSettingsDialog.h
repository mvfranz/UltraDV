//---------------------------------------------------------------------
//
//	File:	TAudioSettingsDialog.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TAUDIOSETTINGSDIALOG_H__
#define __TAUDIOSETTINGSDIALOG_H__

// Forward Declarations
class TAudioSettingsView;
class TAudioSettingsTabView;

// Class definitions
class TAudioSettingsDialog: public BWindow
{
	public:
		TAudioSettingsDialog(BRect bounds);
		
		void	MessageReceived(BMessage* message);	
		
		void 	WindowActivated(bool active);
		bool 	QuitRequested();
						
	private:	
		// Member Functions	
		void		Init();	
		
		// Member Variable
		TAudioSettingsView		*m_BGView;
		TAudioSettingsTabView 	*m_TabView;
};

#endif
