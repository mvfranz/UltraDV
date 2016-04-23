//---------------------------------------------------------------------
//
//	File:	TMediaTabView.cpp
//
//	Author:	Mike Ost
//
//	Date:	08.11.98
//
//	Desc:	Sub-class of TElementsView. Handles the tab labelled 'Media'
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include "TMediaTabView.h"

#include "AppConstants.h"		// for kScrollHeight
#include "AppMessages.h"		// For ADD_REF_MSG
#include <assert.h>				// For assert()
#include "ResourceManager.h"	// for GetIcon16FromResource
#include "TRadioBitmapButton.h"	// For the bitmap button
#include "TElementsView.h"		// For the different member views 
#include "TThumbnailView.h"		// ...
#include "TIconView.h"			// ...
#include "TSizeScrollView.h"	// For TSizeScrollView
#include "TSorterContainer.h"	// For SORTER MSG decls

// Local constants
#define MEDIA_TAB_LIST_VIEW_MSG			'mtLV'
#define MEDIA_TAB_THUMBNAIL_VIEW_MSG	'mtTH'
#define MEDIA_TAB_ICON_VIEW_MSG			'mtIC'

		
//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TMediaTabView::TMediaTabView(BRect bounds) :
	m_CurrentView(kElementsView),
	BView(bounds, "Media", B_FOLLOW_ALL|B_FRAME_EVENTS, 0)
{
	// TODO: for internationalization, pull the title, 'Media', from the resource
	Init();
}


//---------------------------------------------------------------------
//	Destructor
//---------------------------------------------------------------------
//
//

TMediaTabView::~TMediaTabView()
{
}

//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//

void TMediaTabView::Init()
{
	// NOTE: the order of these need to be kept in sync with the EChildID
	// enum
	
	// Create List View button
	BBitmap* up = GetIcon16FromResource("ListViewUp");
	BBitmap* down = GetIcon16FromResource("ListViewDown");
	// Make sure the button is a square, the same width as the scroll bar,
	// is high. It's top is one scroll bar height away from the bottom
	BRect buttBounds(0, 0, kScrollHeight, kScrollHeight);
	m_buttons[0] = new TRadioBitmapButton(buttBounds, "ListViewButton", up, down, this, 
										  new BMessage(MEDIA_TAB_LIST_VIEW_MSG), 
										  B_FOLLOW_LEFT + B_FOLLOW_BOTTOM); 
	// This button is on by default
	m_buttons[0]->SetValue(1);

	// The Thumbnail button goes right next door
	up   = GetIcon16FromResource("ThumbnailUp");
	down = GetIcon16FromResource("ThumbnailDown");
	buttBounds.left = buttBounds.right;
	buttBounds.right = buttBounds.left + kScrollHeight;
	m_buttons[1] = new TRadioBitmapButton(buttBounds, "ThumbnailButton", up, down, 
			this, new BMessage(MEDIA_TAB_THUMBNAIL_VIEW_MSG), B_FOLLOW_LEFT + 
			B_FOLLOW_BOTTOM); 

	// With the icon view button next
	up = GetIcon16FromResource("IconViewUp");
	down = GetIcon16FromResource("IconViewDown");
	buttBounds.left = buttBounds.right;
	buttBounds.right = buttBounds.left + kScrollHeight;
	m_buttons[2] = new TRadioBitmapButton(buttBounds, "IconViewButton", up, down, 
			this, new BMessage(MEDIA_TAB_ICON_VIEW_MSG), B_FOLLOW_LEFT + 
			B_FOLLOW_BOTTOM); 

	// create the views needed to support list, thumbnail and icon view.
	// Tell it to make room for the buttons. Hide the non-default views
	TElementsView* e = new TElementsView(Bounds());
	AddChild(e);
	
	// move the horizontal scroll bar to make room for our buttons. NOTE: add 
	// in one. The bitmap buttons are allowed to overlap so there is just one
	// black pixel shared between them. But the scroll bar changes color. It 
	// needs to not share the space.
	e->MakeRoomForButtons(3 * kScrollHeight + 1);
	// Default view is list view
	ActivateView(kElementsView);

	// Do the other two views
	BView* sizeScroll = AddSizeScrollChild(new TThumbnailView(Bounds()));
	sizeScroll->Hide();

	sizeScroll = AddSizeScrollChild(new TIconView(Bounds()));
	sizeScroll->Hide();
}

