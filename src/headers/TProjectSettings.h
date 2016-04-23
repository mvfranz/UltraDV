//-------------------------------------------------------------------
//
//	File:	TProjectSettings.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.15.98
//
//	Desc:	Dialog for the setting of project defaults
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TPROJECTSETTINGS_H__
#define __TPROJECTSETTINGS_H__

// Messages
#define	RT_BUTTON_MSG	'PSrt'
#define	TWOFOUR_BUTTON_MSG	'PS24'
#define	TWOFIVE_BUTTON_MSG	'PS25'
#define	TWONINE_BUTTON_MSG	'PS29'
#define	THIRTY_BUTTON_MSG	'PS30'

// Class Definition
class TProjectSettings : public BWindow
{
	public:
		TProjectSettings(TCueSheetTimeView *parent, TCueSheetView *cueSheet, BMessage *theMessage);
		~TProjectSettings();
		
		void	MessageReceived(BMessage* message);	
		
	private:
	
		// Member Functions
		void	Init();	
		void 	GetDialogSettings();
		
		// Member Variables
		TCueSheetTimeView 	*m_Parent;	
		TCueSheetView 		*m_CueSheet;
		
		BRadioButton		*m_RealTimeButton;
		BRadioButton		*m_24FPSButton;
		BRadioButton		*m_25FPSButton;
		BRadioButton		*m_2997FPSButton;
		BRadioButton		*m_30FPSButton;
		
		BTextControl		*m_StartTime;
		BTextControl		*m_Duration;
		BTextControl		*m_EndTime;					
		
		int16				m_TimeFormat;
};

#endif
