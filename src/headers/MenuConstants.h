//---------------------------------------------------------------------
//
//	File:	MenuConstants.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	08.24.98
//
//	Desc:	Menu Constants
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __MENUCONSTANTS_H__
#define __MENUCONSTANTS_H__

//	File Menu
typedef enum
{
	kFileMenuItem = 1,
	kFileNewItem,
	kFileNewAccel,
	kFileOpenItem,
	kFileOpenAccel,
	kFileCloseItem,
	kFileCloseAccel,
	kFileSaveItem,
	kFileSaveAccel,
	kFileSaveAsItem,
	kFileRevertItem,
	kFilePageSetupItem,
	kFilePrintItem,
	kFilePrintAccel,
	kFileAboutItem,
	kFileQuitItem,
	kFileQuitAccel
		
} FileMenuItems;

//	Edit Menu
typedef enum
{
	kEditMenuItem = 1,
	kEditUndoItem,
	kEditUndoAccel,
	kEditCutItem,
	kEditCutAccel,
	kEditCopyItem,
	kEditCopyAccel,
	kEditPasteItem,
	kEditPasteAccel,
	kEditClearItem,
	kEditRevertItem,	
	kEditSelectAllItem,
	kEditSelectAllAccel
	
} EditMenuItems;


//	Cue Sheet Menu
typedef enum 
{
	kCueSheetMenuItem = 1,
	kCueSheetSetupItem,
	kCueSheetTimeBaseItem,
	kCueSheetZoomInItem,
	kCueSheetZoomOutItem,
	kCueSheetShowCueDurationsItem,
	kCueSheetShowStopTimesItem,
	kCueSheetInsertChannelsItem,
	kCueSheetDeleteChannelsItem,
	kCueSheetInsertTimeItem,
	kCueSheetAlignCueStartsItem,
	kCueSheetAlignCueEndsItem,
	kCueSheetCollapseAllItem,
	kCueSheetJumpToMarkerItem
	
} CueSheetMenuItems;


//	Cue Menu
typedef enum
{
	kCueMenuItem = 1,
	kCueGetCueInfoItem,
	kCueDefaultCueSetupItem,
	kCueShowCueContentsItem,
	kCueShowAllCueContentsItem,
	kCueHideAllCueContentsItem,
	kCueCueSetupItem,
	kCueOpenCueEditorItem,
	kCueAddKeyframeItem,
	kCueAddKeyframeAccel
	
} CueMenuItems;


//	Stage Menu
typedef enum
{
	kStageMenuItem = 1,
	kStageStageSetupItem,
	kStagePaletteItem,
	kStageBringToFrontItem,
	kStageLiveUpdateItem,
	kStageGridSetupItem,
	kStageTakeStagePictureItem	
	
} StageMenuItems;


//	Capture Menu
typedef enum
{
	kCaptureMenuItem = 1,
	kCaptureVideoItem,
	kCaptureVideoSettingsItem,
	kCaptureAudioItem,
	kCaptureAudioSettingsItem,
	kCapturePreferencesItem,
	kCaptureDisplayLevelsItem			

} CaptureMenuItems;


//	Export Menu
// ABH typedef 
enum
{
	kExportMenuItem = 1
};


//	Windows Menu
typedef enum
{
	kWindowsMenuItem = 1,
	kWindowsHideStageItem,
	kWindowsShowStageItem,
	kWindowsHideBrowserItem,
	kWindowsShowBrowserItem,
	kWindowsHideTransportItem,
	kWindowsShowTransportItem,
	kWindowsHideLocatorItem,
	kWindowsShowLocatorItem,
	kWindowsHideMediaItem,
	kWindowsShowMediaItem,
	kWindowsHideMemoryItem,
	kWindowsShowMemoryItem,
	kWindowsHideStageToolsItem,
	kWindowsShowStageToolsItem
	
} WindowsMenuItems;

#endif
