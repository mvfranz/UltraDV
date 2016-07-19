//---------------------------------------------------------------------
//
//	File:	TSorterBar.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TSORTERBAR_H__
#define __TSORTERBAR_H__ 

// Constants
const int16 kSortbarHeight = 17;

// Forward Declarations
class TSorterContainer;

// Class Declarations
class TSorterBar: public BView
{
	public:
		// Member functions
		TSorterBar(BRect bounds, char *title, TSorterContainer *parent, SorterType theType);
		~TSorterBar();
		
		void 	MessageReceived(BMessage *theMessage);
		
		void 	Draw(BRect updateRect);
		void 	MouseDown(BPoint where);
		void 	MouseUp(BPoint where);
		void 	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		
		void 	FrameMoved(BPoint new_position);
		void 	FrameResized(float new_width, float new_height);
		
		void 	MakeActive(bool theState);
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
				
		// Member variables				
		SorterType			fType;
		TSorterContainer	*fParent;
		bool				fIsActive;
		char 				*fTitle;
		BRect				fResizeRect;
};		

#endif
