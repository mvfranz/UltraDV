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
	fEditor         = NULL;
	fFile                   = NULL;

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
	if ( IsVideo(nodeInfo) ) {
		//      Create a BMessage that includes the entry_ref to send to our open routine
		BMessage *theMessage = new BMessage(B_REFS_RECEIVED);
		theMessage->AddRef("refs", &theRef);

		bool retVal = LoadMovieFile(theMessage);

		// We are succesful.  Init the cue
		if (retVal) {
			Init();
		}
		// If we have an error, ask the user to locate a new data file
		else{
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
	if (fReader)
		delete fReader;

	// Clean up
	if (fFile) {
		fFile->Unset();
		delete fFile;
		fFile = NULL;
	}

	// Close editor
	if ( fEditorOpen && fEditor) {
		fEditor->Hide();
		fEditor->Lock();
		fEditor->Quit();
	}

	//	Free offscreen
	if (fBitmap)
		delete fBitmap;
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
	fReader = new TRIFFReader(fFile);

	//	Create audio and video codec
	retVal = InitCodecs();

	//	Init current frames
	fCurrentVideoFrame = 0;
	fCurrentAudioFrame = 0;

	//	Get AVIHeader
	AVIHeader *header = fReader->GetAVIHeader();

	//	Create offscreen
	BRect movieRect( 0, 0, header->Width-1, header->Height-1);
	fBitmap = new BBitmap(movieRect, B_RGB32);

	// Default initialization
	TVisualCue::Init();

	// Set up area rectangles
	fCuePosition->Outline(movieRect);

	// Editor is closed
	fEditorOpen = false;

	// Set up default settings
	fIsLocked                       = false;
	fIsSelected             = false;
	fLowColor                       = kWhite;
	fHighColor              = kBlack;
	fIsPrepared             = false;
	fIsPlaying              = false;
	fIsVisible                      = true;
	fHasDuration            = true;
	fCanLoop                        = true;
	fCanStretch             = true;
	fCanEnvelope            = true;
	fHasEditor              = true;
	fCanWindow                      = true;
	fCanTransition          = true;
	fCanPath                        = true;

	//	Calculate duration in milliseconds
	fMSecsPerFrame = header->TimeBetweenFrames / 1000;
	fDuration           = header->TotalNumberOfFrames * fMSecsPerFrame;


	// Add the cue to the cue channel
	if ( fChannel->CanInsertCue(this, fInsertTime, true)) {
		fChannel->InsertCue(this, fInsertTime);
		Select();

		// We are now fully instantiated
		fIsInstantiated = true;
	}

	// Adjust cue length based on duration
	ResizeTo( TimeToPixels(fDuration, GetCurrentTimeFormat(), GetCurrentResolution()), Bounds().Height());
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
	if (fReader->HasVideo() == false)
		return false;

	fVideoCodec = NULL;

	//	Get video header to determine compression type
	AVIVIDSHeader *vidsHeader = fReader->GetVIDSHeader();

	//	Determine compression and create codec
	switch(vidsHeader->Compression)
	{
	//	Uncompressed RGB
	case RIFF_RGB:
	case RIFF_rgb:
		fVideoCodec = new TRGBCodec(fReader);
		break;

	//	Apple Video "Road Pizza"
	case RIFF_rpza:
	case RIFF_azpr:
		fVideoCodec = new TAppleVideoCodec(fReader);
		break;

	//	Microsoft Video 01
	case RIFF_wham:
	case RIFF_WHAM:
	case RIFF_cram:
	case RIFF_CRAM:
	case kRiff_msvc_Chunk:
	case kRiff_MSVC_Chunk:
		fVideoCodec = new TMSVideoCodec(fReader);
		break;

	//	Microsoft RLE
	case RIFF_RLE8:
	case RIFF_rle8:
		fVideoCodec = new TMSRLECodec(fReader);
		break;

	//	Radius Cinepak "Compact Video"
	case RIFF_cvid:
	case RIFF_CVID:
		fVideoCodec = new TCinepakCodec(fReader);
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

	if (fVideoCodec)
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
	if (fReader->HasAudio() == false)
		return false;

	fAudioCodec = NULL;

	//	Get video header to determine compression type
	AVIAUDSHeader *audsHeader = fReader->GetAUDSHeader();

	//	Determine compression and create codec
	switch(audsHeader->Format)
	{
	case WAVE_FORMAT_PCM:
		fAudioCodec = new TPCMCodec(fReader);
		break;

	/*
	   case WAVE_FORMAT_ADPCM:
	        {
	                switch(audsHeader.Size)
	                {
	                        case 4:
	                                fAudioType = kAudioADPCM;
	                                break;

	                        default:
	                                fAudioType = kAudioInvalid;
	                                break;
	                }
	        }
	        break;

	   case WAVE_FORMAT_DVI_ADPCM:
	        {
	                fAudioType = kAudioDVI;
	        }
	        break;

	   case WAVE_FORMAT_MULAW:
	        {
	                fAudioType = kAudioULaw;
	        }
	        break;

	   case WAVE_FORMAT_GSM610:
	        {
	                fAudioType = kAudioMSGSM;
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

	if (fAudioCodec)
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

	if (myErr == B_OK) {
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
	if (!fIsInstantiated)
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
	if (fEffectsList->CountItems() > 0) {
		//	If we are here, the effect must be a VisualEffect
		for (int32 index = 0; index < fEffectsList->CountItems(); index++) {
			//	Get the effects in the list
			TCueEffectView *effectView = (TCueEffectView *)fEffectsList->ItemAt(index);
			if (effectView) {
				TVisualEffect *effect = (TVisualEffect *)effectView->Effect();

				//	Setup transformation bitmap buffer
				if (fTransformBitmap) {
					delete fTransformBitmap;
					fTransformBitmap = NULL;
				}

				//	Get current time and convert to cue local time
				uint32 theTime = GetCurrentTime();
				theTime -= fStartTime;

				//	Is it in the effects time span?
				if ( (theTime >= effect->StartTime()) && (theTime <= effect->EndTime()) )
					fTransformBitmap = effect->TransformBitmap(theTime, fBitmap, *fCuePosition, kBestQuality);
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
		fPanel->Hide();

		// Attempt to load audio file
		if ( LoadMovieFile(message) ) {
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

	//      Create messenger to send panel messages to our channel.  We cannot send it to
	//  ourself as we are not part of the view heirarchy.
	BMessenger *messenger = new BMessenger( fChannel,  ((MuseumApp *)be_app)->GetCueSheet());

	// Create message containing pointer to ourself
	BMessage *message = new BMessage();
	message->AddPointer("TheCue", this);

	// Create a RefFilter for a "video" type
	TRefFilter *refFilter = new TRefFilter(kVideoFilter);

	// Construct a file panel and set it to modal
	fPanel = new BFilePanel( B_OPEN_PANEL, messenger, NULL, B_FILE_NODE, false, message, refFilter, true, true );

	// Set it to application's home directory
	app_info appInfo;
	be_app->GetAppInfo(&appInfo);
	BEntry entry(&appInfo.ref);
	BDirectory parentDir;
	entry.GetParent(&parentDir);
	fPanel->SetPanelDirectory(&parentDir);

	// Center Panel
	CenterWindow(fPanel->Window());
	fPanel->Show();

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
	if(fPanel) {
		// Clean up any RefFilters
		TRefFilter *theFilter = static_cast<TRefFilter *>(fPanel->RefFilter() );
		if (theFilter)
			delete theFilter;

		delete fPanel;
		fPanel = NULL;
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

	message->FindRef("refs", 0, &fFileRef);

	// Resolve possible symlink...
	BEntry entry(&fFileRef, true);
	entry.GetRef(&fFileRef);

	// Create BFile from ref...
	fFile = new BFile(&fFileRef, B_READ_ONLY);

	//	Verify that we have an AVI file
	retVal = IsRIFFFile(fFile);

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
	if (fEditorOpen) {
		if (fEditor) {
			fEditor->Show();
			fEditor->Activate(true);
		}
	} else   {
		BMessage *theMessage = GetWindowFromResource("VideoEditorWindow");
		fEditor = new TVideoEditor(&fFileRef, this, theMessage);

		if (fEditor) {
			fEditorOpen = true;
			fEditor->Show();
		} else
			fEditorOpen = false;
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
	fCurrentVideoFrame = (theTime - fStartTime) / fMSecsPerFrame;

	//	Check for out of bounds.  Set to last frame of file.
	if (fCurrentVideoFrame >= fReader->VideoFrameCount())
		fCurrentVideoFrame = fReader->VideoFrameCount() - 1;

	//	Get the frame
	AVIVideoFrame *theFrame = (AVIVideoFrame *)fReader->GetVideoFrameList()->ItemAt(fCurrentVideoFrame);
	if (!theFrame) {
		printf("AVIProducer::INVALID FRAME!\n");
		return;
	}

	//	Decode frame
	bool retVal = fVideoCodec->DecodeFrame(theFrame, fBitmap->Bits(), B_RGB32);
	if (!retVal) {
		printf("AVIProducer::DecodeFrame FAILURE!\n");
		return;
	}

	//	Process with any effects
	RenderData(theTime, fCuePosition->Enclosure(false));
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
	BBitmap *cueIcon = GetAppIcons()->fMovieUpIcon;

	if (cueIcon) {
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);
		fCueIcon = new TBitmapView(area, "CueIcon", cueIcon, false);
		AddChild(fCueIcon);
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
	if (fTransformBitmap)
		return fTransformBitmap;
	else
		return fBitmap;
}
