//---------------------------------------------------------------------
//
//	File:	TTransition.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.03.98
//
//	Desc:	Cue transition routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <algorithm>
// ABH #include <algobase.h>			// for min/max
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "DebugUtils.h"
#include "AppUtils.h"

#include "MuseumApp.h"
#include "TVisualCue.h"
#include "TCueChannel.h"
#include "TStageView.h"

#include "TTransition.h"

// Transition arrays

//const int16 kNumTransitions = 49;		// Number of transitions

TransitionCallback	transitionInTable[] =
{
	// In Transitions
	NULL, WipeRightIn, WipeLeftIn, WipeDownIn, WipeUpIn, WipeTopLeftToBottomRightIn,
	WipeTopRightToBottomLeftIn, WipeBottomLeftToTopRightIn, WipeBottomRightToTopLeftIn,
	RevealRightIn, RevealLeftIn, RevealDownIn, RevealUpIn,
	RevealTopLeftBottomRightIn, RevealTopRightBottomLeftIn, RevealBottomLeftTopRightIn, RevealBottomRightTopLeftIn,
	CurtainsInIn, CurtainsOutIn,

	// Out Transitions
	WipeRightOut, WipeLeftOut, WipeDownOut, WipeUpOut, WipeTopLeftToBottomRightOut,
	WipeTopRightToBottomLeftOut, WipeBottomLeftToTopRightOut, WipeBottomRightToTopLeftOut,
	RevealRightOut, RevealLeftOut, RevealDownOut, RevealUpOut,
	RevealTopLeftBottomRightOut, RevealTopRightBottomLeftOut, RevealBottomLeftTopRightOut, RevealBottomRightTopLeftOut,
	CurtainsInOut, CurtainsOutOut,

	/* IrisCircle, IrisOval, IrisSquare, IrisRectangle,
	BlindsRight, BlindsDown, BlindsLeft, BlindsUp,
	IrisCircleIn, IrisOvalIn, IrisSquareIn, IrisRectangleIn,
	IrisDiamond, IrisDiamondIn, WipeClockwise, WipeCounterClockwise,
	CourseDissolve4, FineDissolve,
	ZoomRight, ZoomDown, ZoomLeft, ZoomUp, ZoomOut,
	NarrowHStrips, WideHStrips, NarrowVStrips, WideVStrips,
	FadeBlack, FadeWhite, CourseDissolve1, CourseDissolve2,
	CourseDissolve3, CourseDissolve4, CourseDissolve5,
	CourseDissolve6, CourseDissolve7, CourseDissolve8,
	CourseDissolve9,*/ NULL
};


// Constants
#define UPDATE_TRANSITION_MSG	'UpTr'

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Initialize the object.  We save the source and destination bitmaps
//	as member variables so they are acessible to the transition callback.
//

TTransition::TTransition( TVisualCue *srcView, BView *dstView, TStageView *theStage, int16 transitionID, uint32 duration, bool transitionIn)
{
	fDstBitmap = NULL;

	ASSERT(srcView);
	ASSERT(dstView);

	// Set up class variables
	fSrcView 			= srcView;
	fDstView 			= dstView;
	fStage				= theStage;
	fID				= transitionID;
	fDuration 			= duration;
	fStartTime			= 0;
	fSrcBitmap 		= srcView->GetBitmap();
	fIsTransitionIn	= transitionIn;

	// Clear region
	fTransitionRegion.MakeEmpty();

	//	gzr: to do... The transitions are pulled from an array of transition procs.  We need
	//	to modify this to work with an add-on architecture
	fTransitionCallback = transitionInTable[fID];
}



//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTransition::~TTransition()
{
	Stop();

	// Free composite
	if (fDstBitmap)
	{
		delete fDstBitmap;
		fDstBitmap = NULL;
	}
}

// ABH missing functions
#ifdef ABH
bool TTransition::HasTransitionIn(){
	return fIsTransitionIn;	// ABH is this right???
}

bool TTransition::HasTransitionOut(){
	return fIsTransitionOut;
}

#endif

//---------------------------------------------------------------------
//	Init()
//---------------------------------------------------------------------
//
//	Initialize transition

void TTransition::Init()
{

	BRect	r1 = fSrcRect;
	BRect 	r2 = fDstRect;

	fIsDone = false;
}


