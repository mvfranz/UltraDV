//---------------------------------------------------------------------
//
//	File:	TAudioSettingsView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.2.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TAUDIOSETTINGSVIEW_H__
#define __TAUDIOSETTINGSVIEW_H__ 

// Forward Declarations
class TAudioSettingsDialog;

// Class Declarations
class TAudioSettingsView: public BView
{
	public:
		// Member functions
		TAudioSettingsView(BRect bounds, TAudioSettingsDialog *parent);
		
		void	MessageReceived(BMessage *theMessage);		
				
		
		// Member variables	
		
		// Inlines
		
	private:				
		// Member functions
		void 	Init();
		
		// Member variables
		TAudioSettingsDialog 	*m_Parent;
		
		BButton			*m_CancelButton;
		BButton			*m_OKButton;
		
};

#endif
