//---------------------------------------------------------------------
//
//	File:	TVideoSettingsTabView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Video Settings Tab VIew
//
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
#include "ResourceManager.h"

#include "TVideoCompressionView.h"
#include "TVideoImageView.h"
#include "TVideoSettingsView.h"
#include "TVideoSourceView.h"

#include "TVideoSettingsTabView.h"

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//
								
TVideoSettingsTabView::TVideoSettingsTabView(TVideoSettingsView *parent, BRect frame, const char *name) :  
//				BTabView(frame, name, B_WIDTH_AS_USUAL, B_FOLLOW_ALL, B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE|B_NAVIGABLE_JUMP|B_FRAME_EVENTS|B_NAVIGABLE)
				BTabView(frame, name)
{
	fParent = parent,
	
	Init();
}


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//	Construct from BMesage
//

TVideoSettingsTabView::TVideoSettingsTabView( BMessage *data) : BTabView(data)
{
	Init();
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TVideoSettingsTabView::Init() 
{
	BMessage *theMessage = NULL;
		
	//
	// Create all the tab views
	//
			
	// Get VideoCompressionView from resource
	BTab *compressionTab = new BTab(); 	
	theMessage = GetWindowFromResource("VideoCompressionView");
	ASSERT(theMessage);
	fCompressionView = new TVideoCompressionView(this, theMessage);
	AddTab(fCompressionView, compressionTab); 
	compressionTab->SetLabel("Compression"); 
	
	// Get VideoImageView from resource
	BTab *imageTab = new BTab(); 	
	theMessage = GetWindowFromResource("VideoImageView");
	ASSERT(theMessage);		
	fImageView = new TVideoImageView(this, theMessage);
	AddTab(fImageView, imageTab); 
	imageTab->SetLabel("Image"); 
	
	// Get VideoImageSource from resource
	BTab *sourceTab = new BTab(); 	
	theMessage = GetWindowFromResource("VideoSourceView");
	ASSERT(theMessage);		
	fSourceView = new TVideoSourceView(this, theMessage);
	AddTab(fSourceView, sourceTab); 
	sourceTab->SetLabel("Source"); 
}
	
#pragma mark -
#pragma mark === Archiving Functions ===

//---------------------------------------------------------------------
//	Instantiate
//---------------------------------------------------------------------
//
//

BArchivable *TVideoSettingsTabView::Instantiate(BMessage *archive) 
{ 

	if ( validate_instantiation(archive, "TVideoSettingsTabView") ) 
		return new TVideoSettingsTabView(archive); 
		
	return NULL; 
}
   
//---------------------------------------------------------------------
//	Archive
//---------------------------------------------------------------------
//
//

status_t TVideoSettingsTabView::Archive(BMessage *data, bool deep) const
{
		
	status_t myErr;
	
	Looper()->Lock();
	
	// Start by calling inherited archive
	myErr = BTabView::Archive(data, deep);
						
	if (myErr == B_OK)
	{
					
	}
	
	Looper()->Unlock();
	
	return myErr;
}