//---------------------------------------------------------------------
//	SetStartTime()
//---------------------------------------------------------------------
//
//	Set up transition start time

void TTransition::SetStartTime(uint32 startTime)
{
	fStartTime = startTime;
}


//---------------------------------------------------------------------
//	SetDuration()
//---------------------------------------------------------------------
//
//	Set up transition duration

void TTransition::SetDuration(uint32 duration)
{
	fDuration = duration;
}


//---------------------------------------------------------------------
//	SetTransitionID
//---------------------------------------------------------------------
//
//	Set the ID and callnack of the transiiton object
//

void TTransition::SetTransitionID(int32 theID)
{
	fID = theID;
	fTransitionCallback = transitionInTable[fID];
}


//---------------------------------------------------------------------
//	Stop
//---------------------------------------------------------------------
//
//	Stop the transition
//

void TTransition::Stop()
{
	fIsDone 	= true;
	fStartTime = 0;
}


//---------------------------------------------------------------------
//	Reset
//---------------------------------------------------------------------
//
//	Reset the transition
//

void TTransition::Reset()
{
	fIsDone 	= false;
	fStartTime = 0;
}


//---------------------------------------------------------------------
//	TransitionTask
//---------------------------------------------------------------------
//
//	Do the actual transition
//

void TTransition::TransitionTask()
{

	if (fIsDone == true)
		return;

	uint32 startTime 	= fStartTime;
	uint32 currentTime 	= GetCurrentTime();
	uint32 taskTime 	= currentTime - startTime;
	uint32 endTime 		= fStartTime + fDuration;

	// percentDone is on a scale of 0 to 1000.  Check for overflow...
	int32  percentDone;

	if ( currentTime < endTime)
		percentDone = taskTime * 1000L / fDuration;
	else
		percentDone = 1001;

	if (percentDone > 1000)
		percentDone = 1000;

	// Sanity check. Make sure negative values do not get through...
	if (percentDone < 0)
	{
		fIsDone = true;
		Reset();
		return;
	}

	// Create composite for transition out
	//if (fIsTransitionIn == false)
	//{
		//if (fDstBitmap)
		//	delete fDstBitmap;

		//fDstBitmap = fStage->CreateComposite(1, fSrcView->GetChannel()->GetID(), currentTime, fSrcView->GetCroppedArea());
		//ASSERT(fDstBitmap);
	//}

	// Check for completions
	if (percentDone == 1000)
	{
		fTransitionCallback(fSrcView, fDstView, fSrcBitmap, fDstBitmap, &fTransitionRegion, percentDone, END_TRANSITION);
		fIsDone = true;
		Reset();
	}
	else
	{
		// Do the transition
		fTransitionCallback(fSrcView, fDstView, fSrcBitmap, fDstBitmap, &fTransitionRegion, percentDone, RUN_TRANSITION);
	}
}


//---------------------------------------------------------------------
//	DrawData
//---------------------------------------------------------------------
//
//	Draw transition to offscreen
//

void TTransition::DrawData(uint32 theTime, BView *offView)
{

	if (fIsDone == true)
		return;

	uint32 startTime = fStartTime;
	uint32 taskTime  = theTime - startTime;
	uint32 endTime 	 = fStartTime + fDuration;

	// percentDone is on a scale of 0 to 1000.  Check for overflow...
	int32  percentDone;

	if ( theTime < endTime)
		percentDone = taskTime * 1000L / fDuration;
	else
		percentDone = 1001;

	if (percentDone > 1000)
		percentDone = 1000;

	// Sanity check. Make sure negative values do not get through...
	if (percentDone < 0)
	{
		fIsDone = true;
		Reset();
		return;
	}

	// Create composite for transition out
	if (fIsTransitionIn == false)
	{
		//if (fDstBitmap)
		//	delete fDstBitmap;

		//fDstBitmap = fStage->CreateComposite(1, fSrcView->GetChannel()->GetID(), theTime, fSrcView->GetCroppedArea());
		//ASSERT(fDstBitmap);
	}

	// Check for completions
	if (percentDone == 1000)
	{
		fTransitionCallback(fSrcView, fDstView, fSrcBitmap, fDstBitmap, &fTransitionRegion, percentDone, END_TRANSITION);
		fIsDone = true;
		Reset();
	}
	else
	{
		// Do the transition
		fTransitionCallback(fSrcView, fDstView, fSrcBitmap, fDstBitmap, &fTransitionRegion, percentDone, RUN_TRANSITION);
	}
}

