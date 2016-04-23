//
//
// Debug file for Watch / Printf
//
//
// ABH

#define TRACE

#ifdef TRACE
#ifndef WATCH
#define WATCH(x) printf(x)
#endif // WATCH
#endif // TRACE


