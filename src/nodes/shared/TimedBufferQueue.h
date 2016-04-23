//	TimedBufferQueue.h
//	Copyright 1998 Be Incorporated. All rights reserved.

#if !defined(TIMED_BUFFER_QUEUE_H)
#define TIMED_BUFFER_QUEUE_H

struct _buffer_queue_imp;

#include <Buffer.h>
// ABH class BBuffer;

//	operations on this queue are thread safe (but successive operations
//	are not race condition safe)
class BTimedBufferQueue {

public:

		BTimedBufferQueue();
		~BTimedBufferQueue();

		status_t PushBuffer(					//	O(log N)
				BBuffer * buffer,
				bigtime_t time);
		BBuffer * PopFirstBuffer(				//	O(log N)
				bigtime_t * out_time);
		bool HasBuffers();						//	use may indicate race in user code
		bigtime_t TimeOfFirstBuffer();			//	use may indicate race in user code
		BBuffer * PeekFirstBufferAtOrAfter(		//	use may indicate race in user code
				bigtime_t time,
				bigtime_t * out_time);
		status_t RemoveBuffer(					//	O(N)
				BBuffer * buffer);
		status_t FlushBefore(					//	O(N log N)
				bigtime_t time);
		status_t FlushFromAndAfter(				//	O(N log N)
				bigtime_t time);

private:

		BTimedBufferQueue(		//	unimplemented
				const BTimedBufferQueue & other);
		BTimedBufferQueue & operator =(	//	unimplemented
				const BTimedBufferQueue & other);

		struct _buffer_queue_imp * m_queue;
		int32 _m_buffer_count;
		uint32 _m_reserved_timed_buffer_queue_[6];
};

#endif	//	TIMED_BUFFER_QUEUE_H
