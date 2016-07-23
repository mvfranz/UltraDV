//---------------------------------------------------------------------
//
//	File:	TAudioCompressionView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//			
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TAUDIOCOMPRESSIONVIEW_H__
#define __TAUDIOCOMPRESSIONVIEW_H__ 

// Forward Declarations
class TAudioLevelsView;
class TLevelsSlider;

// Class Declarations
class TAudioCompressionView: public BView
{
	public:
		// Member functions
		TAudioCompressionView(BMessage *archive);
				
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
				
		// Member variables
		BMenuField 			*fCompressorMenuField;
		TAudioLevelsView	*fAudioLevelsView;
		BBox				*fVolumeBox;
		
		TLevelsSlider		*fInputSlider;
		TLevelsSlider		*fOutputSlider;
			
};	

//  Audio Compressor Messages
#define AUDIO_NONE_MSG  'aNon'

#endif
