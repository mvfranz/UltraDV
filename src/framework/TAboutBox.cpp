//-------------------------------------------------------------------
//
//	File:	TAboutBox.cpp
//
//	Author:	Gene Z. Ragan
//
//	Data:	5.23.97
//
//-------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <interface/StringView.h>

#include "MuseumApp.h"
#include "TAboutBox.h"
#include "TBitmapView.h"

//-------------------------------------------------------------------
//	Constructor
//-------------------------------------------------------------------
//
//

const BRect aboutFrame(200, 100, 703, 409); // 504 310

TAboutBox::TAboutBox() : BWindow(aboutFrame, "About BeDV", B_MODAL_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	Init();
}


//-------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------
//
//

TAboutBox::~TAboutBox()
{
}


//-------------------------------------------------------------------
//	Init
//-------------------------------------------------------------------
//
//

void TAboutBox::Init()
{
	
	#define ABOUT_OK_MSG 'abot'
	
	//if (Lock())
	//{
	//	Activate();
	//	Unlock();
	//	return;
	//}
	

	// Get application info
	app_info info;
	
	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);
	if (file.InitCheck())
		return;
	
	size_t 		size;
	BBitmap 	*data;
	{
		BResources res(&file);
		data = (BBitmap *)res.FindResource('bits', "Splash", &size);
		if (!data)			
			return;
	}
	
	// Load bitmap
	BBitmap *bitmap = new BBitmap(Bounds(), B_COLOR_8_BIT);
	bitmap->SetBits(data, size, 0, B_COLOR_8_BIT);
	//BBitmap *bitmap = new BBitmap(Bounds(), B_RGB_32_BIT);
	//bitmap->SetBits(data, size, 0, B_RGB_32_BIT);
	
	TBitmapView *bm = new TBitmapView(Bounds(), "AboutBitmap", bitmap, true);

	// Add it to the window
	AddChild(bm);

	
	// Create the OK button
	const BRect okFrame( ( (Bounds().right - Bounds().left)/2)-30 , Bounds().bottom-30, ((Bounds().right-Bounds().left)/2)+30,  Bounds().bottom-10);
	m_OKButton = new BButton(okFrame, "OK","OK", new BMessage(ABOUT_OK_MSG), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);						 
	bm->AddChild(m_OKButton);
	m_OKButton->MakeDefault(true);
}


//-------------------------------------------------------------------
//
//	Function:	MessageReceived
//
//	Desc:		
//
//-------------------------------------------------------------------
//
//

void TAboutBox::MessageReceived(BMessage* message)
{

	switch(message->what)
	{
		case ABOUT_OK_MSG:
			Lock();
			Quit();
			break;
			
		default:
			BWindow::MessageReceived(message);						
			break;
	}
	
}
