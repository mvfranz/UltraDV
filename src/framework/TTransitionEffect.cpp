//---------------------------------------------------------------------
//
//	File:	TTransitionEffect.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	11.11.98
//
//	Desc:
//
//	This is a subclass of TTransitionEffect which supports effects that
//	change a cue's bitmap over time.
//
//---------------------------------------------------------------------

#include "BuildApp.h"

#include "AppTypes.h"

#include "TTransitionEffect.h"

#include <support/Debug.h>		// for ASSERT()
#include "TCuePosition.h"		// For class decl

//	Local Constants
const char kTransitionName[] = "BaseTransition";

//---------------------------------------------------------------------
//	Constructor/destructor
//---------------------------------------------------------------------
//

TTransitionEffect::TTransitionEffect(bool transitionIn) : TVisualEffect()
{
	//	Offscreen drawing
	fOffscreenBitmap 	= NULL;
	fOffscreenView 	= NULL;

	//	Are we an In or Out Transition
	fTransitionIn = transitionIn;

	//	Variable set when we initialize our offscreen
	fInited = false;
}

TTransitionEffect::TTransitionEffect(BMessage* msg) : TVisualEffect(msg)
{
}


TTransitionEffect::~TTransitionEffect()
{
	//	Free offscreen
	if (fOffscreenBitmap)
 	{
 		fOffscreenBitmap->Lock();
 		fOffscreenBitmap->RemoveChild(fOffscreenView);
 		delete fOffscreenView;
 		delete fOffscreenBitmap;
 	}
}


//---------------------------------------------------------------------
// BArchivable overrides (file support)
//---------------------------------------------------------------------
//
//

BArchivable* TTransitionEffect::Instantiate(BMessage* data)
{
	// NOTE: this is an error. We can't instantiate this class
	// since it is an abstract base class.
	ASSERT(false);
	return 0;
}

status_t TTransitionEffect::Archive(BMessage* data, bool deep) const
{
	// Start by calling inherited archive
	status_t err = TVisualEffect::Archive(data, deep);
	if (err != B_OK)
		return err;

	// NOTE: we don't instantiate this class --- it's an ABC ---
	// so we don't add the class name data

	// NOTE: add local data here
	return B_OK;
}


//---------------------------------------------------------------------
//	Name()
//---------------------------------------------------------------------
//
//	Return a name to identify the effect by.
//

string TTransitionEffect::Name() const
{
	return( string(kTransitionName));
}


//---------------------------------------------------------------------
//	Name()
//---------------------------------------------------------------------
//
//	Return a pointer to a bitmap which is to be used in the tools
// 	palette. Either the 'up' or 'down' version can be chosen.
// 	The base class version returns nothing.
//

const BBitmap *TTransitionEffect::CueIcon(bool up) const
{
	return NULL;
}


//---------------------------------------------------------------------
//	NewEffectState()
//---------------------------------------------------------------------
//
//	Have the subclass create a new effect state
//

TEffectState *TTransitionEffect::NewEffectState() const
{
	return NULL;
}


//---------------------------------------------------------------------
//	Interpolate()
//---------------------------------------------------------------------
//
//	Have the subclass interpolate a reasonable value given
//	a key frame iterator. Use neighboring key frames to
//	figure out a good value.
//

void TTransitionEffect::Interpolate(TKeyFrameIterator kf) const
{

}

#pragma mark -
#pragma mark === Bitmap Transformation ===

//---------------------------------------------------------------------
// TransformBitmap
//---------------------------------------------------------------------
//
//	Return a transformed version of 'bits' at 'time'. Caller
// 	takes ownership of new'd BBitmap return value. Return
// 	zero if nothing need be done.
//

BBitmap *TTransitionEffect::TransformBitmap(uint32 time, const BBitmap* source,
			const TCuePosition& registration, DisplayQuality quality)
{
	return 0;
}


#pragma mark -
#pragma mark === Offscreen Handling ===

//---------------------------------------------------------------------
//	InitOffscreen
//---------------------------------------------------------------------
//
//	Set up offscreen during first pass
//

void TTransitionEffect::InitOffscreen(BRect offRect)
{

	//	Only create offscreen at first pass
	if (!fInited)
	{
		// Create offscreen bitmap and view
		fOffscreenBitmap 	= new BBitmap(offRect, B_RGB_32_BIT, true);
		fOffscreenView 	= new BView( fOffscreenBitmap->Bounds(), "TransitionOffscreen", B_FOLLOW_ALL, B_WILL_DRAW);

		// Add child and fill with background color
		fOffscreenBitmap->Lock();
		fOffscreenBitmap->AddChild(fOffscreenView);
		fOffscreenView->SetViewColor(B_TRANSPARENT_32_BIT);
		fOffscreenView->SetHighColor(B_TRANSPARENT_32_BIT);
		fOffscreenView->FillRect(fOffscreenView->Bounds());
		fOffscreenView->Sync();
		fOffscreenBitmap->Unlock();

		//	Set inited flag to true
		fInited = true;
	}
}


#pragma mark -
#pragma mark === Utilities ===

//---------------------------------------------------------------------
//	Icon
//---------------------------------------------------------------------
//
//	Return descriptive icon for transition
//

BBitmap *TTransitionEffect::Icon()
{
	return NULL;
}