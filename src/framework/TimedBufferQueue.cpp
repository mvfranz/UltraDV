//
//
// TimedBufferQueue.cpp
//
// ABH would not compile, as provided

#include <OS.h>
using namespace std;


#include <map> //ABH file not found

#include <string>
#include "TimedBufferQueue.h"

#ifdef ABH
#include "/boot/develop/abi/x86/gcc4/tools/gcc-4.6.3-haiku-121101/include/c++/4.6.3/bits/stl_multimap.h"// ABH
//#if defined(__GNUC__) && defined(__INTEL__)
//#include <stl_multimap.h>
#ifdef GCC4
//#include "/boot/develop/abi/x86/gcc4/tools/gcc-4.6.3-haiku-121101/include/c++/4.6.3/bits/multimap.h"
#elsif
#include <multimap.h>
#endif
#endif

//#define assert(X)
//#include "realtime_allocator.h"

// ABH typedef multimap<bigtime_t, BBuffer *, less<bigtime_t>, realtime_allocator<BBuffer *, realtime_blocks_256> > buffer_map;


//#else
//#include <map>
typedef multimap<bigtime_t, BBuffer*> buffer_map;
//#endif


class _buffer_queue_imp {
public:

buffer_map fbuffers;
int32 flock_count;
int32 flock_sem;

_buffer_queue_imp()
{
	flock_count = 1;
	flock_sem = create_sem(0, "_buffer_queue_imp");
}
~_buffer_queue_imp()
{
	delete_sem(flock_sem);
}
bool lock()
{
	if (atomic_add(&flock_count, -1) < 1) {
		status_t err = acquire_sem(flock_sem);
		if (err < B_OK) return false;
	}
	return true;
}
void unlock()
{
	if (atomic_add(&flock_count, 1) < 0) {
		release_sem(flock_sem);
	}
}
};


BTimedBufferQueue::BTimedBufferQueue()
{
	fqueue = new _buffer_queue_imp;
	_fbuffer_count = 0;
}

BTimedBufferQueue::~BTimedBufferQueue()
{
	delete fqueue;
}


status_t
BTimedBufferQueue::PushBuffer(
	BBuffer* buffer,
	bigtime_t time)
{
	if (!fqueue->lock()) return B_ERROR;
	try {
		fqueue->fbuffers.insert(buffer_map::value_type(time, buffer));
		atomic_add(&_fbuffer_count, 1);
		fqueue->unlock();
	}
	catch (...) {
		fqueue->unlock();
		return B_ERROR;
	}
	return B_OK;
}

BBuffer*
BTimedBufferQueue::PopFirstBuffer(
	bigtime_t* out_time)
{
	BBuffer* buf = 0;
	if (!fqueue->lock()) return 0;
	try {
		buffer_map::iterator i(fqueue->fbuffers.begin());
		if (i != fqueue->fbuffers.end()) {
			buf = (*i).second;
			if (out_time) {
				*out_time = (*i).first;
			}
			atomic_add(&_fbuffer_count, -1);
			fqueue->fbuffers.erase(i);
		}
		fqueue->unlock();
	}
	catch (...) {
		fqueue->unlock();
	}
	return buf;
}

bool
BTimedBufferQueue::HasBuffers()
{
#if 1
	return _fbuffer_count > 0;
#else
	if (!fqueue->lock()) return false;
	bool ret = fqueue->fbuffers.size() > 0;
	fqueue->unlock();
	return ret;
#endif
}

bigtime_t
BTimedBufferQueue::TimeOfFirstBuffer()
{
	if (!fqueue->lock()) return false;
	bigtime_t ret = 0;
	buffer_map::iterator i(fqueue->fbuffers.begin());
	if (i != fqueue->fbuffers.end()) {
		ret = (*i).first;
	}
	fqueue->unlock();
	return ret;
}

BBuffer*
BTimedBufferQueue::PeekFirstBufferAtOrAfter(
	bigtime_t time,
	bigtime_t* out_time)
{
	BBuffer* buf = 0;
	if (!fqueue->lock()) return 0;
	try {
		buffer_map::iterator i(fqueue->fbuffers.lower_bound(time));
		if (i != fqueue->fbuffers.end()) {
			buf = (*i).second;
			if (out_time) {
				*out_time = (*i).first;
			}
		}
		fqueue->unlock();
	}
	catch (...) {
		fqueue->unlock();
	}
	return buf;
}

status_t
BTimedBufferQueue::RemoveBuffer(
	BBuffer* buffer)
{
	status_t err = B_ERROR;
	if (!fqueue->lock()) return B_ERROR;
	try {
		buffer_map::iterator i(fqueue->fbuffers.begin());
		while (i != fqueue->fbuffers.end()) {
			if ((*i).second == buffer) {
				atomic_add(&_fbuffer_count, -1);
				fqueue->fbuffers.erase(i);
				err = B_OK;
				break;
			}
			i++;
		}
		fqueue->unlock();
	}
	catch (...) {
		fqueue->unlock();
	}
	return err;
}

status_t
BTimedBufferQueue::FlushBefore(
	bigtime_t time)
{
	status_t err = B_ERROR;
	if (!fqueue->lock()) return B_ERROR;
	try {
		buffer_map::iterator i(fqueue->fbuffers.begin());
		buffer_map::iterator e(fqueue->fbuffers.lower_bound(time));
		while (i != e) {
			atomic_add(&_fbuffer_count, -1);
			fqueue->fbuffers.erase(i++);
		}
		err = B_OK;
		fqueue->unlock();
	}
	catch (...) {
		fqueue->unlock();
	}
	return err;
}

status_t
BTimedBufferQueue::FlushFromAndAfter(
	bigtime_t time)
{
	status_t err = B_ERROR;
	if (!fqueue->lock()) return B_ERROR;
	try {
		buffer_map::iterator i(fqueue->fbuffers.lower_bound(time));
		buffer_map::iterator e(fqueue->fbuffers.end());
		while (i != e) {
			atomic_add(&_fbuffer_count, -1);
			fqueue->fbuffers.erase(i++);
		}
		err = B_OK;
		fqueue->unlock();
	}
	catch (...) {
		fqueue->unlock();
	}
	return err;
}

