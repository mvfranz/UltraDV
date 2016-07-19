//---------------------------------------------------------------------
//
//	File:	TSoloButton.h
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

#ifndef __TSOLOBUTTON_H__
#define __TSOLOBUTTON_H__ 

// Class Declarations
class TSoloButton: public BView
{
	public:
		// Member functions
		TSoloButton( TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		TSoloButton(BMessage *data);
		~TSoloButton();
		
		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;
	
		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		
		void 	SetChannel(TCueChannel *channel);
		
		void 	AttachedToWindow();
		
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