#pragma mark -
#pragma mark === Transition Utility Procedures ===

//---------------------------------------------------------------------
//	TransitionDrawBitmap
//---------------------------------------------------------------------
//
//	Special TransitionDrawBitmap for transition drawing.  We need to
//	handle opacity, etc.
//

void TransitionDrawBitmap(BBitmap *bitmap, TVisualCue *srcView, BView *dstView, BRect srcRect, BRect dstRect)
{
	// Determine if we need to do a straight blit or use transparency
	if ( static_cast<TVisualCue *>(srcView)->GetTransparency() == 1.0)
	{
		// Draw bitmap
		dstView->Looper()->Lock();
		dstView->PushState();

		dstView->DrawBitmap(bitmap, srcRect, dstRect);
		dstView->Sync();

		dstView->PopState();
		dstView->Looper()->Unlock();
	}
	else
	{


	}
}

#pragma mark -
#pragma mark === Transition In Procedures ===

//---------------------------------------------------------------------
//	WipeRightIn
//---------------------------------------------------------------------
//
//	Wipe the transition in from the right side of the stage cue to the left
//

void	WipeRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{

	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	= srcRect.left + srcRect.Width() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	= dstRect.left+srcRect.Width();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;
	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeDown
//---------------------------------------------------------------------
//
//	Wipe the transition in from the top side of the stage cue to the bottom
//

void WipeDownIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.bottom 	= srcRect.top + srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.bottom 	= dstRect.top + srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeLeft
//---------------------------------------------------------------------
//
//	Wipe the transition in from the top side of the stage cue to the bottom
//

void WipeLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{

				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	= srcRect.right - srcRect.Width() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	= dstRect.right - srcRect.Width();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}



void WipeUpIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.top 	= srcRect.bottom - srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.top 	= dstRect.bottom - srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeTopLeftToBottomRight
//---------------------------------------------------------------------
//
//
//

void WipeTopLeftToBottomRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{

	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	= srcRect.left + srcRect.Width() * percentDone / 1000;
				srcRect.bottom 	= srcRect.top + srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	= dstRect.left+srcRect.Width();
				dstRect.bottom 	= dstRect.top+srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeTopRightToBottomLeft
//---------------------------------------------------------------------
//
//
//

void WipeTopRightToBottomLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	= srcRect.right - srcRect.Width() * percentDone / 1000;
				srcRect.bottom 	= srcRect.top + srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	= dstRect.right-srcRect.Width();
				dstRect.bottom 	= dstRect.top+srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	WipeBottomLeftToTopRight
//---------------------------------------------------------------------
//
//
//

void WipeBottomLeftToTopRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	= srcRect.left + srcRect.Width() * percentDone / 1000;
				srcRect.top 	= srcRect.bottom - srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	= dstRect.left + srcRect.Width();
				dstRect.top 	= dstRect.bottom - srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeBottomRightToTopLeft
//---------------------------------------------------------------------
//
//
//

void WipeBottomRightToTopLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	= srcRect.right - srcRect.Width() * percentDone / 1000;
				srcRect.top 	= srcRect.bottom - srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	= dstRect.right - srcRect.Width();
				dstRect.top 	= dstRect.bottom - srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealRight
//---------------------------------------------------------------------
//
//

void RevealRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{

	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	= srcRect.right - srcRect.Width() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	= dstRect.left+srcRect.Width();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;
	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealDown
//---------------------------------------------------------------------
//
//

void RevealDownIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.top 	= srcRect.bottom - srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.bottom 	= dstRect.top+srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);

			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	RevealLeft
//---------------------------------------------------------------------
//
//

void RevealLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	= srcRect.left + srcRect.Width() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	= dstRect.right - srcRect.Width();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
		}

		default:
			break;
	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	RevealUp
//---------------------------------------------------------------------
//
//

void RevealUpIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.bottom 	= srcRect.top + srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.top 	= dstRect.bottom - srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	RevealTopLeftBottomRight
//---------------------------------------------------------------------
//
//

void RevealTopLeftBottomRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{

	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	= srcRect.right - srcRect.Width() * percentDone / 1000;
				srcRect.top 	= srcRect.bottom - srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 		= srcView->GetDrawArea();
				dstRect.right 		= dstRect.left + srcRect.Width();
				dstRect.bottom 		= dstRect.top + srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealTopRightBottomLeft
//---------------------------------------------------------------------
//
//

void RevealTopRightBottomLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	= srcRect.left + srcRect.Width() * percentDone / 1000;
				srcRect.top 	= srcRect.bottom - srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	= dstRect.right - srcRect.Width();
				dstRect.bottom 	= dstRect.top + srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealBottomLeftTopRight
//---------------------------------------------------------------------
//
//

void RevealBottomLeftTopRightIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	= srcRect.right - srcRect.Width() * percentDone / 1000;
				srcRect.bottom 	= srcRect.top + srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	= dstRect.left + srcRect.Width();
				dstRect.top 	= dstRect.bottom - srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	RevealBottomRightTopLeft
//---------------------------------------------------------------------
//
//

void RevealBottomRightTopLeftIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	= srcRect.left + srcRect.Width() * percentDone / 1000;
				srcRect.bottom 	= srcRect.top + srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 		= srcView->GetDrawArea();
				dstRect.left 		= dstRect.right - srcRect.Width();
				dstRect.top 		= dstRect.bottom - srcRect.Height();

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	CurtainsIn
//---------------------------------------------------------------------
//
//
//

void CurtainsInIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
}


//---------------------------------------------------------------------
//	CurtainsOut
//---------------------------------------------------------------------
//
//

void CurtainsOutIn(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			{
				// Draw entire bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcView->GetCroppedArea(), srcView->GetDrawArea());
			}
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();

				int32 midPt 	= srcRect.Width() / 2;
				srcRect.left 	= midPt - (srcRect.Width() / 2) * percentDone / 1000;
				srcRect.right 	= midPt + (srcRect.Width() / 2) * percentDone / 1000;
				srcRect.bottom 	= srcRect.top + srcRect.Height();

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	= midPt - (srcRect.Width() / 2);
				dstRect.right 	= midPt + (srcRect.Width() / 2);

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);

			}
			break;

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


