//---------------------------------------------------------------------
//
//	File:	TChannelCueButton.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	03.17.98
//
//	Desc:	Abstract cue button class
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCHANNELCUEBUTTON_H__
#define __TCHANNELCUEBUTTON_H__ 

// Class Declarations
class TChannelCueButton: public BView
{
	public:
		// Member functions
		TChannelCueButton( TCueView *parent, BRect bounds, const char *name, BBitmap *offBitmap, BBitmap *onBitmap, BHandler *handler, uint32 flags);
		TChannelCueButton(const TChannelCueButton *theButton);
		TChannelCueButton(BMessage *data);
		~TChannelCueButton();
		
		static BArchivable 	*Instantiate(BMessage *archive);
		status_t 		Archive(BMessage *data, bool deep) const;
		
		virtual void Draw(BRect updateRect);
		virtual void MouseDown(BPoint where);
		virtual void MouseUp(BPoint where);
		virtual void MouseMoved(BPoint where, uint32 code, const BMessage *a_message);
			
		// Member variables	
		
	private:				
	
	protected:
		// Member functions
		virtual void 	Init();
		
		virtual void 	DoClick();
		
		// Member variables
		TCueView	*fCue;
		BLooper		*fTarget;
		BHandler	*fHandler;
		bool		fMouseDown;
		BBitmap		*fOffBitmap;
		BBitmap		*fOnBitmap;		
};		

#endif
