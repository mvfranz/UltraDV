
#include "TSoundRecorder.h"
#include <OS.h>
#include <stdio.h>
#include <Buffer.h>
#include <TimeSource.h>

#define NDEBUG 1

#if !NDEBUG
#define FPRINTF fprintf
#else
#define FPRINTF (void)
#endif

#define NODE FPRINTF
#define MESSAGE FPRINTF

TSoundRecorder::TSoundRecorder(
	const char * name,
	void (*RecordFunc)(void * cookie, bigtime_t timestamp, const void * data, size_t size, const media_raw_audio_format & format),
	void (*NotifyFunc)(void * cookie, int32 cause, ...),
	void * cookie) :
	BMediaNode(name ? name : "Video Sound Recording"),
	BBufferConsumer(B_MEDIA_RAW_AUDIO)
{
	NODE(stderr, "SoundRecordNode::SoundRecordNode(%x, %x, %x, %x)\n",
		name, RecordFunc, NotifyFunc, cookie);
	if (!name) name = "Audio Input";
	_mRecordHook = RecordFunc;
	_mNotifyHook = NotifyFunc;
	_mCookie = cookie;
	char pname[32];
	sprintf(pname, "%.20s Control", name);
	_mPort = create_port(10, pname);
	_mInput.destination.port = _mPort;
	_mInput.destination.id = 1;
	sprintf(pname, "%.20s Service", name);
	_mThread = spawn_thread(ThreadEntry, pname, 110, this);
	_mTimeout = 1000000LL;
	resume_thread(_mThread);
	sprintf(_mInput.name, "%.20s Input", name);
}


TSoundRecorder::~TSoundRecorder()
{
	NODE(stderr, "SoundRecordNode::~SoundRecordNode()\n");
	write_port(_mPort, 0x60000000L, NULL, 0);
	status_t s;
	wait_for_thread(_mThread, &s);
	delete_port(_mPort);
}


port_id TSoundRecorder::ControlPort() const
{
	return _mPort;
}


BMediaAddOn* TSoundRecorder::AddOn(
	int32 * internal_id) const
{
	if (internal_id) *internal_id = 0;
	return NULL;
}


void TSoundRecorder::Start(
	bigtime_t performance_time)
{
	if (_mNotifyHook) {
		(*_mNotifyHook)(_mCookie, B_WILL_START, performance_time);
	}
	else {
		Notify(B_WILL_START, performance_time);
	}
}


void TSoundRecorder::Stop(
	bigtime_t performance_time,
	bool immediate)
{
	if (_mNotifyHook) {
		(*_mNotifyHook)(_mCookie, B_WILL_STOP, performance_time, immediate);
	}
	else {
		Notify(B_WILL_STOP, performance_time, immediate);
	}
}


void TSoundRecorder::Seek(
	bigtime_t media_time,
	bigtime_t performance_time)
{
	if (_mNotifyHook) {
		(*_mNotifyHook)(_mCookie, B_WILL_SEEK, performance_time, media_time);
	}
	else {
		Notify(B_WILL_SEEK, performance_time, media_time);
	}
}


void TSoundRecorder::SetRunMode(
	run_mode mode)
{
	&mode;
}


void TSoundRecorder::TimeWarp(
	bigtime_t at_real_time,
	bigtime_t to_performance_time)
{
	if (_mNotifyHook) {
		(*_mNotifyHook)(_mCookie, B_WILL_TIMEWARP, at_real_time, to_performance_time);
	}
	else {
		Notify(B_WILL_TIMEWARP, at_real_time, to_performance_time);
	}
}


void TSoundRecorder::Preroll()
{
}


void TSoundRecorder::SetTimeSource(
	BTimeSource * time_source)
{
	&time_source;
}


status_t TSoundRecorder::HandleMessage(
	int32 message,
	const void * data,
	size_t size)
{
	if ((BMediaNode::HandleMessage(message, data, size) < 0) &&
		(BBufferConsumer::HandleMessage(message, data, size) < 0)) {
		HandleBadMessage(message, data, size);
		return B_ERROR;
	}
	return B_OK;
}



