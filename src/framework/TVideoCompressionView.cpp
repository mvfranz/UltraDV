//---------------------------------------------------------------------
//
//	File:	TVideoCompressionView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.27.98
//
//	Desc:	Child view of TVideoSettingsTabView.  Handles display of video
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

#include "TVideoSettingsDialog.h"
#include "TVideoSettingsTabView.h"
#include "TVideoSettingsView.h"
#include "TVideoPreviewView.h"

#include "TVideoCompressionView.h"


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TVideoCompressionView::TVideoCompressionView(TVideoSettingsTabView *parent, BMessage *archive) : BView(archive)
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

TVideoCompressionView::~TVideoCompressionView()
{

}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TVideoCompressionView::Init()
{
	// Grey background
	SetViewColor(kBeGrey);

	// Find view items
	fCompressorMenuField    = (BMenuField *)FindView("CompressorMenuField");
	fDepthMenuField                 = (BMenuField *)FindView("DepthMenuField");

	fQualityString  = (BStringView *)FindView("QualityString");

	fFPSTextControl         = (BTextControl *)FindView("FPSTextControl");

	fKeyframeCheckBox               = (BCheckBox *)FindView("KeyframeCheckBox");
	fKeyframeTextControl    = (BTextControl *)FindView("KeyframeTextControl");

	fLimitDataRateCheckBox          = (BCheckBox *)FindView("LimitDataRateCheckBox");
	fLimitDataRateTextControl       = (BTextControl *)FindView("LimitDataRateTextControl");

	//	Set up  preview view bounds.  Use attached view in BBox to set up size
	BView *previewView      = (BView *)FindView("PreviewView");
	fPreviewRect = previewView->Frame();
	RemoveChild(previewView);
	delete previewView;

	//
	// Set up menus
	//

	BMenu *theMenu;

	// Setup compressors menu
	theMenu = fCompressorMenuField->Menu();
	if (theMenu) {
		// None
		BMenuItem *noneItem = new BMenuItem("None", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(noneItem);

		// Animation
		BMenuItem *animItem = new BMenuItem("Animation", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(animItem);

		// BMP
		BMenuItem *bmpItem = new BMenuItem("BMP", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(bmpItem);

		// Cinepak
		BMenuItem *cinepakItem = new BMenuItem("Cinepak", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(cinepakItem);

		// DV-NTSC
		BMenuItem *dvntscItem = new BMenuItem("DV-NTSC", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(dvntscItem);

		// DV-PAL
		BMenuItem *dvpalItem = new BMenuItem("DV-PAL", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(dvpalItem);

		// Graphics
		BMenuItem *graphicsItem = new BMenuItem("Graphics", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(graphicsItem);

		// H.263
		BMenuItem *h263Item = new BMenuItem("H.263", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(h263Item);

		// Intel Indeo® Video
		BMenuItem *intelItem = new BMenuItem("Intel Indeo® Video", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(intelItem);

		// Intel Raw
		BMenuItem *intelRawItem = new BMenuItem("Intel Raw", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(intelRawItem);

		// Motion JPEG A
		BMenuItem *motionJPEGAItem = new BMenuItem("Motion JPEG A", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(motionJPEGAItem);

		// Motion JPEG B
		BMenuItem *motionJPEGBItem = new BMenuItem("Motion JPEG B", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(motionJPEGBItem);

		// MPEG
		BMenuItem *mpegItem = new BMenuItem("MPEG", new BMessage(VID_RAW_MSG)  );
		//mpegItem->SetTarget(be_app);
		theMenu->AddItem(mpegItem);

		// Photo JPEG
		BMenuItem *photoJPEGItem = new BMenuItem("Photo JPEG", new BMessage(VID_RAW_MSG)  );
		//mpegItem->SetTarget(be_app);
		theMenu->AddItem(photoJPEGItem);

		// Planar RGB
		BMenuItem *planarRGBItem = new BMenuItem("Planar RGB", new BMessage(VID_RAW_MSG)  );
		//mpegItem->SetTarget(be_app);
		theMenu->AddItem(planarRGBItem);

		// Raw
		BMenuItem *rawItem = new BMenuItem("Raw", new BMessage(VID_RAW_MSG)  );
		//rawItem->SetTarget(be_app);
		theMenu->AddItem(rawItem);

		// Sorenson Video
		BMenuItem *sorensonItem = new BMenuItem("Sorenson Video", new BMessage(VID_RAW_MSG)  );
		//mpegItem->SetTarget(be_app);
		theMenu->AddItem(sorensonItem);

		// Video
		BMenuItem *videoItem = new BMenuItem("Video", new BMessage(VID_RAW_MSG)  );
		//mpegItem->SetTarget(be_app);
		theMenu->AddItem(videoItem);

		// Select the first item
		rawItem->SetMarked(true);
	}

	// Setup compressors menu
	theMenu = fDepthMenuField->Menu();
	if (theMenu) {
		// 8-bit
		BMenuItem *eightBitItem = new BMenuItem("8-bit", new BMessage(VID_RAW_MSG)  );
		//eightBitItem->SetTarget(be_app);
		theMenu->AddItem(eightBitItem);

		// 16-bit
		BMenuItem *sixteenBitItem = new BMenuItem("16-bit", new BMessage(VID_RAW_MSG)  );
		//sixteenBitItem->SetTarget(be_app);
		theMenu->AddItem(sixteenBitItem);

		// 32-bit
		BMenuItem *threetwoItem = new BMenuItem("32-bit", new BMessage(VID_RAW_MSG)  );
		//threetwoItem->SetTarget(be_app);
		theMenu->AddItem(threetwoItem);

		// Select the first item
		eightBitItem->SetMarked(true);
	}

	// Create quality slider
	BRect sliderFrame;
	sliderFrame.left        = fQualityString->Frame().right+10;
	sliderFrame.top         = fQualityString->Frame().top - 4;
	sliderFrame.right       = sliderFrame.left + 100;
	sliderFrame.bottom      = sliderFrame.top + 10;

	fQualitySlider = new TMuseumSlider(sliderFrame, "Quality", NULL, new BMessage(VID_RAW_MSG), -100, +100);
	//fQualitySlider->SetModificationMessage(new BMessage(msg_brightness));
	//fQualitySlider->SetTarget(fSource);
	//fQualitySlider->SetLimitLabels("Least", "Best");
	AddChild(fQualitySlider);

	// Inform VideoSource
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

void TVideoCompressionView::AttachedToWindow()
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


void TVideoCompressionView::DetachedFromWindow()
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

void TVideoCompressionView::ApplyToSource()
{
}
