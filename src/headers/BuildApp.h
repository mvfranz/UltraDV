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
#define DEBUG 0
#endif

#include <Be.h>

/*	trinity_build.h	*/

#include <BeBuild.h>

class MuseumApp;
class TActionCuePalette;
class TActionCuePaletteView;
class TBitmapButton;
class TBitmapView;
class TBrowserWindow;
class TCueButton;
class TCueChannel;
class TCueChannelHeader;
class TCueEvent;
class TCueLockButton;
class TCueMuteButton;
class TChannelNameView;
class TContractAllButton;
class TCuePathButton;
class TPointerButton;
class TCueSheetScrollBarH;
class TCueSheetScrollBarV;
class TCueSheetTimeView;
class TCueSheetWindow;
class TCueSheetView;
class TCueTimeText;
class TCueTransitionButton;
class TCueView;
class TCursor;
class TElementsView;	
class TElementsTrackerView;
class TExpandButton;
class TExpandAllButton;
class TExportTimeView;
class TExportText;
class TExportZone;
class THeaderContainerView;
class TKeyframe;
class TLockButton;
class TMediaCuePalette;
class TMediaCuePaletteView;
class TMIDIEngine;
class TMIDIFile;
class TMIDITrack;
class TMIDIConductor;
class TMuseumIcons;
class TMuseumClipboard;
class TMuseumMenus;	
class TMuseumPrefs;
class TMuteButton;
class TNumberBevelTextView;
class TNumberTextControl;
class TNumberTextView;
class TPlaybackEngine;
class TPlaybackEvent;
class TPlaybackList;	
class TPreset;
class TProjectSettings;
class TRazorButton;
class TSoloButton;
class TStageTools;
class TStageWindow;
class TStageCue;
class TStageView;
class TTimeText;
class TTimeTextView;
class TTimelineView;
class TTimePalette;
class TTimePaletteView;
class TTimeScalerView;
class TToolbar;
class TTransition;
class TTransitionTimer;
class TTransitionTimeText;
class TTransportPalette;
class TTransportPaletteView;
class TTransportText;
class TUndoEngine;
class TVisualCue;
class TVisualKeyframe;
class TZoomButton;


// Cues
class TAnimationCue;
class TAudioCue;
class TMIDICue;
class TMovieCue;
class TPictureCue;
class TTextCue;

#endif
