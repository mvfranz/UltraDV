//---------------------------------------------------------------------
//
//	File:	TStageToolsView.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.13.98
//
//	Desc:	Stage tools view
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <support/Debug.h>

#include "AppConstants.h"
#include "ResourceManager.h"

#include "TStageTools.h"
#include "TStageToolButton.h"
#include "TStatusView.h"

#include "TStageToolsView.h"



//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TStageToolsView::TStageToolsView(TStageTools *parent, BRect bounds) : BView(bounds, "MediaCueView", B_FOLLOW_ALL, B_WILL_DRAW)
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

TStageToolsView::~TStageToolsView()
{
}


//---------------------------------------------------------------------
//	Init
//---------------------------------------------------------------------
//
//	Perform default initialization tasks

void TStageToolsView::Init()
{
	BBitmap 	*offBitmap;
	BBitmap		*onBitmap;	
	
	// Set view background color
	SetViewColor(kBlueGrey);
	
	// Get application info
	app_info info;
	
	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);
	if (file.InitCheck())
		return;
		
	BResources res(&file);
	
	
	//-------------------------------------
	//	Creae the stage tool buttons
	//-------------------------------------
	
	//
	// Row One
	// 
	
	const BRect bounds = Bounds();
	BRect toolRect = bounds;
	
	//	
	//	Move Tool
			
	// Set button location
	toolRect.right 	=  toolRect.left + (kStageToolWidth-1);
	toolRect.bottom =  toolRect.top 	+ (kStageToolHeight-1);
	
	// Load bitmaps
	offBitmap 	= GetStageToolIcon("StageMoveDown");
	onBitmap 	= GetStageToolIcon("StageMoveUp");
	
	// Create button
	m_MoveTool = new TStageToolButton(this, toolRect, "Move Tool", offBitmap, onBitmap, new BMessage(MOVE_TOOL_MSG) );
	AddChild(m_MoveTool);
                             
    //
    //	Rotate Tool	
    
    // Set button location
	toolRect.left 	= toolRect.right;
	toolRect.right 	= toolRect.left + (kStageToolWidth-1);
		
    // Load bitmaps
	offBitmap 	= GetStageToolIcon("StageRotateDown");
	onBitmap 	= GetStageToolIcon("StageRotateUp");
	
	// Create button
	m_RotateTool = new TStageToolButton( this, toolRect, "Rotate Tool", offBitmap, onBitmap, new BMessage(ROTATE_TOOL_MSG));
	AddChild(m_RotateTool);
	
	
    //
    //	Row Two
    //
    
    //
    //	Crop Tool
    
    // Set button location
	toolRect.left 	= bounds.left;
	toolRect.right 	= toolRect.left + (kStageToolWidth-1);
	toolRect.top 	= toolRect.bottom;
	toolRect.bottom = toolRect.top + (kStageToolHeight-1);
	
    // Load bitmaps
	offBitmap 	= GetStageToolIcon("StageCropDown");
	onBitmap 	= GetStageToolIcon("StageCropUp");
	
	// Create button
	m_CropTool = new TStageToolButton( this, toolRect, "Crop Tool", offBitmap, onBitmap, new BMessage(CROP_TOOL_MSG));
	AddChild(m_CropTool);
	
    
    //
    //	Scale Tool
    
 	// Set button location
	toolRect.left 	= toolRect.right;
	toolRect.right 	= toolRect.left + (kStageToolWidth-1);
    
    // Load bitmaps
	offBitmap 	= GetStageToolIcon("StageScaleDown");
	onBitmap 	= GetStageToolIcon("StageScaleUp");
	
	// Create button
	m_ScaleTool = new TStageToolButton( this, toolRect, "Scale Tool", offBitmap, onBitmap, new BMessage(SCALE_TOOL_MSG));
	AddChild(m_ScaleTool);
	
    
    //
    //	Row Three
    //
    
    //
    //	Shear Tool
    
    // Set button location
    toolRect.left 	= bounds.left;
	toolRect.right 	= toolRect.left + (kStageToolWidth-1);
	toolRect.top 	= toolRect.bottom;
	toolRect.bottom = toolRect.top + (kStageToolHeight-1);

				 
    // Load bitmaps
	offBitmap 	= GetStageToolIcon("StageShearDown");
	onBitmap 	= GetStageToolIcon("StageShearUp");
		
	// Create button
	m_ShearTool = new TStageToolButton(this, toolRect, "Shear Tool", offBitmap, onBitmap, new BMessage(SHEAR_TOOL_MSG));
	AddChild(m_ShearTool);
	
	    
    //
    //	Skew Tool
    
	// Set button location
	toolRect.left 	= toolRect.right;
	toolRect.right 	= toolRect.left + (kStageToolWidth-1);

	// Load bitmaps
	offBitmap 	= GetStageToolIcon("StageSkewDown");
	onBitmap 	= GetStageToolIcon("StageSkewUp");
	
	// Create button
	m_SkewTool = new TStageToolButton(this, toolRect, "Skew Tool", offBitmap, onBitmap, new BMessage(SKEW_TOOL_MSG));
	AddChild(m_SkewTool);
	
	//
    //	Row Four
    //
    
    //
    //	PerspectiveTool Tool
    
    // Set button location
    toolRect.left 	= bounds.left;
	toolRect.right 	= toolRect.left + (kStageToolWidth-1);
	toolRect.top 	= toolRect.bottom;
	toolRect.bottom = toolRect.top + (kStageToolHeight-1);

				 
    // Load bitmaps
	offBitmap 	= GetStageToolIcon("StagePerspectiveDown");
	onBitmap 	= GetStageToolIcon("StagePerspectiveUp");
		
	// Create button
	m_PerspectiveTool = new TStageToolButton(this, toolRect, "Perspective Tool", offBitmap, onBitmap, new BMessage(PERSPECTIVE_TOOL_MSG));
	AddChild(m_PerspectiveTool);
	
	    
    //
    //	Mirror Tool
    
	// Set button location
	toolRect.left 	= toolRect.right;
	toolRect.right 	= toolRect.left + (kStageToolWidth-1);

	// Load bitmaps
	offBitmap 	= GetStageToolIcon("StageMirrorDown");
	onBitmap 	= GetStageToolIcon("StageMirrorUp");
	
	// Create button
	m_MirrorTool = new TStageToolButton(this, toolRect, "Mirror Tool", offBitmap, onBitmap, new BMessage(MIRROR_TOOL_MSG));
	AddChild(m_MirrorTool);
		
}


