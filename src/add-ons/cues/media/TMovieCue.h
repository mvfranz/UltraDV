//---------------------------------------------------------------------
//
//	File:	TMovieCue.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Header file
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#if !defined(__TMOVIECUE_H__)
#define __TMOVIECUE_H__

// Includes
#include "TVisualCue.h"

// Forward Declaration
class TRIFFReader;
class TAudioCodec;
class TVideoCodec;
class TVideoEditor;


// Class Definition
class TMovieCue : public TVisualCue
{
	public:
		// Member Functions
		TMovieCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime);
		TMovieCue(entry_ref &theRef, int16 id,  TCueChannel *parent, BRect bounds, uint32 startTime);
		TMovieCue(BMessage *theMessage);
		~TMovieCue();
						
		__declspec(dllexport) static	BArchivable *Instantiate(BMessage *data);
		__declspec(dllexport) status_t 	Archive(BMessage *data, bool deep) const;
		
		void 	Draw(BRect updateRect);
		
		void 	MessageReceived(BMessage *message);
		
		//	Utility Functions
		BBitmap	*GetBitmap(uint32 theTime);
				
		// Member Variables
				
		// Accessor Functions		
		inline TRIFFReader *RIFFReader(){ return m_Reader; }
		inline TAudioCodec *AudioCodec(){ return m_AudioCodec; }
		inline TVideoCodec *VideoCodec(){ return m_VideoCodec; }
		
	private:
		// Member Functions
		void 	Init();
		bool 	InitCodecs();
		bool 	InitVideoCodec();
		bool 	InitAudioCodec();
				
		bool 	LoadMovieFile(BMessage *message);
		void 	OpenEditor();
		
		void 	RenderBitmapData();
				
		void 	LoadCueIcon();
		
		// Playback Routines
		void 	HandlePlayback(uint32 theTime);

	protected:
		// Member Functions
		void 		ShowPanel();
		void		HidePanel();
				
		// Member Variables
		TVideoEditor 	*m_Editor;
		
		TRIFFReader		*m_Reader;		
		TAudioCodec		*m_AudioCodec;
		TVideoCodec		*m_VideoCodec;
		
		uint32			m_MSecsPerFrame;
		uint32 			m_CurrentVideoFrame;
		uint32 			m_CurrentAudioFrame;
};

#endif
