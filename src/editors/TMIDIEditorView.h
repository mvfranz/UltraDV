//-------------------------------------------------------------------
//
//	File:	TMIDIEditorView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
//-------------------------------------------------------------------

#ifndef __TMIDIEditorView__
#define __TMIDIEditorView__ 

// Forward Declarations
class TMIDIEditor;

// Class Definition
class TMIDIEditorView: public BView
{
	public:
		TMIDIEditorView(TMIDIEditor *parent, BRect bounds);
		~TMIDIEditorView();
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
		void	Init();
	
		// Member Variables		
		TMIDIEditor	*fEditor;		
		
		// Member Functions
								
};

#endif
