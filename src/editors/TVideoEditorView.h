//---------------------------------------------------------------------
//
//	File:	TVideoEditorView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.31.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVideoEditorView__
#define __TVideoEditorView__

// Forward Declarations
class TVideoEditor;

// Class Declarations
class TVideoEditorView: public BView
{
	public:
		// Member functions
		TVideoEditorView(TVideoEditor *editor, BMessage *message);
		~TVideoEditorView();
							
		void 	Draw(BRect updateRect);
		void 	DisplayFrame(BBitmap *frame);
							
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
				
										
		// Member variables
		TVideoEditor	*fEditor;
		BPoint			fDrawPoint;
};		

#endif