//---------------------------------------------------------------------
//	MessageReceived
//---------------------------------------------------------------------
//
//

void TMediaTabView::MessageReceived(BMessage* message)
{
	// Check for messages to switch the view
	bool buttonMsg = false;
	EChildID newView;
	switch (message->what)
	{
		case MEDIA_TAB_LIST_VIEW_MSG:
			newView = kElementsView;
			buttonMsg = true;
			break;
		case MEDIA_TAB_THUMBNAIL_VIEW_MSG:
			newView = kThumbnailView;
			buttonMsg = true;
			break;
		case MEDIA_TAB_ICON_VIEW_MSG:
			newView = kIconView;
			buttonMsg = true;
			break;
	}
	if (buttonMsg) 
	{
		if (newView != m_CurrentView) 
		{
			// Protect this section of code.
			Looper()->Lock();
			
			DeactivateView(m_CurrentView);
			m_CurrentView = newView;
			ActivateView(m_CurrentView);

			Looper()->Unlock();
		}

		return;
	}

	// Accept messages from the sorter object
	switch (message->what)
	{
		case SORTER_SELECT_MSG:
		case SORTER_INVOKE_MSG:
			if (m_CurrentView == kElementsView)
				ChildAt(m_CurrentView)->MessageReceived(message);
			return;

		// Some cue is adding an entry_ref to our browser
		// TODO: include the other two views in this. They can
		// have a new REF as well.
		case ADD_REF_MSG:
			if (m_CurrentView == kElementsView)
				ChildAt(m_CurrentView)->MessageReceived(message);
			return;
	}
				
	// Default handler
	BView::MessageReceived(message);						
}	

//---------------------------------------------------------------------
//	Show
//---------------------------------------------------------------------
//

void TMediaTabView::Show()
{
	BRect bounds = Parent()->Bounds();
	bounds.InsetBy(3, 3);
	ResizeTo(bounds.Width(), bounds.Height());
			
	BView::Show();
}

//---------------------------------------------------------------------
//	AddSizeScrollChild
//---------------------------------------------------------------------
//

BView* TMediaTabView::AddSizeScrollChild(BView* target)
{
	assert(target->Bounds() == Bounds());
	
	// Get a good size for the target view. Inset by one for a little boundary
	// within the tab. Remove our target scroll bar sizes.
	target->MoveTo(1, 1);
	target->ResizeBy(-kScrollWidth - 2, -kScrollHeight - 2);
	
	// Create the size/scroll view
	TSizeScrollView* view = new TSizeScrollView("", target, B_FOLLOW_ALL, 
			0, true, true);

	// make room for the buttons by scooting over the horiz scroll bar
	BScrollBar* horiz = view->ScrollBar(B_HORIZONTAL);
	float w = 3 * kScrollHeight + 1;
	horiz->ResizeBy(-w, 0);
	horiz->MoveBy(w, 0);

	// Add the size/scroll it as a child
	AddChild(view);
	
	return view;
}

//---------------------------------------------------------------------
//	ActivateView
//---------------------------------------------------------------------
//

void TMediaTabView::ActivateView(EChildID which)
{
	// Give the view the control buttons
	BView* view = ChildAt(which);
	
	BPoint pt = view->Bounds().LeftBottom();
	// Get the target button position. Slightly different for the 
	// different view types.
	if (which == kElementsView)
		pt.y -= kScrollHeight;
	else {
		pt.x++;
		pt.y -= kScrollHeight + 1;
	}

	for (int i = 0; i < 3; i++) 
	{
		view->AddChild(m_buttons[i]);
		m_buttons[i]->MoveTo(pt);
		pt.x += kScrollHeight;
	}

	// Make sure it is visible
	if (view->IsHidden())
		view->Show();
}

//---------------------------------------------------------------------
//	DeactivateView
//---------------------------------------------------------------------
//

void TMediaTabView::DeactivateView(EChildID which)
{
	// Take away the control buttons
	BView* view = ChildAt(which);

	BView* b = view->ChildAt(0);
	while (b) {
		BView* next = b->NextSibling();
		if (dynamic_cast<TRadioBitmapButton*>(b) != 0)
			view->RemoveChild(b);
		b = next;
	}

	// Make sure the view is invisible
	if (!view->IsHidden())
		view->Hide();
}
