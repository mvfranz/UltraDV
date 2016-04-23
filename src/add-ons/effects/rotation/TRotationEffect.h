#ifndef TROTATIONEFFECT_H
#define TROTATIONEFFECT_H
//---------------------------------------------------------------------
//
//	File:	TRotationEffect.h
//
//	Author:	Gene Ragan (Mike Ost)
//
//	Date:	10.12.98
//
//	Desc:
//
//	This is the effect module which can rotate a bitmap some number
//	of degrees.
//
//---------------------------------------------------------------------

// declarations
#include "TTransformEffect.h"
class TCuePosition;
class TRotateDialog;

class TRotationEffect : public TTransformEffect {
public:
	TRotationEffect();
		// Construct self from a message
	TRotationEffect(BMessage* msg);
		// delete
	virtual ~TRotationEffect();

	//----------------------
	// BArchivable overrides (file support)
	static BArchivable* Instantiate(BMessage* data);
	virtual status_t Archive(BMessage* data, bool deep) const;

	//----------------------
	// Static Class Behavior
		
		// Return a name to identify the effect by. This is used in 
		// the tool palette, in menus and within the cue view.
	virtual string Name() const;
		// Return a pointer to a bitmap which is to be used in the tools
		// palette. Either the 'up' or 'down' version can be chosen.
		// The base class version returns nothing. This won't be called
		// unless DoesPolygons() is true.
	virtual const BBitmap* CueIcon(bool up) const;

	//----------------------
	// Specific Behavior

	// Do the effect
		// Return a transformed version of 'bits' at 'time'. Caller
		// takes ownership of new'd BBitmap return value. Return
		// zero if nothing need be done. 
	virtual BBitmap* TransformBitmap(int32 time, const BBitmap* source,
			const TCuePosition& pos, EffectQuality quality);
		// Transform a TCuePosition given a time. Return true if anything
		// has been done to it.
	virtual bool TransformPosition(int32 time, TCuePosition* pos);
		// The mouse moved when moving the cue position around. Respond
		// to that as appropriate to the effect type.
	virtual bool MouseMovedPosition(BPoint pt, int32 time);
		// Edit either the default key frames (at the start and end)
		// or a single key frame. Bring up the rotation dialog and let
		// the user do their thing.
	virtual bool EditDefaults(BLooper* target);
	virtual bool EditKeyFrame(BLooper* target, TKeyFrameIterator kf);

	// DIALOG INTERFACE
		// Called when rotation value changes. Optionally tell the 
		// effect to trigger an update.
	void DialogValueChanged(bool redraw);
		// Called when OK, or Cancel is clicked
	void DialogIsClosing(bool okd);
	
private:
	// TYPES
		// state data for the key frames
	struct TRotationState : public TEffectState {
		TRotationState();
		TRotationState(float rotation);
		TRotationState(BMessage* msg);
		virtual ~TRotationState();

		//----------------------
		// BArchivable overrides (file support)
		static BArchivable* Instantiate(BMessage* data);
		// TODO: to instantiate we will need to look in an add-on image.
		// Add that to the message somehow?
		virtual status_t Archive(BMessage* data, bool deep) const;

		// Required by the base class
		virtual bool Equals(const TEffectState* s) const;
		virtual bool LessThan(const TEffectState* s) const;
		virtual TEffectState* NewCopy() const;
		virtual bool Assign(const TEffectState* s);

		// Message label
		static const char* kRotationLabel;

		// The data
		float m_rotation;
	};
	
	// DATA
		// The BHandler to target with change messages when the dialog
		// is up.
	BLooper* m_dialogTarget;
		// The vector of key frames we are currently editing.
	TKeyFrameIterator m_keyFrameTarget;
		// The dialog window. Keep it so it can be closed when 'this'
		// is destroyed.
	TRotateDialog* m_keyFramesDialog;
		// If 'true' then we are editing the default key frames, at
		// the start and end.
	bool m_editingDefaults;
	
	// METHODS
		// Have the subclass create a new effect state 
	virtual TEffectState* NewEffectState(void) const;
		// Have the subclass interpolate a reasonable value given 
		// a key frame iterator. Use neighboring key frames to
		// figure out a good value. 
	virtual void Interpolate(TCueEffect::TKeyFrameIterator kf) const;

		//	Rotate using the nearest neighbor algorithm
	void RotateBitmapNN(const BBitmap *srcBitmap, BBitmap *dstBitmap, 
			float degrees, BPoint originPt);
		//	Rotate using bilinear interpolation
	void RotateBitmapBilinear(const BBitmap *srcBitmap, BBitmap *dstBitmap, 
			float degrees, BPoint originPt);

		// Interpolate a rotation angle given a time and our rotation
		// values, stored as key frames.
	float RotationDegrees(int32 time) const;
		// Get the state field and convert it to our type.
	const TRotationState* RotationState(TCueEffect::TKeyFrameIterator) const;
		// Called by EditDefaults and EditKeyFrame. Brings up the
		// dialog and lets 'er rip.
	bool EditKeyFramesDialog(BLooper* target, TKeyFrameIterator 
			kf, bool doingDefaults);
};

#endif		// !TROTATIONEFFECT_H
