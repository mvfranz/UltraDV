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
		BRect					m_PreviewRect;
		TVideoSettingsTabView 	*m_Parent;
		BMenuField 				*m_CompressorMenuField;
		BMenuField 				*m_DepthMenuField;	
		BStringView 			*m_QualityString;		
		BTextControl 			*m_FPSTextControl;
		BSlider					*m_QualitySlider;
		BCheckBox 				*m_KeyframeCheckBox;
		BTextControl 			*m_KeyframeTextControl;
		BCheckBox 				*m_LimitDataRateCheckBox;
		BTextControl 			*m_LimitDataRateTextControl;
			
};	


// Menu messages
#define VID_RAW_MSG  'VCrm'	

#endif
