//---------------------------------------------------------------------
//
//	File:	TVisualEffect.cpp
//
//	Author:	Mike Ost
//
//	Date:	10.13.98
//
//	Desc:
//
//	This is a subclass of TCueEffect which supports effects that
//	change a cue's bitmap over time.
//
//---------------------------------------------------------------------

#include "BuildApp.h"

#include "TAudibleEffect.h"

#include <support/Debug.h>              // for ASSERT()
#include "TCueData.h"           // For IsAudible()

//---------------------------------------------------------------------
//	Constructor/destructor
//---------------------------------------------------------------------
//

TAudibleEffect::TAudibleEffect() :
	TCueEffect()
{
}

TAudibleEffect::~TAudibleEffect()
{
}

TAudibleEffect::TAudibleEffect(BMessage* msg) :
	TCueEffect(msg)
{
}

//---------------------------------------------------------------------
// BArchivable overrides (file support)
//---------------------------------------------------------------------
//
//

BArchivable* TAudibleEffect::Instantiate(BMessage* data)
{
	// NOTE: this is an error. We can't instantiate this class
	// since it is an abstract base class.
	ASSERT(false);
	return 0;
}

status_t TAudibleEffect::Archive(BMessage* data, bool deep) const
{
	// Start by calling inherited archive
	status_t err = BArchivable::Archive(data, deep);
	if (err != B_OK)
		return err;

	// NOTE: we don't instantiate this class --- it's an ABC ---
	// so we don't add the class name data

	// NOTE: add local data here
	return B_OK;
}

//---------------------------------------------------------------------
// WorksWith
//---------------------------------------------------------------------
//
//

bool TAudibleEffect::WorksWith(TCueData* cue)
{
	return cue && cue->IsAudible();
}

//---------------------------------------------------------------------
// Apply
//---------------------------------------------------------------------
//
//

bool TAudibleEffect::Apply(int32 time, const char* source, char* target)
{
	// Default: do nothing
	return false;
}
