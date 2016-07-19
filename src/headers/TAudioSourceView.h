//---------------------------------------------------------------------
//
//	File:	TAudioSourceView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TAUDIOSOURCEVIEW_H__
#define __TAUDIOSOURCEVIEW_H__

// Forward Declarations
class TAudioLevelsView;
class TLevelsSlider;

// Class Declarations
class TAudioSourceView: public BView
{
	public:
		// Member functions
		TAudioSourceView(BMessage *archive);
		
		void	MessageReceived(BMessage *theMessage);		
	
		void 	AttachedToWindow();
				
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
						
		// Member variables
		BMenuField			*fDeviceMenuField;
		BMenuField 			*fInputMenuField;			
		TAudioLevelsView	*fAudioLevelsView;
		BBox				*fVolumeBox;
		
		TLevelsSlider		*fInputSlider;
		TLevelsSlider		*fOutputSlider;
};

// Messages
#define DEVICE_DEFAULT_MSG 	'dDef'
#define SOURCE_NONE_MSG 	'sNon'
#define SOURCE_MIC_MSG 		'sMic'
#define SOURCE_MIC_20_MSG 	'sMiB'
#define SOURCE_CD_MSG 		'sCD '
#define SOURCE_AUX_MSG 		'sAux'

#endif
