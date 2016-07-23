//---------------------------------------------------------------------
//
//	File:	TMuseumIcons.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	04.08.98
//
//	Desc:	Object that handles all application icons
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TMUSEUMICONS_H__
#define __TMUSEUMICONS_H__


class TMuseumIcons
{
	public:
		TMuseumIcons();
		~TMuseumIcons();
		
		void	Init();
		BBitmap *GetIconByName(const char *theName);
		
		// Icons
		BBitmap *fAnimationIcon16;
		BBitmap *fAnimation32;
		BBitmap *fAudioIcon16;
		BBitmap *fAudioIcon32;
		BBitmap *fMIDIIcon16;
		BBitmap *fMIDIIcon32;
		BBitmap *fMovieIcon16;
		BBitmap *fMovieIcon32;
		BBitmap *fPictureIcon16;
		BBitmap *fPictureIcon32;
		BBitmap *fTextIcon16;
		BBitmap *fTextIcon32;

		// Cue
		BBitmap *fUnlock;
		BBitmap *fLock;
		BBitmap *fUnlockDark;
		BBitmap *fLockDark;
		BBitmap *fVisibleUp;
		BBitmap *fVisibleDown;
		BBitmap *fTransitionIn;
		BBitmap *fTransitionOut;
		BBitmap *fPathIn;
		BBitmap *fPathOut;
		
		BBitmap *fAnimationUpIcon;
		BBitmap *fAnimationDnIcon;
		BBitmap *fAudioUpIcon;
		BBitmap *fAudioDnIcon;
		BBitmap *fMIDIUpIcon;
		BBitmap *fMIDIDnIcon;
		BBitmap *fMovieUpIcon;
		BBitmap *fMovieDnIcon;
		BBitmap *fPictureUpIcon;
		BBitmap *fPictureDnIcon;
		BBitmap *fTextUpIcon;
		BBitmap *fTextDnIcon;
		
		// Header
		BBitmap *fMuteUp;
		BBitmap *fMuteDown;
		BBitmap *fSoloUp;
		BBitmap *fSoloDown;
		BBitmap *fContract;
		BBitmap *fExpand;
		
		// Transitions
		BBitmap *fWipeDown	;
		BBitmap *fWipeLeft	;
		BBitmap *fWipeRight;
		BBitmap *fWipeUp;
		BBitmap *fWipeBottomLeftTopRight;
		BBitmap *fWipeBottomRightTopLeft;
		BBitmap *fWipeTopLeftBottomRight;
		BBitmap *fWipeTopRightBottomLeft;
		BBitmap *fCurtainsIn;
		BBitmap *fCurtainsOut;
		BBitmap *fRevealBottomLeftTopRight;
		BBitmap *fRevealBottomRightTopLeft;
		BBitmap *fRevealTopLeftBottomRight;
		BBitmap *fRevealTopRightBottomLeft;
		BBitmap *fRevealUp;
		BBitmap *fRevealDown;
		BBitmap *fRevealLeft;
		BBitmap *fRevealRight;
		
		//	Controls
		BBitmap *fExportSliderLeft;
		BBitmap *fExportSliderRight;
		
		//	Keyframes
		BBitmap	*fKeyframeOff;
		BBitmap	*fKeyframeOn;
	
	private:


};

#endif
