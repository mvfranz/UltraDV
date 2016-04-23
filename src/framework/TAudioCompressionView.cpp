//---------------------------------------------------------------------
//
//	File:	TAudioCompressionView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Child view of TAudioSettingsTabView.  Handles display of audio
//			compression settings.
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
#include "AppUtils.h"
#include "ResourceManager.h"
#include "TMuseumSlider.h"

#include "TLevelsSlider.h"
#include "TAudioLevelsView.h"

#include "TAudioCompressionView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TAudioCompressionView::TAudioCompressionView(BMessage *archive) : BView(archive)
{
	// Perform default initialization
	Init();
}



//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TAudioCompressionView::Init()
{
	// Grey background
	SetViewColor(kBeGrey); 
	
	// Find view items
	m_CompressorMenuField 	= (BMenuField *)FindView("CompressorMenuField");
	m_VolumeBox				= (BBox *)FindView("VolumeBox");
	
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
	m_AudioLevelsView 	= new TAudioLevelsView(levelsView->Frame());
	m_VolumeBox->RemoveChild(levelsView);
	delete levelsView;
	m_VolumeBox->AddChild(m_AudioLevelsView);

	//
	// Set up menus
	//
	
	BMenu *theMenu;
	
	// Setup compressors menu
	theMenu = m_CompressorMenuField->Menu();
	if (theMenu)
	{
		// None
		BMenuItem *noneItem = new BMenuItem("None", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(noneItem);	
		
		// Mace 3:1
		BMenuItem *mace31Item = new BMenuItem("Mace 3:1", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(mace31Item);	
	
		// Mace 6:1
		BMenuItem *mace61Item = new BMenuItem("Mace 6:1", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(mace61Item);	

		// QualComm PureVoice™
		BMenuItem *qcomItem = new BMenuItem("QualComm PureVoice™", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(qcomItem);	
		
		// ALaw 2:1
		BMenuItem *alawItem = new BMenuItem("ALaw 2:1", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(alawItem);	

		// 32-bit Floating Point
		BMenuItem *fp32Item = new BMenuItem("32-bit Floating Point", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(fp32Item);	

		// 64-bit Floating Point
		BMenuItem *fp64Item = new BMenuItem("64-bit Floating Point", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(fp64Item);	

		// IMA 4:1
		BMenuItem *imaItem = new BMenuItem("IMA 4:1", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(imaItem);	

		// 24-bit Integer
		BMenuItem *int24Item = new BMenuItem("24-bit Integer", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(int24Item);	

		// 32-bit Integer
		BMenuItem *int32Item = new BMenuItem("32-bit Integer", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(int32Item);	

		// µLaw 2:1
		BMenuItem *law21Item = new BMenuItem("µLaw 2:1", new BMessage(AUDIO_NONE_MSG) );		
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(law21Item);	

		// Select the first item
		noneItem->SetMarked(true);
	}	
}
