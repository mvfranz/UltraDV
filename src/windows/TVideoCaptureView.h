/*	TVideoCaptureView.h	*/

#if !defined(VID_CONSUMER_H)
#define VID_CONSUMER_H

#include <BufferConsumer.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <MediaNode.h>


#include "TimedBufferQueue.h"

#define MAX_CONNECTIONS 16

class TVideoCaptureWindow;


class TVideoCaptureView : public BView, public BBufferConsumer
{
public:
		TVideoCaptureView(BRect bounds, TVideoCaptureWindow *parent);
		~TVideoCaptureView();
	
/*	BMediaNode */
public:
	
	virtual	BMediaAddOn	*AddOn(long *) const;
	virtual	port_id		ControlPort() const;
	
private:
	
	virtual	void		Start(
							bigtime_t performance_time);						
	virtual	void		Stop(
							bigtime_t performance_time,
							bool immediate);						
	virtual	void		Seek(
							bigtime_t media_time,
							bigtime_t performance_time);						
	virtual	BMediaNode::run_mode
						RunMode();						
	virtual	void		SetRunMode(
							run_mode mode);						
	virtual	void		TimeWarp(
							bigtime_t at_real_time,
							bigtime_t to_performance_time);
	
/*	BBufferConsumer */
public:
	
	virtual	status_t	AcceptFormat(
							const media_destination &dest,
							media_format * format);
	virtual	status_t	GetNextInput(
							int32 * cookie,
							media_input * out_input);
							
	virtual	void		DisposeInputCookie(
							int32 cookie);
	
protected:

	virtual	void		BufferReceived(
							BBuffer * buffer);
	
private:

	virtual	void		ProducerDataStatus(
							const media_destination &for_whom,
							int32 status, bigtime_t at_media_time);									
	virtual	status_t	GetLatencyFor(
							const media_destination &for_whom,
							bigtime_t * out_latency,
							media_node_id * out_id);	
	virtual	status_t	Connected(
							const media_source &producer,
							const media_destination &where,
							const media_format & with_format,
							media_input * out_input);							
	virtual	void		Disconnected(
							const media_source &producer,
							const media_destination &where);							
	virtual	status_t	FormatChanged(
							const media_source & producer,
							const media_destination & consumer, 
							int32 from_change_count,
							const media_format & format);
							
/*	implementation */

public:

	static status_t		sRun(
							void * data);							
	void				ServiceThread(
							void);	
	static status_t		vRun(
							void *data);							
	void				DisplayThread(
							void);

private:

	sem_id				mServiceLock;
	sem_id				mBufferAvailable;

	thread_id			mServiceThread;
	thread_id			mDisplayThread;
	port_id				mPort;
	run_mode			mRunMode;

	volatile bool		mControlQuit;
	volatile bool		mDisplayQuit;
	
	bool 				mRunning;
	bool 				mStarting;
	bool 				mStopping;
	bool				mSeeking;
	
	bigtime_t 			mStartTime;
	bigtime_t 			mStopTime;
	bigtime_t 			mSeekTime;
	bigtime_t 			mMediaTime;
	bigtime_t 			mDeltaTime;
	
	bigtime_t 			mDownstreamLatency;
	bigtime_t 			mMyLatency;
	
	BTimedBufferQueue		*mBufferQueue;

	uint32					mConnectionCount;
	bool					mConnectionActive[MAX_CONNECTIONS];
	media_destination		mDestination[MAX_CONNECTIONS];
	media_raw_video_format	mFormat[MAX_CONNECTIONS];
	BBitmap					*mBitmap[MAX_CONNECTIONS];		
};


#endif /* MY_CONSUMER_H */