//------------------------------------------------------------------
//	AttachedToWindow
//------------------------------------------------------------------
//
//	Select Move tool when first attached to window
//

void TStageToolsView::AttachedToWindow()
{
 
	//	Select Move Tool
	m_MoveTool->Activate(true);
	UpdateStatusText(MOVE_TOOL_MSG);	       	
	
	//	Pass up to parent
	BView::AttachedToWindow();
}

#pragma mark -
#pragma mark === Message Handling ===


//------------------------------------------------------------------
//	MessageReceived
//------------------------------------------------------------------
//
//

void TStageToolsView::MessageReceived(BMessage* message)
{			
	switch (message->what)
	{
		case MOVE_TOOL_MSG:
		case ROTATE_TOOL_MSG:
		case CROP_TOOL_MSG:
		case SCALE_TOOL_MSG:
		case SHEAR_TOOL_MSG:
		case SKEW_TOOL_MSG:
		case PERSPECTIVE_TOOL_MSG:
		case MIRROR_TOOL_MSG:
			UpdateStatusText(message->what);
			DeactivateAllTools(message->what);
			break;
			
		default:
			BView::MessageReceived(message);						
			break;			
	}	
}

#pragma mark -
#pragma mark === Utility Routines ===


//---------------------------------------------------------------------
//	DeactivateAllTools
//---------------------------------------------------------------------
//
//	Deactivate all tools excpet for the on passed in.  If the one passed
//	in is null, deactivate all.
//

