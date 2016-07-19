// Make_media_addon.cpp
// separated

//	Includes
#include <MediaDefs.h>
#include <MediaAddOn.h>

#include <stdio.h>

#include "BuildApp.h"

#include "AVIAddOn.h"


//-------------------------------------------------------------------
//	make_media_addon
//-------------------------------------------------------------------
//
//	Instantiation function used by media kit
//

// ABH updated

extern "C" _EXPORT BMediaAddOn *make_media_addon(image_id addOnID)
{
	return new AVIAddOn(addOnID);
}

