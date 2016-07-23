#ifndef TTRANSFORMEFFECT_H
#define TTRANSFORMEFFECT_H
//---------------------------------------------------------------------
//
//	File:	TTransformEffect.h
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

// This is required to make the vector class work...
#define MSIPL_DEF_TEMPARG

// declarations
#include "TVisualEffect.h"
class TCuePosition;

class TTransformEffect : public TVisualEffect {
public:
	TTransformEffect();
		// Construct self from a message
	TTransformEffect(BMessage* msg);
		// delete
	virtual ~TTransformEffect();

	//----------------------
	// BArchivable overrides (file support)
	static BArchivable* Instantiate(BMessage* data);
	virtual status_t Archive(BMessage* data, bool deep) const;

	// Do the effect
		// Transform a TCuePosition given a time. Return true if anything
		// has been done to it.
	virtual bool TransformPosition(int32 time, TCuePosition* pos) = 0;
		// The mouse moved when moving the cue position around. Respond
		// to that as appropriate to the effect type.
	virtual bool MouseMovedPosition(BPoint pt, int32 time) = 0;
};

#endif		// !TTRANSFORMEFFECT_H
