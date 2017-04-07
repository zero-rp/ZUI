/**
* @file		debug.h
* @brief	调试器.
* @author	[Zero](22249030)
* @version	1.0
* @date		$date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __ZUI_CORE_DEBUG_H__
#define __ZUI_CORE_DEBUG_H__

#define ZLOG_TYPE_ERROR     0
#define ZLOG_TYPE_DEBUG     1
#define ZLOG_TYPE_WARNING     2
#define ZLOG_TYPE_INFO     3

#if LOG_DEBUG || RUN_DEBUG


#define LOG_ERROR(fmt, ...) do{\
    wchar_t buf[512];   \
    _snwprintf(buf, 512, fmt, __VA_ARGS__);\
    ZuiDebugLog(ZLOG_TYPE_ERROR, buf);  \
}while(0)
#define LOG_DEGUB(fmt, ...) do{\
    wchar_t buf[512];   \
    _snwprintf(buf, 512, fmt, __VA_ARGS__);\
    wprintf(buf);\
    ZuiDebugLog(ZLOG_TYPE_DEBUG, buf);  \
}while(0)
#define LOG_WARNING(fmt, ...) do{\
    wchar_t buf[512];   \
    _snwprintf(buf, 512, fmt, __VA_ARGS__);\
    ZuiDebugLog(ZLOG_TYPE_WARNING, buf);  \
}while(0)
#define LOG_INFO(fmt, ...) do{\
    wchar_t buf[512];   \
    _snwprintf(buf, 512, fmt, __VA_ARGS__);\
    ZuiDebugLog(ZLOG_TYPE_INFO, buf);  \
}while(0)


#define LOG_DUK(ctx) do{\
    wchar_t buf[1024];   \
    duk_get_prop_string((ctx), -1, "stack");\
    _snwprintf(buf, 1024, L"%s\r\n",duk_get_string_w((ctx), -1));\
    wprintf(buf);\
    ZuiDebugLog(ZLOG_TYPE_INFO, buf);  \
}while(0)
#else
#define LOG_ERROR(fmt, ...)
#define LOG_DEGUB(fmt, ...)
#define LOG_WARNING(fmt, ...)
#define LOG_INFO(fmt, ...)
#define LOG_DUK(ctx)
#endif 

ZuiVoid ZuiDebugLog(ZuiInt type, const wchar_t *fmt);
ZuiVoid ZuiStartDebug();

#endif //__ZUI_CORE_DEBUG_H__
