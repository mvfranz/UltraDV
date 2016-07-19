#ifndef TCUEEFFECT_H
#define TCUEEFFECT_H
//---------------------------------------------------------------------
//
//	File:	TCueEffect.h
//
//	Author:	Mike Ost
//
//	Date:	10.09.98
//
//	Desc:
//
//	Effects are data modifiers that have a sense of time. They have 
//	a start time and a duration within the cue they are contained by. 
//	As a cue's data is rendered	an effect is allowed to work on a 
//	cue's output. Each of the cue editing tools	in ultraDV --- 
//	transitions, stage tools, opacity, etc. --- is implemented as
//	an effect. They each have a cue as source data and a sense of 
//	time within the cue.
//	
//	The base class supports key frames, which give effects a point
//	in time to make changes in an evolving effect. An effect also
//	has a Settings method which gives it an opportunity to bring up
//	an edit dialog box.
//
// TODO? OPTIMIZATIONS:
//		- ability to tell if an effect is the same at two different
//		times, for caching of stageview bitmaps
//		- ability to tell if it will have no effect on its data, with
//		minimal processing.
//		- ability to tell if all bits are off, or if sound is all the way
//		off, stopping further processing of a cue.
//---------------------------------------------------------------------

// This is required to make the vector class work...
#define MSIPL_DEF_TEMPARG

// declarations

class TCueData;

#include "TKeyFrame.h"

#include <list>
#include <string>
#include <vector>

using namespace std;
//=================
// Message Defs
//
// These messages are sent from a key frame dialog to its target message 
// handler. 
// 
// A value from the dialog has changed the cue effect. Update the 
// stage view to show the change. Note that the effect value has already
// been changed before this message is sent.
#define CUEFX_UPDATE_MSG	'QFXu'
	// name			type			value		notes
	// ----			----			-----		-----
	// "effect"		B_POINTER_TYPE	'this'		the effect being changed
//
// An effects initialization (EditDefaults()) operation has been cancelled
// by the user. The effect expects to be removed and destroyed.
#define CUEFX_CANCEL_MSG	'QFXx'
	// name			type			value		notes
	// ----			----			-----		-----
	// "effect"		B_POINTER_TYPE	'this'		the effect being changed

//=================
// Support Types
//
// This is the type that is matched up with a time to make a key frame.
// Subclasses define a version of this, adding whatever state information 
// makes sense. The data is stored as a pointer, so size doesn't really 
// matter.
//
// NOTE: this would have been better as a subclass of TCueEffect, but
// we are adding this to the public name space so TKeyFrame.h can
// refer to class TEffectState and not TCueEffect::TEffectState.
// That would required #include "TCueEffect.h" which would create a
// circular include situation.
//
struct TEffectState : public BArchivable {
	virtual ~TEffectState() {}

	// These are needed by TKeyFrame to allow its fstate field
	// to be compared and copied. NOTE: Using the operator== and
	// operator< would be weird because the subclass decls would 
	// not be comparing objects of the same type. This seems clearer.
	virtual bool Equals(const TEffectState* s) const = 0;
	virtual bool LessThan(const TEffectState* s) const = 0;
	virtual TEffectState* NewCopy() const = 0;
	// The same for assignment, with the same reasons to avoid operator=.
	// Return true if a change was required.
	virtual bool Assign(const TEffectState* s) = 0;
};

//=================
// Class decl

class TCueEffect : public BArchivable {
protected:
		// This is the container used to hold key frames
// ABH	typedef list<TKeyFrame> TKeyFrameContainer;
#define TKeyFrameContainer list<TKeyFrame>

//	list<TKeyFrame> TKeyFrameContainer;

public:
	// METHODS
		// Default ctor
	TCueEffect();
		// Construct self from a message
	TCueEffect(BMessage* msg);
		// delete
	virtual ~TCueEffect();

	//----------------------
	// BArchivable overrides (file support)
	static BArchivable* Instantiate(BMessage* data);
	// TODO: to instantiate we will need to look in an add-on image.
	// Add that to the message somehow?
	virtual status_t Archive(BMessage* data, bool deep) const;

	//----------------------
	// Static Class Behavior
		
		// Test if a cue is handled by this effect
		// TODO: use RTTI for testing?
	virtual bool WorksWith(const TCueData* cue) const = 0;
		// Test if a different effect can coexist with this effect at
		// the same time. Use RTTI to identify the target. The base class
		// version returns 'false' for effects of the same type, (preventing
		// a cue from stacking two of the same effect on top of each other).
	virtual bool CanOverlap(const TCueEffect* effect) const;
		// Return a name to identify the effect by. This is used in 
		// the tool palette, in menus and within the cue view.
	virtual string Name() const = 0;
		// Return a pointer to a bitmap which is to be used in the tools
		// palette. Either the 'up' or 'down' version can be chosen.
		// The base class version returns nothing.
	virtual const BBitmap* CueIcon(bool up) const = 0;
		// Tells if an effect can be resized. The default is 'no', that is
		// the effect expects to start and end with the cue itself. 
	virtual bool IsSizeable(const TCueEffect* effect) const;

