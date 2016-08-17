//---------------------------------------------------------------------
//
//	File:	TPauseCue.h
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

#ifndef __TPAUSECUE_H__
#define __TPAUSECUE_H__


// Pause messages
#define PAUSE_DONE_MSG	'pdMG'

#include "TPlaybackList.h"

// Pause Cue Types
enum pauseCueTypes
{
	kSecondsPause,
	kMousePause,
	kKeyPause,
	kMessagePause
};

// Forward Declarations
class TCueView;
class TPauseSetup;

// Class Definition
class TPauseCue : public TCueView
{
	public:
		// Member Functions
		TPauseCue(int16 id, TCueChannel *parent, BRect bounds, BPoint point, int32 startTime);
		TPauseCue(BaseCueChunk *theChunk, TCueChannel *parent, BRect bounds);
		TPauseCue(BMessage *data);
		~TPauseCue();
				
		static	BArchivable *Instantiate(BMessage *data);
		status_t 	Archive(BMessage *data, bool deep) const;		
		
		void 	Draw(BRect updateRect);
		void 	MessageReceived(BMessage *message);
		
		void 	Preroll(double currentTime);
		// ABH missing routine
		void	Preroll(TPlaybackEvent *PlaybackEvent, double currentTime);
		void 	Play();
		void 	Pause();
		void 	DoPause(TVideoEngine *theEngine);
		void 	Resume();
		void 	Stop();
		
				
		// Member Variables
		int16	fPauseType;
		int32	fPauseDuration;
		char 	fKey;
		int32	fMessage;
		bool  	fShowCursor;
				
	private:
		// Member Functions
		void 	Init();
		void 	Init(BaseCueChunk *theChunk);
		void 	OpenEditor();
		
		void 	LoadCueIcon();
		
		BMessageRunner* fPauseTimer;

	protected:
				
		// Member Variables
		TPauseSetup	*fEditor;											
};

#endif
