//---------------------------------------------------------------------
//
//	File:	TAudioSourceView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Child view of TAudioSourceView.  Handles display of audio
//			source settings.
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
#include "DebugUtils.h"
#include "AppUtils.h"
#include "ResourceManager.h"

#include "TAudioSettingsView.h"
#include "TAudioLevelsView.h"
#include "TLevelsSlider.h"

#include "TAudioSourceView.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioSourceView::TAudioSourceView(BMessage *archive) : BView(archive)
{
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TAudioSourceView::Init()
{
	// Grey background
	SetViewColor(kBeGrey); 
	
	// Locate view items
	m_DeviceMenuField 	= (BMenuField *)FindView("DeviceMenuField");
	m_InputMenuField 	= (BMenuField *)FindView("InputMenuField");
	m_VolumeBox			= (BBox *)FindView("VolumeBox");
	
	// Set up Input Slider
	BView *inputView = (BView *)FindView("InputView");
	m_InputSlider = new TLevelsSlider(inputView->Frame(), kAudioInputSlider);
	m_VolumeBox->AddChild(m_InputSlider);
	m_VolumeBox->RemoveChild(inputView);
	delete inputView;
	m_InputSlider->Show();

	// Set up Output Slider
	BView *outputView = (BView *)FindView("OutputView");
	m_OutputSlider = new TLevelsSlider(outputView->Frame(), kAudioOutputSlider);
	m_VolumeBox->AddChild(m_OutputSlider);
	m_VolumeBox->RemoveChild(outputView);
	delete outputView;
	m_OutputSlider->Show();
	
	// Set up level indicator
	BView 	*levelsView = (BMenuField *)FindView("LevelsView");
	m_AudioLevelsView = new TAudioLevelsView(levelsView->Frame());
	m_VolumeBox->RemoveChild(levelsView);
	delete levelsView;
	m_VolumeBox->AddChild(m_AudioLevelsView);

	// Setup menus
	BMenu *theMenu;
	
	// Setup Device menu
	theMenu = m_DeviceMenuField->Menu();
	if (theMenu)
	{
		// None
		BMenuItem *defaultItem = new BMenuItem("Default", new BMessage(DEVICE_DEFAULT_MSG) );		
		theMenu->AddItem(defaultItem);	

		// Select the first item
		defaultItem->SetMarked(true);
	}
	
	// Setup Input menu
	theMenu = m_InputMenuField->Menu();
	if (theMenu)
	{
		// None
		BMenuItem *noneItem = new BMenuItem("None", new BMessage(SOURCE_NONE_MSG) );		
		theMenu->AddItem(noneItem);	

		// Mic
		BMenuItem *micItem = new BMenuItem("Mic", new BMessage(SOURCE_MIC_MSG) );		
		theMenu->AddItem(micItem);	

		// Mic+20bB
		BMenuItem *mic20Item = new BMenuItem("Mic+20bB", new BMessage(SOURCE_MIC_20_MSG) );		
		theMenu->AddItem(mic20Item);	

		// CD
		BMenuItem *cdItem = new BMenuItem("CD", new BMessage(SOURCE_CD_MSG) );		
		theMenu->AddItem(cdItem);	

		// Aux
		BMenuItem *auxItem = new BMenuItem("Aux", new BMessage(SOURCE_AUX_MSG) );		
		theMenu->AddItem(auxItem);	
		
		// Select the current input item
		/*int32 currentDevice;
		BADCStream theStream;
		if ( theStream.ADCInput(&currentDevice) == B_OK)
		{
			switch(currentDevice)
			{				
				case B_MIC_IN:
					if (theStream.IsMicBoosted())
						mic20Item->SetMarked(true);
					else
						micItem->SetMarked(true);
					break;
					
				case B_CD_IN:
					cdItem->SetMarked(true);
					break;
					
				case B_LINE_IN:
					auxItem->SetMarked(true);
					break;
					
				default:
					noneItem->SetMarked(true);
					break;
			}		
		}*/
	}

}


#pragma mark -
#pragma mark === Message Handling ===

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TAudioSourceView::MessageReceived(BMessage *theMessage)
{
	switch( theMessage->what)
	{					
		// Set source to none
		case SOURCE_NONE_MSG:
			{
				//BADCStream stream; 
				//stream.SetADCInput(B_CD_IN);
			}
			break;
			
		case SOURCE_MIC_MSG:
			{
				//BADCStream stream; 
				//stream.SetADCInput(B_MIC_IN);
				//stream.BoostMic(false);
			}
			break;
			
		case SOURCE_MIC_20_MSG:
			{
				//BADCStream stream; 
				//stream.SetADCInput(B_MIC_IN);
				//stream.BoostMic(true);
			}
			break;
			
		case SOURCE_CD_MSG:
			{
				//BADCStream stream; 
				//stream.SetADCInput(B_CD_IN);
			}
			break;
			
		case SOURCE_AUX_MSG:
			{
				//BADCStream stream; 
				//stream.SetADCInput(B_LINE_IN);
			}
			break;

		default:
			BView::MessageReceived(theMessage);
			break;			
	}
}


#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Perform initialization when attached to a window
//	

void TAudioSourceView::AttachedToWindow()
{			
	// Set up menu targets
	//
	
	// Device Menu
	BMenu *deviceMenu = m_DeviceMenuField->Menu();
	if (deviceMenu)
	{
		BMenuItem *defaultItem = deviceMenu->FindItem(DEVICE_DEFAULT_MSG);
		defaultItem->SetTarget(this);
	}
	
	// Input Menu
	BMenu *inputMenu = m_InputMenuField->Menu();
	if (inputMenu)
	{
		BMenuItem *noneItem = inputMenu->FindItem(SOURCE_NONE_MSG);
		noneItem->SetTarget(this);
		
		BMenuItem *micItem = inputMenu->FindItem(SOURCE_MIC_MSG);
		micItem->SetTarget(this);

		BMenuItem *mic20Item = inputMenu->FindItem(SOURCE_MIC_20_MSG);
		mic20Item->SetTarget(this);

		BMenuItem *cdItem = inputMenu->FindItem(SOURCE_CD_MSG);
		cdItem->SetTarget(this);

		BMenuItem *auxItem = inputMenu->FindItem(SOURCE_AUX_MSG);
		auxItem->SetTarget(this);
		
		// Select the current input item
		/*int32 currentDevice;
		BADCStream theStream;
		if ( theStream.ADCInput(&currentDevice) == B_OK)
		{
			switch(currentDevice)
			{				
				case B_MIC_IN:
					if (theStream.IsMicBoosted())
						mic20Item->SetMarked(true);
					else
						micItem->SetMarked(true);
					break;
					
				case B_CD_IN:
					cdItem->SetMarked(true);
					break;
					
				case B_LINE_IN:
					auxItem->SetMarked(true);
					break;
					
				default:
					noneItem->SetMarked(true);
					break;
			}		
		}*/
	}
}

