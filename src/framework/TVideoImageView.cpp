//---------------------------------------------------------------------
//
//	File:	TVideoImageView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Child view of TVideoImageView.  Handles display of video
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

#include "TVideoSettingsSlider.h"
#include "TVideoSettings.h"
#include "TVideoSettingsDialog.h"
#include "TVideoSettingsTabView.h"
#include "TVideoSettingsView.h"
#include "TVideoPreviewView.h"

#include "TVideoImageView.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoImageView::TVideoImageView(TVideoSettingsTabView *parent, BMessage *archive) : BView(archive)
{
	fParent = parent;

	// Perform default initialization
	Init();
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoImageView::Init()
{
	// Grey background
	SetViewColor(kBeGrey);

	// Locate view items
	fHueString              = (BStringView *)FindView("HueString");
	fSaturationString       = (BStringView *)FindView("SaturationString");
	fBrightnessString       = (BStringView *)FindView("BrightnessString");
	fContrastString         = (BStringView *)FindView("ContrastString");
	fDefaultsButton         = (BButton *)FindView("DefaultsButton");

	//	Set up  preview view bounds.  Use attached view in BBox to set up size
	BView *previewView      = (BView *)FindView("PreviewView");
	fPreviewRect = previewView->Frame();
	RemoveChild(previewView);
	delete previewView;

	// Create Hue slider
	BRect sliderFrame;
	sliderFrame.left        = fBrightnessString->Frame().right+10;
	sliderFrame.top         = fHueString->Frame().top - 4;
	sliderFrame.right       = sliderFrame.left + 100;
	sliderFrame.bottom      = sliderFrame.top + 10;

	fHueSlider = new TVideoSettingsSlider(sliderFrame, "Hue", NULL, new BMessage(HUE_SLIDER_MSG), -100, +100);
	fHueSlider->SetModificationMessage(new BMessage(HUE_SLIDER_MSG));
	fHueSlider->SetTarget(fParent->GetParent());
	AddChild(fHueSlider);
	fHueSlider->SetValue(fParent->GetParent()->GetParent()->fTempVideoSettings.fVideoImageSettings.fHueValue);

	// Create Saturation slider
	sliderFrame.left        = fBrightnessString->Frame().right+10;
	sliderFrame.top         = fSaturationString->Frame().top - 4;
	sliderFrame.right       = sliderFrame.left + 100;
	sliderFrame.bottom      = sliderFrame.top + 10;

	fSaturationSlider = new TVideoSettingsSlider(sliderFrame, "Saturation", NULL, new BMessage(SATURATION_SLIDER_MSG), -100, +100);
	fSaturationSlider->SetModificationMessage(new BMessage(SATURATION_SLIDER_MSG));
	fSaturationSlider->SetTarget(fParent->GetParent());
	AddChild(fSaturationSlider);
	fSaturationSlider->SetValue(fParent->GetParent()->GetParent()->fTempVideoSettings.fVideoImageSettings.fSaturationValue);

	// Create Brightness slider
	sliderFrame.left        = fBrightnessString->Frame().right+10;
	sliderFrame.top         = fBrightnessString->Frame().top - 4;
	sliderFrame.right       = sliderFrame.left + 100;
	sliderFrame.bottom      = sliderFrame.top + 10;

	fBrightnessSlider = new TVideoSettingsSlider(sliderFrame, "Brightness", NULL, new BMessage(BRIGHTNESS_SLIDER_MSG), -100, +100);
	fBrightnessSlider->SetModificationMessage(new BMessage(BRIGHTNESS_SLIDER_MSG));
	fBrightnessSlider->SetTarget(fParent->GetParent());
	AddChild(fBrightnessSlider);
	fBrightnessSlider->SetValue(fParent->GetParent()->GetParent()->fTempVideoSettings.fVideoImageSettings.fBrightnessValue);

	// Create Contrast slider
	sliderFrame.left        = fBrightnessString->Frame().right+10;
	sliderFrame.top         = fContrastString->Frame().top - 4;
	sliderFrame.right       = sliderFrame.left + 100;
	sliderFrame.bottom      = sliderFrame.top + 10;

	fContrastSlider = new TVideoSettingsSlider(sliderFrame, "Contrast", NULL, new BMessage(CONTRAST_SLIDER_MSG), -100, +100);
	fContrastSlider->SetModificationMessage(new BMessage(CONTRAST_SLIDER_MSG));
	fContrastSlider->SetTarget(fParent->GetParent());
	AddChild(fContrastSlider);
	fContrastSlider->SetValue(fParent->GetParent()->GetParent()->fTempVideoSettings.fVideoImageSettings.fContrastValue);

	//	Inform message targets
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

void TVideoImageView::AttachedToWindow()
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


void TVideoImageView::DetachedFromWindow()
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

void TVideoImageView::ApplyToSource()
{
	fHueSlider->Invoke();
	fSaturationSlider->Invoke();
	fBrightnessSlider->Invoke();
	fContrastSlider->Invoke();
}
