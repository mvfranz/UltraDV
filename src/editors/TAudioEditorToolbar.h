//-------------------------------------------------------------------
//
//	File:	TAudioEditorToolbar.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
//-------------------------------------------------------------------

#ifndef __TAUDIOEDITORTOOLBAR_H__
#define __TAUDIOEDITORTOOLBAR_H__ 

// Forward Declarations
class	TAudioEditor;
class	TAudioZoomInButton; 
class	TAudioZoomOutButton; 
class	TAudioPlayButton; 

// Class Definition
class TAudioEditorToolbar: public BView
{
	public:
		TAudioEditorToolbar(TAudioEditor *parent, BRect bounds);
		~TAudioEditorToolbar();
		void Draw(BRect updateRect);
		void MouseDown(BPoint where);
		void MouseUp(BPoint where);
		void MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
		void WindowActivated(bool state);
		void KeyDown(const char *bytes, int32 numBytes);
		void KeyUp(const char *bytes, int32 numBytes);
		void FrameMoved(BPoint new_position);
		void FrameResized(float new_width, float new_height);
		
		void MessageReceived(BMessage* message);
	private:
		// Member Functions
		void Init();
		
		// Member Variables		
		TAudioEditor		*fParent;
		
		TAudioZoomInButton	*fZoomInButton; 
		TAudioZoomOutButton	*fZoomOutButton; 
		TAudioPlayButton	*fPlayButton; 						
};

#endif
