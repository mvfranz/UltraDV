//---------------------------------------------------------------------
//
//	File:	TLockButton.h
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

#if !defined(__TLOCKBUTTON_H__)
#define __TLOCKBUTTON_H__

// Class Declarations
class TLockButton: public BView
{
	public:
		// Member functions
		TLockButton( TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		TLockButton(BMessage *data);
		~TLockButton();
		
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
