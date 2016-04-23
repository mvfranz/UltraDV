//---------------------------------------------------------------------
//
//	File:	TVideoSourceView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Child view of TVideoSourceView.  Handles display of video
//			image settings.
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

#include "TVideoSettingsDialog.h"
#include "TVideoSettingsTabView.h"
#include "TVideoSettingsView.h"
#include "TVideoPreviewView.h"

#include "TVideoSourceView.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoSourceView::TVideoSourceView(TVideoSettingsTabView *parent, BMessage *archive) : BView(archive)
{
	m_Parent = parent;
	
	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TVideoSourceView::~TVideoSourceView()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoSourceView::Init()
{
	// Grey background
	SetViewColor(kBeGrey);

	// Locate view items
	m_DigitizerMenuField 	= (BMenuField *)FindView("DigitizerMenuField");
	m_InputMenuField 		= (BMenuField *)FindView("InputMenuField");
	m_FormatMenuField 		= (BMenuField *)FindView("FormatMenuField");
		
	m_GammaCorrectionCheckBox 	= (BCheckBox *)FindView("GammaCorrectionCheckBox");	
	m_LumaCoringCheckBox 		= (BCheckBox *)FindView("LumaCoringCheckBox");
	m_ErrorDiffusionCheckBox 	= (BCheckBox *)FindView("ErrorDiffusionCheckBox");	
	m_LumaCombCheckBox 			= (BCheckBox *)FindView("LumaCombCheckBox");
	m_ChromaCombCheckBox 		= (BCheckBox *)FindView("ChromaCombCheckBox");
	
	//	Set up  preview view bounds.  Use attached view in BBox to set up size
	BView *previewView	= (BView *)FindView("PreviewView");
	m_PreviewRect = previewView->Frame();
	RemoveChild(previewView);
	delete previewView;

	//
	//	Setup checkboxes
	//
	
	VideoFilters theFilters = m_Parent->GetParent()->GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Filters;
	
	m_GammaCorrectionCheckBox->SetMessage(new BMessage(msg_gamma));
	m_GammaCorrectionCheckBox->SetTarget(m_Parent->GetParent());
	m_GammaCorrectionCheckBox->SetValue(theFilters.m_GammaCorrection);
	
	m_LumaCoringCheckBox->SetMessage(new BMessage(msg_coring));
	m_LumaCoringCheckBox->SetTarget(m_Parent->GetParent());
	m_LumaCoringCheckBox->SetValue(theFilters.m_LumaCoring);

	m_ErrorDiffusionCheckBox->SetMessage(new BMessage(msg_error_diffusion));
	m_ErrorDiffusionCheckBox->SetTarget(m_Parent->GetParent());
	m_ErrorDiffusionCheckBox->SetValue(theFilters.m_ErrorDiffusion);
	
	m_LumaCombCheckBox->SetMessage(new BMessage(msg_luma_comb));
	m_LumaCombCheckBox->SetTarget(m_Parent->GetParent());
	m_LumaCombCheckBox->SetValue(theFilters.m_LumaComb);
	
	m_ChromaCombCheckBox->SetMessage(new BMessage(msg_chroma_comb));
	m_ChromaCombCheckBox->SetTarget(m_Parent->GetParent());
	m_ChromaCombCheckBox->SetValue(theFilters.m_ChromaComb);
	  
			
	//
	// Set up menus
	//
	
	BMenu *theMenu;
	
	// Setup Digitizer menu
	theMenu = m_DigitizerMenuField->Menu();
	if (theMenu)
	{
		//theMenu->AddItem(new BMenuItem("Autodetect",new BMessage(msg_auto_brand)));
		theMenu->AddItem(new BMenuItem("Hauppauge (Philips)",new BMessage(msg_hauppauge_philips)));
		theMenu->AddItem(new BMenuItem("Hauppauge (Temic)",new BMessage(msg_hauppauge_temic)));
		theMenu->AddItem(new BMenuItem("Hauppauge 400 (Philips)",new BMessage(msg_hauppauge_400_philips)));
		theMenu->AddItem(new BMenuItem("Hauppauge 400 (Temic)",new BMessage(msg_hauppauge_400_temic)));
		theMenu->AddItem(new BMenuItem("Hauppauge 401 (Philips)",new BMessage(msg_hauppauge_401_philips)));
		theMenu->AddItem(new BMenuItem("Hauppauge 401 (Temic)",new BMessage(msg_hauppauge_401_temic)));
		theMenu->AddItem(new BMenuItem("Hauppauge 405 (Philips)",new BMessage(msg_hauppauge_405_philips)));
		theMenu->AddItem(new BMenuItem("Hauppauge 405 (Temic)",new BMessage(msg_hauppauge_405_temic)));
		theMenu->AddItem(new BMenuItem("Hauppauge 406 (Philips)",new BMessage(msg_hauppauge_406_philips)));
		theMenu->AddItem(new BMenuItem("Hauppauge 406 (Temic)",new BMessage(msg_hauppauge_406_temic)));
		theMenu->AddItem(new BMenuItem("Hauppauge 418 (Philips)",new BMessage(msg_hauppauge_418_philips)));
		theMenu->AddItem(new BMenuItem("Hauppauge 452 (Philips)",new BMessage(msg_hauppauge_452_philips)));
		theMenu->AddItem(new BMenuItem("Avermedia (Philips)",new BMessage(msg_avermedia_philips)));
		theMenu->AddItem(new BMenuItem("Intel SVR III",new BMessage(msg_intel_svriii)));
		theMenu->AddItem(new BMenuItem("TurboTV (Philips)",new BMessage(msg_turbotv_philips)));
		theMenu->AddItem(new BMenuItem("TurboTV (Temic)",new BMessage(msg_turbotv_temic)));
		theMenu->AddItem(new BMenuItem("Miro (Philips)",new BMessage(msg_miro_philips)));
		theMenu->AddItem(new BMenuItem("Miro (Temic)",new BMessage(msg_miro_temic)));
		theMenu->AddItem(new BMenuItem("Diamond (Philips)",new BMessage(msg_diamond_philips)));
		theMenu->AddItem(new BMenuItem("Diamond (Temic)",new BMessage(msg_diamond_temic)));
		theMenu->AddItem(new BMenuItem("Osprey 100",new BMessage(msg_osprey_100)));
			
		// Set the target 
		theMenu->SetTargetForItems(m_Parent->GetParent());
		
		// Select the proper item
		SetDigitizerMenuItem(theMenu);		
	}
	
	// Setup Input menu
	theMenu = m_InputMenuField->Menu();
	if (theMenu)
	{
		theMenu->AddItem(new BMenuItem("Composite",new BMessage(msg_composite)));
		theMenu->AddItem(new BMenuItem("Tuner",new BMessage(msg_tuner)));
		theMenu->AddItem(new BMenuItem("S-Video",new BMessage(msg_svideo)));
		theMenu->AddItem(new BMenuItem("Color Bars",new BMessage(msg_color_bars)));
		theMenu->AddItem(new BMenuItem("Camera",new BMessage(msg_camera)));
		
		// Set the target 
		theMenu->SetTargetForItems(m_Parent->GetParent());

		// Select the proper item
		SetInputMenuItem(theMenu);		

	}
	
	// Setup Format menu
	theMenu = m_FormatMenuField->Menu();
	if (theMenu)
	{
		theMenu->AddItem(new BMenuItem("NTSC-M",new BMessage(msg_ntsc_m)));
		theMenu->AddItem(new BMenuItem("NTSC-J",new BMessage(msg_ntsc_j)));
		theMenu->AddItem(new BMenuItem("PAL-BG",new BMessage(msg_pal_bg)));
		theMenu->AddItem(new BMenuItem("PAL-I",new BMessage(msg_pal_i)));
		theMenu->AddItem(new BMenuItem("SECAM",new BMessage(msg_secam)));
	
		// Set the target 
		theMenu->SetTargetForItems(m_Parent->GetParent());

		// Select the first item
		theMenu->FindItem(msg_ntsc_m)->SetMarked(true);	
	}
	
	// inform VideoSource
	ApplyToSource();
}

#pragma mark -
#pragma mark === Visibility Routines ===

//---------------------------------------------------------------------
//	AttachedToWindow
//---------------------------------------------------------------------
//
//	Attach previewView to view
//

void TVideoSourceView::AttachedToWindow()
{			
	printf("TVideoCompressionView::AttachedToWindow()\n");
	
	//	Get pointer to previewView and attach to BBox
	BBox *previewBox = (BBox *)FindView("PreviewBox");		
	TVideoPreviewView *previewView = m_Parent->GetParent()->GetParent()->PreviewView();	
	if (previewView)
	{
		previewBox->AddChild(previewView);
		previewView->MoveTo(m_PreviewRect.left, m_PreviewRect.top);
		previewView->ResizeTo(m_PreviewRect.Width(), m_PreviewRect.Height());
		previewView->Show();
	}
	
	BView::Show();
}

//---------------------------------------------------------------------
//	DetachedFromWindow
//---------------------------------------------------------------------
//
//


void TVideoSourceView::DetachedFromWindow()
{			
	printf("TVideoCompressionView::DetachedFromWindow()\n");
	
	//	Get pointer to previewView and detach from BBox
	BBox *previewBox = (BBox *)FindView("PreviewBox");		
	TVideoPreviewView *previewView = m_Parent->GetParent()->GetParent()->PreviewView();	
	if (previewView)
	{
		previewView->Hide();
		previewBox->RemoveChild(previewView);
	}
	
	BView::DetachedFromWindow();
}

#pragma mark -
#pragma mark === Utility Routines ===


//---------------------------------------------------------------------
//	ApplyToSource
//---------------------------------------------------------------------
//
//	Apply settings to video source
//	


void TVideoSourceView::ApplyToSource()
{			
	BMenu *theMenu;
	
	theMenu = m_DigitizerMenuField->Menu();
	m_Parent->GetParent()->MessageReceived(theMenu->FindMarked()->Message());
	
	theMenu = m_InputMenuField->Menu();
	m_Parent->GetParent()->MessageReceived(theMenu->FindMarked()->Message());	
	
	theMenu = m_FormatMenuField->Menu();
	m_Parent->GetParent()->MessageReceived(theMenu->FindMarked()->Message());
	
	m_GammaCorrectionCheckBox->Invoke();
	m_LumaCoringCheckBox->Invoke();
	m_ErrorDiffusionCheckBox->Invoke();
	m_LumaCombCheckBox->Invoke();
	m_ChromaCombCheckBox->Invoke();
}


#pragma mark -
#pragma mark === Utility Routines ===


//---------------------------------------------------------------------
//	SetDigitizerMenuItem
//---------------------------------------------------------------------
//
//	Setup digitizer menu based on prefs
//	

void TVideoSourceView::SetDigitizerMenuItem(BMenu *theMenu)
{				
	// Switch based on 
	switch( m_Parent->GetParent()->GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Digitizer)
	{
		case kHauppaugePhilips:
			theMenu->FindItem(msg_turbotv_philips)->SetMarked(true);
			break;
			
		case kHauppaugeTemic:
			theMenu->FindItem(msg_hauppauge_temic)->SetMarked(true);		
			break;
			
		case kHauppauge400Philips:
			theMenu->FindItem(msg_hauppauge_400_philips)->SetMarked(true);			
			break;
					
		case kHauppauge400Temic:
			theMenu->FindItem(msg_hauppauge_400_temic)->SetMarked(true);			
			break;
			
		case kHauppauge401Philips:
			theMenu->FindItem(msg_hauppauge_401_philips)->SetMarked(true);
			break;
			
		case kHauppauge401Temic:
			theMenu->FindItem(msg_hauppauge_401_temic)->SetMarked(true);			
			break;
		
		case kHauppauge405Philips:
			theMenu->FindItem(msg_hauppauge_405_philips)->SetMarked(true);
			break;
			
		case kHauppauge405Temic:
			theMenu->FindItem(msg_hauppauge_405_temic)->SetMarked(true);		
			break;
			
		case kHauppauge406Philips:
			theMenu->FindItem(msg_hauppauge_406_philips)->SetMarked(true);					
			break;
			
		case kHauppauge406Temic:
			theMenu->FindItem(msg_hauppauge_406_temic)->SetMarked(true);					
			break;

		case kHauppauge418Philips:
			theMenu->FindItem(msg_hauppauge_418_philips)->SetMarked(true);					
			break;
			
		case kHauppauge452Philips:
			theMenu->FindItem(msg_hauppauge_452_philips)->SetMarked(true);					
			break;
			
		case kAvermediaPhilips:
			theMenu->FindItem(msg_avermedia_philips)->SetMarked(true);
			break;

		case kIntelSVRIII:
			theMenu->FindItem(msg_intel_svriii)->SetMarked(true);
			break;
			
		case kTurboTVPhilips:
			theMenu->FindItem(msg_turbotv_philips)->SetMarked(true);
			break;
			
		case kTurboTVTemic:
			theMenu->FindItem(msg_turbotv_temic)->SetMarked(true);
			break;
			
		case kMiroPhilips:
			theMenu->FindItem(msg_miro_temic)->SetMarked(true);
			break;
			
		case kMiroTemic:
			theMenu->FindItem(msg_turbotv_temic)->SetMarked(true);
			break;
			
		case kDiamondPhilips:
			theMenu->FindItem(msg_diamond_philips)->SetMarked(true);
			break;
			
		case kDiamondTemic:
			theMenu->FindItem(msg_diamond_temic)->SetMarked(true);
			break;
			
		case kOsprey100:
			theMenu->FindItem(msg_osprey_100)->SetMarked(true);
			break;
		
		default:
			theMenu->FindItem(msg_turbotv_philips)->SetMarked(true);
			break;	
	}			
}


//---------------------------------------------------------------------
//	SetInputMenuItem
//---------------------------------------------------------------------
//
//	Setup input menu based on prefs
//	

void TVideoSourceView::SetInputMenuItem(BMenu *theMenu)
{				
	// Switch based on 
	switch( m_Parent->GetParent()->GetParent()->m_TempVideoSettings.m_VideoSourceSettings.m_Input)
	{
		case kComposite:
			theMenu->FindItem(msg_composite)->SetMarked(true);
			break;

		case kTuner:
			theMenu->FindItem(msg_tuner)->SetMarked(true);
			break;

		case kSVideo:
			theMenu->FindItem(msg_svideo)->SetMarked(true);
			break;

		case kColorBars:
			theMenu->FindItem(msg_color_bars)->SetMarked(true);
			break;

		case kCamera:
			theMenu->FindItem(msg_camera)->SetMarked(true);
			break;

		default:
			theMenu->FindItem(msg_composite)->SetMarked(true);
			break;
	}	
}
