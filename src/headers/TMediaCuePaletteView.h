//---------------------------------------------------------------------
//
//	File:	TMediaCuePaletteView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	MediaCue sheet view
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TMediaCuePaletteView__
#define __TMediaCuePaletteView__  

// Messages
#define ANIM_CUE_MSG 			'anmc'
#define AUDIO_CUE_MSG 			'audc'
#define MIDI_CUE_MSG 			'midc'
#define MOVIE_CUE_MSG 			'movc'
#define PICTURE_CUE_MSG 		'pctc'
#define SLIDE_CUE_MSG 			'sldc'
#define TEXT_CUE_MSG 			'txtc'

// Consts

// Class Declarations
class TMediaCuePaletteView: public BView
{
	public:
		// Member functions
		TMediaCuePaletteView(BRect bounds);
		~TMediaCuePaletteView();

		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		
		// Member variables
		TCueButton	*m_AnimCueButton;
		TCueButton	*m_AudioCueButton;
		TCueButton	*m_MIDICueButton;
		TCueButton	*m_MovieCueButton;
		TCueButton	*m_PictureCueButton;
		TCueButton	*m_SlideCueButton;
		TCueButton	*m_TextCueButton;				

};		

#endif
