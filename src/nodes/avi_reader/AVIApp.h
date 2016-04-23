//---------------------------------------------------------------------
//
//	File:	AVIApp.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	AVI Application
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __AVIAPP_H__
#define __AVIAPP_H__

#include <BufferConsumer.h>
#include <MediaRoster.h>
#include <TimeSource.h>

// ABH added
#include "AVIProducer.h"
#include "TimedBufferQueue.h"

//	Class Definition
class AVIApp : public BApplication, public BBufferConsumer
{
	public:
		AVIApp();
		~AVIApp();
		
		bool	QuitRequested();		
		void	 MessageReceived(BMessage* message);
		status_t HandleMessage(int32 message, const void *data, size_t size);
		void 	 RefsReceived(BMessage *message);

		//	MediaNode
		port_id 	ControlPort() const;
		BMediaAddOn	*AddOn(int32 * internal_id) const;
		
	private:
		status_t			InitNodes(entry_ref &fileRef);
		void 				ShowFileOpenPanel();
		
		BMediaRoster 		*m_MediaRoster;
		
		// File Panels				
		BFilePanel			*m_FileOpenPanel;
		BWindow				*m_AVIWindow;
		BView				*m_AVIView;		
		BBitmap				*m_Bitmap;
		
		AVIProducer			*m_AVIProducer;
		media_node			m_TimeSourceNode;
		media_format		m_MediaFormat;
		
		port_id				m_Port;
		thread_id			m_ServiceThread;
		thread_id			m_RunThread;
		BLocker				m_Lock;
		
		media_output 		m_From;
		media_input 		m_To;

		media_destination	m_Destination;
		media_source		m_Producer;	
		media_format		m_ConnectedFormat;
		
		uint32				m_FrameSize;		//	Size, in bytes, of a video field
		uint32				m_FrameAmountRead;	//	Size, in bytes, of a video field data transfered		
		
		uint32				m_FrameCount;
		
		bool				m_TimeToQuit;		
		bool 				m_Connected;
		bool 				m_Running;
		bool				m_Starting;
		bool				m_Stopping;
		bool				m_Seeking;
		
		bigtime_t 			m_StartTime;
		bigtime_t 			m_StopTime;
		bigtime_t			m_SeekTime;
		bigtime_t			m_MediaTime;
		bigtime_t			m_DeltaTime;

		bigtime_t 			m_DownstreamLatency;
		bigtime_t 			m_PrivateLatency;
		
		//	Buffer stack
		BTimedBufferQueue	*m_BufferQueue;
		

		
	protected:		
		//	BufferConsumer
		status_t 	AcceptFormat(const media_destination &dest, media_format *format);
		status_t 	GetNextInput( int32 *cookie, media_input *out_input);
		void 		DisposeInputCookie(int32 cookie);
		void 		BufferReceived(BBuffer * buffer);
		void 		ProducerDataStatus( const media_destination &for_whom, int32 status, bigtime_t at_media_time);
		status_t 	GetLatencyFor(const media_destination &for_whom, bigtime_t *out_latency, media_node_id *out_timesource);
		status_t 	Connected(  const media_source &producer, const media_destination &where,
								const media_format &with_format, media_input *out_input);
		void 		Disconnected( const media_source & producer, const media_destination & where);
		status_t 	FormatChanged( const media_source & producer, const media_destination & consumer, 
								int32 from_change_count, const media_format & format);
								
		void 		Start(bigtime_t performance_time);
		void 		Stop(bigtime_t performance_time, bool immediate);				
		void 		Seek(bigtime_t media_time, bigtime_t performance_time);
		void 		SetRunMode(run_mode mode);		
		void 		TimeWarp(bigtime_t at_real_time, bigtime_t to_performance_time);
		
		//	Thread Routines
		static status_t		service_routine(void *data);
		void				ServiceRoutine();
		static status_t		run_routine(void *data);
		void				RunRoutine();
};

#endif
