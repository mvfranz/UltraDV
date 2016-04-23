//---------------------------------------------------------------------
//
//	File:	TAudioSampleView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TAUDIOSAMPLEVIEW_H__
#define __TAUDIOSAMPLEVIEW_H__ 

// Forward Declarations
class TMuseumSlider;
class TAudioSettingsTabView;
class TAudioLevelsView;
class TLevelsSlider;

// Class Declarations
class TAudioSampleView: public BView
{
	public:
		// Member functions
		TAudioSampleView(TAudioSettingsTabView *parent, BMessage *archive);

		void	MessageReceived(BMessage *theMessage);		
		
		void 	AttachedToWindow();
				
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		void 	ApplyToSource();
						
		// Member variables
		TAudioSettingsTabView	*m_Parent;
		BMenuField 				*m_RateMenuField;
		BMenuField 				*m_DepthMenuField;
		BMenuField 				*m_TypeMenuField;
		TAudioLevelsView		*m_AudioLevelsView;
		BBox					*m_VolumeBox;
		
		TLevelsSlider		*m_InputSlider;
		TLevelsSlider		*m_OutputSlider;				
};	

// Messages
#define SAMP_11_MSG				'sEle'
#define SAMP_22_MSG				'sTwo'
#define SAMP_44_MSG				'sFor'

#define	SAMP_BIT_EIGHT_MSG		'bEig'
#define	SAMP_BIT_SIXTEEN_MSG	'bSiz'

#define	SAMP_MONO_MSG			'sMon'
#define	SAMP_STEREO_MSG			'sSte'

#endif
