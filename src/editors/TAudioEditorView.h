//-------------------------------------------------------------------
//
//	File:	TAudioEditorView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	02.19.98
//
//-------------------------------------------------------------------

#ifndef __TAUDIOEDITORVIEW_H__
#define __TAUDIOEDITORVIEW_H__

// Forward Declarations
class TAudioEditor;

// Class Definition
class TAudioEditorView: public BView
{
	public:
		TAudioEditorView(TAudioEditor *parent, BRect bounds);
		~TAudioEditorView();
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
		
		void SetAudioViewBounds();
		
	private:
		
		// Member Functions
		void 	Clip16Bit(int32 *theValue);
		void 	DrawIndicatorTick(BPoint drawPt);
		void 	CreateWaveFormCache(BView *previewView, const BRect bounds);
				
		int32 	SamplesToPixels(int32 theSamples);
		int32 	PixelsToSamples(int32 thePixels);
		
		void	ZoomIn();
		void	ZoomOut();
				
		// Member Variables		
		TAudioEditor	*m_Parent;		
		BPoint			m_TickPt;
		BPoint			m_OldTickPt;
		
		bool			m_UpdatePreview;
		BBitmap 		*m_PreviewBitmap;	
		
		// Member Functions
		//void			UpdateNumSampleFrames(int32 delta);
		//void			SetSelection(int32 aSelStart, int32 aSelEnd);
		//int32			SnapToZero(int32 aSelection);
								
};

#endif
