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
	fParent = parent;

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
	fDigitizerMenuField     = (BMenuField *)FindView("DigitizerMenuField");
	fInputMenuField                 = (BMenuField *)FindView("InputMenuField");
	fFormatMenuField                = (BMenuField *)FindView("FormatMenuField");

	fGammaCorrectionCheckBox        = (BCheckBox *)FindView("GammaCorrectionCheckBox");
	fLumaCoringCheckBox             = (BCheckBox *)FindView("LumaCoringCheckBox");
	fErrorDiffusionCheckBox         = (BCheckBox *)FindView("ErrorDiffusionCheckBox");
	fLumaCombCheckBox                       = (BCheckBox *)FindView("LumaCombCheckBox");
	fChromaCombCheckBox             = (BCheckBox *)FindView("ChromaCombCheckBox");

	//	Set up  preview view bounds.  Use attached view in BBox to set up size
	BView *previewView      = (BView *)FindView("PreviewView");
	fPreviewRect = previewView->Frame();
	RemoveChild(previewView);
	delete previewView;

	//
	//	Setup checkboxes
	//

	VideoFilters theFilters = fParent->GetParent()->GetParent()->fTempVideoSettings.fVideoSourceSettings.fFilters;

	fGammaCorrectionCheckBox->SetMessage(new BMessage(msg_gamma));
	fGammaCorrectionCheckBox->SetTarget(fParent->GetParent());
	fGammaCorrectionCheckBox->SetValue(theFilters.fGammaCorrection);

	fLumaCoringCheckBox->SetMessage(new BMessage(msg_coring));
	fLumaCoringCheckBox->SetTarget(fParent->GetParent());
	fLumaCoringCheckBox->SetValue(theFilters.fLumaCoring);

	fErrorDiffusionCheckBox->SetMessage(new BMessage(msg_error_diffusion));
	fErrorDiffusionCheckBox->SetTarget(fParent->GetParent());
	fErrorDiffusionCheckBox->SetValue(theFilters.fErrorDiffusion);

	fLumaCombCheckBox->SetMessage(new BMessage(msg_luma_comb));
	fLumaCombCheckBox->SetTarget(fParent->GetParent());
	fLumaCombCheckBox->SetValue(theFilters.fLumaComb);

	fChromaCombCheckBox->SetMessage(new BMessage(msg_chroma_comb));
	fChromaCombCheckBox->SetTarget(fParent->GetParent());
	fChromaCombCheckBox->SetValue(theFilters.fChromaComb);


	//
	// Set up menus
	//

	BMenu *theMenu;

	// Setup Digitizer menu
	theMenu = fDigitizerMenuField->Menu();
	if (theMenu) {
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
		theMenu->SetTargetForItems(fParent->GetParent());

		// Select the proper item
		SetDigitizerMenuItem(theMenu);
	}

	// Setup Input menu
	theMenu = fInputMenuField->Menu();
	if (theMenu) {
		theMenu->AddItem(new BMenuItem("Composite",new BMessage(msg_composite)));
		theMenu->AddItem(new BMenuItem("Tuner",new BMessage(msg_tuner)));
		theMenu->AddItem(new BMenuItem("S-Video",new BMessage(msg_svideo)));
		theMenu->AddItem(new BMenuItem("Color Bars",new BMessage(msg_color_bars)));
		theMenu->AddItem(new BMenuItem("Camera",new BMessage(msg_camera)));

		// Set the target
		theMenu->SetTargetForItems(fParent->GetParent());

		// Select the proper item
		SetInputMenuItem(theMenu);

	}

	// Setup Format menu
	theMenu = fFormatMenuField->Menu();
	if (theMenu) {
		theMenu->AddItem(new BMenuItem("NTSC-M",new BMessage(msg_ntsc_m)));
		theMenu->AddItem(new BMenuItem("NTSC-J",new BMessage(msg_ntsc_j)));
		theMenu->AddItem(new BMenuItem("PAL-BG",new BMessage(msg_pal_bg)));
		theMenu->AddItem(new BMenuItem("PAL-I",new BMessage(msg_pal_i)));
		theMenu->AddItem(new BMenuItem("SECAM",new BMessage(msg_secam)));

		// Set the target
		theMenu->SetTargetForItems(fParent->GetParent());

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
	TVideoPreviewView *previewView = fParent->GetParent()->GetParent()->PreviewView();
	if (previewView) {
		previewBox->AddChild(previewView);
		previewView->MoveTo(fPreviewRect.left, fPreviewRect.top);
		previewView->ResizeTo(fPreviewRect.Width(), fPreviewRect.Height());
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
	TVideoPreviewView *previewView = fParent->GetParent()->GetParent()->PreviewView();
	if (previewView) {
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

	theMenu = fDigitizerMenuField->Menu();
	fParent->GetParent()->MessageReceived(theMenu->FindMarked()->Message());

	theMenu = fInputMenuField->Menu();
	fParent->GetParent()->MessageReceived(theMenu->FindMarked()->Message());

	theMenu = fFormatMenuField->Menu();
	fParent->GetParent()->MessageReceived(theMenu->FindMarked()->Message());

	fGammaCorrectionCheckBox->Invoke();
	fLumaCoringCheckBox->Invoke();
	fErrorDiffusionCheckBox->Invoke();
	fLumaCombCheckBox->Invoke();
	fChromaCombCheckBox->Invoke();
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
	switch( fParent->GetParent()->GetParent()->fTempVideoSettings.fVideoSourceSettings.fDigitizer)
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
	switch( fParent->GetParent()->GetParent()->fTempVideoSettings.fVideoSourceSettings.fInput)
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
