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
		inline bool 		IsRecording(){ return m_IsRecording; }
		inline void 		AbortRecording(){ m_IsRecording = false; }
		inline AudCapPtr 	GetCaptureSettings(){ return &m_CaptureSettings; }
		inline TStatusView	*GetStatusView(){ return m_StatusView; }
		
		inline BStringView	*GetFileSize(){ return m_FileSize; }
		inline BStringView	*GetSpaceRemaining(){ return m_SpaceRemaining; }		
		inline entry_ref    *GetFileRef(){ return &m_FileRef; }
		
		inline	TSoundRecorder *RecordNode(){ return m_RecordNode; }

		
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
		TAudioCaptureToolbar *m_Toolbar;
		BView 				*m_AudCapView;		
		TStatusView			*m_StatusView;
		BStringView			*m_FileSize;
		BStringView			*m_SpaceRemaining;		
		BStringView			*m_TimeRemaining;		
		TAudioLevelsView	*m_LevelsView;
		
		bool				m_IsRecording;						
		thread_id			m_UpdaterThread;	
		AudCapStruct		m_CaptureSettings;
		
		BFilePanel			*m_Panel;
		entry_ref			m_FileRef;
		
		TSoundRecorder 		*m_RecordNode;
		media_input 		m_Input;
		media_output 		m_Output;
		BTimeSource 		*m_Timesource;
};

// Capture Messages
#define	AUDCAP_RECORD_MSG	'acRM'
#define	AUDCAP_FILE_MSG		'acFM'
#define	AUDCAP_PANEL_MSG	'acPM'
#define	AUDCAP_DISK_MSG		'acDK'
#define	AUDCAP_RAM_MSG		'acMM'
#define	AUDCAP_POST_MSG		'acPC'

#endif
