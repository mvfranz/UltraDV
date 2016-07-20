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
		inline TRIFFReader *RIFFReader(){ return fReader; }
		inline TAudioCodec *AudioCodec(){ return fAudioCodec; }
		inline TVideoCodec *VideoCodec(){ return fVideoCodec; }
		
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
		TVideoEditor 	*fEditor;
		
		TRIFFReader		*fReader;		
		TAudioCodec		*fAudioCodec;
		TVideoCodec		*fVideoCodec;
		
		uint32			fMSecsPerFrame;
		uint32 			fCurrentVideoFrame;
		uint32 			fCurrentAudioFrame;
};

#endif
