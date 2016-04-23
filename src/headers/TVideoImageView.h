//---------------------------------------------------------------------
//
//	File:	TVideoImageView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVideoImageView__
#define __TVideoImageView__

// Forward Declarations
class TMuseumSlider;
class TVideoSettingsTabView;
class TVideoPreviewView;

// Class Declarations
class TVideoImageView: public BView
{
	public:
		// Member functions
		TVideoImageView(TVideoSettingsTabView *parent, BMessage *archive);
		
		void	AttachedToWindow();
		void	DetachedFromWindow();
				
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		void 	ApplyToSource();
								
		// Member variables
		BRect					m_PreviewRect;
		TVideoSettingsTabView	*m_Parent;
		BStringView				*m_HueString;
		BStringView				*m_SaturationString;
		BStringView				*m_BrightnessString;
		BStringView				*m_ContrastString;
		BButton					*m_DefaultsButton;
		
		TMuseumSlider			*m_HueSlider;
		TMuseumSlider			*m_SaturationSlider;
		TMuseumSlider			*m_BrightnessSlider;
		TMuseumSlider			*m_ContrastSlider;
	
};		

#endif
