//---------------------------------------------------------------------
//
//	File:	TCueEffect.cpp
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
//---------------------------------------------------------------------

#include "BuildApp.h"

#include <support/Debug.h>		// for ASSERT()

#include "TCueEffect.h"

using namespace std;

// Constants
	// Names used in a message
const char* TCueEffect::kStartLabel = "startTime";
const char* TCueEffect::kDurationLabel = "duration";
const char* TCueEffect::kKeyFrameCountLabel = "keyFrameCount";
const char* TCueEffect::kKeyFrameTimeLabel = "keyFrameTime";
const char* TCueEffect::kKeyFrameStateLabel = "keyFrameState";

//---------------------------------------------------------------------
//	Constructor/destructor
//---------------------------------------------------------------------
//

TCueEffect::TCueEffect() :
	fstartTime(0),
	fduration(0)
{
}

TCueEffect::~TCueEffect()
{
}

TCueEffect::TCueEffect(BMessage* msg) :
	BArchivable(msg)
{
	status_t err;
	err = msg->FindInt32(kStartLabel, (int32 *)&fstartTime);
	if (err != B_OK) {
		ASSERT(false);
		return;
	}
	err = msg->FindInt32(kDurationLabel, (int32 *)&fduration);
	if (err != B_OK) {
		ASSERT(false);
		return;
	}

	// Read in the keyframe list
	uint32 size;
	err = msg->FindInt32(kKeyFrameCountLabel, (int32 *)&size);
	if (err != B_OK) {
		ASSERT(false);
		return;
	}

	for (int i = 0; i < size; i++) {
		// Read in the time field
		TKeyFrame kf;
		err = msg->FindInt32(kKeyFrameTimeLabel, &kf.ftime);
		if (err != B_OK)
			break;

		// Read in, and instantiate, the state field
		BMessage stateMsg;
		err = msg->FindMessage(kKeyFrameStateLabel, i, &stateMsg);
		if (err != B_OK) {
			ASSERT(false);
			break;
		}

		BArchivable* a = instantiate_object(&stateMsg);
		if (a == 0) {
			ASSERT(a);
			break;
		}
		kf.fstate = dynamic_cast<TEffectState*>(a);
		if (kf.fstate == 0) {
			ASSERT(kf.fstate);
			break;
		}

		// Add it to our list
		fkeyFrames.push_back(kf);
	}
}

//---------------------------------------------------------------------
// BArchivable overrides (file support)
//---------------------------------------------------------------------
//
//

BArchivable* TCueEffect::Instantiate(BMessage* data)
{
	// NOTE: this is an error. We can't instantiate this class
	// since it is an abstract base class.
	ASSERT(false);
	return 0;
}

status_t TCueEffect::Archive(BMessage* data, bool deep) const
{
	// Start by calling inherited archive
	status_t err = BArchivable::Archive(data, deep);
	if (err != B_OK)
		return err;

	// NOTE: we don't instantiate this class --- it's an ABC ---
	// so we don't add the class name data

	// And local data
	err = data->AddInt32(kStartLabel, fstartTime);
	if (err != B_OK)
		return err;
	err = data->AddInt32(kDurationLabel, fduration);
	if (err != B_OK)
		return err;

	// Archive out the keyframe list
	err = data->AddInt32(kKeyFrameCountLabel, fkeyFrames.size());
	if (err != B_OK)
		return err;

	for (TKeyFrameIterator k = fkeyFrames.begin(); k != fkeyFrames.end();
			k++) {
//		err = data->AddFloat(kKeyFrameTimeLabel, k->ftime); DOESN'T STL SUPPORT THIS?
		err = data->AddFloat(kKeyFrameTimeLabel, (*k).ftime);
		if (err != B_OK)
			break;

		// Archive the state data
		BMessage state;
//		err = k->fstate->Archive(&state, deep); DOESN'T STL SUPPORT THIS?
		err = (*k).fstate->Archive(&state, deep);
		if (err != B_OK)
			break;
		err = data->AddMessage(kKeyFrameStateLabel, &state);
		if (err != B_OK)
			break;
	}

	return err;
}

//---------------------------------------------------------------------
// CanOverlap
//---------------------------------------------------------------------
//
//

bool TCueEffect::CanOverlap(const TCueEffect* effect) const
{
	// Test for same class. They can't overlap by default.
	if (class_name(this) == class_name(effect))
		return false;
	return true;
}

//---------------------------------------------------------------------
// IsSizeable
//---------------------------------------------------------------------
//
//

bool TCueEffect::IsSizeable(const TCueEffect* effect) const
{
	return false;
}


//---------------------------------------------------------------------
// StartTime
//---------------------------------------------------------------------
//
//

void TCueEffect::StartTime(uint32 time)
{
	fstartTime = time;
}

//---------------------------------------------------------------------
// Duration
//---------------------------------------------------------------------
//
//

void TCueEffect::Duration(uint32 time)
{
	fduration = time;

	// Set up the defaults if they haven't already been
	if (fkeyFrames.size() < 2) {
		SetDefaultKeyFrames();
		return;
	}

	// Set up the last entry to go to the new end of the effect
	TKeyFrameIterator_vol last = LastKeyFrame();
//	last->ftime = time;
	(*last).ftime = time;

	// check for key frames which must be removed
	for (TKeyFrameIterator_vol later = fkeyFrames.begin();
			later != last; later++) {
		// Frames from this point on to the final one are now invalid
//		if (later->ftime >= time) {
		if ((*later).ftime >= time) {
			fkeyFrames.erase(later, last);
			break;
		}
	}
}

