//---------------------------------------------------------------------
//
//	File:	TLevelsSlider.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.06.98
//
//	Desc:	Levels Slider
//			
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TLEVELSSLIDER_H__
#define __TLEVELSSLIDER_H__

typedef enum
{
	kAudioInputSlider,
	kAudioOutputSlider
} AudioSliderType;


// Class Definition
class TLevelsSlider : public BView
{
	public:
		TLevelsSlider(BRect r, AudioSliderType type);		
		~TLevelsSlider();
		
		void 	MessageReceived(BMessage *theMessage);
					
		void	Init();
		
		void	Draw(BRect updateRect);
		
		void 	MouseDown(BPoint where);
		
		void 	AttachedToWindow();
		void 	DetachedFromWindow();
		
	private:
		// Member Functions
		void 	LoadSliders();
		
		void	DrawSliders();
		void	DrawLeftSlider();
		void 	DrawRightSlider();
		void 	DrawSliderTicks();
		
		void 	ClipSliders();
		void 	ClipLeftSlider();
		void 	ClipRightSlider();
		
		void 	TrackSliders();
		void 	TrackLeftSlider();
		void 	TrackRightSlider();
		
		void 	UpdateVolumes(BPoint where);
		void 	UpdateLeftVolume(BPoint where);
		void 	UpdateRightVolume(BPoint where);
				
		// Member Varibles
		AudioSliderType fType;
		BRect			fLeftSliderRect;
		BRect			fRightSliderRect;
		BBitmap 		*fLeftSlider;
		BBitmap 		*fRightSlider;
};

#endif

