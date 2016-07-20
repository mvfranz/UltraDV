//---------------------------------------------------------------------
//
//	File:	TPictureCueEditor.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.26.98
//
//	Desc:	Picture Editor	
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#if !defined(__TPICTURECUEEDITOR_H__)
#define __TPICTURECUEEDITOR_H__

#include "TPictureCueEditorView.h"

// Constants

// Forward Declarations
class TPictureCue;
class TPictureCueEditorToolbar;
class TPictureDurationDialog;

// Class Definition
class TPictureCueEditor: public BWindow
{
	public:
		TPictureCueEditor(BRect bounds, TPictureCue *theCue);
		~TPictureCueEditor();
		
		void		MessageReceived(BMessage* message);	
		
		void 		FrameResized( float newWidth, float newHeight);
		
		// Accessor Functions
		inline 	TPictureCueEditorView *GetEditorView(){ return fEditorView; }	
		
	private:	
		// Member Functions
		bool	Init();		
		
		void 	AdjustScrollBars();
		
		void 	ShowDurationDialog();
		
		// Member Variables
		TPictureCue					*fCue;
		TPictureCueEditorToolbar	*fToolbar;
		BView						*fBackground;
		BBitmap 					*fBitmap;
		TPictureCueEditorView		*fEditorView;
		BScrollBar					*fHScroll;
		BScrollBar					*fVScroll;
		
		TPictureDurationDialog		*fDialog;									
};

#endif
