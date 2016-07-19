//---------------------------------------------------------------------
//
//	File:	TRefFilter.cpp
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.25.98
//
//	Desc:	General Purpose Ref Filter.  See header file for
//			types of filters
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

// Includes
#include "BuildApp.h"

#include <app/Application.h>
#include <assert.h>

#include "AppUtils.h"

#include "TRefFilter.h"
//#include <FilePanel.h>
//#include <sys/stat.h>

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

TRefFilter::TRefFilter(FilterType filterType) : BRefFilter()
{
	fFilterType = filterType;
}


// destructor

TRefFilter::~TRefFilter(void)
{
}

//---------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------
//
//

bool TRefFilter::Filter(const entry_ref *theRef, BNode *theNode,
					struct stat_beos *theStat, const char *mimetype)
{
	BNodeInfo nodeInfo(theNode);


	switch(fFilterType)
	{
		case kAudioFilter:
		{
			// Allow directories
			//ABH if (S_ISDIR(theStat->st_mode))
			//	return true;

			// Allow audio
			if (IsAudio(nodeInfo))
				return true;
		}
		break;

		case kImageFilter:
			{
				// Allow directories
			//	if (S_ISDIR(theStat->st_mode))
			//		return true;

				// Allow images
				if (IsImage(nodeInfo))
					return true;
			}
			break;

		case kTextFilter:
			{
				// Allow directories
			//	if (S_ISDIR(theStat->st_mode))
			//		return true;

				// Allow text
				if (IsText(nodeInfo))
					return true;
			}
			break;

		case kVideoFilter:
			{
				// Allow directories
			//	if (S_ISDIR(theStat->st_mode))
			//		return true;

				// Allow video
				if (IsVideo(nodeInfo))
					return true;
			}
			break;

		case kCueSheetFilter:
			{
				// Allow directories
			//	if (S_ISDIR(theStat->st_mode))
			//		return true;

				// Allow CueSheets
				if (IsCueSheet(nodeInfo))
					return true;
			}
			break;

		default:
			return true;
	}

	// Fail if we get here
	return false;
}