void TStageToolsView::DeactivateAllTools(uint32 theTool)
{						
	switch(theTool)
	{
		case MOVE_TOOL_MSG:
			m_RotateTool->Activate(false);
			m_CropTool->Activate(false);
			m_ScaleTool->Activate(false);
			m_ShearTool->Activate(false);
			m_SkewTool->Activate(false);
			m_PerspectiveTool->Activate(false);
			m_MirrorTool->Activate(false);			
			break;
			
		case ROTATE_TOOL_MSG:
			m_MoveTool->Activate(false);
			m_CropTool->Activate(false);
			m_ScaleTool->Activate(false);
			m_ShearTool->Activate(false);
			m_SkewTool->Activate(false);
			m_PerspectiveTool->Activate(false);
			m_MirrorTool->Activate(false);			
			break;
			
		case CROP_TOOL_MSG:
			m_MoveTool->Activate(false);
			m_RotateTool->Activate(false);
			m_ScaleTool->Activate(false);
			m_ShearTool->Activate(false);
			m_SkewTool->Activate(false);
			m_PerspectiveTool->Activate(false);
			m_MirrorTool->Activate(false);			
			break;
			
		case SCALE_TOOL_MSG:
			m_MoveTool->Activate(false);
			m_RotateTool->Activate(false);
			m_CropTool->Activate(false);
			m_ShearTool->Activate(false);
			m_SkewTool->Activate(false);
			m_PerspectiveTool->Activate(false);
			m_MirrorTool->Activate(false);			
			break;
			
		case SHEAR_TOOL_MSG:
			m_MoveTool->Activate(false);
			m_RotateTool->Activate(false);
			m_CropTool->Activate(false);
			m_ScaleTool->Activate(false);
			m_SkewTool->Activate(false);
			m_PerspectiveTool->Activate(false);
			m_MirrorTool->Activate(false);			
			break;
			
		case SKEW_TOOL_MSG:
			m_MoveTool->Activate(false);
			m_RotateTool->Activate(false);
			m_CropTool->Activate(false);
			m_ScaleTool->Activate(false);
			m_ShearTool->Activate(false);
			m_PerspectiveTool->Activate(false);
			m_MirrorTool->Activate(false);			
			break;
			
		case PERSPECTIVE_TOOL_MSG:
			m_MoveTool->Activate(false);
			m_RotateTool->Activate(false);
			m_CropTool->Activate(false);
			m_ScaleTool->Activate(false);
			m_ShearTool->Activate(false);
			m_SkewTool->Activate(false);
			m_MirrorTool->Activate(false);			
			break;
				
		case MIRROR_TOOL_MSG:
			m_MoveTool->Activate(false);
			m_RotateTool->Activate(false);
			m_CropTool->Activate(false);
			m_ScaleTool->Activate(false);
			m_ShearTool->Activate(false);
			m_SkewTool->Activate(false);
			m_PerspectiveTool->Activate(false);
			break;
					
		default:
			m_MoveTool->Activate(false);
			m_RotateTool->Activate(false);
			m_CropTool->Activate(false);
			m_ScaleTool->Activate(false);
			m_ShearTool->Activate(false);
			m_SkewTool->Activate(false);
			m_PerspectiveTool->Activate(false);
			m_MirrorTool->Activate(false);			
			break;
	
	}
}


//---------------------------------------------------------------------
//	UpdateStatusText
//---------------------------------------------------------------------
//
//	Draw descriptive text about tool in status area
//

void TStageToolsView::UpdateStatusText(uint32 theTool)
{						
	//	Create message to send to status bar
	BMessage *theMessage = new BMessage(UPDATE_STATUS_TEXT_MSG);
															
	switch(theTool)
	{
		case MOVE_TOOL_MSG:				
			theMessage->AddString("StatusText", "Move");
			break;
			
		case ROTATE_TOOL_MSG:
			theMessage->AddString("StatusText", "Rotate");
			break;
			
		case CROP_TOOL_MSG:
			theMessage->AddString("StatusText", "Crop");
			break;
			
		case SCALE_TOOL_MSG:
			theMessage->AddString("StatusText", "Scale");
			break;
			
		case SHEAR_TOOL_MSG:
			theMessage->AddString("StatusText", "Shear");
			break;
			
		case SKEW_TOOL_MSG:
			theMessage->AddString("StatusText", "Skew");
			break;
			
		case PERSPECTIVE_TOOL_MSG:
			theMessage->AddString("StatusText", "Perspective");
			break;
				
		case MIRROR_TOOL_MSG:
			theMessage->AddString("StatusText", "Mirror");
			break;
					
		default:				
			theMessage->AddString("StatusText", "");
			break;
	
	}
	
	//	Post the message
	m_Parent->GetStatusView()->MessageReceived(theMessage);
	
	//	Clean up
	delete theMessage;
}
			
//---------------------------------------------------------------------
//	GetStageToolIcon
//---------------------------------------------------------------------
//
//	Find and return stage tool bitmap by string
//

BBitmap *GetStageToolIcon(const char *theResource)
{	
	// Get application info
	app_info info;
	
	be_app->GetAppInfo(&info);
	BFile file(&info.ref, O_RDONLY);	
	if (file.InitCheck())
		return NULL;
	
	size_t 		size;
	BBitmap 	*data;
	
	BResources res; 
	status_t err; 
	if ( (err = res.SetTo(&file)) != B_NO_ERROR ) 
		return NULL;
		
	data = (BBitmap *)res.FindResource('bits', theResource, &size);
	if (!data)			
		return NULL;
				
	// Load icon
	BRect bounds(0, 0, kStageToolWidth-1, kStageToolHeight-1);
	BBitmap *bitmap = new BBitmap(bounds, B_COLOR_8_BIT);
	ASSERT(bitmap);
	bitmap->SetBits(data, size, 0, B_COLOR_8_BIT);
	
	
	return (bitmap);	
}