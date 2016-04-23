//---------------------------------------------------------------------
//
//	File:	TMuseumPrefs.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Object that handles all application preferences
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TMUSEUMPREFS_H__
#define __TMUSEUMPREFS_H__

// Includes
#include "TRecordPrefsDialog.h"
#include "TVideoSettings.h"

class TMuseumPrefs
{
	public:
		TMuseumPrefs();
		~TMuseumPrefs();
		
		void	Init();
		
		// Member Varibles
		int16			m_Timescale;
		TVideoSettings	m_VideoSettings;
		
		
	private:

};

#endif