#pragma mark -
#pragma mark === Transition Out Procedures ===

//---------------------------------------------------------------------
//	WipeRightOut
//---------------------------------------------------------------------
//
//	Wipe the transition in from the right side of the stage cue to the left
//

void WipeRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{

	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	+=  srcRect.Width() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	+= dstRect.Width() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;
	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeDownOut
//---------------------------------------------------------------------
//
//	Wipe the transition out from the top side of the stage cue to the bottom
//

void WipeDownOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.top 	+=  srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.top 	+= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeLeftOut
//---------------------------------------------------------------------
//
//	Wipe the transition out from the right side of the stage to the left
//

void WipeLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{

				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	-=  srcRect.Width() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	-= dstRect.Width() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeUpOut
//---------------------------------------------------------------------
//
//	Wipe the transition out from the bottom the stage to the top
//

void WipeUpOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.bottom 	-= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.bottom 	-= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeTopLeftToBottomRightOut
//---------------------------------------------------------------------
//
//
//

void WipeTopLeftToBottomRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{

	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	+= srcRect.Width() * percentDone / 1000;
				srcRect.top 	+= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	+= dstRect.Width() * percentDone / 1000;
				dstRect.top 	+= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);

			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeTopRightToBottomLeftOut
//---------------------------------------------------------------------
//
//
//

void WipeTopRightToBottomLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	-= srcRect.Width() * percentDone / 1000;
				srcRect.top 	+= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	-= dstRect.Width() * percentDone / 1000;
				dstRect.top 	+= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	WipeBottomLeftToTopRightOut
//---------------------------------------------------------------------
//
//
//

void WipeBottomLeftToTopRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	+= srcRect.Width() * percentDone / 1000;
				srcRect.bottom 	-= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	+= dstRect.Width() * percentDone / 1000;
				dstRect.bottom 	-= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}
			break;

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	WipeBottomRightToTopLeftOut
//---------------------------------------------------------------------
//
//
//

void WipeBottomRightToTopLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	-= srcRect.Width() * percentDone / 1000;
				srcRect.bottom 	-= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	 = srcView->GetDrawArea();
				dstRect.right 	-= dstRect.Width() * percentDone / 1000;
				dstRect.bottom 	-= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealRightOut
//---------------------------------------------------------------------
//
//

void RevealRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{

	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	-= srcRect.Width() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	+= dstRect.Width() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;
	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealDownOut
//---------------------------------------------------------------------
//
//

void RevealDownOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.bottom 	-= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.top 	+= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	RevealLeftOut
//---------------------------------------------------------------------
//
//

void RevealLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	+= srcRect.Width() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	-= dstRect.Width() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;
	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealUpOut
//---------------------------------------------------------------------
//
//

void RevealUpOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.top 	+= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.bottom 	-= dstRect.Height() * percentDone / 1000;


				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}

//---------------------------------------------------------------------
//	RevealTopLeftBottomRightOut
//---------------------------------------------------------------------
//
//

void RevealTopLeftBottomRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	-= srcRect.Width() * percentDone / 1000;
				srcRect.bottom 	-= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	+= dstRect.Width() * percentDone / 1000;
				dstRect.top 	+= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealTopRightBottomLeftOut
//---------------------------------------------------------------------
//
//

void RevealTopRightBottomLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	+= srcRect.Width() * percentDone / 1000;
				srcRect.bottom 	-= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	-= dstRect.Width() * percentDone / 1000;
				dstRect.top 	+= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealBottomLeftTopRightOut
//---------------------------------------------------------------------
//
//

void RevealBottomLeftTopRightOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.right 	-= srcRect.Width() * percentDone / 1000;
				srcRect.top 	+= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.left 	+= dstRect.Width() * percentDone / 1000;
				dstRect.bottom 	-= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);

			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	RevealBottomRightTopLeftOut
