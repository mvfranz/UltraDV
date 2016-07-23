//---------------------------------------------------------------------
//
//	File:	TRefFilter.h
//
//	Author:	Gene Z. Ragan
//
//	Date:	05.25.98
//
//
//	Copyright ©1998 mediapede Software
//
//---------------------------------------------------------------------

#ifndef __TREFFILTER_H__
#define __TREFFILTER_H__

#include <FilePanel.h>

typedef enum 
{
	kAudioFilter,
	kImageFilter,
	kTextFilter,
	kVideoFilter,
	kCueSheetFilter
} FilterType;

// Class Definition



class TRefFilter : public BRefFilter 
{
	public:
		TRefFilter(FilterType filterType);
		virtual ~TRefFilter(void);
		
//	bool Filter(const entry_ref *, BNode *, struct stat_beos *, const char *);
//	bool Filter(const entry_ref *, BNode *, struct stat_beos *, const char *mimetype);
	bool Filter(const entry_ref* ref, BNode* node, struct stat_beos* stat, const char* mimeType);

		
	private:
		FilterType fFilterType;

};

#endif
