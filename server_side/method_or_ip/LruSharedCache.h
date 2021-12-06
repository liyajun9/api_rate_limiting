#ifndef METHOD_OR_IP_LRUSHAREDCACHE_H
#define METHOD_OR_IP_LRUSHAREDCACHE_H
#include <cstdint>
#include <ctime>
#include <vector>
#include <string>
#include <map>
#include <sys/shm.h>
#include <iostream>
#include <cstring>
#include "TimeTransformer.h"

enum LRU_CACHE_ERROR_E  //错误码
{
    LRU_CACHE_OK,               //ok
    LRU_CACHE_DATA_NOT_FOUND,   //未找到相应缓存数据
    LRU_CACHE_LOCK_ERROR,       //请求互斥锁失败
    LRU_CACHE_MAX = 1000
};

typedef struct _MethodLimitKey  //method key
{
    uint32_t methodId;
    _MethodLimitKey() : methodId(0)
    {
    }
    bool operator<(const _MethodLimitKey& other) const
    {
        return (this->methodId < other.methodId);
    }
} MethodLimitKey;

typedef struct _IpLimitKey  //IP key: ipv4转为uint64形式
{
    uint64_t ipAddr;
    _IpLimitKey() : ipAddr(0)
    {
    }
    bool operator<(const _IpLimitKey& other) const
    {
        return (this->ipAddr < other.ipAddr);
    }
} IpLimitKey;

typedef struct _LimitData   //记录访问次数统计数据
{
    uint32_t reqPerSec;
    uint32_t reqPerMin;
    uint32_t reqPerHor;
    uint32_t reqPerDay;

    time_t startOfSec;
    time_t startOfMin;
    time_t startOfHor;
    time_t startOfDay;

    _LimitData() : reqPerSec(0), reqPerMin(0), reqPerHor(0), reqPerDay(0),
        startOfSec(0), startOfMin(0), startOfHor(0), startOfDay(0)
    {
    }

    void init(time_t t) //初始化为1次
    {
        reqPerSec = reqPerMin = reqPerHor = reqPerDay = 1;
        startOfSec = startOfMin = startOfHor = startOfDay = t;
    }
} LimitData;

typedef struct _Limit    //限制(含4个时间单位维度的限制)
{
    uint32_t maxReqPerSec;
    uint32_t maxReqPerMin;
    uint32_t maxReqPerHor;
    uint32_t maxReqPerDay;

    explicit _Limit(int perSec = 0, int perMin = 0, int perHor = 0, int perDay = 0) : maxReqPerSec(perSec), maxReqPerMin(perMin),
        maxReqPerHor(perHor), maxReqPerDay(perDay)
    {
    }

    void setLimit(std::vector<int>& limit);
    void setLimit(const std::string& sLimit);

} Limit;

template<class K, class V>
struct NODE
{
    K key;
    V value;
    NODE *prev;
    NODE *next;
};

int initMutex(pthread_mutex_t* mutex);

template<class K, class V>
class LruSharedCache
{
public:
    LruSharedCache() : m_pMutex(NULL), m_pHead(NULL), m_pTail(NULL), m_shmid(0), m_pSharedMem(NULL), m_initialized(false)
    {
    }

    ~LruSharedCache()
    {
        if (m_initialized)
        {
            pthread_mutex_destroy(m_pMutex);
        }
        shmdt(m_pSharedMem);
        if (m_initialized)
        {
            shmctl(m_shmid, IPC_RMID, 0);
        }

        if (m_pHead)
        {
            delete m_pHead;
        }
        if (m_pTail)
        {
            delete m_pTail;
        }
    }

    int init(uint32_t cacheSize = 10000, key_t cacheKey = 123456, bool initFlag = true)
    {
        m_initialized = initFlag;
        m_shmid = shmget(cacheKey, cacheSize, initFlag);
        if (-1 ==m_shmid)
        {
            std::cout << "shmget failed. ret:" << errno << " , msg:" << strerror(errno) << std::endl;
            return -1;
        }

        m_pSharedMem = shmat(m_shmid, 0, 0);
        if ((void*)-1 == m_pSharedMem)
        {
            std::cout << "shmat failed. ret:" << errno << " , msg:" << strerror(errno) << std::endl;
            return -1;
        }

        //node从共享内存地址指针和mutex之后开始
        NODE<K, V>* pNode = (NODE<K, V>*)((char*)m_pSharedMem + sizeof(pthread_mutex_t));
        for (auto i = 0; i < cacheSize; ++i)
        {
            m_CacheNodeVec.push_back(pNode + i); //保存每个node的地址
        }

        m_pHead = new NODE<K, V>();
        m_pTail = new NODE<K, V>();
        if (m_pHead == NULL || m_pTail == NULL)
        {
            std::cout << "allocate failed." << std::endl;
            return -1;
        }

        m_pHead->prev = NULL;
        m_pHead->next = m_pTail;
        m_pTail->prev = m_pHead;
        m_pTail->next = NULL;

        return 0;
    }

