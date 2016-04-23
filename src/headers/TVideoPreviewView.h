//---------------------------------------------------------------------
//
//	File:	TVideoPreviewView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#if !defined(__TVideoPreviewView__)
#define __TVideoPreviewView__

//	Includes
#include <BufferConsumer.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <MediaNode.h>
#include "TimedBufferQueue.h"


// Class Declarations
class TVideoPreviewView: public BView, public BBufferConsumer
{
	public:
		// Member functions
		TVideoPreviewView(BRect bounds, TVideoSettingsDialog *parent);
		~TVideoPreviewView();
				
		//	BMediaNode	
		virtual	BMediaAddOn	*AddOn(long *) const;
		virtual	port_id		ControlPort() const;
		
		//	BBufferConsumer
		virtual	status_t	AcceptFormat( const media_destination &dest, media_format * format);
		virtual	status_t	GetNextInput(int32 * cookie,media_input * out_input);							
		virtual	void		DisposeInputCookie(int32 cookie);
		
		//	Implementation
		static status_t sRun( void * data);							
		void			ServiceThread();	
		static status_t	vRun(void *data);							
		void			DisplayThread();
								
		// Member variables	
	
	protected:
		//	BBufferConsumer
		virtual	void		BufferReceived(BBuffer * buffer);

	private:				
		// Member functions
		void 	Init();
		
		//	BBufferConsumer
		virtual	void		ProducerDataStatus( const media_destination &for_whom, int32 status, bigtime_t at_media_time);									
		virtual	status_t	GetLatencyFor( const media_destination &for_whom, bigtime_t * out_latency,
											media_node_id * out_id);	
		virtual	status_t	Connected( const media_source &producer, const media_destination &where,
									   const media_format & with_format, media_input * out_input);							
		virtual	void		Disconnected( const media_source &producer, const media_destination &where);							
		virtual	status_t	FormatChanged( const media_source & producer, const media_destination & consumer, 
										   int32 from_change_count, const media_format & format);
							
		//	BMediaNode	
		virtual	void		Start( bigtime_t performance_time);						
		virtual	void		Stop( bigtime_t performance_time, bool immediate);						
		virtual	void		Seek( bigtime_t media_time, bigtime_t performance_time);						
		virtual	BMediaNode::run_mode RunMode();						
		virtual	void		SetRunMode( run_mode mode);						
		virtual	void		TimeWarp( bigtime_t at_real_time, bigtime_t to_performance_time);

		// thread Routines
		int32			VideoPreview();
		static int32	video_preview(void *arg);
										
		// Member variables
		TVideoSettingsDialog	*m_Parent;
		
		sem_id				mServiceLock;
		sem_id				mBufferAvailable;
		BTimedBufferQueue	*mBufferQueue;
		thread_id			mThread;
		thread_id			vThread;
		port_id				mPort;
		media_destination	mDestination;	/* only one?! */
		media_source		mProducer;		// producer sending us data
		run_mode			mRunMode;
		
		uint32				mXSize;
		uint32				mYSize;
		uint32				mRowBytes;
		color_space			mColorspace;
		
		double 				fps;
		int32 				frameBytes;	// bytes/frame
		
		volatile bool		mControlQuit;
		volatile bool		mDisplayQuit;
		
		bool				mConnected;
		bool 				mRunning;
		bool 				mStarting;
		bool 				mStopping;
		bool				mSeeking;
		
		bigtime_t 			mStartTime;
		bigtime_t 			mStopTime;
		bigtime_t 			mSeekTime;
		bigtime_t 			mMediaTime;
		bigtime_t 			mDeltaTime;
		
		bigtime_t 			mLast;
		bigtime_t 			mDownstreamLatency;
		bigtime_t 			mMyLatency;
		
		BBitmap				*m_Bitmap;		
};		

#endif
