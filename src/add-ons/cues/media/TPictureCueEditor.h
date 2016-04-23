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
		inline 	TPictureCueEditorView *GetEditorView(){ return m_EditorView; }	
		
	private:	
		// Member Functions
		bool	Init();		
		
		void 	AdjustScrollBars();
		
		void 	ShowDurationDialog();
		
		// Member Variables
		TPictureCue					*m_Cue;
		TPictureCueEditorToolbar	*m_Toolbar;
		BView						*m_Background;
		BBitmap 					*m_Bitmap;
		TPictureCueEditorView		*m_EditorView;
		BScrollBar					*m_HScroll;
		BScrollBar					*m_VScroll;
		
		TPictureDurationDialog		*m_Dialog;									
};

#endif
