//---------------------------------------------------------------------
//
//	File:	TAudioSettingsTabView.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.20.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------


#ifndef __TAUDIOSETTINGSTABVIEW_H__
#define __TAUDIOSETTINGSTABVIEW_H__

// Forward Declarations
class TAudioCompressionView;
class TAudioSampleView;
class TAudioSourceView;
class TAudioSettingsView;

// Class Definition
class TAudioSettingsTabView : public BTabView 
{
	public:
		// Member Functions
		TAudioSettingsTabView(TAudioSettingsView *parent, BRect frame, const char *name);
		TAudioSettingsTabView(BMessage*);
		~TAudioSettingsTabView();	
								
		static	BArchivable*	Instantiate(BMessage*);
		status_t				Archive(BMessage*, bool deep=true) const;
		
		void 		WindowActivated(bool state);
		void 		MessageReceived(BMessage *msg);
		void		MouseDown(BPoint);
		void		MouseUp(BPoint);
		void 		MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
		void 		Draw(BRect);
	
		// Inlines
		inline TAudioSettingsView *GetParent(){ return fParent; }
		
	private:
		// Member Functions
		void Init();		
		
		// Member Variables
		TAudioSettingsView		*fParent;
		TAudioCompressionView 	*fCompressionView;
		TAudioSampleView 		*fSampleView;
		TAudioSourceView 		*fSourceView;
};

#endif
