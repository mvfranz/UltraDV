//-------------------------------------------------------------------
//
//	File:	TOpacityDialog.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.15.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TOpacityDialog__
#define __TOpacityDialog__ 


// Message Definitions
#define	OPACITY_MSG			'OPCm'
#define	OPACITY_UPDATE_MSG	'OPUd'
#define	OPACITY_TEXT_MSG	'OPTm'
#define	OPACITY_CLOSE_MSG	'OPCl'

//	Forward Declarations
class TOpacitySlider;

// Class Definition
class TOpacityDialog : public BWindow
{
	public:
		TOpacityDialog(TVisualCue *theCue);
		~TOpacityDialog();
		
		void 	MessageReceived(BMessage* message);	
		
		//	Accessor Functions
		bool	GetRealtimePreview(){ return m_RealtimePreview; }

	private:
	
		// Member Functions
		void 	Init();
		void 	UpdateCue();
		
		// Member Variables		
		TVisualCue 			*m_Cue;
		BView				*m_BackView;
		TOpacitySlider		*m_Slider;
		TNumberTextControl 	*m_OpacityText;
		BCheckBox			*m_UpdateCheckbox;
		
		float				m_SavedOpacity;
		bool				m_RealtimePreview;
		
		BButton 			*m_OKButton;
		BButton 			*m_ApplyButton;
		BButton 			*m_CancelButton;
	
};

#endif
