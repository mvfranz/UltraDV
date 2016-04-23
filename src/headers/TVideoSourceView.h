//---------------------------------------------------------------------
//
//	File:	TVideoSourceView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVIDEOSOURCEVIEW_H__
#define __TVIDEOSOURCEVIEW_H__

// Forward Declarations
class TVideoSettingsTabView;
class TVideoPreviewView;

// Class Declarations
class TVideoSourceView: public BView
{
	public:
		// Member functions
		TVideoSourceView(TVideoSettingsTabView *parent, BMessage *archive);
		~TVideoSourceView();
				
		void	AttachedToWindow();
		void	DetachedFromWindow();
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		void 	ApplyToSource();
		
		// Menu Handling
		void SetDigitizerMenuItem(BMenu *theMenu);
		void SetInputMenuItem(BMenu *theMenu);
						
		// Member variables
		BRect					m_PreviewRect;
		TVideoSettingsTabView 	*m_Parent;
		BMenuField 				*m_DigitizerMenuField;
		BMenuField 				*m_InputMenuField;	
		BMenuField 				*m_FormatMenuField;
		BCheckBox				*m_GammaCorrectionCheckBox;
		BCheckBox				*m_LumaCoringCheckBox;
		BCheckBox				*m_ErrorDiffusionCheckBox;
		BCheckBox				*m_LumaCombCheckBox;
		BCheckBox				*m_ChromaCombCheckBox;
			
};

#endif
