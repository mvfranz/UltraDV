//
//
// Pops up alert dialog
//
//
// ABH 2/7/2013

#include "InfoAlert.h"

int32 InfoAlert(char *theError)
{
	int32 result;

	BAlert *theAlert = new BAlert( "UltraDV", theError, "OK", NULL, NULL, B_WIDTH_FROM_WIDEST, B_WARNING_ALERT);

	theAlert->SetShortcut(0, B_ESCAPE);
	result = theAlert->Go();

	return (result);
}

