#ifndef METHOD_OR_IP_COMMONDEF_H
#define METHOD_OR_IP_COMMONDEF_H

typedef struct _LIMITCONF   //关于访问限制的配置
{
    std::string uri;
    int methodId;
    unsigned int ipLimitOn;
    unsigned int methodLimitOn;
    unsigned int methodSecondUpperLimit;    //每秒允许访问method的次数限制
} LIMITCONF;

enum METHOD_ID
{
    MTD_AUTH_GET_TOKEN = 0,
    MTD_ACCOUNT_MONITOR,
    MTD_DEV_TRACK,
    MTD_DEV_HIS,
    MTD_TOOL_ADDR,
    MTD_ACCOUNT_DEVINFO,
    MTD_TOOL_LNGLAT,
    MTD_TOOL_MODIFY_PWD,
    MTD_TOOL_EFENCE,
    MTD_TOOL_ALARM,
    MTD_TOOL_ORDER,  //10
    MTD_ACCOUNT_UNREG,
    MTD_ACCOUNT_INFO,
    MTD_AREA_RECTSEARCH,
    MTD_TOOL_BLOCKLIST,
    MTD_PUSH_GETCHANNEL,

    //WEB端接口
            MTD_GET_META_DATA,
    MTD_COMMON_SERVICE,
    MTD_GET_DATA_SERVICE,
    MTD_TRACK_SERVICE,
    MTD_GROUP_SERVICE, //20
    MTD_MODIFY_LOCATION,
    MTD_WATCH_SERVICE,
    MTD_LOCAL_SERVICE,
    MTD_GET_SESSION,
    MTD_SEARCH,
    MTD_LOCATION,
    MTD_MESSAGE,
    MTD_DEVICE,
    MTD_IOT_QUERY,
    MTD_HEAT_MAP,

};

#endif //METHOD_OR_IP_COMMONDEF_H
