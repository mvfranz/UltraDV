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

#ifndef __TROTATEDIALOG_H__
#define __TROTATEDIALOG_H__


// Message Definitions
#define	ROTATE_MSG			'ROTm'
#define	ROTATE_TOOL_MSG		'ROTs'
#define	ROTATE_TEXT_MSG		'ROTt'
#define	ROTATE_CLOSE_MSG	'ROTl'
#define ROTATE_UPDATE_MSG	'ROTu'

//	Forward declarations
class TRotateSlider;

// Class Definition
class TRotateDialog : public BWindow
{
	public:
		TRotateDialog(TVisualCue *theCue);
		~TRotateDialog();
		
		void 	MessageReceived(BMessage* message);	
		
		//	Accessor Functions
		bool	GetRealtimePreview(){ return fRealtimePreview; }
		
	private:
	
		// Member Functions
		void	Init();		
		
		void 	UpdateCue();
		
		// Member Variables		
		TVisualCue 			*fCue;
		BView				*fBackView;
		TRotateSlider		*fSlider;
		TNumberTextControl 	*fRotateText;
		BCheckBox			*fUpdateCheckbox;
		
		float				fSavedRotation;
		float				fLastRotation;
		bool				fRealtimePreview;
		
		BButton 			*fOKButton;
		BButton 			*fApplyButton;
		BButton 			*fCancelButton;
	
};

#endif
