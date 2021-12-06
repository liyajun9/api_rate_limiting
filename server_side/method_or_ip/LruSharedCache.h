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

enum LRU_CACHE_ERROR_E  //������
{
    LRU_CACHE_OK,               //ok
    LRU_CACHE_DATA_NOT_FOUND,   //δ�ҵ���Ӧ��������
    LRU_CACHE_LOCK_ERROR,       //���󻥳���ʧ��
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

typedef struct _IpLimitKey  //IP key: ipv4תΪuint64��ʽ
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

typedef struct _LimitData   //��¼���ʴ���ͳ������
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

    void init(time_t t) //��ʼ��Ϊ1��
    {
        reqPerSec = reqPerMin = reqPerHor = reqPerDay = 1;
        startOfSec = startOfMin = startOfHor = startOfDay = t;
    }
} LimitData;

typedef struct _Limit    //����(��4��ʱ�䵥λά�ȵ�����)
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

        //node�ӹ����ڴ��ַָ���mutex֮��ʼ
        NODE<K, V>* pNode = (NODE<K, V>*)((char*)m_pSharedMem + sizeof(pthread_mutex_t));
        for (auto i = 0; i < cacheSize; ++i)
        {
            m_CacheNodeVec.push_back(pNode + i); //����ÿ��node�ĵ�ַ
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

    LRU_CACHE_ERROR_E put(K key, V& value)  //���빲���ڴ�
    {
        if (lock() != 0)
        {
            return LRU_CACHE_LOCK_ERROR;
        }

        typename std::map<K, NODE<K, V>*>::iterator it = m_CacheNodeMap.find(key);
        if (it == m_CacheNodeMap.end()) //��û�и�key, ��node��ȡ��һ��������
        {
            //��ԭʼ��node(vector)��û������, �����ȡһ��������
            NODE<K, V>* pNode = NULL;
            if (!m_CacheNodeVec.empty())
            {
                pNode = m_CacheNodeVec.back();
                m_CacheNodeVec.pop_back();
            }
            else    //��ԭʼ��node(vector)�Ѿ�����, ���listβ����ʼ�ظ�����, ����Ҫ�ǵô�map��ɾ������
            {
                pNode = m_pTail->prev;      //��listβ��ȡ��һ��node
                remove(pNode);
                m_CacheNodeMap.erase(pNode->key);
            }

            pNode->key = key;
            pNode->value = value;
            prepend(pNode);             //��ӵ�����ͷ
            m_CacheNodeMap[key] = pNode;
        }
        else    //�Ѿ���key, ���²��Ƶ�����ͷ
        {
            it->second->value = value; //�Ƶ�����ͷ
            remove(it->second);
            prepend(it->second);
        }

        unlock();
        return LRU_CACHE_OK;
    }

    LRU_CACHE_ERROR_E get(K key, K& retKey, V& retValue)    //��ȡ�����ֵ
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
    void prepend(NODE<K, V>* pNode) //���뵽����, ������map
    {
        pNode->prev = m_pHead;
        pNode->next = m_pHead->next;
        m_pHead->next = pNode;
        m_pHead->next->prev = pNode;

        m_CacheNodeMap.insert(std::make_pair<K, NODE<K, V>*>(pNode->key, pNode));
    }

    void remove(NODE<K, V>* pNode) //��������ɾ��
    {
        if (m_pTail->prev == m_pHead)   //������
        {
            return;
        }

        pNode->prev->next = pNode->next;
        pNode->next->prev = pNode->prev;
    }

    int lock() //������. �����÷�������ʱ, ҲҪ�������,�����������������.
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
    std::vector<NODE<K, V>* > m_CacheNodeVec;   //ԭʼ�����ڴ��Ϸ����nodes
    std::map<K, NODE<K, V>* > m_CacheNodeMap;   //key - node*, ���ڿ��ٲ�ѯ

    //����: ���������Ѿ�ʹ���˵�nodesָ��
    NODE<K, V>* m_pHead;
    NODE<K, V>* m_pTail;

    int m_shmid;
    void* m_pSharedMem;
    bool m_initialized;
};


void updateLimitData(time_t &nowTime, LimitData &data);                          //�յ�һ������ʱ, ����ͳ������

bool methodLimitCheck(const uint32_t& methodId, const uint32_t& limitPerSec);    //����������methodid, �Ƿ񳬹��˸�����ÿ�������������
bool ipLimitCheck(const std::string& remoteIp, const Limit& limit);              //����������ip, �Ƿ񳬹��˸����������������

#endif //METHOD_OR_IP_LRUSHAREDCACHE_H
