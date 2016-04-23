//---------------------------------------------------------------------
//
//	File:	TTick.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.15.98
//
///	Desc:	TTick object interface.  Based on code by Doug Wright
//		
//				
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TTICK_H__
#define __TTICK_H__
 
// Includes
#include "BuildApp.h"

// ABH does not exist? 
#include "TTimecode.h"

const int32		AREA_MSG		= 'area';
const int32		TICK_MSG		= 'tick';


// Class Definition
class TTick 
{

	public:	
		bigtime_t		m_Time;
		uint64			m_Number;
		bigtime_t		m_Period;
		TTimecode		m_Timecode;
		
		// Operators used for sorting chronologically
		bool		operator<(const TTick&) const;
		bool		operator==(const TTick&) const;
		bool		operator>(const TTick&) const;
	
};

inline bool TTick::operator<(const TTick& t) const { return(m_Time < t.m_Time); }
inline bool TTick::operator==(const TTick& t) const { return(m_Time == t.m_Time); }
inline bool TTick::operator>(const TTick& t) const { return(m_Time > t.m_Time); }

#endif
