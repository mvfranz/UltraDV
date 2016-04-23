//---------------------------------------------------------------------
//
//	File:	TVideoEditorTransport.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.13.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TVideoEditorTransport__
#define __TVideoEditorTransport__ 

// Forward Declarations
class TVideoEditor;

// Class Declarations
class TVideoEditorTransport: public BView
{
	public:
		// Member functions
		TVideoEditorTransport(TVideoEditor *editor, BMessage *message);
		~TVideoEditorTransport();
							
		void 	Draw(BRect updateRect);
					
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		void	DrawSlider();
				
										
		// Member variables
		TVideoEditor	*m_Editor;
		BRect			m_SliderRect;
};		

#endif
