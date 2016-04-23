//---------------------------------------------------------------------
//
//	File:	TStageCueMenu.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	04.02.98
//
//	Desc:	Creates and handles stage cue popup menu
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>

#include <algorithm>

// ABH #include <algobase.h>			// for min/max

#include "TVisualCue.h"

#include "TStageCueMenu.h"



// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageCueMenu::TStageCueMenu(TVisualCue *target) : BPopUpMenu("StageCueMenu")
{
	BMessage *menuMessage;
		
	m_Cue = target;
	
	// Create "Lock/Unlock"	
	menuMessage = new BMessage(STAGE_CUE_LOCK_MSG);
	BMenuItem	*lockItem = new BMenuItem("Lock", menuMessage);
	AddItem(lockItem);
	
	// Create "Show/Hide"	
	menuMessage = new BMessage(STAGE_CUE_HIDE_MSG);	
	BMenuItem	*hideItem = new BMenuItem("Hide", menuMessage);
	AddItem(hideItem);
	
	// Seperator01
	AddSeparatorItem();
	
	
	//
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
	switch (target->GetDisplayQuality() )
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
	AddItem(opacityItem);
	
	// Seperator03
	AddSeparatorItem();
	
	//	Create Draw Mode submenu	
	//
	
	BMenu *drawModeSubMenu = new BMenu("Drawing Mode");
	BMenuItem *drawModeSubMenuItem = new BMenuItem(drawModeSubMenu);
	AddItem(drawModeSubMenuItem);
		
	menuMessage = new BMessage(STAGE_CUE_COPY_MSG);
	BMenuItem *copyItem = new BMenuItem("Copy", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_COPY);
	drawModeSubMenu->AddItem(copyItem);
	
	menuMessage = new BMessage(STAGE_CUE_OVER_MSG);
	BMenuItem *overItem = new BMenuItem("Over", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_OVER);
	drawModeSubMenu->AddItem(overItem);	
	
	menuMessage = new BMessage(STAGE_CUE_ERASE_MSG);
	BMenuItem *eraseItem = new BMenuItem("Erase", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_ERASE);
	drawModeSubMenu->AddItem(eraseItem);	
	
	menuMessage = new BMessage(STAGE_CUE_INVERT_MSG);
	BMenuItem *invertItem = new BMenuItem("Invert", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_INVERT);
	drawModeSubMenu->AddItem(invertItem);	
	
	menuMessage = new BMessage(STAGE_CUE_SELECT_MSG);
	BMenuItem *selectItem = new BMenuItem("Select", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_SELECT);
	drawModeSubMenu->AddItem(selectItem);	
	
	menuMessage = new BMessage(STAGE_CUE_MIN_MSG);
	BMenuItem *minItem = new BMenuItem("Min", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_MIN);
	drawModeSubMenu->AddItem(minItem);	
	
	menuMessage = new BMessage(STAGE_CUE_MAX_MSG);
	BMenuItem *maxItem = new BMenuItem("Max", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_MAX);
	drawModeSubMenu->AddItem(maxItem);	
	
	menuMessage = new BMessage(STAGE_CUE_ADD_MSG);
	BMenuItem *addItem = new BMenuItem("Add", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_ADD);
	drawModeSubMenu->AddItem(addItem);
	
	menuMessage = new BMessage(STAGE_CUE_SUBTRACT_MSG);
	BMenuItem *subtractItem = new BMenuItem("Subtract", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_SUBTRACT);
	drawModeSubMenu->AddItem(subtractItem);
	
	menuMessage = new BMessage(STAGE_CUE_BLEND_MSG);
	BMenuItem *blendItem = new BMenuItem("Blend", menuMessage);
	menuMessage->AddInt32("DrawingMode", B_OP_BLEND);
	drawModeSubMenu->AddItem(blendItem);	
		
	// All these items message to the cue
	SetTargetForItems(target);
	
	// Set proper check marks
	UpdateMenu();
		
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TStageCueMenu::~TStageCueMenu()
{

}


//---------------------------------------------------------------------
//	Update
//---------------------------------------------------------------------
//
//	Set the proper menu enabled and checked state

void TStageCueMenu::UpdateMenu()
{
	BMenuItem *theItem;
		
	BMenuItem *drawingItem = FindItem("Drawing Mode");
	BMenu *subMenu = drawingItem->Submenu();
		
	// Set drawing mode check
	switch( m_Cue->GetDrawingMode())
	{
		case B_OP_COPY:
			theItem = subMenu->FindItem(STAGE_CUE_COPY_MSG);
			theItem->SetMarked(true);
			break;
		
		case B_OP_OVER:
			theItem = subMenu->FindItem(STAGE_CUE_OVER_MSG);
			theItem->SetMarked(true);
			break;
			
		case B_OP_ERASE:
			theItem = subMenu->FindItem(STAGE_CUE_ERASE_MSG);
			theItem->SetMarked(true);
			break;
		
		case B_OP_INVERT:
			theItem = subMenu->FindItem(STAGE_CUE_INVERT_MSG);
			theItem->SetMarked(true);
			break;
			
		case B_OP_SELECT:
			theItem = subMenu->FindItem(STAGE_CUE_SELECT_MSG);
			theItem->SetMarked(true);
			break;
			
		case B_OP_MIN:
			theItem = subMenu->FindItem(STAGE_CUE_MIN_MSG);
			theItem->SetMarked(true);
			break;
			
		case B_OP_MAX:
			theItem = subMenu->FindItem(STAGE_CUE_MAX_MSG);
			theItem->SetMarked(true);
			break;
			
		case B_OP_ADD:
			theItem = subMenu->FindItem(STAGE_CUE_ADD_MSG);
			theItem->SetMarked(true);
			break;
			
		case B_OP_SUBTRACT:
			theItem = subMenu->FindItem(STAGE_CUE_SUBTRACT_MSG);
			theItem->SetMarked(true);
			break;
			
		case B_OP_BLEND:
			theItem = subMenu->FindItem(STAGE_CUE_BLEND_MSG);
			theItem->SetMarked(true);
			break;
			
		default:
			theItem = subMenu->FindItem(STAGE_CUE_COPY_MSG);
			theItem->SetMarked(true);
			break;		
			
	}		
}


