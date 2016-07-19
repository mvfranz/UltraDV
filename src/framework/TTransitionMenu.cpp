//---------------------------------------------------------------------
//
//	File:	TTransitionMenu.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.16.98
//
//	Desc:	Creates and handles transition popup menu
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <algorithm>
// ABH #include <algobase.h>			// for min/max

//#include "AppConstants.h"
//#include "AppMessages.h"
//#include "AppUtils.h"
#include "TCueView.h"

#include "TTransitionMenu.h"


// Constants


//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TTransitionMenu::TTransitionMenu(TCueView *target, bool transitionIn) : BPopUpMenu("TransitionMenu")
{
	BMessage *menuMessage;

	// Create "None"
	menuMessage = new BMessage(TRAN_NONE_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem	*noneItem = new BMenuItem("None", menuMessage);
	AddItem(noneItem);

	// Seperator01
	AddSeparatorItem();

	// Create Straight Wipes submenu
	//

	BMenu *straightWipesSubMenu = new BMenu("Straight Wipes");
	BMenuItem *straightWipesSubMenuItem = new BMenuItem(straightWipesSubMenu);
	AddItem(straightWipesSubMenuItem);

	menuMessage = new BMessage(TRAN_WIPE_RIGHT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeRightItem = new BMenuItem("Right", menuMessage);
	straightWipesSubMenu->AddItem(wipeRightItem);

	menuMessage = new BMessage(TRAN_WIPE_LEFT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeLeftItem = new BMenuItem("Left", menuMessage);
	straightWipesSubMenu->AddItem(wipeLeftItem);

	menuMessage = new BMessage(TRAN_WIPE_DOWN_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeDownItem = new BMenuItem("Down", menuMessage);
	straightWipesSubMenu->AddItem(wipeDownItem);

	menuMessage = new BMessage(TRAN_WIPE_UP_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeUpItem = new BMenuItem("Up", menuMessage);
	straightWipesSubMenu->AddItem(wipeUpItem);

	// Create Diagonal Wipes submenu
	//

	BMenu *diagonalWipesSubMenu = new BMenu("Diagonal Wipes");
	BMenuItem *diagonalWipesSubMenuItem = new BMenuItem(diagonalWipesSubMenu);
	AddItem(diagonalWipesSubMenuItem);

	menuMessage = new BMessage(TRAN_WIPE_TOP_LEFT_BOTTOM_RIGHT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeTopLeftBottomRightItem = new BMenuItem("Top Left to Bottom Right", menuMessage);
	diagonalWipesSubMenu->AddItem(wipeTopLeftBottomRightItem);

	menuMessage = new BMessage(TRAN_WIPE_TOP_RIGHT_BOTTON_LEFT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeTopRightToBottomLeftItem = new BMenuItem("Top Right to Bottom Left", menuMessage);
	diagonalWipesSubMenu->AddItem(wipeTopRightToBottomLeftItem);

	menuMessage = new BMessage(TRAN_WIPE_BOTTOM_LEFT_TOP_RIGHT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeBottomLeftToTopRightItem = new BMenuItem("Bottom Left to Top Right", menuMessage);
	diagonalWipesSubMenu->AddItem(wipeBottomLeftToTopRightItem);

	menuMessage = new BMessage(TRAN_WIPE_BOTTOM_RIGHT_TOP_LEFT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *wipeBottomRightToTopLeftItem = new BMenuItem("Bottom Right to Top Left", menuMessage);
	diagonalWipesSubMenu->AddItem(wipeBottomRightToTopLeftItem);


	// Create Circular Wipes submenu
	//

	BMenu *circularWipesSubMenu = new BMenu("Circular Wipes");
	BMenuItem *circularWipesSubMenuItem = new BMenuItem(circularWipesSubMenu);
	AddItem(circularWipesSubMenuItem);


	// Create Iris Out submenu
	//

	BMenu *irisOutSubMenu = new BMenu("Iris Out");
	BMenuItem *irisOutSubMenuItem = new BMenuItem(irisOutSubMenu);
	AddItem(irisOutSubMenuItem);


	// Create Reveal submenu
	//

	BMenu *revealSubMenu = new BMenu("Reveal");
	BMenuItem *revealSubMenuItem = new BMenuItem(revealSubMenu);
	AddItem(revealSubMenuItem);

	menuMessage = new BMessage(TRAN_REVEAL_RIGHT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *revealRightItem = new BMenuItem("Right", menuMessage);
	revealSubMenu->AddItem(revealRightItem);

	menuMessage = new BMessage(TRAN_REVEAL_LEFT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *revealLeftItem = new BMenuItem("Left", menuMessage);
	revealSubMenu->AddItem(revealLeftItem);

	menuMessage = new BMessage(TRAN_REVEAL_DOWN_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *revealDownItem = new BMenuItem("Down", menuMessage);
	revealSubMenu->AddItem(revealDownItem);

	menuMessage = new BMessage(TRAN_REVEAL_UP_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *revealUpItem = new BMenuItem("Up", menuMessage);
	revealSubMenu->AddItem(revealUpItem);

	menuMessage = new BMessage(TRAN_REVEAL_TOP_LEFT_BOTTOM_RIGHT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *revealTLBRItem = new BMenuItem("Top Left to Bottom Right", menuMessage);
	revealSubMenu->AddItem(revealTLBRItem);

	menuMessage = new BMessage(TRAN_REVEAL_TOP_RIGHT_BOTTON_LEFT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *revealTRBLItem = new BMenuItem("Top Right to Bottom Left", menuMessage);
	revealSubMenu->AddItem(revealTRBLItem);

	menuMessage = new BMessage(TRAN_REVEAL_BOTTOM_LEFT_TOP_RIGHT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *revealBLTRItem = new BMenuItem("Bottom Left to Top Right", menuMessage);
	revealSubMenu->AddItem(revealBLTRItem);

	menuMessage = new BMessage(TRAN_REVEAL_BOTTOM_RIGHT_TOP_LEFT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *revealBRTLItem = new BMenuItem("Bottom Right to Top Left", menuMessage);
	revealSubMenu->AddItem(revealBRTLItem);


	// Create Blinds submenu
	//

	BMenu *blindsSubMenu = new BMenu("Blinds");
	BMenuItem *blindsSubMenuItem = new BMenuItem(blindsSubMenu);
	AddItem(blindsSubMenuItem);


	// Create Curtains submenu
	//

	BMenu *curtainsSubMenu = new BMenu("Curtains");
	BMenuItem *curtainsSubMenuItem = new BMenuItem(curtainsSubMenu);
	AddItem(curtainsSubMenuItem);

	menuMessage = new BMessage(TRAN_CURTAINS_IN_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *curtainsInItem = new BMenuItem("In", menuMessage);
	curtainsSubMenu->AddItem(curtainsInItem);

	menuMessage = new BMessage(TRAN_CURTAINS_OUT_MSG);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *curtainsOutItem = new BMenuItem("Out", menuMessage);
	curtainsSubMenu->AddItem(curtainsOutItem);



	// Create Horizontal Strips submenu
	//

	BMenu *hStripsSubMenu = new BMenu("Horizontal Strips");
	BMenuItem *hStripsSubMenuItem = new BMenuItem(hStripsSubMenu);
	AddItem(hStripsSubMenuItem);


	// Create Vertical Strips submenu
	//

	BMenu *vStripsSubMenu = new BMenu("Vertical Strips");
	BMenuItem *vStripsSubMenuItem = new BMenuItem(vStripsSubMenu);
	AddItem(vStripsSubMenuItem);


	// Create Dissolve submenu
	//

	BMenu *dissolveSubMenu = new BMenu("Dissolve");
	BMenuItem *dissolveSubMenuItem = new BMenuItem(dissolveSubMenu);
	AddItem(dissolveSubMenuItem);


	// Create Zoom submenu
	//

	BMenu *zoomSubMenu = new BMenu("Zoom");
	BMenuItem *zoomSubMenuItem = new BMenuItem(zoomSubMenu);
	AddItem(zoomSubMenuItem);


	// All these items message to the cue
	straightWipesSubMenu->SetTargetForItems(target);
	diagonalWipesSubMenu->SetTargetForItems(target);
	circularWipesSubMenu->SetTargetForItems(target);
	irisOutSubMenu->SetTargetForItems(target);
	revealSubMenu->SetTargetForItems(target);
	blindsSubMenu->SetTargetForItems(target);
	curtainsSubMenu->SetTargetForItems(target);
	hStripsSubMenu->SetTargetForItems(target);
	vStripsSubMenu->SetTargetForItems(target);
	dissolveSubMenu->SetTargetForItems(target);
	zoomSubMenu->SetTargetForItems(target);


	// Seperator02
	AddSeparatorItem();

	// Create Settings item
	menuMessage = new BMessage(TRAN_DURATION);
	menuMessage->AddBool("TransitionIn", transitionIn);
	BMenuItem *durationItem = new BMenuItem("Settings...", menuMessage);
	AddItem(durationItem);

	// All these items message to the cue
	SetTargetForItems(target);

}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TTransitionMenu::~TTransitionMenu()
{

}

#pragma mark -
#pragma mark === Menu Location Routines ===


//---------------------------------------------------------------------
//	GetMenuItem
//---------------------------------------------------------------------
//
//

BMenuItem *TTransitionMenu::GetMenuItem(int16 menuID)
{
	switch (menuID)
	{
		case kTransitionNone:
			return ( FindItem(TRAN_NONE_MSG) );
			break;

		// Straight Wipes Submenu
		case kWipeRight:
		case kWipeLeft:
		case kWipeDown:
		case kWipeTop:
			return GetStraightWipesMenuItem(menuID);
			break;

		// Diagonal Wipes Submenu
		case kWipeTopLeftBottomRight:
		case kWipeTopRightBottomLeft:
		case kWipeBottomLeftTopRight:
		case kWipeBottomRightTopLeft:
			return GetDiagonalWipesMenuItem(menuID);
			break;


		// Reveal Submenu
		case kRevealRight:
		case kRevealLeft:
		case kRevealDown:
		case kRevealUp:
		case kRevealTopLeftBottomRight:
		case kRevealTopRightBottomLeft:
		case kRevealBottomLeftTopRight:
		case kRevealBottomRightTopLeft:
			return GetRevealMenuItem(menuID);
			break;

		// Curtain Submenu
		case kCurtainsIn:
		case kCurtainsOut:
			return GetCurtainsMenuItem(menuID);
			break;

		default:
			return NULL;
	}
	return NULL;
}


//---------------------------------------------------------------------
//	GetStraightWipesMenuItem
//---------------------------------------------------------------------
//
//

BMenuItem *TTransitionMenu::GetStraightWipesMenuItem(int16 menuID)
{
	BMenuItem *wipesItem = FindItem("Straight Wipes");
	BMenu *subMenu = wipesItem->Submenu();

	switch (menuID)
	{

		// Wipes Submenu
		case kWipeRight:
			return ( subMenu->FindItem(TRAN_WIPE_RIGHT_MSG) );
			break;

		case kWipeLeft:
			return ( subMenu->FindItem(TRAN_WIPE_LEFT_MSG) );
			break;

		case kWipeDown:
			return ( subMenu->FindItem(TRAN_WIPE_DOWN_MSG) );
			break;

		case kWipeTop:
			return ( subMenu->FindItem(TRAN_WIPE_UP_MSG) );
			break;

		default:
			return NULL;
	}
}



//---------------------------------------------------------------------
//	GetDiagonalWipesMenuItem
//---------------------------------------------------------------------
//
//

BMenuItem *TTransitionMenu::GetDiagonalWipesMenuItem(int16 menuID)
{
	BMenuItem *wipesItem = FindItem("Diagonal Wipes");
	BMenu *subMenu = wipesItem->Submenu();

	switch (menuID)
	{

		// Wipes Submenu
		case kWipeTopLeftBottomRight:
			return ( subMenu->FindItem(TRAN_WIPE_TOP_LEFT_BOTTOM_RIGHT_MSG) );
			break;

		case kWipeTopRightBottomLeft:
			return ( subMenu->FindItem(TRAN_WIPE_TOP_RIGHT_BOTTON_LEFT_MSG) );
			break;

		case kWipeBottomLeftTopRight:
			return ( subMenu->FindItem(TRAN_WIPE_BOTTOM_LEFT_TOP_RIGHT_MSG) );
			break;

		case kWipeBottomRightTopLeft:
			return ( subMenu->FindItem(TRAN_WIPE_BOTTOM_RIGHT_TOP_LEFT_MSG) );
			break;

		default:
			return NULL;
	}
}


//---------------------------------------------------------------------
//	GetRevealMenuItem
//---------------------------------------------------------------------
//
//

BMenuItem *TTransitionMenu::GetRevealMenuItem(int16 menuID)
{
	BMenuItem *wipesItem = FindItem("Reveal");
	BMenu *subMenu = wipesItem->Submenu();

	switch (menuID)
	{

		case kRevealRight:
			return ( subMenu->FindItem(TRAN_REVEAL_RIGHT_MSG) );
			break;

		case kRevealLeft:
			return ( subMenu->FindItem(TRAN_REVEAL_LEFT_MSG) );
			break;

		case kRevealDown:
			return ( subMenu->FindItem(TRAN_REVEAL_DOWN_MSG) );
			break;

		case kRevealUp:
			return ( subMenu->FindItem(TRAN_REVEAL_UP_MSG) );
			break;

		case kRevealTopLeftBottomRight:
			return ( subMenu->FindItem(TRAN_REVEAL_TOP_LEFT_BOTTOM_RIGHT_MSG) );
			break;

		case kRevealTopRightBottomLeft:
			return ( subMenu->FindItem(TRAN_REVEAL_TOP_RIGHT_BOTTON_LEFT_MSG) );
			break;

		case kRevealBottomLeftTopRight:
			return ( subMenu->FindItem(TRAN_REVEAL_BOTTOM_LEFT_TOP_RIGHT_MSG) );
			break;

		case kRevealBottomRightTopLeft:
			return ( subMenu->FindItem(TRAN_REVEAL_BOTTOM_RIGHT_TOP_LEFT_MSG) );
			break;

		default:
			return NULL;
	}
}


//---------------------------------------------------------------------
//	GetCurtainsMenuItem
//---------------------------------------------------------------------
//
//

BMenuItem *TTransitionMenu::GetCurtainsMenuItem(int16 menuID)
{
	BMenuItem *wipesItem = FindItem("Curtains");
	BMenu *subMenu = wipesItem->Submenu();

	switch (menuID)
	{

		case kCurtainsIn:
			return ( subMenu->FindItem(TRAN_CURTAINS_IN_MSG) );
			break;

		case kCurtainsOut:
			return ( subMenu->FindItem(TRAN_CURTAINS_OUT_MSG) );
			break;

		default:
			return NULL;
	}
}
