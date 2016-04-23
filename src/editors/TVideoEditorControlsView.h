//---------------------------------------------------------------------
//
//	File:	TVideoEditorControlsView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.12.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVideoEditorControlsView__
#define __TVideoEditorControlsView__  

// Forward Declarations

// Class Declarations
class TVideoEditorControlsView: public BView
{
	public:
		// Member functions
		TVideoEditorControlsView(BMessage *message);
		~TVideoEditorControlsView();
					
		void 	Draw(BRect updateRect);
										
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
				
		// thread Routines
										
		// Member variables
};		

#endif
