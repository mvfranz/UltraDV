//-------------------------------------------------------------------
//
//	File:	TStageTextView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.28.98
//
//-------------------------------------------------------------------

#ifndef _TSTAGETEXTVIEW_H_
#define _TSTAGETEXTVIEW_H_

#include "TStageCue.h"

// Forward Declarations

// Class Definition
class TStageTextView : public TStageCue
{
	public:
		TStageTextView(BRect bounds, char *name, TCueView *theCue);
		~TStageTextView();

		void 	MouseDown(BPoint where);						
};




#endif	// _TSTAGETEXTVIEW_H_
