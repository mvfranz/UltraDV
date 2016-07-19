//---------------------------------------------------------------------
//
//	File:	TPlaybackEngine.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.10.98
//
///	Desc:	Main playback thread that coordinates the playback of 
//			all cues
//						
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TPLAYBACKENGINE_H__
#define __TPLAYBACKENGINE_H__

// Constants
const int16 kPrerollTime 	= 999;
const int16 kPlaybackItems 	= 1000;

// Includes
#include <MediaNode.h>

// Class Definition
class TPlaybackEngine : public BMediaNode
{
	public:
		// Member Functions
		TPlaybackEngine(TCueSheetView *cueSheet);
		~TPlaybackEngine();
		
		void	Start(bigtime_t performance_time);
		void 	Stop(bigtime_t performance_time, bool immediate);
		void 	Seek(bigtime_t media_time, bigtime_t performance_time);
		void 	Preroll();
		
		void	Pause();
		void	Resume();
								
		//	BMediaNode
		status_t 	HandleMessage(int32 message, const void *data, size_t size);
		port_id 	ControlPort() const;
		BMediaAddOn	*AddOn( int32 *internal_id) const;
		
		//	Accessors
		bool			IsPlaying(){ return fIsPlaying; }
		media_node		GetTimeSource(){ return fTimeSource; }
			
	private:
		// Member Functions			
				
		//	Thread Routines
		static status_t service_routine(void *data);
		void 			ServiceRoutine();
		static status_t run_routine(void *data);
		void 			RunRoutine();

		// Member varibles
		media_node		fTimeSource;
						
		TCueSheetView	*fCueSheet;
				
		bool			fFirstTime;	// Used to track prerolls that start at an odd time
		int32			fArrayIndex;
		
		bool			fTimeToQuit;
		bool			fIsPlaying;
		bool			fIsStopping;
		
		uint32			fLastTime;
		
		port_id			fPort;
		thread_id		fServiceThread;
		thread_id		fRunThread;
		
};

#endif
