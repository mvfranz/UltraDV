//-------------------------------------------------------------------
//
//	File:	AppConstants.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//-------------------------------------------------------------------

#ifndef __APPCONSTANTS_H__
#define __APPCONSTANTS_H__

// Colors
const rgb_color kBlack 			= {0, 	0, 	  0,  255};
const rgb_color kWhite 			= {255, 255, 255, 255};
const rgb_color kPaleGrey 		= {240, 240, 240, 255};
const rgb_color kLightGrey 		= {225, 225, 225, 255};
const rgb_color kSteelGrey 		= {200, 200, 200, 255};
const rgb_color kMediumGrey 	= {128, 128, 128, 255};
const rgb_color kGrey 			= {216, 216, 216, 255};
const rgb_color kDarkGrey 		= {64, 64, 64, 255};

const rgb_color kLightMetallicBlue 	= {143,	166, 240, 255};
const rgb_color kMedMetallicBlue 	= {75, 	96,  154, 255};
const rgb_color kDarkMetallicBlue 	= {78, 	89,  126, 255};

const rgb_color kSteelBlue 			= {205, 205, 255, 255};
const rgb_color kMediumSteelBlue 	= {153, 153, 255, 255};
const rgb_color kDarkSteelBlue 		= {100, 100, 255, 255};
const rgb_color kSIGrey 			= {171, 167, 166, 255};
const rgb_color kBlueGrey 			= {160, 160, 160, 255};
const rgb_color kHeaderGrey 		= {138, 134, 131, 255};

const rgb_color kAdobeBrown 		= {152, 102, 51,  255};
const rgb_color kGreyViolet 		= {100, 100, 109, 255};

const rgb_color kLightKhaki 		= {209, 209, 159, 255};
const rgb_color kKhaki 				= {144, 144, 105, 255};
const rgb_color kDarkKhaki 			= {91,  91,  41, 255};

const rgb_color kLightTaupe 		= {175, 194, 175, 255};
const rgb_color kTaupe 				= {100, 119, 100, 255};
const rgb_color kDarkTaupe 			= {28,  47,  28,  255};

const rgb_color kRed 				= {255,	0, 	 0,   255};
const rgb_color kGreen 				= {0, 	255, 0,   255};
const rgb_color kBlue 				= {0, 	0, 	 255, 255};

const rgb_color kBeHighlight 		= {255, 255, 255, 255};
const rgb_color kBeGrey 			= {216, 216, 216, 255};
const rgb_color kBeDarkGrey 		= {200, 200, 200, 255};
const rgb_color kBeLightShadow 		= {194, 194, 194, 255};  // was 184
const rgb_color kBeShadow 			= {152, 152, 152, 255};
const rgb_color kBeFocusBlue 		= {0, 	 0,  229, 255};
const rgb_color kBeTitleBarYellow 	= {255, 203, 0,	  255};

const rgb_color kTextHilite 	= {188, 183, 182, 255}; 
const rgb_color kTextShadow 	= {100,  94,  90, 255}; 

// Visual element constants
const int16	kTickSpacing 		= 100;
const int16	kHeaderWidth 		= 83;
const int16 kChannelMinHeight 	= 45;
const int16 kChannelMaxHeight 	= 85;
const int16	kMenuHeight 		= 20;
const int16 kToolbarHeight 		= 20;
const int16 kExportZoneHeight 	= 16;
const int16 kTimelineHeight 	= 35;
const int16 kTimeScalerWidth 	= 170;
const int16 kTimeIndicatorWidth	= 8;
const int16 kTimeIndicatorHeight= 8;
const int16 kTimeTextOffset 	= 5;
const int16 kTimeTextWidth 		= 66;
const int16 kTimeTextHeight		= 11;

const int16 kScrollWidth 		= 14;
const int16 kScrollHeight 		= 14;

// Timeline elements
const int16 kTickHeight 			= 8;
const int16	kIndicatorWidth 		= 16;
const int16	kIndicatorHeight 		= 10;
const int16 kIndicatorZoneHeight	= 10;

const int16 kCueInset = 4;


// Cues
const int16	kMinCueWidth 			= kTickSpacing;
const int16 kCueIconWidth 			= 40;
const int16 kCueIconHeight 			= 40;
const int16 kCueButtonOffset 		= 5;
const int16 kCueTextOffset 			= 15;
const int16 kCueButtonWidth 		= 15;
const int16 kCueButtonHeight 		= 15;
const int16 kTriangleHeight 		= 7;
const int16 kTextHeight 			= 15;
const int16 kChannelHeight 			= 20;
const int16 kDurationDeltaOffset 	= 1;
const int16	kItemOffscreenValue		= -500;
const int16 kCueTitleHeight 		= 5;

// ABH const 
enum actionCues
{
	kActionCue,
	kButtonCue,
	kCDCue,
	kControlCue,
	kMarkerCue,
	kPauseCue,
	kVideoCue,
	kTotalActionCues
};

//const 
enum mediaCues
{
	kAnimationCue = kTotalActionCues,
	kAudioCue,
	kMIDICue,
	kMovieCue,
	kPictureCue,
	kSlideCue,
	kTextCue,
	kTotalMediaCues = (kTextCue - kTotalActionCues) + 1
};

// Cursors
//const 
enum
{
	kTestCursor,
	kHResizeCursor
};

// Paths
// ABH
// old const char kPresetsPathString[] = "./add-ons/settings/presets/"; 
const char kPresetsPathString[] = "/boot/home/config/settings/UltraDV/add-ons/settings/presets/";	

// File Types
const char kCueSheetTypeString[] = "application/x-mediapede-cuesheet";
const char kSettingsTypeString[] = "application/x-mediapede-settings";

//	Threads
#define DEFAULT_TIMEOUT 6000000L

#endif
