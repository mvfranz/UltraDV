//-------------------------------------------------------------------
//
//	File:	TRotateDialog.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.22.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#pragma once

//	Forward declarations
class TRotationEffect;
class TNumberTextControl;

// Class Definition
class TRotateDialog : public BWindow
{
	public:
		TRotateDialog(TRotationEffect* e, int32 initialVal);
		virtual ~TRotateDialog();
		
		void 	MessageReceived(BMessage* message);	
			// Called externally to change the rotation amount
		void	SetRotation(float value);
			// Get the state of the 'live update' checkbox
		bool	LiveUpdate(void) const;
			// And the current specified rotation amount.
		int32	Rotation(void) const;

	private:
	
		// Member Functions
		void	Init();		
		
		// Member Variables		
		TRotationEffect		*m_effect;
		int32				m_savedRotation;
		BView				*m_BackView;
		BSlider				*m_Slider;
		TNumberTextControl 	*m_RotateText;
		BCheckBox			*m_UpdateCheckbox;
		BButton 			*m_OKButton;
		BButton 			*m_ApplyButton;
		BButton 			*m_CancelButton;
};
