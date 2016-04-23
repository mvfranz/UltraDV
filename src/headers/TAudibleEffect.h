#ifndef TAUDIBLEEFFECT_H
#define TAUDIBLEEFFECT_H
//---------------------------------------------------------------------
//
//	File:	TAudibleEffect.h
//
//	Author:	Mike Ost
//
//	Date:	10.16.98
//
//	Desc:
//
//	This class supports effects which are applied to audible data:
//	MIDI and digital audio. 
//
//---------------------------------------------------------------------

// This is required to make the vector class work...
#define MSIPL_DEF_TEMPARG

// declarations
#include "TCueEffect.h"
class TCueData;

class TAudibleEffect : public TCueEffect {
public:
		// create
	TAudibleEffect();
	TAudibleEffect(BMessage* msg);
		// delete
	virtual ~TAudibleEffect();

	//----------------------
	// BArchivable overrides (file support)
	static BArchivable* Instantiate(BMessage* data);
	// TODO: to instantiate we will need to look in an add-on image.
	// Add that to the message somehow?
	virtual status_t Archive(BMessage* data, bool deep) const;

	//----------------------
	// Static Class Behavior
		
		// Returns true if cue->IsAudible()
	virtual bool WorksWith(TCueData* cue);

	//----------------------
	// Specific Behavior
		
	// Do the effect
		// Apply an audio effect. Return true if anything has been 
		// done to the sound buffer.  Only called if DoesAudio().
		// TODO: is 'char*' correct?
	virtual bool Apply(int32 time, const char* source, char* target);
};

#endif		// !TAUDIBLEEFFECT_H
