//---------------------------------------------------------------------
//
//	File:	AVIAddOn.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	09.23.98
//
//	Desc:	AVI Media Kit AddOn
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


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
/*
   extern "C" _EXPORT BMediaAddOn *make_media_addon(image_id addOnID)
   {
        return new AVIAddOn(addOnID);
   }
 */

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

AVIAddOn::AVIAddOn(image_id image) : BMediaAddOn(image)
{

}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

AVIAddOn::~AVIAddOn()
{
}


//-------------------------------------------------------------------
//	InitCheck
//-------------------------------------------------------------------
//
//	Return status of initilization
//

status_t AVIAddOn::InitCheck( const char ** out_failure_text)
{
	return B_OK;
}


//-------------------------------------------------------------------
//	CountFlavors
//-------------------------------------------------------------------
//
//	Return number of flavors we can handle
//

int32 AVIAddOn::CountFlavors()
{
	return 1;
}


//-------------------------------------------------------------------
//	GetFlavorAt
//-------------------------------------------------------------------
//
//	Return info about requested flavor
//

status_t AVIAddOn::GetFlavorAt( int32 flavorNum, flavor_info **outInfo)
{
	//	Check for flavor violation
	if ( flavorNum > kNumFlavors)
		return B_ERROR;

	//	Create flavor_info struct
	flavor_info *info = new flavor_info;

	//	Fill struct for requested flavor
	sprintf(info->name, "%s", "AVIAddOn");
	sprintf(info->info, "%s", "Playback and record AVI data.");
	info->kinds                     = B_BUFFER_PRODUCER | B_BUFFER_CONSUMER | B_FILE_INTERFACE;
	info->flavor_flags              = 0;
	info->internal_id               = 0x0EE0;
	info->possible_count    = 0;
	info->in_format_count   = 1;
	info->in_format_flags   = 0;
	info->in_formats                = 0;
	info->out_format_count  = 1;
	info->out_format_flags  = 0;
	info->out_formats               = 0;

	//	Set outInfo to point to struct
	outInfo = &info;

	return B_OK;
}


//-------------------------------------------------------------------
//	InstantiateNodeFor
//-------------------------------------------------------------------
//
//	Instantiate requested node
//

BMediaNode *AVIAddOn::InstantiateNodeFor( const flavor_info *info, BMessage *config, status_t *outError)
{
	//	Check for special configuration info
	//	config

	//	Setup return values
	status_t retVal  = B_ERROR;
	BMediaNode *theNode = NULL;

	//	What type of node to they want?

	//	return results
	*outError = B_ERROR;
	return theNode;
}


//-------------------------------------------------------------------
//	GetConfigurationFor
//-------------------------------------------------------------------
//
//	Save node configuaration stored in BMessage.  Used for archiving
//

status_t AVIAddOn::GetConfigurationFor( BMediaNode *your_node, BMessage * into_message)
{
	return B_ERROR;
}


#pragma mark -
#pragma mark === AutoStart Handling ===

//-------------------------------------------------------------------
//	WantsAutoStart
//-------------------------------------------------------------------
//
//	Do any nodes need to be created at launch time?
//

bool AVIAddOn::WantsAutoStart()
{
	return false;
}


//-------------------------------------------------------------------
//	AutoStart
//-------------------------------------------------------------------
//
//	AutoStart request.   Fix error retruned...
//

status_t AVIAddOn::AutoStart( int in_count, BMediaNode **out_node, int32 *out_internal_id, bool *out_has_more)
{
	return B_ERROR;
}


#pragma mark -
#pragma mark === FileInterface Handling ===


//-------------------------------------------------------------------
//	SniffRef
//-------------------------------------------------------------------
//
//	Do we have a node that can handle this file ref type?
//

status_t AVIAddOn::SniffRef( const entry_ref &file, BMimeType *io_mime_type, float *out_quality, int32 *out_internal_id)
{



}


//-------------------------------------------------------------------
//	SniffType
//-------------------------------------------------------------------
//
//	Do we have a node that can handle this mime type?
//

status_t AVIAddOn::SniffType( const BMimeType &type, float *out_quality, int32 *out_internal_id)
{


}
