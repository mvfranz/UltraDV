//---------------------------------------------------------------------
//
//	File:	TRecordPrefsDialog.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	06.09.98
//
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __TRECORDPREFSDIALOG_H__
#define __TRECORDPREFSDIALOG_H__

// Constants

// Forward Declaration

typedef enum 
{
	kPostCompress,
	kRealTimeCompress
} CompressMethod;

typedef enum 
{
	kCaptureToRAM,
	kCaptureToDisk
} CaptureMethod;

// Class definitions
class TRecordPrefsDialog: public BWindow
{
	public:
		TRecordPrefsDialog(BMessage *theMessage);
		~TRecordPrefsDialog();
		
		virtual void	MessageReceived(BMessage* message);	
		
		
	private:	
		// Member Functions
		void	Init();	
		
	
		// Member Variables
		BView 			*fPrefsView;		
		BRadioButton	*fCaptureToDiskRadio;
		BRadioButton	*fCaptureToRAMRadio;		
		BMenuField		*fSwapDiskMenu;
		BCheckBox 		*fPostCompressCheckbox;
		BList 			fVolumeList;
						
		
};

// Capture Messages
#define	RECORD_DISK_MSG	'rpDM'
#define	RECORD_RAM_MSG	'rpRM'
#define	RECORD_POST_MSG	'rpPM'

#define SWAP_BOOT_MSG	'swBM'

#endif
