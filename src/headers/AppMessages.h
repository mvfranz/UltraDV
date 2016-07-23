//-------------------------------------------------------------------
//
//	File:	AppMessages.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//-------------------------------------------------------------------

#ifndef __APPMESSAGES_H__
#define __APPMESSAGES_H__

// Generic Dialog Messages
#define OK_MSG					'ok__'
#define CANCEL_MSG				'cncl'
#define APPLY_MSG				'aply'

// Channel Messages
#define CHANNEL_DRAG_MSG		'chdm'

// Cue Icon Message
#define CUE_ICON_DRAG_MSG		'cidm'
#define CUE_ICON_DRAG_IN_MSG 	'cidi'
#define CUE_ICON_DRAG_OUT_MSG 	'cido'
#define CUE_DRAG_MSG 			'cued'


// Mute-Solo button message
#define MUTE_BUTTON_MSG			'mute'
#define SOLO_BUTTON_MSG			'solo'

// Channel Expand button message
#define CHANNEL_EXPAND_MSG		'expd'
#define CUE_EXPAND_MSG			'cxpd'

// Lock/Unlock Message
#define LOCK_CHANNEL_MSG		'luch'
#define LOCK_CUE_MSG			'lcum'


// Cue Sheet Toolbar Messages
#define EXPAND_ALL_BUTTON_MSG	'expa'
#define CONTRACT_ALL_BUTTON_MSG	'cnta'


// Transport Button Messages
#define START_BUTTON_MSG		'sttm'
#define REWIND_BUTTON_MSG		'rwnd'
#define PLAY_BUTTON_MSG			'play'
#define PAUSE_BUTTON_MSG		'pase'
#define STOP_BUTTON_MSG			'stop'
#define FF_BUTTON_MSG			'ffbm'
#define END_BUTTON_MSG			'endb'
#define RECORD_BUTTON_MSG		'rcrd'

// Audio Editor Messages
#define UPDATE_AUDIOTIMELINE_MSG 'UPat'
#define UPDATE_AUDIOINDICATOR_MSG 'UPai'

// Audio Editor Button Messages
#define AUDIO_PLAY_BUTTON_MSG		'AEpl'
#define AUDIO_STOP_BUTTON_MSG		'AEst'
#define AUDIO_ZOOMIN_BUTTON_MSG	'AEzi'
#define AUDIO_ZOOMOUT_BUTTON_MSG	'AEzo'

// MIDI Playback Messages
#define MIDI_PLAYBACK_DONE_MSG		'MPdn'

// Ask all windows (except stage) to come to the front
#define ALL_FRONT_MSG				'awfm'
#define	END_STAGE_EDIT_MSG			'eSTe'

// Notify time palette of new time
#define TIMELINE_DRAG_MSG 			'tmdg'
#define UPDATE_TIMELINE_MSG 		'updt'
#define NEW_TIME_MSG 				'newt'
#define	TIMEFORMAT_CHANGED_MSG		'tfch'

// Resolution change
#define RESOLUTION_CHANGE_MSG		'resc'

// Position indicator change
#define UPDATE_INDICATOR_MSG		'dnpi'

// Playback timer messages
#define PLAYBACK_TIMER_MSG			'PLtm'
#define	TRANSITION_IN_EVENT			'Tiev'
#define PATH_IN_EVENT				'Piev'
#define IN_EVENT					'Ciev'
#define	TRANSITION_OUT_EVENT		'Toev'
#define PATH_OUT_EVENT				'Poev'
#define OUT_EVENT					'Coev'
#define	PAUSE_EVENT					'Paev'

// Channel Insertion
#define INSERT_CHANNEL_MSG			'InCm'
#define DELETE_CHANNEL_MSG			'DlCm'

// Marching Ants
#define MARCH_ANTS_MSG				'Marc'

// Cue Button Messages
#define SHOW_BUTTON_MSG				'ShBm'
#define LOCK_CUE_BUTTON_MSG			'LcBm'
#define TRANSITION_BUTTON_MSG		'TrBm'
#define PATH_BUTTON_MSG				'PtBm'
#define CHANGE_ICON_MSG				'CChI'


// Palette Closing Messages
#define ACTION_PALETTE_CLOSE_MSG	'AcPc'	
#define MEDIA_PALETTE_CLOSE_MSG		'MePc'	
#define TIME_PALETTE_CLOSE_MSG		'TpPc'	
#define TRANSPORT_PALETTE_CLOSE_MSG 'TrPc'	


// Time Text View Messages
#define START_TIME_TEXT_FOCUS_MSG	'STVm'
#define END_TIME_TEXT_FOCUS_MSG		'ETVm'	

#define TRANSITION_IN_TEXT_FOCUS_MSG	'TITm'
#define TRANSITION_OUT_TEXT_FOCUS_MSG	'TOTm'

// Menu update messages
#define	FIX_WINDOWS_MENU_MSG		'FWmg'

// Clipboard handling messages
#define CUE_PASTE_MSG				'cPmg'
#define CUE_LIST_MSG				'cLmg'

// New Project
#define NEW_PROJECT_MSG				'NPmg'

// Browser Messages
#define ADD_REF_MSG					'ARmg'
#define	BROWSER_ICON_DRAG_MSG		'BRdm'

#endif

