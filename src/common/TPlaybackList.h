//---------------------------------------------------------------------
//
//	File:	TPlaybackList.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.17.98
//
///	Desc:	List that contains cue events
//			
//						
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef _TPLAYBACKLIST_H_
#define _TPLAYBACKLIST_H_

// ABH missing include
#include <be/support/List.h>
#include "TCueView.h"

// Class definitions
class TPlaybackList : public BList
{
	public:
		TPlaybackList();
		~TPlaybackList();
		
	private:
	
	protected:

};


class TPlaybackEvent
{
	public:
		TPlaybackEvent();
		~TPlaybackEvent();
		
		int32 	fTime;
		BList	fCueEvents;	// This is a list of CueEvents, described below...

	private:
	
	protected:

};


class TCueEvent
{
	public:
		TCueEvent();
		~TCueEvent();
		
		int32 		fTime;
		TCueView	*fCue;
		BList		fEvents;		// This is a list of event BMessages
		
	private:
		
	
	protected:

};

#endif	// _TPLAYBACKLIST_H_
