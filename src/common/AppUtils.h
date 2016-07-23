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
__declspec(dllexport) int32		GetUnitMSec(int16 format, int16 resolution);
__declspec(dllexport) void 		TimeToString(uint32 mSecTime, int16 format, char str[256], bool stripLeadingZeros);
__declspec(dllexport) void 		GetTimeScaleString(int16 resolution, char *timeStr);
__declspec(dllexport) uint32	TimeToPixels(uint32 duration, int16 format, int16 resolution);
__declspec(dllexport) uint32	PixelsToTime(uint32 pixels, int16 format, int16 resolution);
__declspec(dllexport) uint32	ClipTime(uint32 theTime, int16 format, int16 resolution);
__declspec(dllexport) float 	GetFPSValue(timecode_type theType);

// Keyboard Utilities
__declspec(dllexport) bool 	IsKeyDown( char theKey);
__declspec(dllexport) bool 	IsShiftKeyDown();
__declspec(dllexport) bool 	IsCommandKeyDown();
__declspec(dllexport) bool 	IsOptionKeyDown();
__declspec(dllexport) bool 	IsControlKeyDown();

__declspec(dllexport) void 	AdjustScrollBar( BScrollBar *scrollBar, float page, float pageStep, float total, float start);

// Time Utilities
__declspec(dllexport) uint32 StartTime();
__declspec(dllexport) uint32 Duration();
__declspec(dllexport) uint32 GetCurrentTime();

__declspec(dllexport) int16 		GetCurrentResolution();
__declspec(dllexport) timecode_type 	GetCurrentTimeFormat();
__declspec(dllexport) bool 			IsPlaying();

// Cue Utilities
int SortCuesByChannelID( const void *itemOne, const void *itemTwo);

// Window Utilities
__declspec(dllexport) BWindow  *FindWindow(const char *title);
__declspec(dllexport) void 	CenterWindow(BWindow *theWindow);

//	Stage Utilities
__declspec(dllexport) void UpdateStage();

// Unit Utilities
bool DiffRect( BRect *rectOne, BRect *rectTwo, BRect *dstRect);

// Bitmap routines
__declspec(dllexport) void FillBitmap(BBitmap * map, uint32 color);
__declspec(dllexport) BBitmap *CloneBitmap(const BBitmap &input);

// File Utilities
__declspec(dllexport) bool IsSupportedType(BNodeInfo &nodeInfo);
__declspec(dllexport) bool IsImage(BNodeInfo &nodeInfo);
__declspec(dllexport) bool IsAudio(BNodeInfo &nodeInfo);
__declspec(dllexport) bool IsMIDI(BNodeInfo &nodeInfo);
__declspec(dllexport) bool IsVideo(BNodeInfo &nodeInfo);
__declspec(dllexport) bool IsText(BNodeInfo &nodeInfo);
__declspec(dllexport) bool IsCueSheet(BNodeInfo &nodeInfo);

// Thread Utilities
__declspec(dllexport) bool ValidThread( thread_id theThread);

#endif
