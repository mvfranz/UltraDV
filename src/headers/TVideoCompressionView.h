//---------------------------------------------------------------------
//
//	File:	TVideoCompressionView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//			
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVideoCompressionView__
#define __TVideoCompressionView__

// Forward Declarations
class TVideoSettingsTabView;
class TVideoPreviewView;

// Class Declarations
class TVideoCompressionView: public BView
{
	public:
		// Member functions
		TVideoCompressionView(TVideoSettingsTabView *parent, BMessage *archive);
		~TVideoCompressionView();
				
		void	AttachedToWindow();
		void	DetachedFromWindow();
				
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		void 	ApplyToSource();
				
		// Member variables
		BRect					fPreviewRect;
		TVideoSettingsTabView 	*fParent;
		BMenuField 				*fCompressorMenuField;
		BMenuField 				*fDepthMenuField;	
		BStringView 			*fQualityString;		
		BTextControl 			*fFPSTextControl;
		BSlider					*fQualitySlider;
		BCheckBox 				*fKeyframeCheckBox;
		BTextControl 			*fKeyframeTextControl;
		BCheckBox 				*fLimitDataRateCheckBox;
		BTextControl 			*fLimitDataRateTextControl;
			
};	


// Menu messages
#define VID_RAW_MSG  'VCrm'	

#endif
