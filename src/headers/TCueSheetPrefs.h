//---------------------------------------------------------------------
//
//	File:	TCueSheetPrefs.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.15.98
//
//	Desc:	Object that handles all cue sheet preferences
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCUESHEETPREFS_H__
#define __TCUESHEETPREFS_H__

// Includes
#include <TimeCode.h>

#include "TRecordPrefsDialog.h"
#include "TVideoSettings.h"

class TCueSheetPrefs
{
	public:
		TCueSheetPrefs();
		~TCueSheetPrefs();
		
		void	Init();
		
		// Member Varibles
		timecode_type	m_Timescale;
		TVideoSettings	m_VideoSettings;
		
	private:

};

#endif
