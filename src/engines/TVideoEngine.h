/*
 * Copyright 2016, Dario Casalinuovo, <b.vitruvio@gmail.com>.
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef _TVIDEOENGINE_H_
#define _TVIDEOENGINE_H_

#include <SupportDefs.h>

#include "TCueSheetView.h"


class TVideoEngine
{
public:
					TVideoEngine(TCueSheetView* cueSheetView);
					~TVideoEngine();

	void			Start(bigtime_t performanceTime);
	void			Stop(bigtime_t performanceTime);
	void			Seek(bigtime_t performanceTime);
	bool			IsStarted() const;

private:
	static status_t stage_thread(void* data);
	void			StageThread();

	bool			fStarted;
	thread_id		fStageThread;
	TCueSheetView*	fCueSheetView;
	bigtime_t		fLastTime;
};

#endif	// _TVIDEOENGINE_H_