//---------------------------------------------------------------------
//
//

void RevealBottomRightTopLeftOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();
				srcRect.left 	+= srcRect.Height() * percentDone / 1000;
				srcRect.top 	+= srcRect.Height() * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				dstRect.right 	-= dstRect.Height() * percentDone / 1000;
				dstRect.bottom 	-= dstRect.Height() * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}


//---------------------------------------------------------------------
//	CurtainsInOut
//---------------------------------------------------------------------
//
//
//

void CurtainsInOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
}


//---------------------------------------------------------------------
//	CurtainsOutOut
//---------------------------------------------------------------------
//
//

void CurtainsOutOut(TVisualCue *srcView, BView *dstView, BBitmap *srcBitmap, BBitmap *dstBitmap, BRegion *theRegion, int16 percentDone, int16 selector )
{
	// Lock Views
	srcView->Looper()->Lock();
	dstView->Looper()->Lock();

	switch(selector)
	{
		case START_TRANSITION:
			break;

		case END_TRANSITION:
			break;

		case RUN_TRANSITION:
			{
				// Set up source rectangle
				BRect srcRect 	= srcView->GetCroppedArea();

				int32 midPt 	= srcRect.Width() / 2;
				srcRect.left 	= midPt - (srcRect.Width() / 2);
				srcRect.right 	= midPt + (srcRect.Width() / 2);

				srcRect.left 	+= midPt - (srcRect.Width() / 2) * percentDone / 1000;
				srcRect.right 	-= midPt + (srcRect.Width() / 2) * percentDone / 1000;

				// Set up destination rectangle
				BRect dstRect 	= srcView->GetDrawArea();
				//dstRect.left 	= midPt - (srcRect.Width() / 2);
				//dstRect.right 	= midPt + (srcRect.Width() / 2);

				dstRect.left 	+= midPt - (dstRect.Width() / 2) * percentDone / 1000;
				dstRect.right 	-= midPt + (dstRect.Width() / 2) * percentDone / 1000;

				// Draw bitmap
				TransitionDrawBitmap(srcView->GetBitmap(), srcView, dstView, srcRect, dstRect);
			}
			break;

		default:
			break;

	}

	// Unlock everyone
	srcView->Looper()->Unlock();
	dstView->Looper()->Unlock();
}