status_t TSoundRecorder::AcceptFormat(
	const media_destination & dest,
	media_format * format)
{
	if (dest != _mInput.destination) {
		return B_MEDIA_BAD_DESTINATION;
	}
	if (format->type <= 0) {
		format->type = B_MEDIA_RAW_AUDIO;
		format->u.raw_audio = media_raw_audio_format::wildcard;
	}
	else if (format->type != B_MEDIA_RAW_AUDIO) {
		format->type = B_MEDIA_RAW_AUDIO;
		format->u.raw_audio = media_raw_audio_format::wildcard;
		return B_MEDIA_BAD_FORMAT;
	}
	return B_OK;
}


status_t TSoundRecorder::GetNextInput(
	int32 * cookie,
	media_input * out_input)
{
	NODE(stderr, "SoundRecordNode: GetNextInput()\n");
	if (!*cookie) {
		if (_mInput.source == media_source::null) {
			_mInput.format.type = B_MEDIA_RAW_AUDIO;
			_mInput.format.u.raw_audio = media_raw_audio_format::wildcard;
			_mInput.node = Node();
			_mInput.destination.port = ControlPort();
			_mInput.destination.id = 1;
		}
		*out_input = _mInput;
		*cookie = 1;
		return B_OK;
	}
	return B_BAD_INDEX;
}


void TSoundRecorder::DisposeInputCookie(
	int32 cookie)
{
	&cookie;
}


void TSoundRecorder::BufferReceived(
	BBuffer * buffer)
{
	NODE(stderr, "SoundRecordNode::BufferReceived()\n");
	if (_mRecordHook) {
		(*_mRecordHook)(_mCookie, buffer->Header()->start_time, buffer->Data(), buffer->Header()->size_used, _mInput.format.u.raw_audio);
	}
	buffer->Recycle();
}


void TSoundRecorder::ProducerDataStatus(
	const media_destination & for_whom,
	int32 status,
	bigtime_t at_media_time)
{
	if (for_whom == _mInput.destination) {
		if (_mNotifyHook) {
			(*_mNotifyHook)(_mCookie, B_PRODUCER_DATA_STATUS, status, at_media_time);
		}
		else {
			Notify(B_PRODUCER_DATA_STATUS, status, at_media_time);
		}
	}
}


status_t TSoundRecorder::GetLatencyFor(
	const media_destination & for_whom,
	bigtime_t * out_latency,
	media_node_id * out_timesource)
{
	if (for_whom != _mInput.destination) {
		return B_MEDIA_BAD_DESTINATION;
	}
	*out_latency = Latency();
	*out_timesource = TimeSource()->Node().node;
	return B_OK;
}


status_t TSoundRecorder::Connected(
	const media_source & producer,	/* here's a good place to request buffer group usage */
	const media_destination & where,
	const media_format & with_format,
	media_input * out_input)
{
	NODE(stderr, "SoundRecordNode::Connected()\n");
	if (_mInput.source != media_source::null) {
		return B_MEDIA_BAD_DESTINATION;	//	busy
	}
	if (where != _mInput.destination) {
		return B_MEDIA_BAD_DESTINATION;
	}
	_mInput.source = producer;
	_mInput.format = with_format;
	if (_mNotifyHook) {
		(*_mNotifyHook)(_mCookie, B_CONNECTED, _mInput.name);
	}
	else {
		Notify(B_CONNECTED, _mInput.name);
	}
	*out_input = _mInput;
	return B_OK;
}


void TSoundRecorder::Disconnected(
	const media_source & producer,
	const media_destination & where)
{
	if (where != _mInput.destination) {
		return;
	}
	if (producer != _mInput.source) {
		return;
	}
	if (_mNotifyHook) {
		(*_mNotifyHook)(_mCookie, B_DISCONNECTED);
	}
	else {
		Notify(B_DISCONNECTED);
	}
	_mInput.source = media_source::null;
}


status_t TSoundRecorder::FormatChanged(
	const media_source & producer,
	const media_destination & consumer, 
	int32 frofchange_count,
	const media_format & format)
{
	NODE(stderr, "SoundRecordNode::Connected()\n");
	media_format fmt(format);
	status_t err = AcceptFormat(_mInput.destination, &fmt);
	if (err >= B_OK) {
		_mInput.format = format;
		if (_mNotifyHook) {
			(*_mNotifyHook)(_mCookie, B_FORMAT_CHANGED, &_mInput.format.u.raw_audio);
		}
		else {
			Notify(B_FORMAT_CHANGED, &_mInput.format.u.raw_audio);
		}
	}
	return err;
}


