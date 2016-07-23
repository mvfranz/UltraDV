//---------------------------------------------------------------------
//
//	File:	TAudioCaptureWindow.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.07.98
//
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#if !defined(__TAUDIOCAPTUREWINDOW_H__)
#define __TAUDIOCAPTUREWINDOW_H__

// Includes
#include "TRecordPrefsDialog.h"

// Constants
const float kAudCapToolbarHeight = 23;
const float kAudCapViewWidth 	 = 319;
const float kAudCapViewHeight	 = 239;

// Forward Declaration
class TAudioCaptureToolbar;
class TAudioLevelsView;
class TAudioCaptureWindow;
class TStatusView;
class TSoundRecorder;

// Structures
typedef struct
{
	CaptureMethod		captureMethod;
	CompressMethod		compressMethod;
	BMallocIO 			*audioBuf;
	BFile				*audioFile;
	TAudioCaptureWindow	*audioWindow;
} AudCapStruct, *AudCapPtr;

// Class definitions
class TAudioCaptureWindow: public BWindow
{
	public:
		TAudioCaptureWindow(BMessage *theMessage);
		~TAudioCaptureWindow();
		
		void MessageReceived(BMessage* message);	
		
		// Accessor Functions
		inline bool 		IsRecording(){ return fIsRecording; }
		inline void 		AbortRecording(){ fIsRecording = false; }
		inline AudCapPtr 	GetCaptureSettings(){ return &fCaptureSettings; }
		inline TStatusView	*GetStatusView(){ return fStatusView; }
		
		inline BStringView	*GetFileSize(){ return fFileSize; }
		inline BStringView	*GetSpaceRemaining(){ return fSpaceRemaining; }		
		inline entry_ref    *GetFileRef(){ return &fFileRef; }
		
		inline	TSoundRecorder *RecordNode(){ return fRecordNode; }

		
	private:	
		// Member Functions
		void	Init();	
		
		void	StartRecording();
		void	StopRecording();
		
		
		void 	SetupRecordNode( BMediaNode *recorder, media_input &input,
								media_output &output, BTimeSource *&timesource);
		void 	StartRecordNode( BMediaNode *node, media_input &input,
   								media_output & output, BTimeSource *timesource);
		void 	StopRecordNode();

		
		void	SelectAudioFile(BMessage *theMessage);
		void 	ShowPanel();
		void 	HidePanel();
		
		void 	CreateTempFile();
		
		// Member Variables
		TAudioCaptureToolbar *fToolbar;
		BView 				*fAudCapView;		
		TStatusView			*fStatusView;
		BStringView			*fFileSize;
		BStringView			*fSpaceRemaining;		
		BStringView			*fTimeRemaining;		
		TAudioLevelsView	*fLevelsView;
		
		bool				fIsRecording;						
		thread_id			fUpdaterThread;	
		AudCapStruct		fCaptureSettings;
		
		BFilePanel			*fPanel;
		entry_ref			fFileRef;
		
		TSoundRecorder 		*fRecordNode;
		media_input 		fInput;
		media_output 		fOutput;
		BTimeSource 		*fTimesource;
};

// Capture Messages
#define	AUDCAP_RECORD_MSG	'acRM'
#define	AUDCAP_FILE_MSG		'acFM'
#define	AUDCAP_PANEL_MSG	'acPM'
#define	AUDCAP_DISK_MSG		'acDK'
#define	AUDCAP_RAM_MSG		'acMM'
#define	AUDCAP_POST_MSG		'acPC'

#endif
