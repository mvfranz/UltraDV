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
	m_time(0),
	m_state(0)
{}

TKeyFrame::~TKeyFrame()
{
	delete m_state;
}

TKeyFrame::TKeyFrame(const TKeyFrame& rhs) :
	m_time(rhs.m_time)
{
	if (rhs.m_state)
		m_state = rhs.m_state->NewCopy();
	else
		m_state = rhs.m_state;
}

//---------------------------------------------------------------------
// Comparison operators
//---------------------------------------------------------------------
//
//

bool TKeyFrame::operator<(const TKeyFrame& rhs)
{
	if (m_time < rhs.m_time)
		return true;
	else if (m_time == rhs.m_time && m_state->LessThan(rhs.m_state))
		return true;
	return false;
}

bool TKeyFrame::operator==(const TKeyFrame& rhs)
{
	return m_time == rhs.m_time && m_state->Equals(rhs.m_state);
}
