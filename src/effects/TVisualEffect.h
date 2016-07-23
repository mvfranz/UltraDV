//---------------------------------------------------------------------
//
//	File:	TVisualEffect.h
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

#if !defined(TVISUALEFFECT_H)
#define TVISUALEFFECT_H

// declarations
#include  "TCueEffect.h"

class TCueData;
class TCuePosition;
	
class TVisualEffect : public TCueEffect {
public:
		// create
	TVisualEffect();
	TVisualEffect(BMessage* msg);
		// delete
	virtual ~TVisualEffect();

	//----------------------
	// BArchivable overrides (file support)
	static BArchivable* Instantiate(BMessage* data);
	// TODO: to instantiate we will need to look in an add-on image.
	// Add that to the message somehow?
	virtual status_t Archive(BMessage* data, bool deep) const;

	//----------------------
	// Static Class Behavior
		
		// Returns true if cue->IsVisual()
	virtual bool WorksWith(const TCueData* cue) const;

	//----------------------
	// Specific Behavior

	//	Effect quality types
	enum EffectsQuality
	{
		kWireframeQuality,
		kPreviewQuality,
		kBetterQuality,
		kBestQuality
	};
	
	// Do the effect
		// Return a transformed version of 'bits' at 'time'. Caller
		// takes ownership of new'd BBitmap return value. Return
		// zero if nothing need be done. 
	virtual BBitmap* TransformBitmap(uint32 time, const BBitmap* source,
			const TCuePosition& registration, DisplayQuality quality);
			
protected:

	// Generic support methods
		// Return the size of a pixel based on one of the Be pixel 
		// size constants (e.g., B_RGB16) given the color space. 
		// Or -1 if the size is unsupported.
	int32 GetBitmapPixelSize(color_space colorSpace) const;
};

#endif		// !TVISUALEFFECT_H
