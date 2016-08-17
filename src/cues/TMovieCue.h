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
#include "TVideoEditor.h"

#include <MediaFile.h>


// Class Definition
class TMovieCue : public TVisualCue
{
	public:
		// Member Functions
		TMovieCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime);
		TMovieCue(entry_ref &theRef, int16 id,  TCueChannel *parent, BRect bounds, uint32 startTime);
		TMovieCue(BMessage *theMessage);
		~TMovieCue();
						
		static	BArchivable *Instantiate(BMessage *data);
		status_t 	Archive(BMessage *data, bool deep) const;
		
		void 	Draw(BRect updateRect);
		
		void 	MessageReceived(BMessage *message);
		
		//	Utility Functions
		BBitmap	*GetBitmap(uint32 theTime);
				
	private:
		// Member Functions
		void 	Init();
				
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
		
		BMediaFile*		fMediaFile;
		BMediaTrack*	fVideoTrack;
		media_format	fMediaFormat;
		bigtime_t		fCurrentTime;
};

#endif
