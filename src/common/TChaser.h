//---------------------------------------------------------------------
//
//	File:	TChaser.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.16.98
//
///	Desc:	Timecode follower based on Doug Wright's code	
//		
//						
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TCHASER_H__
#define __TCHASER_H__

#include "TThread.h"

// Forward Declarations
class TTick;

// Class Definition
class TChaser : public TThread
{
	public:
		// Member Functions
		//TChaser( void (*ticked_func)(void *arg, TTick *t), void *arg, const char *name = "chaserThread", int32 aPriority = B_URGENT_PRIORITY);
		TChaser( void (*ticked_func)(void *arg, TTick *t), void *arg, const char *name = "chaserThread", int32 aPriority = B_NORMAL_PRIORITY);
		
		virtual	status_t	Start();
		virtual	void		Stop();
			
		// Inlines
		inline bool 	IsStopping() { return fIsStopping; }
		inline port_id 	PortID() const { return fPortID; }
	
	private:
		// Member Functions
		virtual	status_t	ThreadMain();				//overridden from TThread
		void				(*Ticked)(void *arg, TTick *t);		//points to ticked_func
	
		// Memeber Variables
		void		*fCaller;
		char		*fAreaAddress;
		area_id		fAreaID;
		port_id		fPortID;
		bool		fIsStopping;
};

#endif
