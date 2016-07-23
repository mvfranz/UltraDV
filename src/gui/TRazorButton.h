//---------------------------------------------------------------------
//
//	File:	TRazorButton.h
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

#ifndef __TRAZORBUTTON_H__
#define __TRAZORBUTTON_H__ 

// Class Declarations
class TRazorButton: public BView
{
	public:
		// Member functions
		TRazorButton( TToolbar *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		TRazorButton(BMessage *data);		
		~TRazorButton();

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
		TToolbar	*fParent;
		BHandler	*fHandler;
		bool		fMouseDown;
		BBitmap		*fOffBitmap;
		BBitmap		*fOnBitmap;
		
};		

#endif
