//---------------------------------------------------------------------
//
//	File:	AVIProducer.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	Header file for AVIProducer.cpp
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __AVIPRODUCER_H__
#define __AVIPRODUCER_H__

//	Includes
#include <BufferProducer.h>
#include <FileInterface.h>

#include "AVINodeTypes.h"

//	Structs
struct media_message 
{
	char whatever[B_MEDIA_MESSAGE_SIZE];
};


//	Forward Declarations
class TRunHandler;
class TRIFFReader;
class TAudioCodec;
class TVideoCodec;

//	Class Definition
class AVIProducer : public BBufferProducer, public BFileInterface
{
	public:
		AVIProducer();
		//AVIProducer(const entry_ref *fileRef);
		~AVIProducer();
		
		port_id		ControlPort() const;
		BMediaAddOn *AddOn(int32 *internal_id) const;
		void 		Preroll();
		
		status_t 	HandleMessage(int32 message, const void *data, size_t size);
				
		//	Accessors
		inline uint16 		*GetGammaTable(){ return m_GammaTable; }
						
	protected:
		//	Member Variables
		TRIFFReader			*m_Reader;
		TAudioCodec			*m_AudioCodec;
		TVideoCodec			*m_VideoCodec;

		entry_ref			m_EntryRef;
		BFile				*m_File;
		
		thread_id			m_ServiceThread;
		thread_id 			m_RunThread;
		BLocker				m_Lock;
		
		media_format		m_MediaFormat;
		port_id				m_Port;
		media_output		m_Output;
		
		bool				m_TimeToQuit;
		bool				m_Running;
		bool				m_Starting;		
		bool				m_Stopping;
		bool				m_Seeking;
		
		bigtime_t 			m_StartTime;
		bigtime_t 			m_StopTime;		
		bigtime_t 			m_MediaTime;
		bigtime_t 			m_DeltaTime;
		bigtime_t			m_SeekTime;
		bigtime_t			m_DownstreamLatency;
		bigtime_t			m_PrivateLatency;
		
		float				m_UsecPerFrame;

		BBufferProducer		*m_Producer;
		BBufferGroup		*m_BufferGroup;
		media_destination	m_Destination;

		BBuffer				*m_CurrentBuffer;
		int64				m_FramesRead;
		int32				m_ChangeTag;
		bool				m_DisableOutput;
				
		uint32				m_FrameSize;		//	Size, in bytes, of a video field
		
		//	Frame tracking
		uint32 	m_CurrentVideoFrame;
		uint32 	m_CurrentAudioFrame;
		
		//	Color Handling
		uint16		m_GammaTable[256];
	
	//	Private Methods					
	private:
		bool	Init();
		void 	InitMediaFormat();
		
		bool 	InitCodecs();
		bool 	InitVideoCodec();
		bool 	InitAudioCodec();

		float 	VisualDataRate();
		
		//	BMediaNode
		void Start(bigtime_t performance_time);
		void Stop(bigtime_t performance_time, bool immediate);				
		void Seek(bigtime_t media_time, bigtime_t performance_time);
		void SetRunMode(BMediaNode::run_mode mode);		
		void TimeWarp(bigtime_t at_real_time, bigtime_t to_performance_time);
				
		void		DisposeFileFormatCookie(int32 cookie);
		status_t 	GetNextFileFormat( int32 *cookie, media_file_format *outFormat);
		status_t 	FormatSuggestionRequested( media_type type, int32 quality, media_format *format);
		status_t 	FormatProposal( const media_source & output, media_format * format);
		status_t 	FormatChangeRequested( const media_source &source, const media_destination &destination,
										   media_format *io_format, int32 *out_change_count);

		status_t 	GetNextOutput( int32 * cookie, media_output * out_output);
		status_t 	DisposeOutputCookie(int32 cookie);
		
		void 		LateNoticeReceived( const media_source & what, bigtime_t how_much, bigtime_t performance_time);
		status_t 	GetDuration(bigtime_t *outDuration);
		
		//	BFileInterface
		status_t 	GetRef(entry_ref *outRef, char *outMimeType);
		status_t 	SetRef(const entry_ref &file, bool create, bigtime_t *outTime);
		status_t 	SniffRef( const entry_ref &file, char *outMimeType, float *outQuality);
		
		//	BBufferProducer
		status_t 	PrepareToConnect( const media_source &what, const media_destination &where,
									  media_format *format, media_source *out_source, char *out_name);
		void		Connect( status_t error,  const media_source &source,
					  		 const media_destination &destination, const media_format &format,
					   		  char *out_name);
		void 		Disconnect( const media_source &what, const media_destination &where);

		// ABH 
		// 3rd param deprecated
		void	EnableOutput(const media_source &what, bool enabled, int32* _deprecated_);

		
		status_t 	SetBufferGroup( const media_source &for_source, BBufferGroup *group);
		
		status_t 	VideoClippingChanged( 	const media_source &for_source, int16 num_shorts,
										int16 *clip_data, const media_video_display_info &display,
										int32* out_from_change_count);
										
		void 		SetTimeSource(BTimeSource * time_source);
		
		//	Thread Routines
		static status_t		service_routine(void *data);
		void				ServiceRoutine();
		
		// ABH
		static status_t		run_routine(void *data);
		void				RunRoutine();
};

#endif
