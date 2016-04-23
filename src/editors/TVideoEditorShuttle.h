//---------------------------------------------------------------------
//
//	File:	TVideoEditorShuttle.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.13.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVideoEditorShuttle__
#define __TVideoEditorShuttle__   

// Forward Declarations
class TVideoEditor;

// Class Declarations
class TVideoEditorShuttle: public BView
{
	public:
		// Member functions
		TVideoEditorShuttle(TVideoEditor *editor, BMessage *message);
		~TVideoEditorShuttle();
							
		void 	Draw(BRect updateRect);
					
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
				
										
		// Member variables
		TVideoEditor	*m_Editor;
};		

#endif
