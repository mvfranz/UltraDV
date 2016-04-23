//---------------------------------------------------------------------
//
//	File:	TZoomButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.16.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TZOOMBUTTON_H__
#define __TZOOMBUTTON_H__

// Class Declarations
class TZoomButton: public BView
{
	public:
		// Member functions
		TZoomButton( TToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		TZoomButton(BMessage *data);		
		~TZoomButton();

		static BArchivable *Instantiate(BMessage *archive);
		status_t 	Archive(BMessage *data, bool deep) const;

		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		
		void 	AttachedToWindow();
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		
		// Member variables
		TToolbar	*m_Parent;
		BHandler	*m_Handler;
		bool		m_MouseDown;
		BBitmap		*m_OffBitmap;
		BBitmap		*m_OnBitmap;
		
};		

#endif
