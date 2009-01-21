#ifndef TS_PRINT_HPP
#define TS_PRINT_HPP

#include <boost/thread/mutex.hpp>
#include <iostream>

extern boost::mutex mutexPrint;

#define PRINT_LOCK boost::mutex::scoped_lock lock(mutexPrint)

#endif /* TS_PRINT_HPP */
