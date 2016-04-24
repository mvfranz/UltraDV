//---------------------------------------------------------------------
//
//	File:	BuldApp.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.14.98
//
//	Desc:	Header file designed to build applications
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __BUILDAPP_H__
#define __BUILDAPP_H__

// Enable/disable debugging
#ifndef DEBUG
#define DEBUG false
#endif

#include <Be.h>

//
// Set up product build
//

// _BUILDING_MUSEUM
// ABH #define _BUILDING_MUSEUM true

/*	trinity_build.h	*/

#include <BeBuild.h>

#ifdef _BUILDING_MUSEUM
	#define _IMPEXP_MUSEUM //__declspec(dllexport)
#else
	#define _IMPEXP_MUSEUM //__declspec(dllimport)
#endif

class _IMPEXP_MUSEUM MuseumApp;
class _IMPEXP_MUSEUM TActionCuePalette;
class _IMPEXP_MUSEUM TActionCuePaletteView;
class _IMPEXP_MUSEUM TBitmapButton;
class _IMPEXP_MUSEUM TBitmapView;
class _IMPEXP_MUSEUM TBrowserWindow;
class _IMPEXP_MUSEUM TCueButton;
class _IMPEXP_MUSEUM TCueChannel;
class _IMPEXP_MUSEUM TCueChannelHeader;
class _IMPEXP_MUSEUM TCueEvent;
class _IMPEXP_MUSEUM TCueLockButton;
class _IMPEXP_MUSEUM TCueMuteButton;
class _IMPEXP_MUSEUM TChannelNameView;
class _IMPEXP_MUSEUM TContractAllButton;
class _IMPEXP_MUSEUM TCuePathButton;
class _IMPEXP_MUSEUM TPointerButton;
class _IMPEXP_MUSEUM TCueSheetScrollBarH;
class _IMPEXP_MUSEUM TCueSheetScrollBarV;
class _IMPEXP_MUSEUM TCueSheetTimeView;
class _IMPEXP_MUSEUM TCueSheetWindow;
class _IMPEXP_MUSEUM TCueSheetView;
class _IMPEXP_MUSEUM TCueTimeText;
class _IMPEXP_MUSEUM TCueTransitionButton;
class _IMPEXP_MUSEUM TCueView;
class _IMPEXP_MUSEUM TCursor;
class _IMPEXP_MUSEUM TElementsView;	
class _IMPEXP_MUSEUM TElementsTrackerView;
class _IMPEXP_MUSEUM TExpandButton;
class _IMPEXP_MUSEUM TExpandAllButton;
class _IMPEXP_MUSEUM TExportTimeView;
class _IMPEXP_MUSEUM TExportText;
class _IMPEXP_MUSEUM TExportZone;
class _IMPEXP_MUSEUM THeaderContainerView;
class _IMPEXP_MUSEUM TKeyframe;
class _IMPEXP_MUSEUM TLockButton;
class _IMPEXP_MUSEUM TMediaCuePalette;
class _IMPEXP_MUSEUM TMediaCuePaletteView;
class _IMPEXP_MUSEUM TMIDIEngine;
class _IMPEXP_MUSEUM TMIDIFile;
class _IMPEXP_MUSEUM TMIDITrack;
class _IMPEXP_MUSEUM TMIDIConductor;
class _IMPEXP_MUSEUM TMuseumIcons;
class _IMPEXP_MUSEUM TMuseumClipboard;
class _IMPEXP_MUSEUM TMuseumMenus;	
class _IMPEXP_MUSEUM TMuseumPrefs;
class _IMPEXP_MUSEUM TMuteButton;
class _IMPEXP_MUSEUM TNumberBevelTextView;
class _IMPEXP_MUSEUM TNumberTextControl;
class _IMPEXP_MUSEUM TNumberTextView;
class _IMPEXP_MUSEUM TPlaybackEngine;
class _IMPEXP_MUSEUM TPlaybackEvent;
class _IMPEXP_MUSEUM TPlaybackList;	
class _IMPEXP_MUSEUM TPreset;
class _IMPEXP_MUSEUM TProjectSettings;
class _IMPEXP_MUSEUM TRazorButton;
class _IMPEXP_MUSEUM TSoloButton;
class _IMPEXP_MUSEUM TStageTools;
class _IMPEXP_MUSEUM TStageWindow;
class _IMPEXP_MUSEUM TStageCue;
class _IMPEXP_MUSEUM TStageView;
class _IMPEXP_MUSEUM TTimeText;
class _IMPEXP_MUSEUM TTimeTextView;
class _IMPEXP_MUSEUM TTimelineView;
class _IMPEXP_MUSEUM TTimePalette;
class _IMPEXP_MUSEUM TTimePaletteView;
class _IMPEXP_MUSEUM TTimer;
class _IMPEXP_MUSEUM TTimeScalerView;
class _IMPEXP_MUSEUM TToolbar;
class _IMPEXP_MUSEUM TTransition;
class _IMPEXP_MUSEUM TTransitionTimer;
class _IMPEXP_MUSEUM TTransitionTimeText;
class _IMPEXP_MUSEUM TTransportPalette;
class _IMPEXP_MUSEUM TTransportPaletteView;
class _IMPEXP_MUSEUM TTransportText;
class _IMPEXP_MUSEUM TUndoEngine;
class _IMPEXP_MUSEUM TVisualCue;
class _IMPEXP_MUSEUM TVisualKeyframe;
class _IMPEXP_MUSEUM TZoomButton;


// Cues
class _IMPEXP_MUSEUM TAnimationCue;
class _IMPEXP_MUSEUM TAudioCue;
class _IMPEXP_MUSEUM TMIDICue;
class _IMPEXP_MUSEUM TMovieCue;
class _IMPEXP_MUSEUM TPictureCue;
class _IMPEXP_MUSEUM TTextCue;

#endif
