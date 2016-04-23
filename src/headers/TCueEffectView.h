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
		inline TCueEffect 	*Effect(){ return m_Effect; }
		inline bool 		Selected(){ return m_IsSelected; }
		inline BRect 		Bounds(){ return m_Bounds; }		
		
	private:		
		void 	DragPicture(BPoint);
							
	protected:
		virtual void	Init();
		void 			UpdateResizeZones();
		
		BRect		m_Bounds;
		BRect		m_LResizeZone;
		BRect		m_RResizeZone;
		TCueView	*m_Cue;
		TCueEffect	*m_Effect;		
		bool		m_IsSelected;	
		BBitmap		*m_Icon;
		
};


#endif
