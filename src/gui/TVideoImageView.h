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
		BRect					fPreviewRect;
		TVideoSettingsTabView	*fParent;
		BStringView				*fHueString;
		BStringView				*fSaturationString;
		BStringView				*fBrightnessString;
		BStringView				*fContrastString;
		BButton					*fDefaultsButton;
		
		TMuseumSlider			*fHueSlider;
		TMuseumSlider			*fSaturationSlider;
		TMuseumSlider			*fBrightnessSlider;
		TMuseumSlider			*fContrastSlider;
	
};		

#endif
