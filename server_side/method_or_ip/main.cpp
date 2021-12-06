#include "LruSharedCache.h"
#include "commondef.h"

LIMITCONF g_limitConfTable[] = {
    {"/1/auth/access_token", MTD_AUTH_GET_TOKEN, 1, 1, 1000},
    {"monitor", MTD_ACCOUNT_MONITOR, 1, 1, 1000},
    {"tracking", MTD_DEV_TRACK, 1, 1, 1000},
    {"history", MTD_DEV_HIS, 1, 1, 100},
    {"address", MTD_TOOL_ADDR, 1, 1, 3000},
    {"devinfo", MTD_ACCOUNT_DEVINFO, 1, 1, 100},
    {"lnglat", MTD_TOOL_LNGLAT, 1, 1, 3000},
    {"modify_pwd", MTD_TOOL_MODIFY_PWD, 1, 1, 100},
    {"efence", MTD_TOOL_EFENCE, 1, 1, 1000},
    {"get_alarminfo", MTD_TOOL_ALARM, 1, 1, 1000},
    {"order", MTD_TOOL_ORDER, 1, 1, 1000},
    {"unregister", MTD_ACCOUNT_UNREG, 1, 1, 1000},
    {"info", MTD_ACCOUNT_INFO, 1, 1, 1000},
    {"rectsearch", MTD_AREA_RECTSEARCH, 1, 1, 1000},
    {"blacklist", MTD_TOOL_BLOCKLIST, 1, 1, 3000},
    {"getchannelid", MTD_PUSH_GETCHANNEL, 1, 1, 100},

    //WEB端接口的开始
    {"get_meta_data_js", MTD_GET_META_DATA, 1, 1, 3000},
    {"commonService", MTD_COMMON_SERVICE, 1, 1, 3000},
    {"GetDataService", MTD_GET_DATA_SERVICE, 1, 1, 3000},
    {"TrackService", MTD_TRACK_SERVICE, 1, 1, 3000},
    {"GetGroupInfoService", MTD_GROUP_SERVICE, 1, 1, 3000},
    {"GetModifiedLocationInfo", MTD_MODIFY_LOCATION, 1, 1, 3000},
    {"WatchService", MTD_WATCH_SERVICE, 1, 1, 3000},
    {"RefreshLocalService", MTD_LOCAL_SERVICE, 1, 1, 3000},
    {"get_session_info", MTD_GET_SESSION, 1, 1, 3000},
    {"search", MTD_SEARCH, 1, 1, 3000},
    {"message", MTD_MESSAGE, 1, 1, 3000},
    {"device", MTD_DEVICE, 1, 1, 3000},
    {"IoTquery",MTD_IOT_QUERY, 1, 1, 3000},

};

unsigned int g_limitConfTableSize = sizeof(g_limitConfTable) / sizeof(LIMITCONF);
std::map<std::string, LIMITCONF> g_limitConfMap;

//检查ip是否超过每秒100次的限制
bool ipVisitLimitCheck(const std::string& script_name, const std::string& remoteIp)
{
    if (script_name.empty() || remoteIp.empty())
    {
        return true;
    }

    std::map<std::string, LIMITCONF>::iterator iter = g_limitConfMap.find(script_name);
    if (iter == g_limitConfMap.end() || 0 == iter->second.ipLimitOn)    //没有limit配置或没有开启limit检查, 允许访问
    {
        return true;
    }

    Limit limit(100); //每秒最多100次请求
    return ipLimitCheck(remoteIp, limit); //检查是否超过
}

//检查method是否超过配置的每秒访问次数
bool methodVisitLimitCheck(const std::string& script_name)
{
    if (script_name.empty())
    {
        return true;
    }

    std::map<std::string, LIMITCONF>::iterator iter = g_limitConfMap.find(script_name);
    if (iter == g_limitConfMap.end() || 0 == iter->second.methodLimitOn)    //没有limit配置或没有开启limit检查, 允许访问
    {
        return true;
    }

    return methodLimitCheck(iter->second.methodId, iter->second.methodSecondUpperLimit);
}

int main(int argc, char** argv)
{

    return 0;
}
