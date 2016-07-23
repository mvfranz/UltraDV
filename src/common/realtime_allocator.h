//	realtime_allocator.h
//	Copyright 1998, Be Incorporated
//
//	This allocator was written for the HP/SGI STL version 3.11 to
//	provide memory management for containers within a pre-allocated
//	locked area of memory. We do this so that we don't have to use
//	malloc() or free(), which use a shared lock, or enter VM, which
//	would cause unpredictable delays.
//
//	Thus, this allocator should be used in real-time threads and
//	other cases where predictability is an important factor, and where
//	you can determine an upper bound on the number of allocations that
//	will be required.
//
//	The allocator has been tested with <set> and <map>.
//
//	Note that this allocator is no thread-safe on a per-allocator object,
//	but since each container has an allocator, and each container is not
//	thread-safe on a per-container object, this is not a real problem.
//
//	Typical usage:
//
//	// use a typedef to reduce the typing strain imposed
//	typedef map<int, BString, less<int>, realtime_allocator<BString, realtime_blocks_256>> map_type;
//	map_type m;
//	m[2] = "hello, world!";
//	map_type::iterator i(m.begin());
//	while (i != m.end()) { cout << (*i).second.String(); i++; };
//

#if !defined(REALTIME_ALLOCATOR_H)
#define REALTIME_ALLOCATOR_H

#if !defined(__GNUC__) || !defined(__INTEL__)
#error This allocator only works with HP/SGI STL 3.11 using EGCS on Intel
#endif

#include <sys/types.h>
#include <typeinfo>
#include <OS.h>
#include <stdexcept>
#include <stdio.h>	//	for sprintf

//	how many times to run the test
#define NITER 10


class realtime_blocks_64 {
public:
	enum { NUM_BLOCKS = 64 };
};
class realtime_blocks_256 {
public:
	enum { NUM_BLOCKS = 256 };
};
class realtime_blocks_1024 {
public:
	enum { NUM_BLOCKS = 1024 };
};

template<class Type, class BlockCount = realtime_blocks_256>
class realtime_allocator {
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template <class T> class types {
	public:
		typedef T value_type;
		typedef T * pointer;
		typedef const T * const_pointer;
		typedef T & reference;
		typedef const T & const_reference;
	};
	template <class OtherType> struct rebind {
		typedef realtime_allocator<OtherType> other;
	};

	enum { NUM_BLOCKS = BlockCount::NUM_BLOCKS };

	inline size_t round_size(size_t size) {
		return (size+31)&~31;
	}
	realtime_allocator() {
		farea = NULL;
	}
	~realtime_allocator() {
		delete farea;
	}
	template<class Other>
	realtime_allocator(const realtime_allocator<Other> & other) {
		farea = NULL;
		//	We only allow copying of unused allocators, as that is the
		//	behaviour exhibited under testing <map> and <set>. Anything
		//	more would bring re-entrancy issues in the picture.
		//	assert(other.farea == NULL);
	}

//	This flavor was specified by the standard
//	Type * allocate(size_t size, const void * hint) {
//		return malloc(size*sizeof(Type));
//	}
	Type * allocate(size_t size) {
		if (!farea) {
			init_object();
		}
		size_t siz = size*sizeof(Type);
		assert(siz <= fsize);
		if (ffree == NULL) {
			throw bad_alloc();
		}
		Type * ret = reinterpret_cast<Type *>(ffree);
		ffree = *((char **)ffree);
		return ret;
	}
//	This flavor was specified by the standard
//	void deallocate(void * p) {
//		free(p);
//	}
	void deallocate(void * p, size_t size) {
		assert(size*sizeof(Type) <= fsize);
		assert((p >= fstart) && (p < (fstart+(fsize*fblocks))) && (((((char *)p)-fstart) % fsize) == 0));
		*(char **)p = ffree;
		ffree = (char *)p;
	}
	inline size_type max_size() const {
		return fblocks;
	}

private:

	realtime_allocator<Type> & operator=(const realtime_allocator<Type> & other);	// unimplemented

	class area_ref;

	area_ref * farea;
	size_t fsize;
	char * fstart;
	char * ffree;
	int fblocks;

	void init_object() {
		fsize = round_size(sizeof(Type));
		size_t s = NUM_BLOCKS*fsize;
		fblocks = NUM_BLOCKS;
		if (s > 65536) {
			fblocks = NUM_BLOCKS/4;
		}
		fstart = NULL;
		size_t pad_size = (fsize*fblocks+B_PAGE_SIZE-1)&~(B_PAGE_SIZE-1);
		char name[32];
		sprintf(name, "RTA:%.27s", typeid(Type).name());
		area_id area = create_area(name, (void **)&fstart,
			B_ANY_ADDRESS, pad_size, B_FULL_LOCK, B_READ_AREA|B_WRITE_AREA);
		if (area < 0) {
			throw runtime_error("Could not allocate area in allocator::init_object()");
		}
		ffree = fstart;
		// create linked free list
		char ** ptr = (char **)ffree;
		char * next = ffree+fsize;
		for (int ix=0; ix<fblocks-1; ix++) {
			*ptr = next;
			next += fsize;
			ptr += fsize/sizeof(char *);
		}
		*ptr = NULL;
		farea = new area_ref(area);
	}

	//	Class area_ref is used to actually reference the area we allocate
	//	out of, and reference count it.
	class area_ref {
	public:
		area_ref(area_id area, int32 ref_count = 1) {
			farea = area;
			fref_count = ref_count;
		}
		~area_ref() {
			if (atomic_add(&fref_count, -1) == 1) {
				delete_area(farea);
			}
		}
	private:
	
		area_ref(const area_ref & other);
		const area_ref & operator=(const area_ref & other);

		area_id farea;
		int32 fref_count;
	};

};

//	Specialize for void because there are no void references.
//	I wanted template<class Type> class realtime_allocator<Type>::types<void>
//	but that generated an internal compiler error in EGCS on 1998-10-28.
// ABH error in next line

//template<class Type> class realtime_allocator<Type>::types<void>;


class realtime_allocator<void>::types<void> {
public:
	typedef void * pointer;
	typedef const void * const_pointer;
	typedef void value_type;
};



//	Use operator new(size, allocator) and operator delete(size, allocator)
//	for malloc()-style allocation using the allocator. Note that the allocator
//	is blocking; no block can be bigger than the (rounded-up) size of the
//	Type argument.
template<class Type, class BlockSize>
inline void * operator new(size_t size, realtime_allocator<Type, BlockSize> & a)
{
	return a.allocate(size);
}

template<class Type, class BlockSize>
inline void operator delete(void * ptr, realtime_allocator<Type, BlockSize> & a)
{
	if (!ptr) return;
	a.deallocate(ptr);
}


#endif // REALTIME_ALLOCATOR_H

