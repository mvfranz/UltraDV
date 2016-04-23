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
	delete m_AnimationIcon16;
	delete m_Animation32;
	delete m_AudioIcon16;
	delete m_AudioIcon32;
	delete m_MIDIIcon16;
	delete m_MIDIIcon32;
	delete m_MovieIcon16;
	delete m_MovieIcon32;
	delete m_PictureIcon16;
	delete m_PictureIcon32;
	delete m_TextIcon16;
	delete m_TextIcon32;
		
	delete m_Lock;
	delete m_Unlock;
	delete m_LockDark;
	delete m_UnlockDark;
	delete m_VisibleUp;
	delete m_VisibleDown;
	delete m_TransitionIn;
	delete m_TransitionOut;
	delete m_PathIn;
	delete m_PathOut;	
	
	delete m_AnimationUpIcon;
	delete m_AnimationDnIcon;
	delete m_AudioUpIcon;
	delete m_AudioDnIcon;
	delete m_MIDIUpIcon;
	delete m_MIDIDnIcon;
	delete m_MovieUpIcon;
	delete m_MovieDnIcon;
	delete m_PictureUpIcon;
	delete m_PictureDnIcon;
	delete m_TextUpIcon;		
	delete m_TextDnIcon;		
	
	delete m_MuteUp;
	delete m_MuteDown;
	delete m_SoloUp;
	delete m_SoloDown;
	delete m_Contract;
	delete m_Expand;
	
	// Free transition icons
	delete m_WipeDown;
	delete m_WipeLeft;
	delete m_WipeRight;
	delete m_WipeUp;
	delete m_WipeBottomLeftTopRight;
	delete m_WipeBottomRightTopLeft;
	delete m_WipeTopLeftBottomRight;
	delete m_WipeTopRightBottomLeft;
	delete m_CurtainsIn;
	delete m_CurtainsOut;
	delete m_RevealBottomLeftTopRight;
	delete m_RevealBottomRightTopLeft;
	delete m_RevealTopLeftBottomRight;
	delete m_RevealTopRightBottomLeft;
	delete m_RevealUp;
	delete m_RevealDown;
	delete m_RevealLeft;
	delete m_RevealRight;
	
	//	Free controls
	delete m_ExportSliderLeft;
	delete m_ExportSliderRight;

	//	Free keyframes
	delete m_KeyframeOff;
	delete m_KeyframeOn;

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TMuseumIcons::Init()
{

	// Cue icons
	m_AnimationIcon16 	= GetMICNFromResource("Animation");
	m_Animation32		= GetICONFromResource("Animation");
	m_AudioIcon16		= GetMICNFromResource("Audio");
	m_AudioIcon32		= GetICONFromResource("Audio");
	m_MIDIIcon16		= GetMICNFromResource("MIDI");
	m_MIDIIcon32		= GetICONFromResource("MIDI");
	m_MovieIcon16		= GetMICNFromResource("Movie");
	m_MovieIcon32		= GetICONFromResource("Movie");
	m_PictureIcon16		= GetMICNFromResource("Picture");
	m_PictureIcon32		= GetICONFromResource("Picture");
	m_TextIcon16		= GetMICNFromResource("Text");
	m_TextIcon32		= GetICONFromResource("Text");

	m_Lock 				= GetIcon16FromResource("Lock");
	m_Unlock 			= GetIcon16FromResource("Unlock");
	m_LockDark 			= GetIcon16FromResource("LockDark");
	m_UnlockDark 		= GetIcon16FromResource("UnlockDark");
	m_VisibleUp 		= GetIcon16FromResource("VisibleUp");
	m_VisibleDown 		= GetIcon16FromResource("VisibleDown");
	m_TransitionIn 		= GetIcon16FromResource("TransitionIn");
	m_TransitionOut 	= GetIcon16FromResource("TransitionOut");
	m_PathIn  			= GetIcon16FromResource("Path");
	m_PathOut  			= GetIcon16FromResource("Path");
		
	m_AnimationUpIcon 	= GetCicnFromResource("AnimationCueUp");
	m_AnimationDnIcon 	= GetCicnFromResource("AnimationCueDn");
	m_AudioUpIcon 		= GetCicnFromResource("AudioCueUp");
	m_AudioDnIcon 		= GetCicnFromResource("AudioCueDn");
	m_MIDIUpIcon 		= GetCicnFromResource("MIDICueUp");
	m_MIDIDnIcon 		= GetCicnFromResource("MIDICueDn");
	m_MovieUpIcon 		= GetCicnFromResource("MovieCueUp");
	m_MovieDnIcon 		= GetCicnFromResource("MovieCueDn");
	m_PictureUpIcon 	= GetCicnFromResource("PictureCueUp");
	m_PictureDnIcon 	= GetCicnFromResource("PictureCueDn");
	m_TextUpIcon 		= GetCicnFromResource("TextCueUp");
	m_TextDnIcon 		= GetCicnFromResource("TextCueDn");
	
	// Header icons	
	m_MuteUp 			= GetIcon16FromResource("MuteUp");
	m_MuteDown 			= GetIcon16FromResource("MuteDown");
	m_SoloUp 			= GetIcon16FromResource("SoloUp");
	m_SoloDown 			= GetIcon16FromResource("SoloDown");
	m_Contract 			= GetIcon16FromResource("Contract");
	m_Expand 			= GetIcon16FromResource("Expand");
	
	// Transitions
	m_WipeDown					= GetIcon16FromResource("WipeDown");
	m_WipeLeft					= GetIcon16FromResource("WipeLeft");
	m_WipeRight					= GetIcon16FromResource("WipeRight");
	m_WipeUp					= GetIcon16FromResource("WipeUp");
	m_WipeBottomLeftTopRight	= GetIcon16FromResource("WipeBottomLeftTopRight");
	m_WipeBottomRightTopLeft	= GetIcon16FromResource("WipeBottomRightTopLeft");
	m_WipeTopLeftBottomRight	= GetIcon16FromResource("WipeTopLeftBottomRight");
	m_WipeTopRightBottomLeft	= GetIcon16FromResource("WipeTopRightBottomLeft");
	m_CurtainsIn				= GetIcon16FromResource("CurtainsIn");
	m_CurtainsOut				= GetIcon16FromResource("CurtainsOut");
	m_RevealBottomLeftTopRight	= GetIcon16FromResource("RevealBottomLeftTopRight");
	m_RevealBottomRightTopLeft	= GetIcon16FromResource("RevealBottomRightTopLeft");
	m_RevealTopLeftBottomRight	= GetIcon16FromResource("RevealTopLeftBottomRight");
	m_RevealTopRightBottomLeft	= GetIcon16FromResource("RevealTopRightBottomLeft");
	m_RevealUp					= GetIcon16FromResource("RevealUp");
	m_RevealDown				= GetIcon16FromResource("RevealDown");
	m_RevealLeft				= GetIcon16FromResource("RevealLeft");
	m_RevealRight				= GetIcon16FromResource("RevealRight");	
	
	//	Controls
	m_ExportSliderLeft		= GetCicnFromResource("ExportSliderLeft");
	m_ExportSliderRight		= GetCicnFromResource("ExportSliderRight");;

	//	Keyframes
	m_KeyframeOff 	= GetCicnFromResource("KeyframeOff");
	m_KeyframeOn  	= GetCicnFromResource("KeyframeOn");
}

