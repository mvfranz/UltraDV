//---------------------------------------------------------------------
//
//	File:	TAudioEditorView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
//	Desc:	Audio editor view.  Handle display of waveform
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"


#include "TAudioEditor.h"
#include "TAudioEditorView.h"
#include "TAudioTimelineView.h"
#include "TAudioEngine.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioEditorView::TAudioEditorView(TAudioEditor *parent, BRect bounds) : BView(bounds, "AudioEditorView", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_FRAME_EVENTS)
{
	// Save parent
	fParent = parent;

	// Set up member varibales
	fUpdatePreview = true;
	fPreviewBitmap  = NULL;

	// We handle our own drawing
	SetViewColor(B_TRANSPARENT_32_BIT);
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TAudioEditorView::~TAudioEditorView()
{
	// Clean up old preview
	if (fPreviewBitmap) {
		fPreviewBitmap->Lock();
		BView *oldView = fPreviewBitmap->ChildAt(0);
		fPreviewBitmap->RemoveChild(oldView);
		delete oldView;
		delete fPreviewBitmap;
	}
}


#pragma mark -
#pragma mark === Mouse Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TAudioEditorView::MouseDown(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseUp
//---------------------------------------------------------------------
//
//	Handle mouse up events
//

void TAudioEditorView::MouseUp(BPoint where)
{
}


//---------------------------------------------------------------------
//	MouseMoved
//---------------------------------------------------------------------
//
//	Handle mouse moved events
//

void TAudioEditorView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message )
{

	// Update Timeline.  Clip the location of where.x to be within the bounds
	// of the channel bounds
	//if (where.x > Bounds().right)
	//	where.x = Bounds().right;

	//BMessage *message = new BMessage(UPDATE_AUDIOTIMELINE_MSG);
	//message->AddPoint("Where", where);
	//Window()->PostMessage( message, (BHandler *)((TAudioEditor *)Window())->GetTimeline() );
	//delete message;
}


//---------------------------------------------------------------------
//	WindowActivated
//---------------------------------------------------------------------
//
//	Handle window activated events
//

void TAudioEditorView::WindowActivated(bool state)
{
}


//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key down event
//

void TAudioEditorView::KeyDown(const char *bytes, int32 numBytes)
{

	BView::KeyDown(bytes, numBytes);
}



//---------------------------------------------------------------------
//	KeyDown
//---------------------------------------------------------------------
//
//	Handle key up event
//

void TAudioEditorView::KeyUp(const char *bytes, int32 numBytes)
{
}


#pragma mark -
#pragma mark === Frame Handling ===


//---------------------------------------------------------------------
//	FrameMoved
//---------------------------------------------------------------------
//
//	Handle movement of frame
//

void TAudioEditorView::FrameMoved(BPoint new_position)
{

	//	Inform parent
	BView::FrameMoved(new_position);
}


//---------------------------------------------------------------------
//	FrameResized
//---------------------------------------------------------------------
//
//	Handle resize of frame
//

