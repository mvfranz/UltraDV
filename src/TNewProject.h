//-------------------------------------------------------------------
//
//	File:	TNewProject.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.15.98
//
//	Desc:	Dialog for the setting of project defaults
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TNEWPROJECT_H__
#define __TNEWPROJECT_H__

// Foraward Declarations
class TNewProjectBox;

// Messages
#define		LIST_SELECT_MSG	'lsMG'
#define		LIST_INVOKE_MSG	'liMG'

// Class Definition
class TNewProject : public BWindow
{
	public:
		TNewProject(MuseumApp *parent, BMessage *theMessage);
		~TNewProject();
		
		void	MessageReceived(BMessage* message);	
		
		int32 	GetSelectedItem();
		
		// Inlines
		inline BList *GetPresetsList(){ return fPresetList; }
		
	private:
	
		// Member Functions
		void	Init();	
		void 	GetDialogSettings();
		
		BList 	*LoadPresets();
		
		void 	SetupPresetList(BList *presetList, BListView *presetListView );
		void 	FreePresetList(BList *presetList);
		
		void 	CreateDefaultPresets();
		
		// Member Variables		
		MuseumApp		*fParent;
		
		BView 			*fBackground;
		TNewProjectBox	*fSettingsBox;
		BBox			*fPresetsBox;
		BScrollView		*fPresetsListScrollView;
		BListView		*fPresetsListView;
		BList			*fPresetList;
		
};

#endif
