//---------------------------------------------------------------------
//
//	File:	TCueMenu.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.23.98
//
//	Desc:	Creates and handles cue popup menu
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

// ABH
#ifdef GCC4
//#include <algobase.h>			// for min/max
#include "/boot/develop/abi/x86/gcc4/tools/gcc-4.6.3-haiku-121101/include/c++/4.6.3/bits/algobase.h"
#elsif GCC2
#include <algobase.h>
#endif

#include "TCueMenu.h"
#include "TCueView.h"
#include "TVisualCue.h"

#include "TStageCueMenu.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TCueMenu::TCueMenu(TCueView *target) : BPopUpMenu("CueMenu")
{
	BMessage *menuMessage;

	// Create "Lock/Unlock"
	//

	menuMessage = new BMessage(CUE_LOCK_MSG);
	BMenuItem	*lockItem;
	if (target->IsLocked())
		lockItem = new BMenuItem("Unlock", menuMessage);
	else
		lockItem = new BMenuItem("Lock", menuMessage);
	AddItem(lockItem);

	// Create "Show/Hide"
	//

	menuMessage = new BMessage(CUE_HIDE_MSG);
	BMenuItem	*hideItem = new BMenuItem("Hide", menuMessage);
	AddItem(hideItem);

	//	Cue Duration/End
	menuMessage = new BMessage(CUE_DURATION_MSG);
	BMenuItem *durationItem;
	if ( target->IsShowingDuration())
		durationItem = new BMenuItem("Show End", menuMessage);
	else
		durationItem = new BMenuItem("Show Duration", menuMessage);
	AddItem(durationItem);

	// Disable item if we do not have duration
	if (target->HasDuration() == false)
		durationItem->SetEnabled(false);



	// Seperator
	//

	AddSeparatorItem();

	//
	//	Create Settings submenu
	//

	//if (target->IsVisible())
	{
		BMenu *settingsSubMenu = new BMenu("Settings");
		BMenuItem *settingsSubMenuItem = new BMenuItem(settingsSubMenu);
		AddItem(settingsSubMenuItem);

		//	Cue Controls
		menuMessage = new BMessage(CUE_CONTROLS_MSG);
		BMenuItem *controlsItem = new BMenuItem("Controls", menuMessage);
		settingsSubMenu->AddItem(controlsItem);

		//	Cue Contents
		menuMessage = new BMessage(CUE_CONTENTS_MSG);
		BMenuItem *contentsItem = new BMenuItem("Contents", menuMessage);
		settingsSubMenu->AddItem(contentsItem);

		//	Determine which item to check
		switch (target->GetDisplayMode())
		{
			case kCueControls:
				controlsItem->SetMarked(true);
				break;

			case kCueContents:
				contentsItem->SetMarked(true);
				break;

			default:
				TRESPASS();
				break;
		}
	}

	//
	//	Create Quality submenu if visible cue
	//

	if (target->IsVisible())
	{
		//	Create Quality submenu
		//

		BMenu *qualitySubMenu = new BMenu("Quality");
		BMenuItem *qualitySubMenuItem = new BMenuItem(qualitySubMenu);
		AddItem(qualitySubMenuItem);

		// Create "Wireframe"
		menuMessage = new BMessage(STAGE_CUE_WIREFRAME_MSG);
		BMenuItem	*wireItem = new BMenuItem("Wireframe", menuMessage);
		qualitySubMenu->AddItem(wireItem);

		// Create "Preview"
		menuMessage = new BMessage(STAGE_CUE_PREVIEW_MSG);
		BMenuItem	*previewItem = new BMenuItem("Preview", menuMessage);
		qualitySubMenu->AddItem(previewItem);

		// Create "Better"
		menuMessage = new BMessage(STAGE_CUE_BETTER_MSG);
		BMenuItem	*betterItem = new BMenuItem("Better", menuMessage);
		qualitySubMenu->AddItem(betterItem);

		// Create "Best"
		menuMessage = new BMessage(STAGE_CUE_BEST_MSG);
		BMenuItem	*bestItem = new BMenuItem("Best", menuMessage);
		qualitySubMenu->AddItem(bestItem);

		//	Check current display quality setting
		switch( static_cast<TVisualCue *>(target)->GetDisplayQuality() )
		{
			case kWireframeQuality:
				wireItem->SetMarked(true);
				break;

			case kPreviewQuality:
				previewItem->SetMarked(true);
				break;

			case kBetterQuality:
				betterItem->SetMarked(true);
				break;

			case kBestQuality:
				bestItem->SetMarked(true);
				break;

			default:
				wireItem->SetMarked(true);
				break;
		}
	}


	//
	//	Create Transform submenu if visible cue
	//

	if (target->IsVisible())
	{
		//	Create Transform submenu
		//

		BMenu *transformSubMenu = new BMenu("Transform");
		BMenuItem *transformSubMenuItem = new BMenuItem(transformSubMenu);
		AddItem(transformSubMenuItem);

		// Create "Rotate..."
		menuMessage = new BMessage(STAGE_CUE_ROTATE_MSG);
		BMenuItem	*rotateItem = new BMenuItem("Rotation...", menuMessage);
		transformSubMenu->AddItem(rotateItem);

		// Create "Crop..."
		menuMessage = new BMessage(STAGE_CUE_CROP_MSG);
		BMenuItem	*cropItem = new BMenuItem("Crop...", menuMessage);
		transformSubMenu->AddItem(cropItem);

		// Create "Scale..."
		menuMessage = new BMessage(STAGE_CUE_SCALE_MSG);
		BMenuItem	*scaleItem = new BMenuItem("Scale...", menuMessage);
		transformSubMenu->AddItem(scaleItem);

		// Create "Shear..."
		menuMessage = new BMessage(STAGE_CUE_SHEAR_MSG);
		BMenuItem	*shearItem = new BMenuItem("Shear...", menuMessage);
		transformSubMenu->AddItem(shearItem);

		// Create "Skew..."
		menuMessage = new BMessage(STAGE_CUE_SKEW_MSG);
		BMenuItem	*skewItem = new BMenuItem("Skew...", menuMessage);
		transformSubMenu->AddItem(skewItem);

		// Create "Perspective..."
		menuMessage = new BMessage(STAGE_CUE_PERSP_MSG);
		BMenuItem	*perspItem = new BMenuItem("Perspective...", menuMessage);
		transformSubMenu->AddItem(perspItem);

		// Create "Mirror..."
		menuMessage = new BMessage(STAGE_CUE_MIRROR_MSG);
		BMenuItem	*mirrorItem = new BMenuItem("Mirror...", menuMessage);
		transformSubMenu->AddItem(mirrorItem);

		// Create "Opacity..."
		menuMessage = new BMessage(STAGE_CUE_OPACITY_MSG);
		BMenuItem	*opacityItem = new BMenuItem("Opacity...", menuMessage);
		transformSubMenu->AddItem(opacityItem);
	}

	/*
	// Create Transitions submenu
	//

	BMenu *wipesSubMenu = new BMenu("Wipes");
	BMenuItem *wipesSubMenuItem = new BMenuItem(wipesSubMenu);
	AddItem(wipesSubMenuItem);

	menuMessage = new BMessage(TRAN_WIPE_RIGHT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeRightItem = new BMenuItem("Right", menuMessage);
	wipesSubMenu->AddItem(wipeRightItem);

	menuMessage = new BMessage(TRAN_WIPE_LEFT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeLeftItem = new BMenuItem("Left", menuMessage);
	wipesSubMenu->AddItem(wipeLeftItem);

	menuMessage = new BMessage(TRAN_WIPE_DOWN_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeDownItem = new BMenuItem("Down", menuMessage);
	wipesSubMenu->AddItem(wipeDownItem);

	menuMessage = new BMessage(TRAN_WIPE_UP_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeUpItem = new BMenuItem("Up", menuMessage);
	wipesSubMenu->AddItem(wipeUpItem);

	menuMessage = new BMessage(TRAN_WIPE_TOP_LEFT_BOTTOM_RIGHT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeTopLeftBottomRightItem = new BMenuItem("Top Left to Bottom Right", menuMessage);
	wipesSubMenu->AddItem(wipeTopLeftBottomRightItem);

	menuMessage = new BMessage(TRAN_WIPE_TOP_RIGHT_BOTTON_LEFT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeTopRightToBottomLeftItem = new BMenuItem("Top Right to Bottom Left", menuMessage);
	wipesSubMenu->AddItem(wipeTopRightToBottomLeftItem);

	menuMessage = new BMessage(TRAN_WIPE_BOTTOM_LEFT_TOP_RIGHT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeBottomLeftToTopRightItem = new BMenuItem("Bottom Left to Top Right", menuMessage);
	wipesSubMenu->AddItem(wipeBottomLeftToTopRightItem);

	menuMessage = new BMessage(TRAN_WIPE_BOTTOM_RIGHT_TOP_LEFT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeBottomRightToTopLeftItem = new BMenuItem("Bottom Right to Top Left", menuMessage);
	wipesSubMenu->AddItem(wipeBottomRightToTopLeftItem);

	// All these items message to the cue
	wipesSubMenu->SetTargetForItems(target);

	// Seperator02
	AddSeparatorItem();

	// Create Duration item
	menuMessage = new BMessage(TRAN_DURATION);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *durationItem = new BMenuItem("Duration...", menuMessage);
	AddItem(durationItem);
	*/

	// All these items message to the cue
	SetTargetForItems(target);

}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TCueMenu::~TCueMenu()
{

}