//---------------------------------------------------------------------
//	GetIconByName
//---------------------------------------------------------------------
//
//	Return icon by resource name

BBitmap *TMuseumIcons::GetIconByName(const char *theName)
{
	
	if ( strcmp(theName, "TransitionIn") == 0 )
		return m_TransitionIn;
	
	if ( strcmp(theName, "TransitionOut") == 0 )
		return m_TransitionOut;

	if ( strcmp(theName, "WipeDown") == 0 )
		return m_WipeDown;
			
	if ( strcmp(theName, "WipeLeft") == 0 )
		return m_WipeLeft;
	
	if ( strcmp(theName, "WipeRight") == 0 )
		return m_WipeRight;
	
	if ( strcmp(theName, "WipeUp") == 0 )
		return m_WipeUp;
		
	if ( strcmp(theName, "WipeBottomLeftTopRight") == 0 )
		return m_WipeBottomLeftTopRight;
			
	if ( strcmp(theName, "WipeBottomRightTopLeft") == 0 )
		return m_WipeBottomRightTopLeft;
	
	if ( strcmp(theName, "WipeTopLeftBottomRight") == 0 )
		return m_WipeTopLeftBottomRight;
	
	if ( strcmp(theName, "WipeTopRightBottomLeft") == 0 )
		return m_WipeTopRightBottomLeft;
		
	if ( strcmp(theName, "CurtainsIn") == 0 )
		return m_CurtainsIn;
			
	if ( strcmp(theName, "CurtainsOut") == 0 )
		return m_CurtainsOut;
	
	if ( strcmp(theName, "RevealBottomLeftTopRight") == 0 )
		return m_RevealBottomLeftTopRight;
	
	if ( strcmp(theName, "RevealBottomRightTopLeft") == 0 )
		return m_RevealBottomRightTopLeft;
		
	if ( strcmp(theName, "RevealTopLeftBottomRight") == 0 )
		return m_RevealTopLeftBottomRight;
	
	if ( strcmp(theName, "RevealTopRightBottomLeft") == 0 )
		return m_RevealTopRightBottomLeft;
					
	if ( strcmp(theName, "RevealUp") == 0 )
		return m_RevealUp;
	
	if ( strcmp(theName, "RevealDown") == 0 )
		return m_RevealDown;
		
	if ( strcmp(theName, "RevealLeft") == 0 )
		return m_RevealLeft;
	
	if ( strcmp(theName, "RevealRight") == 0 )
		return m_RevealRight;
	
	return NULL;
}
