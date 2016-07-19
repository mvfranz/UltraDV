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

#include <support/Debug.h>		// for ASSERT()

#include "AppTypes.h"
#include "TCueEffect.h"

#include "TCueData.h"			// For IsVisual()
#include "TCuePosition.h"		// For class decl

#include "TVisualEffect.h"

//---------------------------------------------------------------------
//	Constructor/destructor
//---------------------------------------------------------------------
//

TVisualEffect::TVisualEffect() :
	TCueEffect()
{
}

TVisualEffect::~TVisualEffect()
{
}

TVisualEffect::TVisualEffect(BMessage* msg) :
	TCueEffect(msg)
{
}

//---------------------------------------------------------------------
// BArchivable overrides (file support)
//---------------------------------------------------------------------
//
//

BArchivable* TVisualEffect::Instantiate(BMessage* data)
{
	// NOTE: this is an error. We can't instantiate this class
	// since it is an abstract base class.
	ASSERT(false);
	return 0;
}

status_t TVisualEffect::Archive(BMessage* data, bool deep) const
{
	// Start by calling inherited archive
	status_t err = TCueEffect::Archive(data, deep);
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

bool TVisualEffect::WorksWith(const TCueData* cue) const
{
	return cue && cue->IsVisual();
}

//---------------------------------------------------------------------
// TransformBitmap
//---------------------------------------------------------------------
//
//

BBitmap* TVisualEffect::TransformBitmap(uint32 time, const BBitmap* source,
			const TCuePosition& registration, DisplayQuality quality)
{
	// Default: do nothing
	return 0;
}

//---------------------------------------------------------------------
//	GetBitmapPixelSize
//---------------------------------------------------------------------
//
//	Return size of bitmap pixle based on color space

int32 TVisualEffect::GetBitmapPixelSize(color_space colorSpace) const
{
	switch(colorSpace)
	{
		// Unused and non-values
		case B_NO_COLOR_SPACE:
		case B_RGB24:
		case B_RGB24_BIG:
		case B_YUV422:
		case B_YUV411:
		case B_YUV420:
		case B_YUV444:
		case B_YUV9:
		case B_YUV12:
			return(-1);
			break;

		// 32-bit
		case B_RGB32:
		case B_RGBA32:
		case B_RGB32_BIG:
		case B_RGBA32_BIG:
			return sizeof(uint32);
			break;

		// 16-bit
		case B_RGB16:
		case B_RGB15:
		case B_RGBA15:
		case B_RGB16_BIG:
		case B_RGB15_BIG:
		case B_RGBA15_BIG:
			return sizeof(uint16);
			break;


		// 8-bit
		case B_CMAP8:
		case B_GRAY8:
			return sizeof(uchar);
			break;

		// Monochrome
		case B_GRAY1:
			return sizeof(uchar);
			break;

		default:
			return(-1);
			break;

	}
}
