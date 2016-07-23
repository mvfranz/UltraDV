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
		TCueSheetTimeView 	*fParent;	
		TCueSheetView 		*fCueSheet;
		
		BRadioButton		*fRealTimeButton;
		BRadioButton		*f24FPSButton;
		BRadioButton		*f25FPSButton;
		BRadioButton		*f2997FPSButton;
		BRadioButton		*f30FPSButton;
		
		BTextControl		*fStartTime;
		BTextControl		*fDuration;
		BTextControl		*fEndTime;					
		
		int16				fTimeFormat;
};

#endif
