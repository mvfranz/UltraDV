//---------------------------------------------------------------------
//
//	File:	TCueData.cpp
//
//	Author:	Mike Ost
//
//	Date:	10.09.98
//
//	Desc:
//
//	TCueData is a class which handles the data which supports an 
//	ultraDV cue. The primary function of the class is to stream 
//	data out to the application for	display and/or playback. The 
//	cue will be asked to respond in real-time as well as during 
//	non-real-time operations like scrubbing, or editing.
//	
//	The class has other supporting behaviors regarding the MIME types 
//	it supports, sample icons, and the like. But its main responsibilities 
//	are for pulling data from a file and streaming it out to ultraDV.
//
//---------------------------------------------------------------------

#include "BuildApp.h"

#include "TCueData.h"

#include <support/Debug.h>		// for ASSERT()

// Class Constants
	// Name used for m_entryRef in a message
const char* TCueData::kRefTagName = "ref";
	// Contains all the MIME types supported by this class
TCueData::TTypeVector TCueData::sm_types;
	// The default file name
string TCueData::sm_defaultName;
	// The 'up' and 'down' cue icons
BBitmap* TCueData::sm_upIcon;
BBitmap* TCueData::sm_downIcon;
	// Type information
bool TCueData::sm_isVisual;
bool TCueData::sm_isAudible;
bool TCueData::sm_canStretch;

//---------------------------------------------------------------------
//	Constructor/destructor
//---------------------------------------------------------------------
//

TCueData::TCueData()
{
}

TCueData::TCueData(BMessage* msg)
{
	msg->FindRef(kRefTagName, &m_entryRef);
}

TCueData::~TCueData()
{
}


//---------------------------------------------------------------------
// BArchivable overrides (file support)
//---------------------------------------------------------------------
//
//

BArchivable* TCueData::Instantiate(BMessage* data)
{
	// NOTE: this is an error. We can't instantiate this class
	// since it is an abstract base class.
	ASSERT(false);
	return 0; 
}

status_t TCueData::Archive(BMessage* data, bool deep) const
{
	// Start by calling inherited archive
	status_t err = BArchivable::Archive(data, deep);
	if (err != B_OK)
		return err;
	
	// NOTE: we don't instantiate this class --- it's an ABC ---
	// so we don't add the class name data

	// Add the file ref
	data->AddRef(kRefTagName, &m_entryRef);
	return B_OK;
}


//---------------------------------------------------------------------
// IsValidFile
//---------------------------------------------------------------------
//
//
	
bool TCueData::IsValidFile(const entry_ref& ref) const
{
	// Get the file's mime type
	char type[B_MIME_TYPE_LENGTH];
	BNode node(&ref);
	BNodeInfo info(&node);
	if (info.GetType(type) != B_NO_ERROR)
		return false;
		
	// work through the type list and look for a match
	for (TTypeIterator t = FirstType(); t != EndOfTypes(); t++) {
		if (*t == type)
			return true;
	}

	return false;	
}

//---------------------------------------------------------------------
// SetTo
//---------------------------------------------------------------------
//
//
	
status_t TCueData::SetTo(const entry_ref& ref)
{
	m_entryRef = ref;
	return B_NO_ERROR;
}

//---------------------------------------------------------------------
// Unset
//---------------------------------------------------------------------
//
//
	
void TCueData::Unset(void)
{
	// initialize to the default
	m_entryRef = entry_ref();
}
	
//---------------------------------------------------------------------
// Info
//---------------------------------------------------------------------
//
//
	
string TCueData::Info() const
{
	return "-";
}

//---------------------------------------------------------------------
// NodeChangedMessage
//---------------------------------------------------------------------
//
//
	
bool TCueData::NodeChangedMessage(BMessage* msg)
{
	// TODO: do we need to respond? Update m_entryRef perhaps?
	return false;
}


//---------------------------------------------------------------------
// Description
//---------------------------------------------------------------------
//
//

string TCueData::Description() const
{
	return string("Nothing");
}


//---------------------------------------------------------------------
// OriginalArea
//---------------------------------------------------------------------
//	
//	The cue's natural size, if applicable. This won't be called
// 	for cues that aren't naturally visible.
//

BRect TCueData::OriginalArea() const
{
	return (BRect(0, 0, 0, 0));
}


//---------------------------------------------------------------------
// OriginalArea
//---------------------------------------------------------------------
//	
//	The cue's duration in milliseconds, derived from the file.
//	If a cue has no natural duration, this is a default value for it.
//

int32 TCueData::OriginalDuration() const
{
	return 0;
}