    LRU_CACHE_ERROR_E put(K key, V& value)  //存入共享内存
    {
        if (lock() != 0)
        {
            return LRU_CACHE_LOCK_ERROR;
        }

        typename std::map<K, NODE<K, V>*>::iterator it = m_CacheNodeMap.find(key);
        if (it == m_CacheNodeMap.end()) //还没有该key, 从node中取出一个来保存
        {
            //若原始的node(vector)还没有用完, 则从中取一个来保存
            NODE<K, V>* pNode = NULL;
            if (!m_CacheNodeVec.empty())
            {
                pNode = m_CacheNodeVec.back();
                m_CacheNodeVec.pop_back();
            }
            else    //若原始的node(vector)已经用完, 则从list尾部开始重复利用, 但是要记得从map中删除索引
            {
                pNode = m_pTail->prev;      //从list尾部取出一个node
                remove(pNode);
                m_CacheNodeMap.erase(pNode->key);
            }

            pNode->key = key;
            pNode->value = value;
            prepend(pNode);             //添加到链表头
            m_CacheNodeMap[key] = pNode;
        }
        else    //已经有key, 更新并移到链表头
        {
            it->second->value = value; //移到链表头
            remove(it->second);
            prepend(it->second);
        }

        unlock();
        return LRU_CACHE_OK;
    }

    LRU_CACHE_ERROR_E get(K key, K& retKey, V& retValue)    //读取保存的值
    {
        if (lock() != 0)
        {
            return LRU_CACHE_LOCK_ERROR;
        }

        typename std::map<K, NODE<K, V>*>::iterator it = m_CacheNodeMap.find(key);
        if (it != m_CacheNodeMap.end())
        {
            retKey = it->second->key;
            retValue = it->second->value;
            unlock();
            return LRU_CACHE_OK;
        }

        unlock();
        return LRU_CACHE_DATA_NOT_FOUND;
    }

private:
    void prepend(NODE<K, V>* pNode) //加入到链表, 并加入map
    {
        pNode->prev = m_pHead;
        pNode->next = m_pHead->next;
        m_pHead->next = pNode;
        m_pHead->next->prev = pNode;

        m_CacheNodeMap.insert(std::make_pair<K, NODE<K, V>*>(pNode->key, pNode));
    }

    void remove(NODE<K, V>* pNode) //从链表中删除
    {
        if (m_pTail->prev == m_pHead)   //空链表
        {
            return;
        }

        pNode->prev->next = pNode->next;
        pNode->next->prev = pNode->prev;
    }

    int lock() //申请锁. 但调用发生错误时, 也要允许调用,正常情况则需申请锁.
    {
        if (!m_initialized || m_pMutex == NULL)
        {
            return -1;
        }

        int ret = pthread_mutex_lock(m_pMutex);
        if (0 == ret)
        {
            return 0;
        }
        else if(EOWNERDEAD == ret)
        {
            std::cout << "eowner dead" << std::endl;

            ret = pthread_mutex_consistent(m_pMutex);
            if (0 != ret)
            {
                std::cout << "pthread_mutex_consistent failed. ret:" << errno << " , msg:" << strerror(errno) << std::endl;
                return -1;
            }
            else
            {
                std::cout << "pthread_mutex_consistent success." << std::endl;
                return 0;
            }
        }
        else if(ENOTRECOVERABLE == ret)
        {
            std::cout << "not recoverable" << std::endl;
            pthread_mutex_destroy(m_pMutex);
            initMutex(m_pMutex);
            ret = pthread_mutex_lock(m_pMutex);
            if (0 == ret)
            {
                std::cout << "recover success." << std::endl;
                return 0;
            }
            else
            {
                std::cout << "pthread_mutex_lock failed. ret:" << errno << " , msg:" << strerror(errno) << std::endl;
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }

    int unlock()
    {
        if (!m_initialized || m_pMutex == NULL)
        {
            return -1;
        }
        pthread_mutex_unlock(m_pMutex);
        return 0;
    }

private:
    pthread_mutex_t* m_pMutex;
    std::vector<NODE<K, V>* > m_CacheNodeVec;   //原始共享内存上分配的nodes
    std::map<K, NODE<K, V>* > m_CacheNodeMap;   //key - node*, 用于快速查询

    //链表: 保存所有已经使用了的nodes指针
    NODE<K, V>* m_pHead;
    NODE<K, V>* m_pTail;

    int m_shmid;
    void* m_pSharedMem;
    bool m_initialized;
};


void updateLimitData(time_t &nowTime, LimitData &data);                          //收到一个请求时, 更新统计数据

bool methodLimitCheck(const uint32_t& methodId, const uint32_t& limitPerSec);    //检查给定请求methodid, 是否超过了给定的每秒请求次数限制
bool ipLimitCheck(const std::string& remoteIp, const Limit& limit);              //检查给定请求ip, 是否超过了给定的请求次数限制

#endif //METHOD_OR_IP_LRUSHAREDCACHE_H
