#include "LruSharedCache.h"
#include "StringUtility.h"
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

LruSharedCache<MethodLimitKey, LimitData> g_MethodLimit;    //存储各method的访问统计数据
LruSharedCache<IpLimitKey, LimitData> g_IpLimit;            //存储各ip的访问统计数据

Limit g_oIpLimit;   //每ip访问次数限制

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

    return 0;
}

void updateLimitData(time_t &nowTime, LimitData &data)
{
    time_t diffToSec = nowTime - data.startOfSec;
    time_t diffToMin = nowTime - data.startOfMin;
    time_t diffToHor = nowTime - data.startOfHor;
    time_t diffToDay = nowTime - data.startOfDay;

    if (diffToSec <= 1)
    {
        ++data.reqPerSec;
    }
    else
    {
        data.startOfSec = nowTime;
        data.startOfSec = 1;
    }

    if (diffToMin <= 60)
    {
        ++data.reqPerMin;
    }
    else
    {
        data.startOfMin = nowTime;
        data.reqPerMin = 1;
    }

    if (diffToHor <= (60 * 60))
    {
        ++data.startOfHor;
    }
    else
    {
        data.startOfHor = nowTime;
        data.reqPerHor = 1;
    }

    if (diffToDay <= (24 * 60 * 60))
    {
        ++data.reqPerDay;
    }
    else
    {
        data.startOfDay = nowTime;
        data.reqPerDay = 1;
    }
}

bool ipLimitCheck(const std::string& remoteIp, const Limit& limit)
{
    if (remoteIp.empty())
    {
        return true; //未获取到ip, 允许访问
    }

    uint64_t t[10] = {0};
    t[0] = my_ns::GetTimeMicrosecond();

    //查询存储的统计数据
    time_t nowTime = time(NULL);
    IpLimitKey key;
    key.ipAddr = ntohl(inet_addr(remoteIp.c_str()));
    IpLimitKey retKey;
    LimitData data;
    LRU_CACHE_ERROR_E ret = g_IpLimit.get(key, retKey, data);
    if (LRU_CACHE_OK == ret) //找到数据,更新,加上当前这次请求
    {
        updateLimitData(nowTime, data);
    }
    else if (LRU_CACHE_DATA_NOT_FOUND == ret) //找不到数据,初始化一个
    {
        data.init(nowTime);
    }
    else if (LRU_CACHE_LOCK_ERROR == ret) //不是由于超出了限制,而是锁异常,要允许访问
    {
        std::cout << "lock error. remoteIp = " << remoteIp << " , ret:" << ret << std::endl;
        return true;
    }
    else    //同上
    {
        std::cout << "other error. remoteIp = " << remoteIp << " , ret:" << ret << std::endl;
        return true;
    }

    //存储统计数据
    if (g_IpLimit.put(key, data))
    {
        std::cout << "put failed." << remoteIp << " , ret:" << ret << std::endl;
    }

    t[1] = my_ns::GetTimeMicrosecond();
    if (data.reqPerSec <= limit.maxReqPerSec &&
        data.reqPerMin <= limit.maxReqPerMin &&
        data.reqPerHor <= limit.maxReqPerHor &&
        data.reqPerDay <= limit.maxReqPerDay)
    {
        std::cout << "ipLimitcheck ok(sec cur:" << data.reqPerSec << "-limit:" << limit.maxReqPerSec <<
                ", min cur:" << data.reqPerMin << "-limit:" << limit.maxReqPerMin <<
                ", hour cur:" << data.reqPerHor << "-limit:" << limit.maxReqPerHor <<
                ", day cur" << data.reqPerDay << "-limit:" << limit.maxReqPerDay <<
        "). methodId = " << remoteIp << " , time cost in us:" << t[1] - t[0] << std::endl;
        return true;
    }
    else
    {
        std::cout << "ipLimitcheck failed(sec cur:" << data.reqPerSec << "-limit:" << limit.maxReqPerSec <<
                  ", min cur:" << data.reqPerMin << "-limit:" << limit.maxReqPerMin <<
                  ", hour cur:" << data.reqPerHor << "-limit:" << limit.maxReqPerHor <<
                  ", day cur" << data.reqPerDay << "-limit:" << limit.maxReqPerDay <<
                  "). methodId = " << remoteIp << " , time cost in us:" << t[1] - t[0] << std::endl;
        return false;
    }
}


bool methodLimitCheck(const uint32_t& methodId, const uint32_t& limitPerSec)
{
    uint64_t t[10] = {0};
    t[0] = my_ns::GetTimeMicrosecond();

    //查询存储的统计数据
    time_t nowTime = time(NULL);
    MethodLimitKey key;
    key.methodId = methodId;
    MethodLimitKey retKey;
    LimitData data;
    LRU_CACHE_ERROR_E ret = g_MethodLimit.get(key, retKey, data);
    if (LRU_CACHE_OK == ret) //找到数据,更新,加上当前这次请求
    {
        updateLimitData(nowTime, data);
    }
    else if (LRU_CACHE_DATA_NOT_FOUND == ret) //找不到数据,初始化一个
    {
        data.init(nowTime);
    }
    else if (LRU_CACHE_LOCK_ERROR == ret) //不是由于超出了限制,而是锁异常,要允许访问
    {
        std::cout << "lock error. methodId = " << methodId << " , ret:" << ret << std::endl;
        return true;
    }
    else    //同上
    {
        std::cout << "other error. methodId = " << methodId << " , ret:" << ret << std::endl;
        return true;
    }

    //存储统计数据
    if (g_MethodLimit.put(key, data))
    {
        std::cout << "put failed." << methodId << " , ret:" << ret << std::endl;
    }

    t[1] = my_ns::GetTimeMicrosecond();
    if (data.reqPerSec <= limitPerSec)
    {
        std::cout << "methodLimitcheck ok(cur:" << data.reqPerSec << "-limit:" << limitPerSec << "). methodId = " << methodId << " , time cost in us:" << t[1] - t[0] << std::endl;
        return true;
    }
    else
    {
        std::cout << "methodLimitCheck failed(cur:" << data.reqPerSec << "-limit:" << limitPerSec << "). methodId = " << methodId << " , time cost in us:" << t[1] - t[0] << std::endl;
        return false;
    }
}