void TAudioEditorView::FrameResized(float new_width, float new_height)
{
	// For now, invalidate the whole screen and get a redraw filled to the
	// current frame size
	Invalidate();

	//	Inform parent
	BView::FrameResized(new_width, new_height);
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TAudioEditorView::MessageReceived(BMessage* message)
{
	switch (message->what)
	{

	case AUDIO_PLAY_BUTTON_MSG:
		break;

	case AUDIO_STOP_BUTTON_MSG:
		break;

	case AUDIO_ZOOMIN_BUTTON_MSG:
		ZoomIn();
		break;

	case AUDIO_ZOOMOUT_BUTTON_MSG:
		ZoomOut();
		break;

	//  Draw playback indicator
	case UPDATE_AUDIOINDICATOR_MSG:
	{
		BPoint drawPt;
		message->FindPoint("IndicatorPoint", &drawPt);
		DrawIndicatorTick(drawPt);
	}
	break;
	}
}


#pragma mark -
#pragma mark === Clipping Functions ===

//---------------------------------------------------------------------
//	Clip16Bit
//---------------------------------------------------------------------
//
//	Clip a value within the 16 bit boundary

void TAudioEditorView::Clip16Bit(int32 *theValue)
{

	if (*theValue > 32767)
		*theValue = 32767;

	if (*theValue < -32768)
		*theValue = -32768;
}


#pragma mark -
#pragma mark === Drawing Functions ===

//---------------------------------------------------------------------
//	Draw
//---------------------------------------------------------------------
//
//

void TAudioEditorView::Draw(BRect updateRect)
{
	// Set up environment
	PushState();
	SetPenSize(0.0);

	//const BRect bounds = Bounds();

	// Do we need to update the cached waveform?
	if (fUpdatePreview) {
		// Clean up old preview
		if (fPreviewBitmap) {
			fPreviewBitmap->Lock();
			BView *oldView = fPreviewBitmap->ChildAt(0);
			fPreviewBitmap->RemoveChild(oldView);
			delete oldView;
			delete fPreviewBitmap;
		}

		// Create preview bitmap the size of the view
		fPreviewBitmap = new BBitmap( updateRect, B_CMAP8, true);

		// Create preview view
		BView *previewView = new BView(updateRect, "PreviewView", B_FOLLOW_ALL, 0);
		fPreviewBitmap->AddChild(previewView);

		// Draw waveform into bitmap
		CreateWaveFormCache(previewView, updateRect);
		previewView->Looper()->Lock();
		previewView->Sync();
		previewView->Looper()->Unlock();

		fUpdatePreview = false;
	}

	// Draw the cached waveform bitmap
	BPoint drawPt(updateRect.left, updateRect.top);
	DrawBitmap(fPreviewBitmap, updateRect, updateRect);

	// Restore environment
	PopState();
}


//---------------------------------------------------------------------
//	DrawIndicatorTick
//---------------------------------------------------------------------
//
//	This draws the playback indicator tick mark at the appropriate
//	location.  It also cleans up the last postion of the tick mark.
//

void TAudioEditorView::DrawIndicatorTick(BPoint drawPt)
{
	BPoint startPt, endPt;
	BRect bounds = Bounds();

	// Set up the environment
	rgb_color saveColor = HighColor();
	drawing_mode saveMode = DrawingMode();
	float oldPenSize = PenSize();
	SetHighColor(kBlack);
	SetDrawingMode(B_OP_INVERT);

	// Save point into tracking member variable points
	fTickPt = drawPt;

	// If point is at new location, go ahead and draw
	if (fTickPt.x != fOldTickPt.x) {
		// Erase last indicator tick. Clip out the outline lines
		startPt.Set(fOldTickPt.x, bounds.top+1);
		endPt.Set(fOldTickPt.x, bounds.bottom-1);
		StrokeLine(startPt, endPt);

		// Draw the new indicator tick. Clip out the outline lines
		startPt.Set(fTickPt.x, bounds.top+1);
		endPt.Set(fTickPt.x, bounds.bottom-1);
		StrokeLine(startPt, endPt);

		// Save tick location for next compare
		fOldTickPt = fTickPt;
	}

	// Restore environment
	SetPenSize(oldPenSize);
	SetDrawingMode(saveMode);
	SetHighColor(saveColor);
}


//---------------------------------------------------------------------
//	SetAudioViewBounds
//---------------------------------------------------------------------
//
//	Resize view based on sound file attributes
//

void TAudioEditorView::SetAudioViewBounds()
{
	BPoint insertPt;

	// Set up bounds...
	BRect bounds = Bounds();
	bounds.right = fParent->fNumSamples / fParent->fSamplesPerPixel;

	// Resize toolbar and view...
	ResizeTo( bounds.Width(), bounds.Height());
	fParent->fTimeline->SetTimelineViewBounds(bounds);

	// Adjust scroll bars
	fParent->fHScroll->SetRange( 0, bounds.Width() );
	//fParent->fHScroll->SetProportion(1.0);
	//fVScroll
}


//---------------------------------------------------------------------
//	CreatePreview
//---------------------------------------------------------------------
//
//	Render cached waveform view
//

void TAudioEditorView::CreateWaveFormCache(BView *previewView, const BRect bounds)
{
	BPoint startPt, endPt;

	previewView->Looper()->Lock();

	PushState();

	// Fill background
	previewView->SetHighColor(kLightGrey);
	previewView->FillRect( Bounds() );

	// Set pen color
	previewView->SetHighColor(kDarkGrey);

	/*
	   // Get middle point of view
	   float viewMiddle = previewView->Bounds().Height() / 2;

	   // Don't draw past end of file
	   if (fParent->fSamplesPerPixel * (bounds.left + 1) < fParent->fNumSamples)
	   {

	        // Get pointer to sound file
	        BSoundFile *soundFile = fParent->fEngine->fSoundFile;

	        // Create buffer the size of the number of pixels per sample * the sounds frame size
	        int32 bufferSize = fParent->fSamplesPerPixel * soundFile->FrameSize();
	        char *soundBuffer = (char *)malloc(bufferSize);

	        // Save current position, rewind the buffer and seek to proper frame position
	        off_t frameIndex = fParent->fEngine->fSoundFile->FrameIndex();
	        off_t seekVal = soundFile->SeekToFrame( bounds.left * bufferSize);
	        if ( seekVal >= 0 )
	        {

	                // reset our horizontal position
	                int32 hPos = bounds.left;

	                // Draw the waveform from left to right
	                for (int32 currentPos = bounds.left; currentPos < bounds.right; currentPos++)
	                {
	                        // Exit if we have passed the end of the file
	                        if ( fParent->fSamplesPerPixel * (currentPos + 1) < fParent->fNumSamples)
	                        {

	                                // Read data into buffer
	                                status_t framesRead = soundFile->ReadFrames(soundBuffer, bufferSize);

	                                // Go through each sample at this pixel and find the high and low points
	                                int32 low, high;
	                                low  =  32767;
	                                high = -32768;

	                                // Make sure we have some data
	                                if (framesRead > 0)
	                                {
	                                        if ( fParent->fSampleSize == 1)
	                                        {
	                                                // Go through each sample at this pixel and find the high and low points
	                                                char *ptr       = (char *)soundBuffer;
	                                                int32 endPt =  fParent->fSamplesPerPixel * fParent->fNumChannels;

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
	                                                int16 *ptr      = (int16 *)soundBuffer;
	                                                int32 endPt =  fParent->fSamplesPerPixel * fParent->fNumChannels;

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
	                                previewView->StrokeLine(startPt, endPt);

	                                // Increment horizontal position
	                                hPos++;
	                        }
	                }
	        }

	        // Free buffer
	        free(soundBuffer);

	        // Restore file position
	        soundFile->SeekToFrame(frameIndex);
	   }

	   // Draw center divider line
	   previewView->SetHighColor(kBlack);
	   previewView->MovePenTo(bounds.left, viewMiddle);
	   endPt.Set(bounds.right, viewMiddle);
	   previewView->StrokeLine(endPt);
	 */

	// Draw selections
	//LInvertRect(&selectionRect);

	//if (insertOn)
	//	DrawInsert();

	//if (itsEditor->isPlaying)
	//	DrawPlayLine(lastPos);

	PopState();
}

#pragma mark -
#pragma mark === Utility Functions ===


//---------------------------------------------------------------------
//	PixelsToSamples
//---------------------------------------------------------------------
//
//	Coverts samples to pixels
//

int32 TAudioEditorView::SamplesToPixels(int32 theSamples)
{
	return (theSamples / fParent->fSamplesPerPixel);
}


//---------------------------------------------------------------------
//	PixelsToSamples
//---------------------------------------------------------------------
//
//	Coverts pixels to samples
//

int32 TAudioEditorView::PixelsToSamples(int32 thePixels)
{
	return (thePixels * fParent->fSamplesPerPixel);
}



//---------------------------------------------------------------------
//	ZoomIn
//---------------------------------------------------------------------
//
//	Zoom in on wave and adjust size of view and timeline
//

void TAudioEditorView::ZoomIn()
{
	if ( fParent->fSamplesPerPixel > 1) {
		// Adjust samples per pixel
		fParent->fSamplesPerPixel /= 2;

		// Resize view and toolbar
		SetAudioViewBounds();

		// Scroll to proper position
		//itsPane->GetPosition(&lp);
		//lp.h *= 2;
		//itsPane->ScrollTo(&lp, FALSE);

		// Force redraw and create main bitmap
		fUpdatePreview = true;
	}
}


//---------------------------------------------------------------------
//	ZoomOut
//---------------------------------------------------------------------
//
//	Zoom out on wave and adjust size of view and timeline
//

void TAudioEditorView::ZoomOut()
{
	// Zoom out if only the frame bounds is larger than the window bounds...
	float winWidth = Window()->Bounds().Width();
	float frameWidth = Frame().Width();

	if ( frameWidth >= winWidth ) {
		//if ( Window()->Bounds().Width() > Frame().Width())
		// Adjust sample per pixel
		fParent->fSamplesPerPixel *= 2;

		// Resize view and toolbar width
		SetAudioViewBounds();

		// Halve the current position to retain the same position at the right
		// edge of the window.
		//itsPane->GetPosition(&lp);
		//lp.h /= 2;
		//itsPane->ScrollTo(&lp, FALSE);

		// Force redraw and create main bitmap
		fUpdatePreview = true;

	}
}