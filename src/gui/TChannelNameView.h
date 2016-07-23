//---------------------------------------------------------------------
//
//	File:	TChannelNameView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.04.98
//
//	Desc:	Museum implimentation of BStringView
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TCHANNELNAMEVIEW_H__
#define __TCHANNELNAMEVIEW_H__

// Class Definition
class TChannelNameView : public BStringView
{

	public:
		TChannelNameView( TCueChannel *channel, BRect bounds, const char *name, const char *text,
					 uint32 resizeFlags = B_FOLLOW_LEFT | B_FOLLOW_TOP,
					 uint32 flags = B_WILL_DRAW);				
		TChannelNameView(BMessage *data);
		~TChannelNameView();

		static		BArchivable *Instantiate(BMessage *data);
		status_t	Archive(BMessage *data, bool deep = true) const;

		void		Draw(BRect bounds);

		void		MouseDown(BPoint pt);
		
		//	Accessor Functions
		void 		SetChannel(TCueChannel *channel);

	private:
		TCueChannel	*fChannel;
};

#endif
