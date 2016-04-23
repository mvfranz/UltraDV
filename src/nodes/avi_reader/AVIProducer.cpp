//---------------------------------------------------------------------
//
//	File:	AVIProducer.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.25.98
//
//	Desc:	AVIReaderNode
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


//	Includes
#include "BuildApp.h"

#include <OS.h>
#include <Debug.h>
#include <Buffer.h>
#include <BufferGroup.h>
#include <TimeSource.h>
#include <MediaAddOn.h>

#include <stdio.h>
#include <string.h>

#include "AppAlerts.h"
#include "DebugUtils.h"

#include "TRIFFReader.h"

#include "TVideoCodec.h"
#include "TAppleVideoCodec.h"
#include "TCinepakCodec.h"
#include "TMSRLECodec.h"
#include "TMSVideoCodec.h"
#include "TRGBCodec.h"

#include "TAudioCodec.h"
#include "TPCMCodec.h"

#include "AVIUtils.h"

#include "AVIProducer.h"
#ifndef ErrorAlert
#define ErrorAlert(x) printf(x)
#endif

//	Defines
#define DEFAULT_TIMEOUT 6000000L
#define ERROR(EXP) { \
	status_t err = (EXP); \
	if (err < 0) \
	fprintf (stderr, "AVIProducer: " #EXP " == 0x%x (%s)\n", err, strerror(err)); \
// ABH
using namespace std;


//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//	Defualt Contructor
//

AVIProducer::AVIProducer() 
	: 	BMediaNode("AVIProducer"), 
		BBufferProducer(B_MEDIA_MULTISTREAM), 
		BFileInterface()
{
	//	Init variables
	m_Reader		= NULL;
	m_File			= NULL;
	m_Port			= NULL;
	m_ServiceThread	= NULL;
	m_RunThread		= NULL;

	//	Default initialization
	bool retVal = Init();
}

//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

AVIProducer::~AVIProducer()
{
	
	//	Signal threads to quit
	m_TimeToQuit = true;
	
	//	Quit service thread
	if (write_port_etc(m_Port, 0x60000000, NULL, 0, B_TIMEOUT, DEFAULT_TIMEOUT))
		kill_thread(m_ServiceThread);
	
	status_t result;
	wait_for_thread(m_ServiceThread, &result);

	//	Run thread
	wait_for_thread(m_RunThread, &result);		

	//	RIFFReader
	if (m_Reader)
		delete m_Reader;
		
	if (m_File)
		delete m_File;		
}


#pragma mark -
#pragma mark === Initialization Routines ===

//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//	Default initialization
//

bool AVIProducer::Init()
{
	//	Set up defaults
	m_Destination 	= media_destination::null;
	m_CurrentBuffer = 0;
	m_ChangeTag 	= 0;
	m_FrameSize		= 0;
	m_TimeToQuit	= false;
	m_Running 		= false;
	m_Starting 		= false;
	m_Stopping 		= false;
	m_Seeking 		= false;
	m_StartTime 	= 0;		//	When to start in performance time
	m_StopTime 		= 0;		//	When to stop in performance time
	m_MediaTime		= 0;
	m_DeltaTime		= 0;
	m_SeekTime		= 0;
	
	m_DownstreamLatency = 0;
	m_PrivateLatency	= 0;

	//	Init current frames
	m_CurrentVideoFrame = 0;
	m_CurrentAudioFrame = 0;
	
	//	Create our port
	m_Port = create_port(60, "AVIReaderNodePort");
			
	//	Set up identifying media_format structure
	InitMediaFormat();

	//	Create port service thread
	m_ServiceThread = spawn_thread(service_routine, "AVIProducer:Service", B_NORMAL_PRIORITY, this);
	resume_thread(m_ServiceThread);	
	
	//	Create producer thread
	int runPrio = suggest_thread_priority(B_VIDEO_PLAYBACK, 30, 1000, 1000);
	m_RunThread = spawn_thread(run_routine, "AVIProducer::Run", runPrio, this);	
	resume_thread(m_RunThread);
	
	m_PrivateLatency = estimate_max_scheduling_latency(find_thread(NULL));
	
	return true;	
}


//-------------------------------------------------------------------
//	InitMediaFormat
//-------------------------------------------------------------------
//
//	Init media_format info
//


void AVIProducer::InitMediaFormat()
{
	//	Set up identifying media_format structure
	m_MediaFormat.type = B_MEDIA_MULTISTREAM;
	
	//	Init from attached file
	if (m_Reader){		
		m_MediaFormat.u.multistream.avg_bit_rate 	= m_Reader->GetAVIHeader()->MaximumDataRate;	
		m_MediaFormat.u.multistream.max_bit_rate 	= m_Reader->GetAVIHeader()->MaximumDataRate;
		m_MediaFormat.u.multistream.avg_chunk_size 	= m_Reader->Width() * m_Reader->Height() * 8;
		m_MediaFormat.u.multistream.max_chunk_size 	= m_Reader->Width() * m_Reader->Height() * 8;
		m_MediaFormat.u.multistream.flags			= m_MediaFormat.u.multistream.B_HEADER_HAS_FLAGS | m_MediaFormat.u.multistream.B_CLEAN_BUFFERS | m_MediaFormat.u.multistream.B_HOMOGENOUS_BUFFERS;
		m_MediaFormat.u.multistream.format 			= m_MediaFormat.u.multistream.B_AVI;
			
		m_MediaFormat.u.multistream.u.avi.us_per_frame 	= m_Reader->UsPerFrame();
		m_MediaFormat.u.multistream.u.avi.width 		= m_Reader->Width();
		m_MediaFormat.u.multistream.u.avi.height 		= m_Reader->Height();
	}
	//	Set to bogus defaults
	else {
		m_MediaFormat.u.multistream.avg_bit_rate 	= 0;	
		m_MediaFormat.u.multistream.max_bit_rate 	= 0;
		m_MediaFormat.u.multistream.avg_chunk_size 	= 0;
		m_MediaFormat.u.multistream.max_chunk_size 	= 0;
		m_MediaFormat.u.multistream.flags			= m_MediaFormat.u.multistream.B_HEADER_HAS_FLAGS | m_MediaFormat.u.multistream.B_CLEAN_BUFFERS | m_MediaFormat.u.multistream.B_HOMOGENOUS_BUFFERS;
		m_MediaFormat.u.multistream.format 			= m_MediaFormat.u.multistream.B_AVI;
			
		m_MediaFormat.u.multistream.u.avi.us_per_frame 	= 0;	
		m_MediaFormat.u.multistream.u.avi.width 		= 0;
		m_MediaFormat.u.multistream.u.avi.height 		= 0;		
	}
	
	//	Types we love...
	m_MediaFormat.u.multistream.u.avi.type_count 	= 4;	
	m_MediaFormat.u.multistream.u.avi.types[0] 		= B_MEDIA_RAW_AUDIO;
	m_MediaFormat.u.multistream.u.avi.types[1] 		= B_MEDIA_RAW_VIDEO;
	m_MediaFormat.u.multistream.u.avi.types[2] 		= B_MEDIA_ENCODED_AUDIO;
	m_MediaFormat.u.multistream.u.avi.types[3] 		= B_MEDIA_ENCODED_VIDEO;
	
	//	Calculate size of visual field in bytes (32 bit)
	m_FrameSize = (m_MediaFormat.u.multistream.u.avi.width * m_MediaFormat.u.multistream.u.avi.height) * 8;
		
	//	Internal UsPerFrame
	m_UsecPerFrame = m_MediaFormat.u.multistream.u.avi.us_per_frame;

	//	Set up output
	m_Output.node 		 	= Node();
	m_Output.source.port 	= m_Port;
	m_Output.source.id 		= 0;
	m_Output.format 		= m_MediaFormat;
	sprintf(m_Output.name, "AVIProducer");
}


//---------------------------------------------------------------------
//	InitCodecs
//---------------------------------------------------------------------
//
//	Determine compression and initialize codec
//

bool AVIProducer::InitCodecs()
{
	bool retVal = false;
	
	// ABH need to check BOTH return values!
	retVal = InitAudioCodec();
	retVal = InitVideoCodec();
	
	return retVal;
}


//---------------------------------------------------------------------
//	InitVideoCodec
//---------------------------------------------------------------------
//
//	Determine video compression and initializae codec
//

bool AVIProducer::InitVideoCodec()
{
	if (m_Reader->HasVideo() == false)
		return false;
	
	m_VideoCodec = NULL;
	
	//	Get video header to determine compression type
	AVIVIDSHeader *vidsHeader = m_Reader->GetVIDSHeader();
		
	//	Determine compression and create codec
	switch(vidsHeader->Compression)
	{
		//	Uncompressed RGB
		case RIFF_RGB:  
    	case RIFF_rgb:
			m_VideoCodec = new TRGBCodec(m_Reader);
    		break;
    
		//	Apple Video "Road Pizza"
		case RIFF_rpza:
		case RIFF_azpr:
			m_VideoCodec = new TAppleVideoCodec(m_Reader);
			break;
					
		//	Microsoft Video 01
		case RIFF_wham:
		case RIFF_WHAM:
		case RIFF_cram:
		case RIFF_CRAM:
		case kRiff_msvc_Chunk:
		case kRiff_MSVC_Chunk:
			m_VideoCodec = new TMSVideoCodec(m_Reader);
			break;

		//	Microsoft RLE
		case RIFF_RLE8: 
    	case RIFF_rle8: 
			m_VideoCodec = new TMSRLECodec(m_Reader);
			break;
			
		//	Radius Cinepak "Compact Video"
		case RIFF_cvid: 
    	case RIFF_CVID: 
			m_VideoCodec = new TCinepakCodec(m_Reader);
			break;

		default:
			ErrorAlert("Unhandled CODEC.");
			
			#ifdef DEBUG
				printf("Unhandled CODEC: ");
				DumpRIFFID(vidsHeader->Compression);				
			#endif
			return false;
			break;		
	}
	
	if (m_VideoCodec)
		return true;
	else
		return false;	
}


//---------------------------------------------------------------------
//	InitAudioCodec
//---------------------------------------------------------------------
//
//	Determine audio compression and initializae codec
//

bool AVIProducer::InitAudioCodec()
{
	if (m_Reader->HasAudio() == false)
		return false;
	
	m_AudioCodec = NULL;
	
	//	Get video header to determine compression type
	AVIAUDSHeader *audsHeader = m_Reader->GetAUDSHeader();
		
	//	Determine compression and create codec
	switch(audsHeader->Format)
	{
		case WAVE_FORMAT_PCM:
			m_AudioCodec = new TPCMCodec(m_Reader);
			break;
			
		/*
		case WAVE_FORMAT_ADPCM:
			{
				switch(audsHeader.Size)
				{
					case 4:
						m_AudioType = kAudioADPCM;
						break;
						
					default:
						m_AudioType = kAudioInvalid;
						break;									
				}
			}
			break;
			
		case WAVE_FORMAT_DVI_ADPCM:
			{
				m_AudioType = kAudioDVI;
			}
			break;
	
		case WAVE_FORMAT_MULAW:
			{
				m_AudioType = kAudioULaw;
			}
			break;
			
		case WAVE_FORMAT_GSM610:
			{
				m_AudioType = kAudioMSGSM;
			}
			break;*/
			
		default:
			{
				ErrorAlert("Unhandled Audio CODEC.");
				
				#ifdef DEBUG
					printf("Unhandled Audio CODEC: ");
				#endif
				return false;
			}
			break;		
	}
	
	if (m_AudioCodec)
		return true;
	else
		return false;	
}


#pragma mark -
#pragma mark === Node Control ===

//-------------------------------------------------------------------
//	Start
//-------------------------------------------------------------------
//
//	Begin node execution
//

void AVIProducer::Start(bigtime_t performance_time)
{
	BAutolock lock(m_Lock);
	
	if (!m_Stopping || performance_time > m_StopTime)
	{
		if (!m_Running || m_Stopping)
		{
			m_Starting 		= true;
			m_StartTime 	= performance_time;
			m_DisableOutput = false;
		}
	}
}


//-------------------------------------------------------------------
//	Stop
//-------------------------------------------------------------------
//
//	Stop node execution
//

void AVIProducer::Stop(bigtime_t performance_time, bool immediate)
{
	BAutolock lock(m_Lock);
	
	if (!m_Starting || performance_time > m_StartTime){
		if (m_Running || m_Starting){
			m_Stopping = true;
			m_StopTime = performance_time;							
		}
	}
	
	if (immediate){
		m_Running = false;
	}
}


//-------------------------------------------------------------------
//	Seek
//-------------------------------------------------------------------
//
//	Seet to specified time
//

void AVIProducer::Seek(bigtime_t media_time, bigtime_t performance_time)
{
	printf("AVIProducer::Seek\n");
	
	BAutolock lock(m_Lock);
	
	m_MediaTime	= media_time;
	m_SeekTime 	= performance_time;
	m_Seeking 	= true;	
}


//-------------------------------------------------------------------
//	SetRunMode
//-------------------------------------------------------------------
//
//

void AVIProducer::SetRunMode(BMediaNode::run_mode mode)
{
	printf("AVIProducer::SetRunMode\n");
	BMediaNode::SetRunMode(mode);
}


//-------------------------------------------------------------------
//	TimeWarp
//-------------------------------------------------------------------
//
//

void AVIProducer::TimeWarp(bigtime_t at_real_time, bigtime_t to_performance_time)
{
	printf("AVIProducer::TimeWarp\n");
	
	BBufferProducer::TimeWarp(at_real_time, to_performance_time);
	BMediaNode::TimeWarp(at_real_time, to_performance_time);
}


//-------------------------------------------------------------------
//	DisposeFileFormatCookie
//-------------------------------------------------------------------
//
//

void AVIProducer::DisposeFileFormatCookie(int32 cookie)
{
}


#pragma mark -
#pragma mark === Node Control ===

//-------------------------------------------------------------------
//	ControlPort
//-------------------------------------------------------------------
//
//	Return node's control port
//

port_id AVIProducer::ControlPort() const
{
	return m_Port;
}


//-------------------------------------------------------------------
//	AddOn
//-------------------------------------------------------------------
//
//	Return AddOn that instantiated us
//

BMediaAddOn *AVIProducer::AddOn(int32 *internal_id) const
{
	return NULL;
}

//-------------------------------------------------------------------
//	Preroll
//-------------------------------------------------------------------
//
//	Prepare data for playback
//

void AVIProducer::Preroll()
{
	BMediaNode::Preroll();
}


#pragma mark -
#pragma mark === Format Handling ===

//-------------------------------------------------------------------
//	GetNextFileFormat
//-------------------------------------------------------------------
//
//	Return info about the file format that we support
//

status_t AVIProducer::GetNextFileFormat( int32 *cookie, media_file_format *outFormat)
{
	//	We only handle one format
	if (*cookie > 1)
		return B_ERROR;
		
	//	Increment cookie
	*cookie++;
	
	//	File format structue
	
	// ABH was B_KNOWS_VIDEO, B_KNOWS_AUDIO
//	outFormat->flags = outFormat->B_READABLE|outFormat->B_WRITABLE|outFormat->B_KNOWS_ENCODED_VIDEO|
//					   outFormat->B_KNOWS_ENCODED_AUDIO|outFormat->B_PERFECTLY_SEEKABLE;	
	
	outFormat->capabilities = outFormat->B_READABLE|outFormat->B_WRITABLE|outFormat->B_KNOWS_ENCODED_VIDEO|
					   outFormat->B_KNOWS_ENCODED_AUDIO|outFormat->B_PERFECTLY_SEEKABLE;	
	strcpy( outFormat->mime_type, "video/avi");

	return B_OK;
}


//-------------------------------------------------------------------
//	FormatSuggestionRequested
//-------------------------------------------------------------------
//
//

status_t AVIProducer::FormatSuggestionRequested( media_type type, int32 quality, media_format *format)
{
	//	Handle wildcard
	if (type == B_MEDIA_NO_TYPE){
		type = B_MEDIA_MULTISTREAM;
	}
	
	//	Can we handle requested type
	if (type == B_MEDIA_MULTISTREAM){	
		//	Set up format to what we can deliver
		*format = m_MediaFormat;				
	}
	else
		return B_MEDIA_BAD_FORMAT;
		
	quality = quality;
	
	return B_OK;
		
}


//-------------------------------------------------------------------
//	FormatProposal
//-------------------------------------------------------------------
//
//	Determine if we can handle requested format
//

status_t AVIProducer::FormatProposal( const media_source &output, media_format *format)
{
	//	Check for wildcards
	if (format->type == 0){
		//	Set to our preferred type
		format->type = B_MEDIA_MULTISTREAM;
		
		return B_OK;	
	}
	
	//	We only handle AVI video
	switch(format->type){
		case B_MEDIA_MULTISTREAM:{
				//	Set up format
				
				/*
					struct media_encoded_video_format 
					{
						enum video_encoding {
							B_ANY = 0,
							B_CINEPAK = 200,
							B_INDEO,
							B_DV,
							B_MPEG1,
							B_MPEG2,
							B_PRIVATE = 90000,
							B_FIRST_USER_TYPE = 100000
						};
						media_raw_video_format	output;	// set unknowns to wildcard
						float			avg_bit_rate;
						float			max_bit_rate;
						video_encoding	encoding;
						size_t		frame_size;
		
						static media_encoded_video_format wildcard;
					};
					*/
			
				return B_OK;
			}
			
		default:
			return B_MEDIA_BAD_FORMAT;		
		
	}
}


//-------------------------------------------------------------------
//	FormatChangeRequested
//-------------------------------------------------------------------
//
//	Determine if we can handle requested format
//

status_t AVIProducer::FormatChangeRequested( const media_source &source, const media_destination &destination,
										 media_format *io_format, int32 *out_change_count)
{
	return B_ERROR;
}										


#pragma mark -
#pragma mark === Output Handling ===

//-------------------------------------------------------------------
//	GetNextOutput
//-------------------------------------------------------------------
//
//

status_t AVIProducer::GetNextOutput(int32 *cookie, media_output *out_output)
{
	if (*cookie == 0){
		*out_output = m_Output;
		*cookie 	= 1;
		return B_OK;
	}
	else {
		return B_BAD_INDEX;
	}
}

//-------------------------------------------------------------------
//	DisposeOutputCookie
//-------------------------------------------------------------------
//
//

status_t AVIProducer::DisposeOutputCookie(int32 cookie)
{
	return B_OK;
}


#pragma mark -
#pragma mark === Duration Handling ===

//-------------------------------------------------------------------
//	GetDuration
//-------------------------------------------------------------------
//
//	Return duration of media in microseconds
//

status_t AVIProducer::GetDuration(bigtime_t *outDuration)
{
	return 0;
}


#pragma mark -
#pragma mark === Message Handling ===

//-------------------------------------------------------------------
//	HandleMessage
//-------------------------------------------------------------------
//
//

status_t AVIProducer::HandleMessage(int32 message, const void *data, size_t size)
{
	switch(message)
	{
				
		//	Call derived classes
		default:
			{
				if ( (BBufferProducer::HandleMessage(message, data, size)) && 
					 (BFileInterface::HandleMessage(message, data, size))  &&
					 (BMediaNode::HandleMessage(message, data, size)) )
				{				
					//printf("AVIProducer::HandleBadMessage\n");
					BMediaNode::HandleBadMessage(message, data, size); 
				}
			}
			break;
	}
	
	return B_OK;	
}

//-------------------------------------------------------------------
//	LateNoticeReceived
//-------------------------------------------------------------------
//
//

void AVIProducer::LateNoticeReceived( const media_source & what, bigtime_t how_much, bigtime_t performance_time)
{

}


#pragma mark -
#pragma mark === Ref Handling ===

//-------------------------------------------------------------------
//	GetRef
//-------------------------------------------------------------------
//
//	Return m_EntryRef and file's mime type
//

status_t AVIProducer::GetRef(entry_ref *outRef, char *outMimeType)
{
	//	Set ref
	outRef = &m_EntryRef;
	
	//	Set mime type
	strcpy(outMimeType, "video/avi");
	
	return B_OK;
}


//-------------------------------------------------------------------
//	SetRef
//-------------------------------------------------------------------
//
//	Set m_EntryRef
//

status_t AVIProducer::SetRef(const entry_ref &fileRef, bool create, bigtime_t *outTime)
{	
	status_t retVal = B_OK;
	
	//	We don't handle writing yet
	if (create)
		return B_ERROR;
		
	//	Set ref
	m_EntryRef = fileRef;
	
	//	Set up file
	if (m_File)
		delete m_File;
	
	m_File = new BFile(&fileRef, B_READ_ONLY);
	if (m_File->InitCheck() == B_ERROR)
		return B_ERROR;
	
	//	Set up RIFFReader
	if (m_Reader)
		delete m_Reader;
			
	m_Reader = new TRIFFReader(m_File);
	if (!m_Reader)
		return B_ERROR;
				
	//	Reinit media_format info
	InitMediaFormat();
	
	//	Create audio and video codec
	retVal = InitCodecs();
	// ABH unrecognized or bad Video or audio codec
	if (!retVal) {
		printf("AVIProd::SetRef bad video or audio\n");
		return B_ERROR;
// ABH		be_app->PostMessage(B_QUIT_REQUESTED);
		return retVal;
	}
	
	//	Get playback rate (frames per second)
	float rate;	
	if (m_Reader->HasVideo()){		
		//	Find video stream
		int32 streams = m_Reader->GetStreamCount();
		
		if (streams == 1){
			AVIStreamHeader *header = m_Reader->GetStreamHeaderOne();
			rate = header->DataRate;
		}
		else {
			AVIStreamHeader *header;			
			header = m_Reader->GetStreamHeaderOne();
			if (header->DataType == kRiff_vids_Chunk){
				rate = header->DataRate;
			}
			else {
				header = m_Reader->GetStreamHeaderTwo();
				rate = header->DataRate;
			}							
		}		
	}
	else {
		printf("No video data!\n");
		be_app->PostMessage(B_QUIT_REQUESTED);
		return false;
	}
		
	return retVal;
}


//-------------------------------------------------------------------
//	SniffRef
//-------------------------------------------------------------------
//
//	Examine entry_ref and determine if we can
//	handle file of this type.  Set outMimeType to MIME type
//	of file handled.
//

status_t AVIProducer::SniffRef( const entry_ref &file, char *outMimeType, float *outQuality)
{
	status_t retVal = B_MEDIA_NO_HANDLER;
		
	// Create a file from entry_ref
	BFile *theFile = new BFile(&file, B_READ_ONLY);
	
	if (theFile->InitCheck() == B_NO_ERROR){
		if ( IsRIFFFile(theFile)){
			//	We like this file
			retVal = B_OK;
			
			//	Set the MIME type
			strcpy(outMimeType, "video/x-msvideo");
			
			//	Set quality and ID
			*outQuality = 1.0;
		}
	}
	
	//	Clean up 
	delete theFile;
	
	//	Return result
	return retVal;
}


#pragma mark -
#pragma mark === Node Connection ===


//-------------------------------------------------------------------
//	PrepareToConnect
//-------------------------------------------------------------------
//
//

status_t AVIProducer::PrepareToConnect( const media_source &what, const media_destination &where,
									media_format *format, media_source *out_source, char *out_name)
{
	status_t err;
	
	// ABH cannot call protected/virtual method!
	// err = BBufferProducer::PrepareToConnect(what, where, format, out_source, out_name);
	
	err = FormatProposal(what, format);
	if (err != B_OK)
		return err;
		
	if (what.port != m_Port || what.id != 0)
		return B_MEDIA_BAD_SOURCE;
		
// ABH	ASSERT(mConnection == media_destination::null);
	
	
	if (where == media_destination::null)
		return B_MEDIA_BAD_DESTINATION;
		
	m_Output.destination = where;

	media_source source(m_Port, what.id);
	*out_source = source;
	
	//	Update size of visual field in bytes (32 bit)
	m_FrameSize = (format->u.multistream.u.avi.width * format->u.multistream.u.avi.height) * sizeof(uint32);

	//	Create a buffer group to copy image data into
	if (m_BufferGroup == NULL)
	{
		m_BufferGroup 	= new BBufferGroup(m_FrameSize, 6, B_ANY_ADDRESS);
		status_t status = m_BufferGroup->InitCheck();
		if (B_OK != status)
			return B_ERROR;
	}
			
	return B_OK;
}

//-------------------------------------------------------------------
//	Connect
//-------------------------------------------------------------------
//
//

void AVIProducer::Connect( status_t error,  const media_source &source,
					   const media_destination &destination, const media_format &format,
					   char *out_name)
{	
	// ABH cannot call protected/virtual method!
	// BBufferProducer::Connect(error, source, destination, format, out_name);

	if (error < B_OK)
		return;

	BAutolock lock(m_Lock);
		
	m_Output.destination 	= destination;
	m_Output.format 		= format;
	BBufferProducer::GetLatency(&m_DownstreamLatency);	
	strcpy(out_name, Name());
}


//-------------------------------------------------------------------
//	Disconnect
//-------------------------------------------------------------------
//
//

void AVIProducer::Disconnect( const media_source &what, const media_destination &where)
{
	
	if (what != m_Output.source || where != m_Output.destination)
	{
		printf("AVIProducer::Disconnect() - BAD CONNECTION\n");
		return;
	}
		
	BAutolock lock(m_Lock);
		
	//	Close connection
	m_Output.destination = media_destination::null;
		
	//	Stop using the buffer group
	delete m_BufferGroup;
	m_BufferGroup = NULL;
	
	return;
}


//-------------------------------------------------------------------
//	EnableOutput
//-------------------------------------------------------------------
//
//


// ABH 3rd param now deprecated

void AVIProducer::EnableOutput(const media_source &source, bool enabled, int32* _deprecated_){
	
	if (source != m_Output.source)
		return;
		
	m_DisableOutput = !enabled;
	
}

/*
void AVIProducer::EnableOutput(const media_source& what, bool enabled, int32* change_tag){
	if (what != m_Output.source)
		return;
		
	m_DisableOutput = !enabled;
// ABH	*change_tag = IncrementChangeTag();
	
	if (m_Output.destination != media_destination::null){
		SendDataStatus( m_DisableOutput ? B_DATA_NOT_AVAILABLE : B_DATA_AVAILABLE, 
						m_Output.destination, TimeSource()->Now());
	}
}
*/

#pragma mark -
#pragma mark === Buffer Handling ===

//-------------------------------------------------------------------
//	SetBufferGroup
//-------------------------------------------------------------------
//
//

status_t AVIProducer::SetBufferGroup( const media_source &for_source, BBufferGroup *group)
{
	/*
	if ( for_source.port != m_Port || for_source.id || m_Connection == media_destination::null)
	{
		return B_MEDIA_BAD_SOURCE;
	}
	
	if (group != NULL)
		m_BufferGroup = group;
	*/
	return B_ERROR;
}


#pragma mark -
#pragma mark === Clipping ===

//-------------------------------------------------------------------
//	VideoClippingChanged
//-------------------------------------------------------------------
//
//

status_t AVIProducer::VideoClippingChanged( const media_source &for_source, int16 num_shorts,
										int16 *clip_data, const media_video_display_info &display,
										int32* out_from_change_count)
{
	return B_ERROR;
}									


#pragma mark -
#pragma mark === Timing ===

//-------------------------------------------------------------------
//	SetTimeSource
//-------------------------------------------------------------------
//
//

void AVIProducer::SetTimeSource(BTimeSource *time_source)
{
	BMediaNode::SetTimeSource(time_source);
}									


//-------------------------------------------------------------------
//	VisualDataRate
//-------------------------------------------------------------------
//
//	Get node's visual data rate
//

float AVIProducer::VisualDataRate()
{

	//	Get playback rate (frames per second)
	float rate = 0;	
	if (m_Reader->HasVideo()){		
		//	Find video stream
		int32 streams = m_Reader->GetStreamCount();
		
		if (streams == 1){
			AVIStreamHeader *header = m_Reader->GetStreamHeaderOne();
			rate = header->DataRate;
		}
		else {
			AVIStreamHeader *header;			
			header = m_Reader->GetStreamHeaderOne();
			if (header->DataType == kRiff_vids_Chunk){
				rate = header->DataRate;
			}
			else {
				header = m_Reader->GetStreamHeaderTwo();
				rate = header->DataRate;
			}
		}							
	}
	
	return rate;
}


#pragma mark -
#pragma mark === Thread Functions ===

//-------------------------------------------------------------------
//	service_routine
//-------------------------------------------------------------------
//
//	Static service thread function
//

status_t AVIProducer::service_routine(void * data)
{
	((AVIProducer *)data)->ServiceRoutine();
	
	return 0;
}


//-------------------------------------------------------------------
//	ServiceRoutine
//-------------------------------------------------------------------
//
//	Service thread function
//

void AVIProducer::ServiceRoutine()
{
	while (!m_TimeToQuit){
		//	Read message
		status_t 		err  = 0;
		int32 			code = 0;
		media_message	msg;
		
		err = read_port_etc(m_Port, &code, &msg, sizeof(msg), B_TIMEOUT, 10000);		
		
		if (err == B_TIMED_OUT) 
			continue;
		
		if (err < B_OK){
			printf("AVIProducer::ServiceRoutine: Unexpected error in read_port(): %x\n", err);
			continue;
		}
		
		// dispatch message
		if (code == 0x60000000)
			break;		
		
		HandleMessage(code, &msg, err);
	}
}


//-------------------------------------------------------------------
//	run_routine
//-------------------------------------------------------------------
//
//	Static run thread function
//

status_t AVIProducer::run_routine(void *data)
{
	((AVIProducer *)data)->RunRoutine();
	
	return 0;
}



//-------------------------------------------------------------------
//	RunRoutine
//-------------------------------------------------------------------
//
//	Run thread function
//

void AVIProducer::RunRoutine()
{
	m_PrivateLatency = estimate_max_scheduling_latency(find_thread(NULL));
	
	while(!m_TimeToQuit){
		snooze(5000);
		
		//	Check and see if we are connected to anyone
		bool connected = (m_Output.destination != media_destination::null);
		
		bool timeSourceRunning = TimeSource()->IsRunning();
		
		if (timeSourceRunning){
			//	If there are no connections, then do nothing
			if (!connected)
				continue;
			
			//	Bail out if there's no buffer group set up
			if (m_BufferGroup == NULL)
				continue;
			
			bigtime_t realTimeNow = BTimeSource::RealTime();
			
			//	Handle stop
			if (m_Stopping){
				if (realTimeNow >= TimeSource()->RealTimeFor(m_StopTime, m_PrivateLatency + m_DownstreamLatency)){
					m_Running 	= false;
					m_Stopping 	= false;
					
					// deal with any pending Seek
					if (m_Seeking)
						m_Seeking = false;
						
					if (connected)
						SendDataStatus(B_DATA_NOT_AVAILABLE, m_Output.destination, m_StopTime);
						
					continue;
				}
			}
			
			//	Handle seek
			if (m_Seeking){
				if (realTimeNow >= TimeSource()->RealTimeFor(m_SeekTime, m_PrivateLatency + m_DownstreamLatency)){
					m_Seeking 	= false;
					m_DeltaTime = m_MediaTime;
	
					continue;
				}
			}
			
			
			//	Handle start
			if (m_Starting){
				if (realTimeNow >= TimeSource()->RealTimeFor(m_StartTime, m_PrivateLatency + m_DownstreamLatency)){
					m_Running  = true;
					m_Starting = false;
		
					m_DeltaTime 	= TimeSource()->Now();
					m_FramesRead 	= 0;
					
					if (connected)
						SendDataStatus(B_DATA_AVAILABLE, m_Output.destination, m_StartTime);
					
					continue;
				}
			}
			
			// If a buffer doesn't currently exist, then try to acquire one from the buffer group.
			if (!m_CurrentBuffer){
				BAutolock lock(m_Lock);
				
				if (m_BufferGroup)
					m_CurrentBuffer = m_BufferGroup->RequestBuffer(m_FrameSize);			
			}
			
			//	Serve up tasty buffers
			if (m_Running){
				if (m_CurrentBuffer){	
				
				// ABH No Looping!
					//	Do we need to loop the data?
//					if (m_CurrentVideoFrame >= m_Reader->VideoFrameCount())
//						m_CurrentVideoFrame = 0;
					
					//	Get the frame
					AVIVideoFrame *theFrame = (AVIVideoFrame *)m_Reader->GetVideoFrameList()->ItemAt(m_CurrentVideoFrame);
					if (!theFrame){
						printf("AVIProducer::INVALID FRAME!\n");
						return;
					}
					
					//	Increment m_CurrentVideoFrame and total frames read
					m_CurrentVideoFrame++;
					m_FramesRead++;
					
					//	Decode frame
					bool retVal = m_VideoCodec->DecodeFrame(theFrame, m_CurrentBuffer->Data(), B_RGB32);
					if (!retVal){
						printf("AVIProducer::DecodeFrame FAILURE!\n");
						return;
					}
					
					//	Set up buffer header			
					media_header *bufHeader	= m_CurrentBuffer->Header();
					bufHeader->type 		= B_MEDIA_MULTISTREAM;						
					bufHeader->size_used 	= m_FrameSize;
					bufHeader->time_source	= TimeSource()->ID();
					bufHeader->start_time 	= m_DeltaTime + 
											  (bigtime_t)(m_FramesRead * 1000000. / VisualDataRate()) +
											  m_DownstreamLatency;
					
					//printf("Buffer Time: %.4f\n", (double)bufHeader->start_time/1000000.);
											
					//	Send buffer to Consumer
					status_t status = B_OK;
					
					// ABH was using deprecated 2 parameter version of SendBuffer
					status = SendBuffer(m_CurrentBuffer, (const media_source &)m_Output, m_Output.destination);
					
					//	Set current buffer to NULL
					m_CurrentBuffer = 0;
					
					// If the SendBuffer didn't happen for some reason, then
					// we still holding onto this buffer so we need to recycle it
					// ourself because no one else will.
					if (status != B_OK){
						printf("AVIProducer::RunRoutine() - failed in SendBuffer 0x%x\n", status);
						m_CurrentBuffer->Recycle();
					}
				}
				else {
					//	If we didn't acquire the buffer, skip the frame
					printf("AVIProducer::BUFFER ACQUIRE ERROR!\n");
					m_CurrentVideoFrame++;
				}
			}
		}
	}
}
