#include "LruSharedCache.h"
#include "StringUtility.h"
#include <pthread.h>

void _Limit::setLimit(std::vector<int> &limit)
{
    while (limit.size() < 4)
    {
        limit.emplace_back(0);
    }
    maxReqPerSec = limit[0];
    maxReqPerMin = limit[1];
    maxReqPerHor = limit[2];
    maxReqPerDay = limit[3];
}

void _Limit::setLimit(const std::string &sLimit)
{
    std::vector<std::string> sVec;
    my_ns::SplitString(sLimit, ",", sVec);

    int limitNumber(0);
    std::vector<int> intVec;
    for (auto it = sVec.cbegin(); it != sVec.cend(); ++it)
    {
        limitNumber = atoi(it->c_str());
        intVec.emplace_back(limitNumber);
    }

    setLimit(intVec);
}

int initMutex(pthread_mutex_t* mutex)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
    pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
}
