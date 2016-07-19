//---------------------------------------------------------------------
//
//	File:	TAudioCue.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <assert.h>

#include <Application.h>
#include <Debug.h>
#include <FilePanel.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"
#include "TRefFilter.h"

#include "TBitmapView.h"
#include "TCueView.h"
#include "TAudioCue.h"
#include "TAudioEditor.h"
#include "TCueTimeText.h"
#include "TCueChannel.h"
#include "TCueSheetView.h"
#include "TCueSheetWindow.h"

#include "TAudioEngine.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioCue::TAudioCue(int16 id, TCueChannel *parent, BRect bounds, uint32 startTime) : 
		   TCueView(id, parent, bounds, startTime, "AudioCue")
{	
	// Init member variables 
	fEditor		= NULL;
	fPreviewBitmap	= NULL;
	fSoundFile		= NULL;
	fSound			= NULL;
				
	// Load audio file
	ShowPanel();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from an entry_ref

TAudioCue::TAudioCue(entry_ref &theRef, int16 id,  TCueChannel *parent, BRect bounds, uint32 startTime) : 
	TCueView(id, parent, bounds, startTime, "AudioCue")
{	
	// Init member variables 
	fEditor		= NULL;
	fPreviewBitmap	= NULL;
	fSoundFile		= NULL;
	fSound			= NULL;
		
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
	if ( IsAudio(nodeInfo) )
	{
		// 	Create a BMessage that includes the entry_ref to send to our open routine
		BMessage *theMessage = new BMessage(B_REFS_RECEIVED);
		theMessage->AddRef("refs", &theRef);
		
		bool retVal = LoadAudioFile(theMessage);
		
		// We are succesful.  Init the cue
		if (retVal)
		{
			Init();
			AddToBrowser();
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

TAudioCue::TAudioCue(BMessage *theMessage) : TCueView(theMessage)
{	
	// Load cue icon
	LoadCueIcon();	
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioCue::~TAudioCue()
{	
	// Clean up old preview
	if (fPreviewBitmap)
	{		
		delete fPreviewBitmap;
	}
				
	// Close and free soundfile
	if (fSoundFile)
	{
		delete fSoundFile;
	}
			
	// Close editor
	if ( fEditorOpen && fEditor)
	{
		fEditor->Hide();
		fEditor->Lock();
		fEditor->Quit();
	}
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks


void TAudioCue::Init()
{		
	// Set up member variables
	fUpdatePreview = true;
		
	// Set up default duration
	fDuration = GetDefaultLength();
		
	// Adjust cue length based on duration
	ResizeTo( TimeToPixels( (fDuration), GetCurrentTimeFormat(), GetCurrentResolution()), Bounds().Height());
	
	// Get sound attrubutes
	InitSoundfileAttributes();

	// Editor is closed
	fEditorOpen = false;
	
	// Set up default settings	
	fIsLocked 			= false;	
	fIsSelected 		= false;	
	fLowColor 			= kWhite;	
	fHighColor 		= kBlack;	
	fIsPrepared 		= false;
	fIsPlaying 		= false;	
	fIsVisible			= false;	
	fHasDuration 		= true;					
	fCanLoop			= true;
	fCanStretch		= true;		
	fCanEnvelope		= true;
	fHasEditor 		= true;		
	fCanWindow			= true;
	fCanTransition		= false;
	fCanPath			= false;
		
	// Default initialization
	TCueView::Init();
	
	// Set cues fFile to point to the fSoundFile
	fFile = (BFile *)1;

	//	Get pointer to CueSheet AudioEngine
	fAudioEngine = fChannel->GetCueSheet()->GetParent()->GetAudioEngine();
	
	// Add the cue to the cue channel
	if ( fChannel->CanInsertCue(this, fInsertTime, true))
	{
		fChannel->InsertCue(this, fInsertTime);		
		Select();
		
		// We are now fully instantiated
		fIsInstantiated = true;
	}
}

#pragma mark -
#pragma mark === Archiving Functions ===


//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TAudioCue::Instantiate(BMessage *archive) 
{ 	
	if ( validate_instantiation(archive, "TAudioCue") ) 
		return new TAudioCue(archive); 
		
	return NULL; 
}
  

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TAudioCue::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = TCueView::Archive(data, deep);
	
	if (myErr == B_OK)
	{				
		// Add ourselves to the archive
		data->AddString("class", "TAudioCue");						
	}
	
	Looper()->Unlock();
		
	return myErr;
}


#pragma mark -
#pragma mark === Drawing Routines ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioCue::Draw(BRect updateRect)
{
	// Save colors
	rgb_color saveColor = HighColor();
			
	// Restore color
	SetHighColor(saveColor);
		
	// Pass up to parent
	TCueView::Draw(updateRect);
	
	//	Determine drawing state	
	switch (fCueDisplayMode)
	{
		case kCueControls:
			break;
			
		case kCueContents:
			{
				// Do we need to update the preview contents?
				if (fUpdatePreview)
				{
					// Clean up old preview
					if (fPreviewBitmap)
					{
						fPreviewBitmap->Lock();
						BView *oldView = fPreviewBitmap->ChildAt(0);
						fPreviewBitmap->RemoveChild(oldView);
						delete oldView;
						delete fPreviewBitmap;
					}
							
					// Create preview bitmap the size of the view
					BRect bounds = Bounds();
					bounds.right -= (kResizeZoneWidth+2)*2;
					bounds.InsetBy(0, 1);
					fPreviewBitmap = new BBitmap( bounds, B_CMAP8, true);
					
					// Create preview view
					BView *previewView = new BView(bounds, "PreviewView", B_FOLLOW_ALL, 0);
					fPreviewBitmap->AddChild(previewView);
					
					// Draw waveform into bitmap
					CreatePreview(previewView);
					previewView->Looper()->Lock();
					previewView->Sync();
					previewView->Looper()->Unlock();
								
					fUpdatePreview = false;
				}
				
				// Draw update into view
				BPoint drawPt(fLResizeZone.right+2, 1);
				DrawBitmap(fPreviewBitmap, drawPt);
			}
			break;
									
		default:
			TRESPASS();
			break;
	}
}


//---------------------------------------------------------------------
//	CreateOverview
//---------------------------------------------------------------------
//
//	Create bitmap of waveform
//

void TAudioCue::CreatePreview(BView *previewView)
{
	BPoint startPt, endPt;
	
	// Set up environment
	previewView->Looper()->Lock();
	previewView->PushState();
	previewView->SetPenSize(0.0);
	
	const BRect bounds = previewView->Bounds();
			
	// Fill backgound
	previewView->SetHighColor(kBlueGrey);
	previewView->FillRect(bounds);
	
	// Set pen color
	previewView->SetHighColor(kDarkGrey);
					
	// Get middle point of view
	float viewMiddle = previewView->Bounds().Height() / 2;
		
	// Don't draw past end of file
	if (fSamplesPerPixel * (bounds.left + 1) < fNumSamples)
	{												
		// Create buffer the size of the number of pixels per sample * the sounds frame size	
		int32 bufferSize = fSamplesPerPixel * fSoundFile->FrameSize();	
		char *soundBuffer = (char *)malloc(bufferSize);
		
		// Rewind the buffer and seek to proper frame position
		off_t seekVal = fSoundFile->SeekToFrame( bounds.left * bufferSize);
		if ( seekVal >= 0 )
		{
			// reset our horizontal position		
			int32 hPos = bounds.left;		
		
			// Draw the waveform from left to right	
			for (int32 currentPos = bounds.left; currentPos < bounds.right; currentPos++) 
			{
				// Exit if we have passed the end of the file		
				if ( fSamplesPerPixel * (currentPos + 1) >= fNumSamples)		
					break;
								
				// Read data into buffer
				status_t framesRead = fSoundFile->ReadFrames(soundBuffer, bufferSize);
				
				// Go through each sample at this pixel and find the high and low points			
				int32 low, high;
				low  =  32767;
				high = -32768;
		
				// Make sure we have some data
				if (framesRead > 0)
				{									 			
					if ( fSampleSize == 1)
					{
						// Go through each sample at this pixel and find the high and low points				
						char *ptr 	= (char *)soundBuffer;
						int32 endPt =  fSamplesPerPixel * fNumChannels;
						
						for (int32 index = 0; index < framesRead; index++) 
						{
							// Locate the lowest and highest samples in the buffer											
							if ( *ptr < low )
								low = *ptr;
							
							if ( *ptr > high )
								high = *ptr;
							
							// Increment pointer						
							ptr++;
						}						
					}
					else
					{
						int16 *ptr 	= (int16 *)soundBuffer;
						int32 endPt =  fSamplesPerPixel * fNumChannels;
		
						for (int32 index = 0; index < framesRead; index++) 
						{
							// Locate the lowest and highest samples in the buffer											
							if ( *ptr < low )
								low = *ptr;
							
							if ( *ptr > high )
								high = *ptr;
							
							// Increment pointer					
							ptr++;
						}			
						
						// Convert to 8 bit for drawing
						low  >>= 8;			
						high >>= 8;
					}				
				}
						
				// Set points and draw line	
				startPt.Set( hPos, viewMiddle - (high * viewMiddle / 128) );
				endPt.Set( hPos, viewMiddle + ( -low * viewMiddle / 128) );	
				
				// Draw low and high points
				//previewView->StrokeLine(startPt, endPt);
				BPoint tmpPt(startPt.x, startPt.y+1);
				previewView->StrokeLine(startPt, tmpPt);
				tmpPt.Set(endPt.x, endPt.y-1);
				previewView->StrokeLine(tmpPt, endPt);
				
				// Increment horizontal position	
				hPos++;
			}
		}
		
		// Free buffer
		free(soundBuffer);
	}
	
	// Draw center divider line
	//previewView->SetHighColor(kBlack);
	startPt.Set(bounds.left, viewMiddle );
	endPt.Set(bounds.right, viewMiddle);
	previewView->StrokeLine(startPt, endPt);
	
	// Restore environment
	previewView->PopState();	
	previewView->Looper()->Unlock();
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TAudioCue::MessageReceived(BMessage *message)
{
	switch(message->what)
	{		
					
		case B_OK:
		case B_REFS_RECEIVED:
			{
				fPanel->Hide();
				
				// Attempt to load audio file
				if ( LoadAudioFile(message) )
				{
					Init();
					AddToBrowser();
				}
			}
			break;
											
		default:
			TCueView::MessageReceived(message);						
			break;			
	}
}


#pragma mark -
#pragma mark === Time Format and Resolution Handling ===


//---------------------------------------------------------------------
//	ResolutionChanged
//---------------------------------------------------------------------
//
//

void TAudioCue::ResolutionChanged( int16 theFormat, int16 theResolution)
{			
	// Pass up to parent
	TCueView::ResolutionChanged(theFormat, theResolution);
		
	// Update samples per pixel
	fSamplesPerPixel = fNumSamples / Bounds().Width() + 1;	
		
	// Create new preview
	fUpdatePreview = true;
}


#pragma mark -
#pragma mark === File Handling ===


//---------------------------------------------------------------------
//	ShowPanel
//---------------------------------------------------------------------
//
//	Show file panel
//

void TAudioCue::ShowPanel()
{
 	
	TCueView::ShowPanel();
 		
 	// 	Create messenger to send panel messages to our channel.  We cannot send it to 
	//  ourself as we are not part of the view heirarchy.
 	BMessenger *messenger = new BMessenger( fChannel,  ((MuseumApp *)be_app)->GetCueSheet());
 		
 	// Create message containing pointer to ourself
	BMessage *message = new BMessage();
	message->AddPointer("TheCue", this);
	
	// Create a RefFilter for an "image" type
 	TRefFilter *refFilter = new TRefFilter(kAudioFilter);

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

void TAudioCue::HidePanel()
{
	if(fPanel)
	{
		// Clean up any RefFilters
		TRefFilter *theFilter = static_cast<TRefFilter *>( fPanel->RefFilter() );
		if (theFilter)
			delete theFilter;
			
		delete fPanel;
		fPanel = NULL;
	}
	
	TCueView::HidePanel();
}


//---------------------------------------------------------------------
//	LoadAudioFile
//---------------------------------------------------------------------
//
//	Load an audio file using BSoundFile
//

bool TAudioCue::LoadAudioFile(BMessage *message)
{
	message->FindRef("refs", 0, &fFileRef);
	
	// Resolve possible symlink...
	BEntry entry(&fFileRef, true);
	entry.GetRef(&fFileRef);
	
	// Create BFile from ref...
	fSoundFile = new BSoundFile(&fFileRef, B_READ_ONLY);
		
	// Determine the file type
	switch( fSoundFile->FileFormat() )
	{
		case B_AIFF_FILE:
			break;
			
		case B_WAVE_FILE:
			break;
			
		case B_UNIX_FILE:
			break;
			
		case B_UNKNOWN_FILE:
			//return false;
			break;							
	}	

	return true;
}


//---------------------------------------------------------------------
//	OpenEditor
//---------------------------------------------------------------------
//
//	Open data editor if cue can edit it's data
//

void TAudioCue::OpenEditor()
{

	// If editor is already open, bring it to front
	if (fEditorOpen)
	{
		if (fEditor)
		{	
			fEditor->Show();
			fEditor->Activate(true);
		}
	}
	else
	{		
				
		float maxWidthHeight = kAudioEditorHeight+kAudioToolbarHeight+kTimelineHeight;
		BRect bounds(50, 50, 300, 50+maxWidthHeight);
		fEditor = new TAudioEditor(bounds, &fFileRef, this);
	
		if (fEditor)
		{
			fEditorOpen = true;
			fEditor->Show();
		}
		else
			fEditorOpen = false;
	}
}


#pragma mark -
#pragma mark === Utility Functions ===

//---------------------------------------------------------------------
//	LoadCueIcon
//---------------------------------------------------------------------
//
//	Load cues identifying icon
//

void TAudioCue::LoadCueIcon()
{
	BBitmap *cueIcon = GetAppIcons()->fAudioUpIcon;

	if (cueIcon)
	{
		BRect area(0, 0+(kTimeTextHeight+kTimeTextOffset+3), kCueIconWidth-1, (kCueIconWidth-1)+(kTimeTextHeight+kTimeTextOffset+3));
		area.OffsetBy(kResizeZoneWidth+5, 0);		
		fCueIcon = new TBitmapView(area, "CueIcon", cueIcon, false);
		AddChild(fCueIcon);		
	}

	//	Pass up to parent
	TCueView::LoadCueIcon();	
	
}


//---------------------------------------------------------------------
//	GetDefaultLength
//---------------------------------------------------------------------
//
//	Get the unadjusted length of the audio file.
//

int32 TAudioCue::GetDefaultLength()
{
	// Get total channels
	int32 numChannels = fSoundFile->CountChannels();	
					
	// Determine number of samples
	int32 numSamples = ( fSoundFile->CountFrames() * fSoundFile->FrameSize() ) / numChannels;

	return ((double)numSamples * 1000) / fSoundFile->SamplingRate();
}

//---------------------------------------------------------------------
//	InitSoundfileAttributes
//---------------------------------------------------------------------
//
//

void TAudioCue::InitSoundfileAttributes()
{
	// Get total channels
	fNumChannels = fSoundFile->CountChannels();	
					
	// Determine number of samples
	fNumSamples = ( fSoundFile->CountFrames() * fSoundFile->FrameSize() ) / fNumChannels;
	
	// Determine samples per pixel
	fSamplesPerPixel = fNumSamples / Bounds().IntegerWidth() + 1;	
	
	// Get sample size
	fSampleSize = fSoundFile->SampleSize();	
}


#pragma mark -
#pragma mark === Playback Routines ===

//---------------------------------------------------------------------
//	Preroll
//---------------------------------------------------------------------
//
//	Prepare cue for playback
//

void TAudioCue::PrerollCue(uint32 theTime)
{

}

//---------------------------------------------------------------------
//	PlayCue
//---------------------------------------------------------------------
//
//	Tell cue to start playback at millisecond time
//

void TAudioCue::PlayCue(uint32 theTime)
{
	TCueView::PlayCue(theTime);
	
	//	Create the BSound
	fSound = new BSound(&fFileRef);
	if(fSound->InitCheck() == B_OK)
	{
		//	Start playback at time in microseconds
		fAudioEngine->SetVolume(1.0);
		fPlaybackID = fAudioEngine->StartPlaying(fSound);
		fSound->ReleaseRef();
	}		
}

//---------------------------------------------------------------------
//	StopCue
//---------------------------------------------------------------------
//
//	Tell cue we are stopping
//

void TAudioCue::StopCue(uint32 theTime)
{	
	TCueView::StopCue(theTime);
	
	//	Stop playback at time in microseconds
	if (fAudioEngine->IsPlaying(fPlaybackID))
	{
		fAudioEngine->StopPlaying(fPlaybackID);
		
		//	Purge the file entry_ref to deallocate the BSound object
		fSound->ReleaseRef();
		fSound = NULL;				
	}		
}


//---------------------------------------------------------------------
//	Pause
//---------------------------------------------------------------------
//
//	Tell cue we are pausing
//

void TAudioCue::PauseCue()
{
	TCueView::PauseCue();
}


//---------------------------------------------------------------------
//	Resume
//---------------------------------------------------------------------
//
//	Tell cue we are resuming playback
//

void TAudioCue::ResumeCue()
{
	TCueView::ResumeCue();
}

//---------------------------------------------------------------------
//	HandlePlayback
//---------------------------------------------------------------------
//
//	Process tick received from playback engine.  We receive a tick each 
//	millisecond.
//

void TAudioCue::HandlePlayback(uint32 theTime)
{
	//	Check for volume changes
}


#pragma mark -
#pragma mark === Resizing Routines ===

//---------------------------------------------------------------------
//	Contract
//---------------------------------------------------------------------
//
// 	Hide cue controls
//

void TAudioCue::Contract(bool force)
{
	if (force)
		fIsExpanded = true;
		
	// Do nothing if we are already contracted
	if (fIsExpanded == false)
		return;

	fUpdatePreview = true;
	
	TCueView::Contract();
}


//---------------------------------------------------------------------
//	Contract
//---------------------------------------------------------------------
//
// 	Hide cue controls
//

void TAudioCue::Expand(bool force)
{
	
	if (force)
		fIsExpanded = false;

	// Do nothing if we are already contracted
	if (fIsExpanded == true)
		return;

	fUpdatePreview = true;
	
	TCueView::Expand();
}
