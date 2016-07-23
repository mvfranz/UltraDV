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
		bool	GetRealtimePreview(){ return fRealtimePreview; }

	private:
	
		// Member Functions
		void 	Init();
		void 	UpdateCue();
		
		// Member Variables		
		TVisualCue 			*fCue;
		BView				*fBackView;
		TOpacitySlider		*fSlider;
		TNumberTextControl 	*fOpacityText;
		BCheckBox			*fUpdateCheckbox;
		
		float				fSavedOpacity;
		bool				fRealtimePreview;
		
		BButton 			*fOKButton;
		BButton 			*fApplyButton;
		BButton 			*fCancelButton;
	
};

#endif
