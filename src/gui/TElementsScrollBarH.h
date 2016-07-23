//---------------------------------------------------------------------
//
//	File:	TElementsScrollBarH.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//	Desc:	Custom Horizontal scroll bar to scroll Elements View
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TElementsScrollBarH__
#define __TElementsScrollBarH__

// Class Definition
class TElementsScrollBarH : public BScrollBar 
{

	public:
	
		// Member Functions
		TElementsScrollBarH(BRect frame, const char *name, BView *target, float min, float max);
		~TElementsScrollBarH();
			
		void	ValueChanged(float newValue);

	private:
	
		//Member Functions
		void Init();
		
		// Member Variables
		float			fLastValue;

};

#endif
