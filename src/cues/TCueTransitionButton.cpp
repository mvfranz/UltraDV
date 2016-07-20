//---------------------------------------------------------------------
//
//	File:	TCueTransitionButton.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.14.98
//
//	Desc:	Cue transition button.
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "AppMessages.h"
#include "MuseumApp.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TCueTransitionButton.h"
#include "TCueSheetWindow.h"
#include "TCueSheetView.h"
#include "TCueView.h"


// Constants

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueTransitionButton::TCueTransitionButton( TCueView* parent, BRect bounds, const char* name, BBitmap* offBitmap, BBitmap* onBitmap, BHandler* handler, uint32 flags, bool transitionIn) :
	TChannelCueButton(parent, bounds, name, offBitmap, onBitmap, handler, flags)
{

	fTransitionIn = transitionIn;
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueTransitionButton::TCueTransitionButton(const TCueTransitionButton* theButton) : TChannelCueButton(theButton)
{
	fTransitionIn = theButton->fTransitionIn;
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueTransitionButton::TCueTransitionButton(BMessage* data) : TChannelCueButton(data)
{
	// Find our member variables in the BMessage
	data->FindBool("TransitionIn", &fTransitionIn);
}

//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueTransitionButton::~TCueTransitionButton()
{
}


#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable* TCueTransitionButton::Instantiate(BMessage* archive)
{

	if ( validate_instantiation(archive, "TCueTransitionButton") )
		return new TCueTransitionButton(archive);

	return NULL;
}

//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TCueTransitionButton::Archive(BMessage* data, bool deep) const
{

	status_t myErr;

	Looper()->Lock();

	// Start by calling inherited archive
	myErr = TChannelCueButton::Archive(data, deep);

	if (myErr == B_OK) {
		// Add our class name to the archive
		data->AddString("class", "TCueTransitionButton");

		// Add our member variables to the BMessage
		data->AddBool("TransitionIn", fTransitionIn);

	}

	Looper()->Unlock();

	return myErr;
}


#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Double check that we have a valid parent.
//	This is intended for use when instantiating cues from an archive.
//

void TCueTransitionButton::AttachedToWindow()
{
	if (fCue == NULL) {
		fCue            = (TCueView*)Parent();
		fHandler        = fCue;

		if (fTransitionIn) {
			fOffBitmap = GetAppIcons()->fTransitionIn;
			fOnBitmap  = GetAppIcons()->fTransitionIn;
		} else {
			fOffBitmap = GetAppIcons()->fTransitionOut;
			fOnBitmap  = GetAppIcons()->fTransitionOut;
		}
	}

	//	Pass up to parent
	BView::AttachedToWindow();
}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//	Handle messages
//

void TCueTransitionButton::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
	// Set icon button to new transition
	case CHANGE_ICON_MSG:
	{
		const char* namePtr;
		message->FindString("TransitionName", &namePtr);
		BBitmap* offBitmap= GetAppIcons()->GetIconByName(namePtr);
		BBitmap* onBitmap = GetAppIcons()->GetIconByName(namePtr);
		if (offBitmap) {
			fOffBitmap = offBitmap;
			fOnBitmap  = onBitmap;
			Invalidate();
		}

		// Tell cue view
		Parent()->Window()->PostMessage(message, Parent());
	}
	break;


	default:
		TChannelCueButton::MessageReceived(message);
		break;
	}
}


#pragma mark -
#pragma mark === Click Handling ===

//---------------------------------------------------------------------
//	MouseDown
//---------------------------------------------------------------------
//
//	Handle mouse down events
//

void TCueTransitionButton::MouseDown(BPoint where)
{

	// Create and send message
	BMessage* message = new BMessage(TRANSITION_BUTTON_MSG);
	BPoint menuPt( Frame().right, Frame().top);
	message->AddPoint("MenuPoint", menuPt);
	message->AddBool("TransitionIn", fTransitionIn);
	fCue->Window()->PostMessage(message, fCue);
	delete message;

	// Restore and invalidate
	fMouseDown = false;

}

#pragma mark -
#pragma mark === Click Handling ===

//---------------------------------------------------------------------
//	DoClick
//---------------------------------------------------------------------
//
//	Button has been clicked
//

void TCueTransitionButton::DoClick()
{
}
