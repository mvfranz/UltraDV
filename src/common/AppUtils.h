//---------------------------------------------------------------------
//
//	File:	AppUtils.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	01.26.98
//
//	Desc:	Misc utility routines
//
//	Copyright ©1998 mediapede software
//
//---------------------------------------------------------------------

#ifndef __APPUTILS_H__
#define __APPUTILS_H__

#include <TimeCode.h>

#include "AppTypes.h"
#include "TCursor.h"

// Function protoypes
int32		GetUnitMSec(int16 format, int16 resolution);
void 		TimeToString(uint32 mSecTime, int16 format, char str[256], bool stripLeadingZeros);
void 		GetTimeScaleString(int16 resolution, char *timeStr);
uint32	TimeToPixels(uint32 duration, int16 format, int16 resolution);
uint32	PixelsToTime(uint32 pixels, int16 format, int16 resolution);
uint32	ClipTime(uint32 theTime, int16 format, int16 resolution);
float 	GetFPSValue(timecode_type theType);

// Keyboard Utilities
bool 	IsKeyDown( char theKey);
bool 	IsShiftKeyDown();
bool 	IsCommandKeyDown();
bool 	IsOptionKeyDown();
bool 	IsControlKeyDown();

void 	AdjustScrollBar( BScrollBar *scrollBar, float page, float pageStep, float total, float start);

// Time Utilities
uint32 StartTime();
uint32 Duration();
uint32 GetCurrentTime();

int16 		GetCurrentResolution();
timecode_type 	GetCurrentTimeFormat();
bool 			IsPlaying();

// Cue Utilities
int SortCuesByChannelID( const void *itemOne, const void *itemTwo);

// Window Utilities
BWindow  *FindWindow(const char *title);
void 	CenterWindow(BWindow *theWindow);

//	Stage Utilities
void UpdateStage();

// Unit Utilities
bool DiffRect( BRect *rectOne, BRect *rectTwo, BRect *dstRect);

// Bitmap routines
void FillBitmap(BBitmap * map, uint32 color);
BBitmap *CloneBitmap(const BBitmap &input);

// File Utilities
bool IsSupportedType(BNodeInfo &nodeInfo);
bool IsImage(BNodeInfo &nodeInfo);
bool IsAudio(BNodeInfo &nodeInfo);
bool IsMIDI(BNodeInfo &nodeInfo);
bool IsVideo(BNodeInfo &nodeInfo);
bool IsText(BNodeInfo &nodeInfo);
bool IsCueSheet(BNodeInfo &nodeInfo);

// Thread Utilities
bool ValidThread( thread_id theThread);

#endif