	//----------------------
	// Specific Behavior

		// Timing (within a cue). If the start time or duration changes,
		// the base class version eliminates any stranded key frames.
	uint32 StartTime() const			{ return fstartTime; }
	virtual void StartTime(uint32 time);
	uint32 EndTime() const			{ return fstartTime + fduration; }
	void EndTime(uint32 time)		{ Duration(time - fstartTime); }
	uint32 Duration() const			{ return fduration; }
	void Duration(uint32 time);
		
	// NOTE: See subclasses for actual effects application methods

	// Key frames
		// Helpful type decls
		// The iterator type. The world at large just gets a const
		// iterator.
// ABH

//	typedef TKeyFrameContainer::const_iterator TKeyFrameIterator;
//	TKeyFrameContainer::const_iterator TKeyFrameIterator;

#define TKeyFrameIterator TKeyFrameContainer::const_iterator

		// Make the key frames be simply default values at the start
		// and the end of the effect
	void SetDefaultKeyFrames();
		// Add a new key frame at 'time'
	TKeyFrameIterator MarkKeyFrame(uint32 time);
		// Report on the effect's key frames
	TKeyFrameIterator FirstKeyFrame() const 	{ return fkeyFrames.begin(); }
	TKeyFrameIterator EndOfKeyFrames() const	{ return fkeyFrames.end(); }
		// Remove the indicated key frame
	void RemoveKeyFrame(TKeyFrameIterator kf);
		// Move a key frame to a new time. Returns the new iterator.
		// TODO: add an option to move keyframes between old time and
		// new by the same amount?
		// TODO: what if the kf lands on an old kf... delete the old?
	void MoveKeyFrame(TKeyFrameIterator kf, uint32 newTime);

		// Launch the key frame edit dialog. Return false if key
		// frames are already being edited. First edit the default
		// start and end key frames
	virtual bool EditDefaults(BLooper* target)
	{
		return false;
	}
		// Edit a single keyframe
	virtual bool EditKeyFrame(BLooper* target, TKeyFrameIterator frame)
	{
		return false;
	}

protected:
		// Have the subclass create a new effect state 
	virtual TEffectState* NewEffectState(void) const = 0;
		// Have the subclass interpolate a reasonable value given 
		// a key frame iterator. Use neighboring key frames to
		// figure out a good value. 
	virtual void Interpolate(TKeyFrameIterator kf) const = 0;
		// Use our friend status with TKeyFrame to give subclasses
		// access to this pointer. This lets them see the value
	const TEffectState* KeyFrameState(TKeyFrameIterator kf) const 
						{ return (*kf).fstate; }
		// And this lets them change it. The value of 'state' is copied
		// into the current key frame state variable. Return value indicates
		// if an actual change was required.
	bool KeyFrameState(TKeyFrameIterator kf, const TEffectState* state) const;

		// Let sub classes trigger a stage update
	void UpdateEffect();
		// Let sub classes cancel an initialization operation
	void CancelEffect();

private:
	// Message labels
	static const char* kStartLabel;
	static const char* kDurationLabel;
	static const char* kKeyFrameCountLabel;
	static const char* kKeyFrameTimeLabel;
	static const char* kKeyFrameStateLabel;

	// Types
		// A volatile iterator type for local use only. 
		
// ABH
//	typedef TKeyFrameContainer::iterator TKeyFrameIterator_vol;

#define TKeyFrameIterator_vol TKeyFrameContainer::iterator 

	// Methods
		// Return the last key frame in the bunch, the one that marks the 
		// end of the effect. NOTE: this isn't a const function so 
		// fkeyFrames.end() will return a non-const iterator, which we 
		// want for a return type.
	TKeyFrameIterator_vol LastKeyFrame() const;
		// Find the volatile version of the key frame iterator. We only
		// hand out const iterators to the outside world, but we need 
		// volatile ones here.
	TKeyFrameIterator_vol VolatileIterator(TKeyFrameIterator kf) const;

	// Data
		// when the effect starts and how long it lasts
	uint32 fstartTime;
	uint32 fduration;
		// key frames within the passing of the effect
	TKeyFrameContainer fkeyFrames;
};

#endif		// !TCUEEFFECT_H
