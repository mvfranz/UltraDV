//---------------------------------------------------------------------
//
//	File:	TStageToolsView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	07.13.98
//
//	Desc:	Stage tools view
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TStageToolsView__
#define __TStageToolsView__

// Messages
#define MOVE_TOOL_MSG			'mtmG'
#define ROTATE_TOOL_MSG			'rtmG'
#define CROP_TOOL_MSG			'ctmG'
#define SCALE_TOOL_MSG			'stmG'
#define SHEAR_TOOL_MSG			'shtG'
#define SKEW_TOOL_MSG			'sktG'
#define PERSPECTIVE_TOOL_MSG	'ptmg'
#define MIRROR_TOOL_MSG			'mrtG'

// Consts

//	Forward Declarations
class TStageToolButton;

// Class Declarations
class TStageToolsView: public BView
{
	public:
		// Member functions
		TStageToolsView(TStageTools *parent, BRect bounds);
		~TStageToolsView();
		
		void 	AttachedToWindow();
		
		void 	MessageReceived(BMessage* message);
		
		void 	DeactivateAllTools(uint32 theTool);
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		void 	UpdateStatusText(uint32 theTool);
		
		// Member variables
		TStageTools 		*fParent;
		
		TStageToolButton	*fMoveTool;
		TStageToolButton	*fRotateTool;
		TStageToolButton	*fCropTool;
		TStageToolButton	*fScaleTool;
		TStageToolButton	*fShearTool;
		TStageToolButton	*fSkewTool;
		TStageToolButton	*fPerspectiveTool;
		TStageToolButton	*fMirrorTool;

};	

// Utility Functions
BBitmap *GetStageToolIcon(const char *theResource);	

#endif