status_t
TSoundRecorder::ThreadEntry(
	void * data)
{
	((TSoundRecorder *)data)->ServiceThread();
	return 0;
}

	struct set_hooks_q {
		port_id reply;
		void * cookie;
		void (*record)(void *, bigtime_t, const void *, size_t, const media_raw_audio_format &);
		void (*notify)(void *, int32, ...);
	};

status_t
TSoundRecorder::SetHooks(
	void (*RecordFunc)(void * cookie, bigtime_t timestamp, const void * data, size_t size, const media_raw_audio_format & format),
	void (*NotifyFunc)(void * cookie, int32 code, ...),
	void * cookie)
{
	set_hooks_q cmd;
	cmd.record = RecordFunc;
	cmd.notify = NotifyFunc;
	cmd.cookie = cookie;
	cmd.reply = create_port(1, "SetHooks reply");
	status_t err = write_port(ControlPort(), 0x60000001L, &cmd, sizeof(cmd));
	if (err >= 0) {
		int32 code;
		err = read_port_etc(cmd.reply, &code, NULL, 0, B_TIMEOUT, 6000000LL);
		if (err > 0) err = 0;
		NODE(stderr, "SoundRecordNode::SetHooks read reply: %x\n", err);
	}
	delete_port(cmd.reply);
	return err;
}



void
TSoundRecorder::ServiceThread()
{
	char * msg = new char[B_MEDIA_MESSAGE_SIZE];
	int bad = 0;
	while (true) {
		int32 code = 0;
		bigtime_t timeout = Timeout();
		status_t err = read_port_etc(_mPort, &code, msg, B_MEDIA_MESSAGE_SIZE,
			B_TIMEOUT, timeout);
		MESSAGE(stderr, "SoundRecordNode::ServiceThread() port %d message %x\n", _mPort, code);
		if (err >= 0) {
			_mTimeout = 0;
			bad = 0;
			if (code == 0x60000000L) {
				if (_mNotifyHook) {
					(*_mNotifyHook)(_mCookie, B_NODE_DIES, 0);
				}
				else {
					Notify(B_NODE_DIES, 0);
				}
				break;
			}
			else if (code == 0x60000001L) {
				if (_mNotifyHook) {
					(*_mNotifyHook)(_mCookie, B_HOOKS_CHANGED);
				}
				else {
					Notify(B_HOOKS_CHANGED);
				}
				set_hooks_q * ptr = (set_hooks_q *)msg;
				_mRecordHook = ptr->record;
				_mNotifyHook = ptr->notify;
				_mCookie = ptr->cookie;
				write_port(ptr->reply, 0, NULL, 0);
			}
			else {
				HandleMessage(code, msg, err);
			}
		}
		else if (err == B_TIMED_OUT) {
			if (_mNotifyHook) {
				(*_mNotifyHook)(_mCookie, B_OP_TIMED_OUT, timeout);
			}
			else {
				Notify(B_OP_TIMED_OUT, timeout);
			}
		}
		else {
			FPRINTF(stderr, "SoundRecordNode: error %x\n", err);
			bad++;
			if (bad > 3) {
				if (_mNotifyHook) {
					(*_mNotifyHook)(_mCookie, B_NODE_DIES, bad, err, code, msg);
				}
				else {
					Notify(B_NODE_DIES, bad, err, code, msg);
				}
				break;
			}
		}
	}
	delete[] msg;
}

bigtime_t
TSoundRecorder::Timeout()
{
	_mTimeout = (_mTimeout < 1000000) ? 1000000 : _mTimeout*2;
	return _mTimeout;
}

bigtime_t
TSoundRecorder::Latency()
{
	return 2000LL;
}

void
TSoundRecorder::Record(
	bigtime_t time,
	const void * data,
	size_t size,
	const media_raw_audio_format & format)
{
	//	do nothing
	&time;
	&data;
	&size;
	&format;
}

void
TSoundRecorder::Notify(
	int32 cause,
	...)
{
	//	do nothing
	&cause;
}





