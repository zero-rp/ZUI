#ifndef __ZUI_CORE_RESDB_H__
#define __ZUI_CORE_RESDB_H__
#include <ZUI.h>
#include "tree.h"
/*
资源池设计方案


*/
/*资源包类型*/
enum ZRESDBT
{
    ZRESDBT_ZIP_FILE = 1, ///压缩文件,来自文件
    ZRESDBT_ZIP_STREAM, ///压缩文件,来着字节流
    ZRESDBT_FILE, ///文件
    ZRESDBT_STREAM, ///字节流
    ZRESDBT_URL, ///网络资源
    ZRESDBT_PE, ///应用资源
    ZRESDBT_FONT ///字体也属于一类资源,由字体管理器管理
};
/**资源包结构*/
typedef struct _ZResDB
{
#if (defined PLATFORM_OS_WIN)
    HINSTANCE Instance; //pe句柄
#endif // PLATFORM_OS_WIN
    void* uf; //压缩包句柄
    ZuiText pass; //压缩包密码
    ZuiInt type; //资源包类型
} *ZuiResDB, ZResDB;
/**单个资源*/
typedef struct _ZRes
{
    ZuiAny p; //资源指针
    ZuiInt plen; //资源长度

    ZuiInt hash;
    ZuiInt ref; //引用计数器
    ZuiInt type; //资源类型
} *ZuiRes, ZRes;
/**资源池*/
typedef struct _ZResDBPool
{
    rb_root *resdb; //所有已经加载过的资源包
    rb_root *res; //所有已经加载过的资源
} *ZuiResDBPool, ZResDBPool;
ZuiBool ZuiResDBInit();
ZuiVoid ZuiResDBUnInit();

#endif //__ZUI_CORE_RESDB_H__

