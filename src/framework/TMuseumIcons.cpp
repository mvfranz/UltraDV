//---------------------------------------------------------------------
//
//	File:	TMuseumIcons.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	04.08.98
//
//	Desc:	Object that handles all application icons
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "TMuseumIcons.h"
#include "ResourceManager.h"



//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMuseumIcons::TMuseumIcons()
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMuseumIcons::~TMuseumIcons()
{
	// Free bitmaps
	delete fAnimationIcon16;
	delete fAnimation32;
	delete fAudioIcon16;
	delete fAudioIcon32;
	delete fMIDIIcon16;
	delete fMIDIIcon32;
	delete fMovieIcon16;
	delete fMovieIcon32;
	delete fPictureIcon16;
	delete fPictureIcon32;
	delete fTextIcon16;
	delete fTextIcon32;

	delete fLock;
	delete fUnlock;
	delete fLockDark;
	delete fUnlockDark;
	delete fVisibleUp;
	delete fVisibleDown;
	delete fTransitionIn;
	delete fTransitionOut;
	delete fPathIn;
	delete fPathOut;

	delete fAnimationUpIcon;
	delete fAnimationDnIcon;
	delete fAudioUpIcon;
	delete fAudioDnIcon;
	delete fMIDIUpIcon;
	delete fMIDIDnIcon;
	delete fMovieUpIcon;
	delete fMovieDnIcon;
	delete fPictureUpIcon;
	delete fPictureDnIcon;
	delete fTextUpIcon;
	delete fTextDnIcon;

	delete fMuteUp;
	delete fMuteDown;
	delete fSoloUp;
	delete fSoloDown;
	delete fContract;
	delete fExpand;

	// Free transition icons
	delete fWipeDown;
	delete fWipeLeft;
	delete fWipeRight;
	delete fWipeUp;
	delete fWipeBottomLeftTopRight;
	delete fWipeBottomRightTopLeft;
	delete fWipeTopLeftBottomRight;
	delete fWipeTopRightBottomLeft;
	delete fCurtainsIn;
	delete fCurtainsOut;
	delete fRevealBottomLeftTopRight;
	delete fRevealBottomRightTopLeft;
	delete fRevealTopLeftBottomRight;
	delete fRevealTopRightBottomLeft;
	delete fRevealUp;
	delete fRevealDown;
	delete fRevealLeft;
	delete fRevealRight;

	//	Free controls
	delete fExportSliderLeft;
	delete fExportSliderRight;

	//	Free keyframes
	delete fKeyframeOff;
	delete fKeyframeOn;

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TMuseumIcons::Init()
{

	// Cue icons
	fAnimationIcon16        = GetMICNFromResource("Animation");
	fAnimation32            = GetICONFromResource("Animation");
	fAudioIcon16            = GetMICNFromResource("Audio");
	fAudioIcon32            = GetICONFromResource("Audio");
	fMIDIIcon16             = GetMICNFromResource("MIDI");
	fMIDIIcon32             = GetICONFromResource("MIDI");
	fMovieIcon16            = GetMICNFromResource("Movie");
	fMovieIcon32            = GetICONFromResource("Movie");
	fPictureIcon16          = GetMICNFromResource("Picture");
	fPictureIcon32          = GetICONFromResource("Picture");
	fTextIcon16             = GetMICNFromResource("Text");
	fTextIcon32             = GetICONFromResource("Text");

	fLock                           = GetIcon16FromResource("Lock");
	fUnlock                         = GetIcon16FromResource("Unlock");
	fLockDark                       = GetIcon16FromResource("LockDark");
	fUnlockDark             = GetIcon16FromResource("UnlockDark");
	fVisibleUp              = GetIcon16FromResource("VisibleUp");
	fVisibleDown            = GetIcon16FromResource("VisibleDown");
	fTransitionIn           = GetIcon16FromResource("TransitionIn");
	fTransitionOut  = GetIcon16FromResource("TransitionOut");
	fPathIn                         = GetIcon16FromResource("Path");
	fPathOut                        = GetIcon16FromResource("Path");

	fAnimationUpIcon        = GetCicnFromResource("AnimationCueUp");
	fAnimationDnIcon        = GetCicnFromResource("AnimationCueDn");
	fAudioUpIcon            = GetCicnFromResource("AudioCueUp");
	fAudioDnIcon            = GetCicnFromResource("AudioCueDn");
	fMIDIUpIcon             = GetCicnFromResource("MIDICueUp");
	fMIDIDnIcon             = GetCicnFromResource("MIDICueDn");
	fMovieUpIcon            = GetCicnFromResource("MovieCueUp");
	fMovieDnIcon            = GetCicnFromResource("MovieCueDn");
	fPictureUpIcon  = GetCicnFromResource("PictureCueUp");
	fPictureDnIcon  = GetCicnFromResource("PictureCueDn");
	fTextUpIcon             = GetCicnFromResource("TextCueUp");
	fTextDnIcon             = GetCicnFromResource("TextCueDn");

	// Header icons
	fMuteUp                         = GetIcon16FromResource("MuteUp");
	fMuteDown                       = GetIcon16FromResource("MuteDown");
	fSoloUp                         = GetIcon16FromResource("SoloUp");
	fSoloDown                       = GetIcon16FromResource("SoloDown");
	fContract                       = GetIcon16FromResource("Contract");
	fExpand                         = GetIcon16FromResource("Expand");

	// Transitions
	fWipeDown                                       = GetIcon16FromResource("WipeDown");
	fWipeLeft                                       = GetIcon16FromResource("WipeLeft");
	fWipeRight                                      = GetIcon16FromResource("WipeRight");
	fWipeUp                                 = GetIcon16FromResource("WipeUp");
	fWipeBottomLeftTopRight = GetIcon16FromResource("WipeBottomLeftTopRight");
	fWipeBottomRightTopLeft = GetIcon16FromResource("WipeBottomRightTopLeft");
	fWipeTopLeftBottomRight = GetIcon16FromResource("WipeTopLeftBottomRight");
	fWipeTopRightBottomLeft = GetIcon16FromResource("WipeTopRightBottomLeft");
	fCurtainsIn                             = GetIcon16FromResource("CurtainsIn");
	fCurtainsOut                            = GetIcon16FromResource("CurtainsOut");
	fRevealBottomLeftTopRight       = GetIcon16FromResource("RevealBottomLeftTopRight");
	fRevealBottomRightTopLeft       = GetIcon16FromResource("RevealBottomRightTopLeft");
	fRevealTopLeftBottomRight       = GetIcon16FromResource("RevealTopLeftBottomRight");
	fRevealTopRightBottomLeft       = GetIcon16FromResource("RevealTopRightBottomLeft");
	fRevealUp                                       = GetIcon16FromResource("RevealUp");
	fRevealDown                             = GetIcon16FromResource("RevealDown");
	fRevealLeft                             = GetIcon16FromResource("RevealLeft");
	fRevealRight                            = GetIcon16FromResource("RevealRight");

	//	Controls
	fExportSliderLeft               = GetCicnFromResource("ExportSliderLeft");
	fExportSliderRight              = GetCicnFromResource("ExportSliderRight");;

	//	Keyframes
	fKeyframeOff    = GetCicnFromResource("KeyframeOff");
	fKeyframeOn     = GetCicnFromResource("KeyframeOn");
}

//---------------------------------------------------------------------
//	GetIconByName
//---------------------------------------------------------------------
//
//	Return icon by resource name

BBitmap* TMuseumIcons::GetIconByName(const char* theName)
{

	if ( strcmp(theName, "TransitionIn") == 0 )
		return fTransitionIn;

	if ( strcmp(theName, "TransitionOut") == 0 )
		return fTransitionOut;

	if ( strcmp(theName, "WipeDown") == 0 )
		return fWipeDown;

	if ( strcmp(theName, "WipeLeft") == 0 )
		return fWipeLeft;

	if ( strcmp(theName, "WipeRight") == 0 )
		return fWipeRight;

	if ( strcmp(theName, "WipeUp") == 0 )
		return fWipeUp;

	if ( strcmp(theName, "WipeBottomLeftTopRight") == 0 )
		return fWipeBottomLeftTopRight;

	if ( strcmp(theName, "WipeBottomRightTopLeft") == 0 )
		return fWipeBottomRightTopLeft;

	if ( strcmp(theName, "WipeTopLeftBottomRight") == 0 )
		return fWipeTopLeftBottomRight;

	if ( strcmp(theName, "WipeTopRightBottomLeft") == 0 )
		return fWipeTopRightBottomLeft;

	if ( strcmp(theName, "CurtainsIn") == 0 )
		return fCurtainsIn;

	if ( strcmp(theName, "CurtainsOut") == 0 )
		return fCurtainsOut;

	if ( strcmp(theName, "RevealBottomLeftTopRight") == 0 )
		return fRevealBottomLeftTopRight;

	if ( strcmp(theName, "RevealBottomRightTopLeft") == 0 )
		return fRevealBottomRightTopLeft;

	if ( strcmp(theName, "RevealTopLeftBottomRight") == 0 )
		return fRevealTopLeftBottomRight;

	if ( strcmp(theName, "RevealTopRightBottomLeft") == 0 )
		return fRevealTopRightBottomLeft;

	if ( strcmp(theName, "RevealUp") == 0 )
		return fRevealUp;

	if ( strcmp(theName, "RevealDown") == 0 )
		return fRevealDown;

	if ( strcmp(theName, "RevealLeft") == 0 )
		return fRevealLeft;

	if ( strcmp(theName, "RevealRight") == 0 )
		return fRevealRight;

	return NULL;
}
