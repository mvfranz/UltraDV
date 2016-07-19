//---------------------------------------------------------------------
//
//	File:	TTransformEffect.cpp
//
//	Author:	Mike Ost
//
//	Date:	10.13.98
//
//	Desc:
//
//	This class supports the visual effects that work via the stage
//	tool palette. They not only modify a bitmap, but also transform
//	a cue's aspect and registration over time.
//
//	A support class, TCuePosition, contains five BPoints that describe
//	the cue's outline and registration. This class supports effects
//	that can alter those points.
//
//---------------------------------------------------------------------

#include <support/Debug.h>              // for ASSERT()

#include "AppTypes.h"

#include "TCueEffect.h"         // For class decl
#include "TCuePosition.h"               // For class decl

#include "TTransformEffect.h"

//---------------------------------------------------------------------
//	Constructor/destructor
//---------------------------------------------------------------------
//

TTransformEffect::TTransformEffect() :
	TVisualEffect()
{
}

TTransformEffect::~TTransformEffect()
{
}

TTransformEffect::TTransformEffect(BMessage* msg) :
	TVisualEffect(msg)
{
}

//---------------------------------------------------------------------
// BArchivable overrides (file support)
//---------------------------------------------------------------------
//
//

BArchivable* TTransformEffect::Instantiate(BMessage* data)
{
	// NOTE: this is an error. We can't instantiate this class
	// since it is an abstract base class.
	ASSERT(false);
	return 0;
}

status_t TTransformEffect::Archive(BMessage* data, bool deep) const
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
// TransformBitmap
//---------------------------------------------------------------------
//
//

bool TTransformEffect::TransformPosition(int32 time, TCuePosition* pos)
{
	// Default: do nothing
	return 0;
}
