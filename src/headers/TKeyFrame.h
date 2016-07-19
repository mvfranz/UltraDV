#ifndef TKEYFRAME_H
#define TKEYFRAME_H
//---------------------------------------------------------------------
//
//	File:	TKeyFrame.h
//
//	Author:	Mike Ost
//
//	Date:	10.23.98
//
//	Desc:
//
//	TKeyFrame is owned by TCueEffect. It associates a time with a
//	TEffectState, as defined by the add-on effects classes. This
//	provides a generic interface to key frames, with the details 
//	of effects states handled elsewhere.
//
//---------------------------------------------------------------------

// declarations
#include "BuildApp.h"

class TEffectState;
class TCueEffect;

class TKeyFrame {
public:
	// create and destroy
	TKeyFrame();
	~TKeyFrame();

	// copy and assign. NOTE: we can't have the value of this 
	// object changed without TCueEffect knowing about it. Use
	// TCueEffect interface to make changes. This is only for 
	// the use of 'list'.
	TKeyFrame(const TKeyFrame& rhs);
	const TKeyFrame& operator=(const TKeyFrame& rhs);
	
	// comparison ops used by 'list'
	bool operator<(const TKeyFrame& rhs);
	bool operator==(const TKeyFrame& rhs);

	// Key frame info
	int32 Time() const;
	
private:
		// Time within the cue effect; i.e., ftime will never >
		// TCueEffect::fduration
	int32 ftime;
		// This is the type specific state information. 
	TEffectState* fstate;
		// Let the TCueEffect get at the time and state variables
	friend class TCueEffect;
};

#endif 		//   !TKEYFRAME_H
