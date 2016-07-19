//---------------------------------------------------------------------
//
//	File:	TKeyFrame.cpp
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

#include "TKeyFrame.h"

#include "TCueEffect.h"

//---------------------------------------------------------------------
// Create and Destroy
//---------------------------------------------------------------------
//
//

TKeyFrame::TKeyFrame() :
	ftime(0),
	fstate(0)
{}

TKeyFrame::~TKeyFrame()
{
	delete fstate;
}

TKeyFrame::TKeyFrame(const TKeyFrame& rhs) :
	ftime(rhs.ftime)
{
	if (rhs.fstate)
		fstate = rhs.fstate->NewCopy();
	else
		fstate = rhs.fstate;
}

//---------------------------------------------------------------------
// Comparison operators
//---------------------------------------------------------------------
//
//

bool TKeyFrame::operator<(const TKeyFrame& rhs)
{
	if (ftime < rhs.ftime)
		return true;
	else if (ftime == rhs.ftime && fstate->LessThan(rhs.fstate))
		return true;
	return false;
}

bool TKeyFrame::operator==(const TKeyFrame& rhs)
{
	return ftime == rhs.ftime && fstate->Equals(rhs.fstate);
}
