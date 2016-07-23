//---------------------------------------------------------------------
//
//	File:	TMediaCuePaletteView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	MediaCue sheet view
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "ResourceManager.h"

#include "TMediaCuePaletteView.h"
#include "TCueButton.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMediaCuePaletteView::TMediaCuePaletteView(BRect bounds) : BView(bounds, "MediaCueView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMediaCuePaletteView::~TMediaCuePaletteView()
{
	// Cue button bitmaps are owned by TMuseumIcon object
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TMediaCuePaletteView::Init()
{
	BMessage* theMessage = NULL;
	BBitmap* offBitmap;
	BBitmap* onBitmap;

	// Set view background color
	SetViewColor(kBlueGrey);

	// Get application info
	app_info info;

	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);
	if (file.InitCheck())
		return;

	BResources res(&file);


	//-------------------------------------
	// Create the media cue icon subviews
	//-------------------------------------

	//
	// Row One
	//

	BRect cueRect = Bounds();

	// Animation Cue
	//

	// Set button location
	cueRect.left    += kCueButtonOffset;
	cueRect.right   =  cueRect.left + (kCueIconWidth-1);
	cueRect.top     += kCueButtonOffset;
	cueRect.bottom  =  cueRect.top + (kCueIconHeight-1);

	// Load bitmaps
	offBitmap       = GetAppIcons()->fAnimationUpIcon;
	onBitmap        = GetAppIcons()->fAnimationDnIcon;

	// Create button
	fAnimCueButton = new TCueButton(cueRect, "Animation Cue", offBitmap, onBitmap, theMessage, kAnimationCue);
	AddChild(fAnimCueButton);

	//
	//	Audio Cue

	// Set button location
	cueRect.left    = cueRect.right + kCueButtonOffset;
	cueRect.right   = cueRect.left + (kCueIconWidth-1);

	// Load bitmaps
	offBitmap       = GetAppIcons()->fAudioUpIcon;
	onBitmap        = GetAppIcons()->fAudioDnIcon;

	// Create button
	fAudioCueButton = new TCueButton(cueRect, "Audio Cue", offBitmap, onBitmap, theMessage, kAudioCue);
	AddChild(fAudioCueButton);


	//
	//	Row Two
	//

	// MIDI Cue
	//

	// Set button location
	cueRect.left    = Bounds().left + kCueButtonOffset;
	cueRect.right   = cueRect.left + (kCueIconWidth-1);
	cueRect.top     = cueRect.bottom + kCueButtonOffset;
	cueRect.bottom  = cueRect.top + (kCueIconHeight-1);

	// Load bitmaps
	offBitmap       = GetAppIcons()->fMIDIUpIcon;
	onBitmap        = GetAppIcons()->fMIDIDnIcon;

	// Create button
	fMIDICueButton = new TCueButton(cueRect, "MIDI Cue", offBitmap, onBitmap, theMessage, kMIDICue);
	AddChild(fMIDICueButton);


	// Movie Cue
	//

	// Set button location
	cueRect.left    = cueRect.right + kCueButtonOffset;
	cueRect.right   = cueRect.left + (kCueIconWidth-1);

	// Load bitmaps
	offBitmap       = GetAppIcons()->fMovieUpIcon;
	onBitmap        = GetAppIcons()->fMovieDnIcon;

	// Create button
	fMovieCueButton = new TCueButton(cueRect, "Movie Cue", offBitmap, onBitmap, theMessage, kMovieCue);
	AddChild(fMovieCueButton);


	//
	//	Row Three
	//

	// Picture Cue
	//

	// Set button location
	cueRect.left        = Bounds().left + kCueButtonOffset;
	cueRect.right   = cueRect.left + (kCueIconWidth-1);
	cueRect.top     = cueRect.bottom + kCueButtonOffset;
	cueRect.bottom  = cueRect.top + (kCueIconHeight-1);


	// Load bitmaps
	offBitmap       = GetAppIcons()->fPictureUpIcon;
	onBitmap        = GetAppIcons()->fPictureDnIcon;

	// Create button
	fPictureCueButton = new TCueButton(cueRect, "Picture Cue", offBitmap, onBitmap, theMessage, kPictureCue);
	AddChild(fPictureCueButton);


	// Text Cue
	//

	// Set button location
	cueRect.left    = cueRect.right + kCueButtonOffset;
	cueRect.right   = cueRect.left + (kCueIconWidth-1);

	// Load bitmaps
	offBitmap       = GetAppIcons()->fTextUpIcon;
	onBitmap        = GetAppIcons()->fTextDnIcon;

	// Create button
	fTextCueButton = new TCueButton(cueRect, "Text Cue", offBitmap, onBitmap, theMessage, kTextCue);
	AddChild(fTextCueButton);

}
