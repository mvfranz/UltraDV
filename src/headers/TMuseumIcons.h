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
		BBitmap *m_AnimationIcon16;
		BBitmap *m_Animation32;
		BBitmap *m_AudioIcon16;
		BBitmap *m_AudioIcon32;
		BBitmap *m_MIDIIcon16;
		BBitmap *m_MIDIIcon32;
		BBitmap *m_MovieIcon16;
		BBitmap *m_MovieIcon32;
		BBitmap *m_PictureIcon16;
		BBitmap *m_PictureIcon32;
		BBitmap *m_TextIcon16;
		BBitmap *m_TextIcon32;

		// Cue
		BBitmap *m_Unlock;
		BBitmap *m_Lock;
		BBitmap *m_UnlockDark;
		BBitmap *m_LockDark;
		BBitmap *m_VisibleUp;
		BBitmap *m_VisibleDown;
		BBitmap *m_TransitionIn;
		BBitmap *m_TransitionOut;
		BBitmap *m_PathIn;
		BBitmap *m_PathOut;
		
		BBitmap *m_AnimationUpIcon;
		BBitmap *m_AnimationDnIcon;
		BBitmap *m_AudioUpIcon;
		BBitmap *m_AudioDnIcon;
		BBitmap *m_MIDIUpIcon;
		BBitmap *m_MIDIDnIcon;
		BBitmap *m_MovieUpIcon;
		BBitmap *m_MovieDnIcon;
		BBitmap *m_PictureUpIcon;
		BBitmap *m_PictureDnIcon;
		BBitmap *m_TextUpIcon;
		BBitmap *m_TextDnIcon;
		
		// Header
		BBitmap *m_MuteUp;
		BBitmap *m_MuteDown;
		BBitmap *m_SoloUp;
		BBitmap *m_SoloDown;
		BBitmap *m_Contract;
		BBitmap *m_Expand;
		
		// Transitions
		BBitmap *m_WipeDown	;
		BBitmap *m_WipeLeft	;
		BBitmap *m_WipeRight;
		BBitmap *m_WipeUp;
		BBitmap *m_WipeBottomLeftTopRight;
		BBitmap *m_WipeBottomRightTopLeft;
		BBitmap *m_WipeTopLeftBottomRight;
		BBitmap *m_WipeTopRightBottomLeft;
		BBitmap *m_CurtainsIn;
		BBitmap *m_CurtainsOut;
		BBitmap *m_RevealBottomLeftTopRight;
		BBitmap *m_RevealBottomRightTopLeft;
		BBitmap *m_RevealTopLeftBottomRight;
		BBitmap *m_RevealTopRightBottomLeft;
		BBitmap *m_RevealUp;
		BBitmap *m_RevealDown;
		BBitmap *m_RevealLeft;
		BBitmap *m_RevealRight;
		
		//	Controls
		BBitmap *m_ExportSliderLeft;
		BBitmap *m_ExportSliderRight;
		
		//	Keyframes
		BBitmap	*m_KeyframeOff;
		BBitmap	*m_KeyframeOn;
	
	private:


};

#endif
