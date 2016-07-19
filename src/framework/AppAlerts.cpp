//---------------------------------------------------------------------
//
//	File:	AppAlerts.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.14.98
//
//	Desc:	Alert Handling Routines
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <interface/InterfaceDefs.h>
#include <support/Debug.h>

#include "AppAlerts.h"
#include "AppUtils.h"

//---------------------------------------------------------------------
//	CueBoundsAlert
//---------------------------------------------------------------------
//
//	Inform user that a moved cue has violated the bounds of another cue
//

void CueBoundsAlert()
{
	BAlert *theAlert = new BAlert("Museum", "A cue may not occupy the same\n space as another cue.", "OK");
	CenterWindow(theAlert);
	theAlert->Go();
}


//---------------------------------------------------------------------
//	UnhandledImageFormat
//---------------------------------------------------------------------
//
//	Inform user that we cannot read the data in the image
//

void UnhandledImageFormat(entry_ref *ref)
{
	char msg[1024];
	sprintf(msg, "No Translation Kit add-on found for '%s'.", ref->name);
	BAlert *theAlert = new BAlert("Museum", msg, "OK");
	CenterWindow(theAlert);
	theAlert->Go();
}

//---------------------------------------------------------------------
//	UnhandledImageFormat
//---------------------------------------------------------------------
//
//	Inform user that we cannot read the data in the image
//

void UnhandledMIDIFormat(entry_ref *ref)
{
	char msg[1024];
	sprintf(msg, "Unable to read '%s'.  Invalid MIDI file.", ref->name);
	BAlert *theAlert = new BAlert("Museum", msg, "OK");
	CenterWindow(theAlert);
	theAlert->Go();
}


//---------------------------------------------------------------------
//	SaveAlert
//---------------------------------------------------------------------
//
//	Inform user that the document is dirty and should be saved
//

int32 SaveAlert()
{
	int32 result;

	BAlert *theAlert = new BAlert( "Museum", "This document has been changed since the last save.  Save before quiting?",
	                               "Don't Save", "Cancel", "Save", B_WIDTH_FROM_WIDEST, B_WARNING_ALERT);

	theAlert->SetShortcut(0, B_ESCAPE);
	CenterWindow(theAlert);
	result = theAlert->Go();

	return (result);
}



//---------------------------------------------------------------------
//	ErrorAlert
//---------------------------------------------------------------------
//
//	General purpose error dialog
//

int32 ErrorAlert(char *theError)
{
	int32 result;

	BAlert *theAlert = new BAlert( "Museum", theError, "OK", NULL, NULL, B_WIDTH_FROM_WIDEST, B_WARNING_ALERT);

	theAlert->SetShortcut(0, B_ESCAPE);
	CenterWindow(theAlert);
	result = theAlert->Go();

	return (result);
}