//---------------------------------------------------------------------
// MarkKeyFrame
//---------------------------------------------------------------------
//
//

void TCueEffect::SetDefaultKeyFrames()
{
	// Set up the defaults
	if (fkeyFrames.size() < 2) {
		TKeyFrame kf;
		fkeyFrames.push_front(kf);
		fkeyFrames.push_back(kf);

		// Set up the first
		TKeyFrameIterator_vol itr = fkeyFrames.begin();
		(*itr).ftime = 0;
		(*itr).fstate = NewEffectState();

		// and last
		itr++;
		(*itr).ftime = fduration;
		(*itr).fstate = NewEffectState();

		return;
	}

	// Eliminate all key frames between the first and last
	TKeyFrameIterator_vol second = fkeyFrames.begin();
	second++;
	TKeyFrameIterator_vol last = LastKeyFrame();
	if (second != last)
		fkeyFrames.erase(second, last);
}

//---------------------------------------------------------------------
// MarkKeyFrame
//---------------------------------------------------------------------
//
//

// ABH

//TCueEffect::TKeyFrameIterator TCueEffect::MarkKeyFrame(uint32 time)

TKeyFrameIterator TCueEffect::MarkKeyFrame(uint32 time)
{
	// Convert from a global to a local time value
	time -= fstartTime;

	// Can't mark a key frame after the effect is through...
	if (time > fduration) {
		ASSERT(false);
		return TKeyFrameIterator();
	}

	// Find the key frame after or at 'time'
	TKeyFrameIterator_vol before = fkeyFrames.begin();
	TKeyFrameIterator_vol later = before;
	for ( ; later != fkeyFrames.end(); later++) {
		if ((*later).ftime >= time)
			break;
		before++;
	}

	// There is already a key frame at this time. Choose a different time
	if ((*later).ftime == time) {
		ASSERT(false);
		return TKeyFrameIterator();
	}

	// Create a key frame here
	TKeyFrameIterator_vol kf;
// ABH!
//	kf  = fkeyFrames.insert(later);
	kf = later; 		// dirty hack to get a compile

//	(*kf).ftime = time;
//	(*kf).fstate = NewEffectState();
printf("TCueEffect::MarkKeyFrame - Needs to be implemented!\n");

	// Interpolate a reasonable value
	Interpolate(kf);

	return kf;
}

//---------------------------------------------------------------------
// RemoveKeyFrame
//---------------------------------------------------------------------
//
//

// ABH

//void TCueEffect::RemoveKeyFrame(TCueEffect::TKeyFrameIterator kf)

void TCueEffect::RemoveKeyFrame(TKeyFrameIterator kf)
{
	// Can't remove the first or last key frame
	if (kf == fkeyFrames.begin() || kf == LastKeyFrame())
		return;

	TKeyFrameIterator_vol v = VolatileIterator(kf);
	fkeyFrames.erase(v);
}

//---------------------------------------------------------------------
// MoveKeyFrame
//---------------------------------------------------------------------
//
//

// ABH

//void TCueEffect::MoveKeyFrame(TCueEffect::TKeyFrameIterator kf, uint32 newTime)

void TCueEffect::MoveKeyFrame(TKeyFrameIterator kf, uint32 newTime)
{
	// Can't move the first or last key frame
	if (kf == fkeyFrames.begin() || kf == LastKeyFrame())
		return;

	TKeyFrameIterator_vol v = VolatileIterator(kf);
	// Set the key frame to a new time, converted from global to local
//	v->ftime = newTime - fstartTime;
	(*v).ftime = newTime - fstartTime;
	fkeyFrames.sort();
}

//---------------------------------------------------------------------
// LastKeyFrame
//---------------------------------------------------------------------
//
//

// ABH

//TCueEffect::TKeyFrameIterator_vol TCueEffect::LastKeyFrame(void) const

TKeyFrameIterator_vol TCueEffect::LastKeyFrame(void) const
{
	// force 'end()' to return a non-const iterator
	TCueEffect* volatileMe = const_cast<TCueEffect*>(this);
	TKeyFrameIterator_vol last = volatileMe->fkeyFrames.end();
	last--;
	return last;
}

//---------------------------------------------------------------------
// VolatileIterator
//---------------------------------------------------------------------
//
//

// ABH
//TCueEffect::TKeyFrameIterator_vol TCueEffect::VolatileIterator(

TKeyFrameIterator_vol TCueEffect::VolatileIterator(

// ABH

//		TCueEffect::TKeyFrameIterator kf) const

		TKeyFrameIterator kf) const
{
	// TODO: isn't there a way to just cast one of these puppies?

	// force 'begin()' to return a non-const iterator
	TCueEffect* volatileMe = const_cast<TCueEffect*>(this);
	TKeyFrameIterator_vol v = volatileMe->fkeyFrames.begin();
	//for ( ; v != fkeyFrames.end(); v++)
	//{
	//	if (v == kf)
	//		return v;
	//}

	// 'kf' couldn't be found
	ASSERT(false);
	return v;
}

//---------------------------------------------------------------------
// KeyFrameState
//---------------------------------------------------------------------
//
//

// ABH

//bool TCueEffect::KeyFrameState(TCueEffect::TKeyFrameIterator kf,

bool TCueEffect::KeyFrameState(TKeyFrameIterator kf,
		const TEffectState* state) const
{
	TKeyFrameIterator_vol v = VolatileIterator(kf);
//	if (v->fstate)
//		v->fstate->Assign(state);
	if ((*v).fstate)
		return (*v).fstate->Assign(state);
	return false;
}

