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

#include "TMovieCue.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMovieCue::TMovieCue(int16 id, TCueChannel* parent, BRect bounds, uint32 startTime) :
	TVisualCue(id, parent, bounds, startTime, "MovieCue"),
	fMediaFile(NULL),
	fVideoTrack(NULL)
{
	// Load picture file
	ShowPanel();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from an entry_ref

TMovieCue::TMovieCue(entry_ref &theRef, int16 id,  TCueChannel* parent, BRect bounds, uint32 startTime) :
	TVisualCue(id, parent, bounds, startTime, "PictureCue"),
	fMediaFile(NULL),
	fVideoTrack(NULL)
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
		BMessage* theMessage = new BMessage(B_REFS_RECEIVED);
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

TMovieCue::TMovieCue(BMessage* theMessage) : TVisualCue(theMessage)
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
	/*delete fFile;

	// Clean up
	if (fFile) {
		fFile->Unset();
		delete fFile;
		fFile = NULL;
	}*/

	// Close editor
	if ( fEditorOpen && fEditor) {
		fEditor->Hide();
		fEditor->Lock();
		fEditor->Quit();
	}

	//	Free offscreen
	if (fBitmap)
		delete fBitmap;

	delete fMediaFile;
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

	//	Init current frames
	//fCurrentVideoFrame = 0;
	//fCurrentAudioFrame = 0;

	// Default initialization
	TVisualCue::Init();

	// Set up area rectangles
	fCuePosition->Outline(fBitmap->Bounds());

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
	fDuration = (int32)fVideoTrack->Duration()/1000;

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


#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TMovieCue::Instantiate(BMessage* archive)
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

status_t TMovieCue::Archive(BMessage* data, bool deep) const
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
			TCueEffectView* effectView = (TCueEffectView*)fEffectsList->ItemAt(index);
			if (effectView) {
				TVisualEffect* effect = (TVisualEffect*)effectView->Effect();

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
void TMovieCue::MessageReceived(BMessage* message)
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
	BMessenger* messenger = new BMessenger( fChannel,  ((MuseumApp*)be_app)->GetCueSheet());

	// Create message containing pointer to ourself
	BMessage* message = new BMessage();
	message->AddPointer("TheCue", this);

	// Create a RefFilter for a "video" type
	TRefFilter* refFilter = new TRefFilter(kVideoFilter);

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
		TRefFilter* theFilter = static_cast<TRefFilter*>(fPanel->RefFilter() );
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

bool TMovieCue::LoadMovieFile(BMessage* message)
{
	//	Create offscreen

	bool retVal = false;

	message->FindRef("refs", 0, &fFileRef);

	// Resolve possible symlink...
	BEntry entry(&fFileRef, true);
	entry.GetRef(&fFileRef);

	// Create BFile from ref...
	fFile = new BFile(&fFileRef, B_READ_ONLY);

	fMediaFile = new BMediaFile(fFile);

	status_t err = fMediaFile->InitCheck();
	if (err != B_OK)
		return false;

	// Check if we have a video track
	media_format format;
	media_format encFormat;
	memset(&format, 0, sizeof(media_format));

	for (int32 i = 0; i < fMediaFile->CountTracks(); i++) {
		BMediaTrack* track = fMediaFile->TrackAt(i);
		if (track->InitCheck() != B_OK)
			continue;

		err = track->EncodedFormat(&encFormat);
		if (err != B_OK)
			continue;

		fBitmap = new BBitmap(BRect(0, 0, encFormat.u.encoded_video.output.display.line_width-1,
			encFormat.u.encoded_video.output.display.line_count-1), B_RGB32);

		memset(&format, 0, sizeof(media_format));
		format.u.raw_video.display.bytes_per_row = fBitmap->BytesPerRow();
		format.u.raw_video.last_active = (int32) (fBitmap->Bounds().Height() - 1.0);
		format.u.raw_video.orientation = B_VIDEO_TOP_LEFT_RIGHT;
		format.u.raw_video.pixel_width_aspect = 1;
		format.u.raw_video.pixel_height_aspect = 3;
		format.u.raw_video.display.format = B_RGB32;
		format.u.raw_video.display.line_width = fBitmap->Bounds().Width();
		format.u.raw_video.display.line_count = fBitmap->Bounds().Height();

		err = track->DecodedFormat(&format);
		if (err != B_OK)
			continue;
		if (format.u.raw_video.display.format == B_RGB32) {
			fVideoTrack = track;
			retVal = true;
			break;
		}
	}

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
	} else {
		BMessage* theMessage = GetWindowFromResource("VideoEditorWindow");
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

	// TODO: rework the process chain

	if (fVideoTrack->InitCheck() != B_OK)
		return;

	if (fBitmap == NULL)
		return;

	bigtime_t frames;
	media_header header;
	if (fVideoTrack->ReadFrames(fBitmap->Bits(), &frames, &header) != B_OK)
		return;

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
	BBitmap* cueIcon = GetAppIcons()->fMovieUpIcon;

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

BBitmap* TMovieCue::GetBitmap(uint32 theTime)
{
	// Return modified bitmap if we have created it
	if (fTransformBitmap)
		return fTransformBitmap;
	else
		return fBitmap;
}
