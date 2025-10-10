#ifndef SERVERPROJECT_SWITCHCONCURRENTQUEUE_H
#define SERVERPROJECT_SWITCHCONCURRENTQUEUE_H

#ifdef USE_SELF_CQ

#include "concurrentQueue.h"

template<typename T>
using concurrentQueue = SelfCQ::concurrentQueue<T>;

#else

#include <concurrentqueue/concurrentqueue.h>

template<typename T>
using concurrentQueue = moodycamel::ConcurrentQueue<T>;

#endif

#endif //SERVERPROJECT_SWITCHCONCURRENTQUEUE_H
