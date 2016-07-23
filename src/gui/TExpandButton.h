//---------------------------------------------------------------------
//
//	File:	TExpandButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.07.98
//
//	Desc:	
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TExpandButton__
#define __TExpandButton__ 

// Class Declarations
class TExpandButton: public BView
{
	public:
		// Member functions
		TExpandButton( TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		TExpandButton(BMessage *message);
		~TExpandButton();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;

		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		
		void 	AttachedToWindow();
		
		void 	SetChannel(TCueChannel *channel);
		
		// Member variables	
		
	private:				
		// Member functions
		void 	Init();
		
		
		// Member variables
		TCueChannel	*fChannel;
		BHandler	*fHandler;
		bool		fMouseDown;
		BBitmap		*fOffBitmap;
		BBitmap		*fOnBitmap;
		
};		

#endif
