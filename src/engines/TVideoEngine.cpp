/*
 * Copyright 2016, Dario Casalinuovo, <b.vitruvio@gmail.com>.
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "BuildApp.h"

#include "AppUtils.h"
#include "TCueChannel.h"
#include "TCueSheetWindow.h"
#include "TCueView.h"
#include "TStageView.h"
#include "TStageWindow.h"
#include "TVideoEngine.h"
#include "TVisualCue.h"


TVideoEngine::TVideoEngine(TCueSheetView* cueSheetView)
	:
	fStarted(false),
	fStageThread(-1),
	fCueSheetView(cueSheetView),
	fLastTime(0)
{
	fStageThread = spawn_thread(stage_thread,
		"VideoEngine", B_REAL_TIME_PRIORITY, this);
	resume_thread(fStageThread);
}


TVideoEngine::~TVideoEngine()
{

}


void
TVideoEngine::Start(bigtime_t performanceTime)
{
	fStarted = true;
}


void
TVideoEngine::Stop(bigtime_t performanceTime)
{
	fStarted = false;
}


void
TVideoEngine::Seek(bigtime_t performanceTime)
{

}


bool
TVideoEngine::IsStarted() const
{
	return fStarted;
}


status_t
TVideoEngine::stage_thread(void* data)
{
	((TVideoEngine*)data)->StageThread();

	return B_OK;
}


void
TVideoEngine::StageThread()
{
	TStageView* stageView = fCueSheetView->GetParent()->GetStage()->GetStageView();
	float fps = GetFPSValue(GetCurrentTimeFormat());
	uint32 frameTime = 1000/fps;

	while (true) {
		if (fStarted) {
			uint32 newTime = fCueSheetView->GetCurrentTime() + frameTime;

			fCueSheetView->SetCurrentTime(newTime);

			BList* channelList = fCueSheetView->GetChannelList();
			if (stageView) {
				if (stageView->LockLooper()) {
					stageView->UpdateStage(stageView->Bounds(), newTime);
					stageView->UnlockLooper();
				}
				snooze(1000000/GetFPSValue(GetCurrentTimeFormat()));
			}
		} else {
			snooze(100000);
		}
	}
}
