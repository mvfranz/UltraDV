//---------------------------------------------------------------------
//
//	File:	TElementsScrollBarV.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	Custom Vertical scroll bar to scroll Elements View
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TElementsScrollBarV__
#define __TElementsScrollBarV__

// Forward Declaration
class TElementsSorter;

// Class Definition
class TElementsScrollBarV : public BScrollBar 
{

	public:
	
		// Member Functions
		TElementsScrollBarV(BRect frame, const char *name, TElementsSorter *target, float min, float max);
		~TElementsScrollBarV();

		void	ValueChanged(float newValue);

	private:
	
		//Member Functions
		void Init();
		
		// Member Variables
		TElementsSorter	*fParent;		
		float			fLastValue;
		
};

#endif
