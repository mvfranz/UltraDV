//---------------------------------------------------------------------
//
//	File:	TMovieCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Digital Video Cue
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <string.h>
#include <Application.h>
#include <Debug.h>
#include <MediaRoster.h>

#include "MuseumApp.h"
#include "AppAlerts.h"
#include "AppConstants.h"
#include "AppMessages.h"
#include "AppUtils.h"
#include "DebugUtils.h"
#include "ResourceManager.h"

#include "TBitmapView.h"
#include "TCueChannel.h"
#include "TCueView.h"
#include "TVisualCue.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TRefFilter.h"
#include "TVideoEditor.h"

#include "TCueEffectView.h"
#include "TVisualEffect.h"

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

#include "TMovieCue.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMovieCue::TMovieCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime) : 
			TVisualCue(id, parent, bounds, startTime, "MovieCue")
{	
	// Load picture file
	ShowPanel();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from an entry_ref

TMovieCue::TMovieCue(entry_ref &theRef, int16 id,  TCueChannel *parent, BRect bounds, uint32 startTime) : 
	TVisualCue(id, parent, bounds, startTime, "PictureCue")
{	
	// Init member variables 
	m_Editor		= NULL;
	m_File			= NULL;
	
	//
	// Attempt to load data file
	//
	
	BNode theNode(&theRef);
	if (theNode.InitCheck() != B_OK)
		return;
		
	BNodeInfo nodeInfo(&theNode);
	if (nodeInfo.InitCheck() != B_OK)
		return;

	// First, make sure we have a valid ref
	if ( IsVideo(nodeInfo) )
	{
		// 	Create a BMessage that includes the entry_ref to send to our open routine
		BMessage *theMessage = new BMessage(B_REFS_RECEIVED);
		theMessage->AddRef("refs", &theRef);
		
		bool retVal = LoadMovieFile(theMessage);
		
		// We are succesful.  Init the cue
		if (retVal)
		{
			Init();
		}
		// If we have an error, ask the user to locate a new data file
		else
		{
			ShowPanel();
		}
	}		
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from a BMessage
//

TMovieCue::TMovieCue(BMessage *theMessage) : TVisualCue(theMessage)
{	
	// Load cue icon
	LoadCueIcon();	
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMovieCue::~TMovieCue()
{
	//	Free our RIFFReader
	if (m_Reader)
		delete m_Reader;
		
	// Clean up
	if (m_File)
	{		
		m_File->Unset();
		delete m_File;
		m_File = NULL;
	}
		
	// Close editor
	if ( m_EditorOpen && m_Editor)
	{
		m_Editor->Hide();
		m_Editor->Lock();
		m_Editor->Quit();
	}
	
	//	Free offscreen
	if (m_Bitmap)
		delete m_Bitmap;
}


#pragma mark -
#pragma mark === Initialization Functions ===

//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TMovieCue::Init()
{				
	bool retVal;
				
	//	Set up RIFFReader
	m_Reader = new TRIFFReader(m_File);
	
	//	Create audio and video codec
	retVal = InitCodecs();
	
	//	Init current frames
	m_CurrentVideoFrame = 0;
	m_CurrentAudioFrame = 0;
	
	//	Get AVIHeader
	AVIHeader *header = m_Reader->GetAVIHeader();
	
	//	Create offscreen
	BRect movieRect( 0, 0, header->Width-1, header->Height-1); 
	m_Bitmap = new BBitmap(movieRect, B_RGB32);
					
	// Default initialization
	TVisualCue::Init();

	// Set up area rectangles	
	m_CuePosition->Outline(movieRect);

	// Editor is closed
	m_EditorOpen = false;
	
	// Set up default settings	
	m_IsLocked 			= false;	
	m_IsSelected 		= false;	
	m_LowColor 			= kWhite;	
	m_HighColor 		= kBlack;	
	m_IsPrepared 		= false;
	m_IsPlaying 		= false;	
	m_IsVisible			= true;	
	m_HasDuration 		= true;					
	m_CanLoop			= true;
	m_CanStretch		= true;		
	m_CanEnvelope		= true;
	m_HasEditor 		= true;		
	m_CanWindow			= true;
	m_CanTransition		= true;
	m_CanPath			= true;
				
	//	Calculate duration in milliseconds
	m_MSecsPerFrame = header->TimeBetweenFrames / 1000;
	m_Duration 	    = header->TotalNumberOfFrames * m_MSecsPerFrame;

		
	// Add the cue to the cue channel
	if ( m_Channel->CanInsertCue(this, m_InsertTime, true))
	{
		m_Channel->InsertCue(this, m_InsertTime);		
		Select();								
				
		// We are now fully instantiated
		m_IsInstantiated = true;
	}
	
	// Adjust cue length based on duration
	ResizeTo( TimeToPixels(m_Duration, GetCurrentTimeFormat(), GetCurrentResolution()), Bounds().Height());		
}


//---------------------------------------------------------------------
//	InitCodecs
//---------------------------------------------------------------------
//
//	Determine compression and initializae codec
//

bool TMovieCue::InitCodecs()
{
	bool retVal = false;
	
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

bool TMovieCue::InitVideoCodec()
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

bool TMovieCue::InitAudioCodec()
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
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TMovieCue::Instantiate(BMessage *archive) 
{ 	
	if ( validate_instantiation(archive, "TMovieCue") ) 
		return new TMovieCue(archive); 
		
	return NULL; 
}
  

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TMovieCue::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = TVisualCue::Archive(data, deep);
	
	if (myErr == B_OK)
	{				
		// Add ourselves to the archive
		data->AddString("class", "TMovieCue");						
	}
	
	Looper()->Unlock();
		
	return myErr;
}

#pragma mark -
#pragma mark === Drawing Functions ===


//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TMovieCue::Draw(BRect updateRect)
{
	// Return if were not done cooking...
	if (!m_IsInstantiated)
		return;
	
	// Pass up to parent
	TVisualCue::Draw(updateRect);
}


//---------------------------------------------------------------------
//	RenderBitmapData
//---------------------------------------------------------------------
//
//	Render internal bitmap modifications
//

void TMovieCue::RenderBitmapData()
{
	
	//	Handle effects stack
	if (m_EffectsList->CountItems() > 0)
	{
		//	If we are here, the effect must be a VisualEffect
		for (int32 index = 0; index < m_EffectsList->CountItems(); index++)
		{
			//	Get the effects in the list
			TCueEffectView *effectView = (TCueEffectView *)m_EffectsList->ItemAt(index);			
			if (effectView)
			{
				TVisualEffect *effect = (TVisualEffect *)effectView->Effect();
				
				//	Setup transformation bitmap buffer
				if (m_TransformBitmap)
				{
					delete m_TransformBitmap;
					m_TransformBitmap = NULL;
				}
				
				//	Get current time and convert to cue local time
				uint32 theTime = GetCurrentTime();
				theTime -= m_StartTime;
				
				//	Is it in the effects time span?
				if ( (theTime >= effect->StartTime()) && (theTime <= effect->EndTime()) )
					m_TransformBitmap = effect->TransformBitmap(theTime, m_Bitmap, *m_CuePosition, kBestQuality);
			}
		}
	}
}

#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Receive messages
//
void TMovieCue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{	
		case B_OK:
		case B_REFS_RECEIVED:
			{
				m_Panel->Hide();
				
				// Attempt to load audio file
				if ( LoadMovieFile(message) )
				{
					Init();
					AddToBrowser();
				}
			}
			break;
						
		default:
			TVisualCue::MessageReceived(message);						
			break;	
	}
}


#pragma mark -
#pragma mark === File Handling ===


//---------------------------------------------------------------------
//	ShowPanel
//---------------------------------------------------------------------
//
//	Show file panel
//

void TMovieCue::ShowPanel()
{
	
	TVisualCue::ShowPanel();
		
	// 	Create messenger to send panel messages to our channel.  We cannot send it to 
	//  ourself as we are not part of the view heirarchy.
 	BMessenger *messenger = new BMessenger( m_Channel,  ((MuseumApp *)be_app)->GetCueSheet());
	
	// Create message containing pointer to ourself
	BMessage *message = new BMessage();
	message->AddPointer("TheCue", this);
	
 	// Create a RefFilter for a "video" type
	TRefFilter *refFilter = new TRefFilter(kVideoFilter);
 	
 	// Construct a file panel and set it to modal
 	m_Panel = new BFilePanel( B_OPEN_PANEL, messenger, NULL, B_FILE_NODE, false, message, refFilter, true, true );
 
 	// Set it to application's home directory
 	app_info appInfo;
 	be_app->GetAppInfo(&appInfo); 	
 	BEntry entry(&appInfo.ref);
 	BDirectory parentDir;
 	entry.GetParent(&parentDir);
 	m_Panel->SetPanelDirectory(&parentDir);
 		
	// Center Panel
	CenterWindow(m_Panel->Window());
	m_Panel->Show();
	
	// Clean up
	delete messenger;
	delete message;
}



//---------------------------------------------------------------------
//	HidePanel
//---------------------------------------------------------------------
//
//	Hide and delete file panel
//

void TMovieCue::HidePanel()
{	
	if(m_Panel)
	{		
		// Clean up any RefFilters
		TRefFilter *theFilter = static_cast<TRefFilter *>( m_Panel->RefFilter() );
		if (theFilter)
			delete theFilter;
			
		delete m_Panel;
		m_Panel = NULL;
	}
	
	TVisualCue::HidePanel();
}


//---------------------------------------------------------------------
//	LoadMovieFile
//---------------------------------------------------------------------
//
//	Load movie file
//

bool TMovieCue::LoadMovieFile(BMessage *message)
{
	
	bool retVal = false;
		
	message->FindRef("refs", 0, &m_FileRef);
	
	// Resolve possible symlink...
	BEntry entry(&m_FileRef, true);
	entry.GetRef(&m_FileRef);
	
	// Create BFile from ref...
	m_File = new BFile(&m_FileRef, B_READ_ONLY);
		
	//	Verify that we have an AVI file
	retVal = IsRIFFFile(m_File);
	     						
	//	Return the value
	return retVal;
}

#pragma mark -
#pragma mark === Editor ===

//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TMovieCue::OpenEditor()
{

	// If editor is already open, bring it to front
	if (m_EditorOpen)
	{
		if (m_Editor)
		{	
			m_Editor->Show();
			m_Editor->Activate(true);
		}
	}
	else
	{		
		BMessage *theMessage = GetWindowFromResource("VideoEditorWindow");
		m_Editor = new TVideoEditor(&m_FileRef, this, theMessage);
	
		if (m_Editor)
		{
			m_EditorOpen = true;
			m_Editor->Show();
		}
		else
			m_EditorOpen = false;
	}
}

#pragma mark -
#pragma mark === Playback Routines ====

//---------------------------------------------------------------------
//	HandlePlayback
//---------------------------------------------------------------------
//
//	Process tick received from playback engine.  We receive a tick each 
//	millisecond.
//

void TMovieCue::HandlePlayback(uint32 theTime)
{	
	//	Calculate the frame for this time
	m_CurrentVideoFrame = (theTime - m_StartTime) / m_MSecsPerFrame;
		
	//	Check for out of bounds.  Set to last frame of file.
	if (m_CurrentVideoFrame >= m_Reader->VideoFrameCount())
		m_CurrentVideoFrame = m_Reader->VideoFrameCount() - 1;
	
	//	Get the frame
	AVIVideoFrame *theFrame = (AVIVideoFrame *)m_Reader->GetVideoFrameList()->ItemAt(m_CurrentVideoFrame);
	if (!theFrame)
	{
		printf("AVIProducer::INVALID FRAME!\n");
		return;
	}
		
	//	Decode frame
	bool retVal = m_VideoCodec->DecodeFrame(theFrame, m_Bitmap->Bits(), B_RGB32);
	if (!retVal)
	{
		printf("AVIProducer::DecodeFrame FAILURE!\n");
		return;
	}

	//	Process with any effects
	RenderData(theTime, m_CuePosition->Enclosure(false));
}


#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	LoadCueIcon
//---------------------------------------------------------------------
//
//	Load cues identifying icon
//

void TMovieCue::LoadCueIcon()
{
	BBitmap *cueIcon = GetAppIcons()->m_MovieUpIcon;

	if (cueIcon)
	{
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);		
		m_CueIcon = new TBitmapView(area, "CueIcon", cueIcon, false);
		AddChild(m_CueIcon);		
	}	

	//	Pass up to parent
	TVisualCue::LoadCueIcon();	

}


#pragma mark -
#pragma mark === Bitmap Routines ===

//---------------------------------------------------------------------
//	GetBitmap
//---------------------------------------------------------------------
//
//	Return visual display of data at the time specified.  If theTime is
//	NULL, return the data for the current time
//

BBitmap *TMovieCue::GetBitmap(uint32 theTime)
{
	// Return modified bitmap if we have created it
	if (m_TransformBitmap)
		return m_TransformBitmap;
	else
		return m_Bitmap;
}
