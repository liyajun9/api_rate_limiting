#include "TimeTransformer.h"
#include <assert.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>


namespace my_ns
{
    uint64_t GetTimeMicrosecond()
    {
        struct timeval stTime;
        gettimeofday(&stTime, NULL);
        uint64_t t = 1000000 * stTime.tv_sec + stTime.tv_usec;
        return t;
    }

    uint64_t GetTimeMillisecond()
    {
        struct timeval stTime;
        gettimeofday(&stTime, NULL);
        uint64_t t = 1000 * stTime.tv_sec + stTime.tv_usec / 1000;
        return t;
    }

    std::string UnixTime2TimeString(time_t t, const char * pFMT)
    {
        char szTmp[50] = {0};
        struct tm oTM;
        memset(&oTM, 0, sizeof(oTM));
        localtime_r(&t, &oTM);
        strftime(szTmp, sizeof(szTmp), pFMT, &oTM);
        return std::string(szTmp);
    }

    time_t TimeString2UnixTime(const char * pTimeStr, const char * pFMT)
    {
        assert(pFMT != NULL);
        std::string strZeroTime = UnixTime2TimeString(0, pFMT);
        if(strcmp(pTimeStr, strZeroTime.c_str()) < 0)
        {
            return 0;
        }

        struct tm oTM;
        memset(&oTM, 0, sizeof(oTM));
        oTM.tm_mday = 1;    //in case pFMT = "%Y-%m"
        char * p = strptime(pTimeStr, pFMT, &oTM);
        if ((NULL == p) || (*p != '\0'))
        {
            return -1;
        }
        time_t t = mktime(&oTM);
        return t;
    }

    time_t BCD2Time(const char * pBCD, int dwLen)
    {
        assert(pBCD != NULL);
        struct tm oTime;
        oTime.tm_year = CharHex2Dec(pBCD[0]) + 100;
        oTime.tm_mon = CharHex2Dec(pBCD[1]) - 1;
        oTime.tm_mday = CharHex2Dec(pBCD[2]);
        oTime.tm_hour = CharHex2Dec(pBCD[3]);
        oTime.tm_min = CharHex2Dec(pBCD[4]);
        oTime.tm_sec = CharHex2Dec(pBCD[5]);
        return mktime(&oTime);
    }

    char * Time2BCD(time_t dwTime, int dwBcdLen, char * pBuf)
    {
        char szTmp[10] = {0};
        struct tm oTime;
        localtime_r(&dwTime, &oTime);

        if (kBcdSize6 == dwBcdLen)
        {
            snprintf(szTmp, sizeof(szTmp), "%c%c%c%c%c%c",
                CharDec2Hex(oTime.tm_year - 100),
                CharDec2Hex(oTime.tm_mon + 1),
                CharDec2Hex(oTime.tm_mday),
                CharDec2Hex(oTime.tm_hour),
                CharDec2Hex(oTime.tm_min),
                CharDec2Hex(oTime.tm_sec));    
            memcpy(pBuf, szTmp, dwBcdLen);
        }
        else if (kBcdSize7 == dwBcdLen)
        {
            snprintf(szTmp, sizeof(szTmp), "%c%c%c%c%c%c%c",
                CharDec2Hex(oTime.tm_year / 100 + 19),
                CharDec2Hex(oTime.tm_year - 100),
                CharDec2Hex(oTime.tm_mon + 1),
                CharDec2Hex(oTime.tm_mday),
                CharDec2Hex(oTime.tm_hour),
                CharDec2Hex(oTime.tm_min),
                CharDec2Hex(oTime.tm_sec));    
            memcpy(pBuf, szTmp, dwBcdLen);
        }
        return pBuf;
    }

}


