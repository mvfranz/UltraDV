//-------------------------------------------------------------------
//
//	File:	TCueEffectView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	11.12.98
//
//-------------------------------------------------------------------

#if !defined(__TCUEEFFECTVIEW_H__)
#define __TCUEEFFECTVIEW_H__ 

//	Includes
#include "TCueEffect.h"
#include "TCueView.h"

//	Constants
const int32 kEffectHeight     = 20;

// Class Definition
class TCueEffectView
{
	public:
		// Member Functions
		TCueEffectView(BRect bounds, TCueView *theCue, TCueEffect *theEffect);
		virtual ~TCueEffectView();
		
		//	Drawing routines				
		virtual void 	Draw(BRect updateRect);
		
		virtual void 	MouseDown(BPoint where, bool doubleClick = false);
		virtual void 	MouseMoved(	BPoint where);
		
		virtual void	Resized();
						
		void 	Select();
		void	Deselect();
		
		void	DrawSelectionRect();
		void 	InvalidateSelectionRect();
						
		// Accessor Functions
		inline TCueEffect 	*Effect(){ return fEffect; }
		inline bool 		Selected(){ return fIsSelected; }
		inline BRect 		Bounds(){ return fBounds; }		
		
	private:		
		void 	DragPicture(BPoint);
							
	protected:
		virtual void	Init();
		void 			UpdateResizeZones();
		
		BRect		fBounds;
		BRect		fLResizeZone;
		BRect		fRResizeZone;
		TCueView	*fCue;
		TCueEffect	*fEffect;		
		bool		fIsSelected;	
		BBitmap		*fIcon;
		
};


#endif
