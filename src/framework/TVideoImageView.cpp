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
	m_Parent = parent;
	
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
	m_HueString 		= (BStringView *)FindView("HueString");
	m_SaturationString 	= (BStringView *)FindView("SaturationString");
	m_BrightnessString 	= (BStringView *)FindView("BrightnessString");
	m_ContrastString 	= (BStringView *)FindView("ContrastString");
	m_DefaultsButton   	= (BButton *)FindView("DefaultsButton");
	
	//	Set up  preview view bounds.  Use attached view in BBox to set up size
	BView *previewView	= (BView *)FindView("PreviewView");
	m_PreviewRect = previewView->Frame();
	RemoveChild(previewView);
	delete previewView;
		
	// Create Hue slider
	BRect sliderFrame;
	sliderFrame.left 	= m_BrightnessString->Frame().right+10;
	sliderFrame.top 	= m_HueString->Frame().top - 4;	
	sliderFrame.right 	= sliderFrame.left + 100;
	sliderFrame.bottom 	= sliderFrame.top + 10;
	
	m_HueSlider = new TVideoSettingsSlider(sliderFrame, "Hue", NULL, new BMessage(HUE_SLIDER_MSG), -100, +100);
	m_HueSlider->SetModificationMessage(new BMessage(HUE_SLIDER_MSG));
	m_HueSlider->SetTarget(m_Parent->GetParent());
	AddChild(m_HueSlider);
	m_HueSlider->SetValue(m_Parent->GetParent()->GetParent()->m_TempVideoSettings.m_VideoImageSettings.m_HueValue);	
	
	// Create Saturation slider
	sliderFrame.left 	= m_BrightnessString->Frame().right+10;
	sliderFrame.top 	= m_SaturationString->Frame().top - 4;	
	sliderFrame.right 	= sliderFrame.left + 100;
	sliderFrame.bottom 	= sliderFrame.top + 10;
	
	m_SaturationSlider = new TVideoSettingsSlider(sliderFrame, "Saturation", NULL, new BMessage(SATURATION_SLIDER_MSG), -100, +100);
	m_SaturationSlider->SetModificationMessage(new BMessage(SATURATION_SLIDER_MSG));
	m_SaturationSlider->SetTarget(m_Parent->GetParent());
	AddChild(m_SaturationSlider);
	m_SaturationSlider->SetValue(m_Parent->GetParent()->GetParent()->m_TempVideoSettings.m_VideoImageSettings.m_SaturationValue);	
	
	// Create Brightness slider
	sliderFrame.left 	= m_BrightnessString->Frame().right+10;
	sliderFrame.top 	= m_BrightnessString->Frame().top - 4;	
	sliderFrame.right 	= sliderFrame.left + 100;
	sliderFrame.bottom 	= sliderFrame.top + 10;
	
	m_BrightnessSlider = new TVideoSettingsSlider(sliderFrame, "Brightness", NULL, new BMessage(BRIGHTNESS_SLIDER_MSG), -100, +100);
	m_BrightnessSlider->SetModificationMessage(new BMessage(BRIGHTNESS_SLIDER_MSG));
	m_BrightnessSlider->SetTarget(m_Parent->GetParent());
	AddChild(m_BrightnessSlider);
	m_BrightnessSlider->SetValue(m_Parent->GetParent()->GetParent()->m_TempVideoSettings.m_VideoImageSettings.m_BrightnessValue);
	
	// Create Contrast slider
	sliderFrame.left 	= m_BrightnessString->Frame().right+10;
	sliderFrame.top 	= m_ContrastString->Frame().top - 4;	
	sliderFrame.right 	= sliderFrame.left + 100;
	sliderFrame.bottom 	= sliderFrame.top + 10;
	
	m_ContrastSlider = new TVideoSettingsSlider(sliderFrame, "Contrast", NULL, new BMessage(CONTRAST_SLIDER_MSG), -100, +100);
	m_ContrastSlider->SetModificationMessage(new BMessage(CONTRAST_SLIDER_MSG));
	m_ContrastSlider->SetTarget(m_Parent->GetParent());
	AddChild(m_ContrastSlider);
	m_ContrastSlider->SetValue(m_Parent->GetParent()->GetParent()->m_TempVideoSettings.m_VideoImageSettings.m_ContrastValue);
	
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


void TVideoImageView::DetachedFromWindow()
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

void TVideoImageView::ApplyToSource()
{
	m_HueSlider->Invoke();	
	m_SaturationSlider->Invoke();	
	m_BrightnessSlider->Invoke();	
	m_ContrastSlider->Invoke();	
}
