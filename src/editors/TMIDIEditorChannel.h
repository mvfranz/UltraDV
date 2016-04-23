//-------------------------------------------------------------------
//
//	File:	TMIDIEditorChannel.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.23.98
//
//-------------------------------------------------------------------

#ifndef __TMIDIEditorChannel__
#define __TMIDIEditorChannel__ 

// Forward Declarations
class TMIDIEditor;

// Class Definition
class TMIDIEditorChannel: public BView
{
	public:
		TMIDIEditorChannel(TMIDIEditor *parent, BRect bounds);
		~TMIDIEditorChannel();
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
	
		// Member Variables		
		TMIDIEditor	*m_Parent;		
		
		// Member Functions
								
};

#endif
