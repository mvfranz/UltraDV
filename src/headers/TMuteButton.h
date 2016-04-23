//---------------------------------------------------------------------
//
//	File:	TMuteButton.h
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

#ifndef __TMUTEBUTTON_H__
#define __TMUTEBUTTON_H__ 

// Class Declarations
class TMuteButton: public BView
{
	public:
		// Member functions
		TMuteButton( TCueChannel *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler);
		TMuteButton(BMessage *data);
		~TMuteButton();
		
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
		TCueChannel	*m_Channel;
		BHandler	*m_Handler;
		bool		m_MouseDown;
		BBitmap		*m_OffBitmap;
		BBitmap		*m_OnBitmap;				
};		

#endif
