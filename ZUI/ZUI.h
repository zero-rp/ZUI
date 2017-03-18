/**
* @file     ZUI.h
* @brief    公共头文件.
* @author   [Zero](22249030)
* @version  1.0
* @date     $date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __ZUI_H__
#define __ZUI_H__

#include "duktape/duktape.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//config

//图形引擎配置
#define PLATFORM_GRAPH_SKIA 0
#define PLATFORM_GRAPH_GDI 1
#define PLATFORM_GRAPH_CAIRO 0
#define PLATFORM_GRAPH_AGG 0
//图像解码器
#define PLATFORM_GRAPH_IMAGE_PNG 0
#define PLATFORM_GRAPH_IMAGE_JPEG 0

//运行平台配置
#ifdef WIN32
    #define PLATFORM_OS_WIN
#elif defined _WIN64
    #define PLATFORM_OS_WIN
#elif defined LINUX
    #define PLATFORM_OS_LINUX
#elif defined MACX
    #define PLATFORM_OS_MACX
#endif

#define LOG_DEBUG   1       //打印调试日志 有运行时调试功能则打印到日志窗口
#define MEM_DEBUG   0       //开启内存调试功能
#define RUN_DEBUG   1       //开启运行时调试功能


#define JS_GCTIMER  10      //js内存回收间隔 s


#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>


#ifdef PLATFORM_OS_WIN
    #ifdef __cplusplus
        #define ZEXPORT extern "C" __declspec(dllexport)
    #else
        #define ZEXPORT __declspec(dllexport)
    #endif
    #define ZCALL __stdcall
#else
    #ifdef __cplusplus
        #define ZEXPORT extern "C" 
    #else
        #define ZEXPORT
    #endif
        #define ZCALL __attribute__((__stdcall__))
#endif

#define DEFARG(name, defval) ((#name[0]) ? (name + 0) : defval)
#define ZProc(Control,Proc,Param1,Param2,Param3) ZuiControlCall(Proc_##Proc,Control,(ZuiAny)DEFARG(Param1, NULL), (ZuiAny)DEFARG(Param2, NULL), (ZuiAny)DEFARG(Param3, NULL));


#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif


typedef wchar_t*        ZuiText, _ZuiText;	//内核默认Unicode存储字符
typedef float           ZuiReal;
#ifdef _WIN64
typedef int64_t         ZuiInt;
typedef uint64_t        ZuiUInt;
#else
typedef int             ZuiInt;
typedef unsigned int    ZuiUInt;
#endif
typedef int             ZuiBool;
typedef void            ZuiVoid;
typedef void*           ZuiAny;
typedef int             ZuiColor;
typedef unsigned        ZuiByte;
/**矩形*/
typedef struct _ZRect
{
    int Left;   ///左边
    int Top;    ///顶边
    int Width;  ///宽度
    int Height; ///高度
} *ZuiRect, ZRect;
/**矩形(浮点)*/
typedef struct _ZRectR
{
    ZuiReal Left;///左边
    ZuiReal Top;///顶边
    ZuiReal Width;///宽度
    ZuiReal Height;///高度
} *ZuiRectR, ZRectR;
/**点*/
typedef struct _ZPoint
{
    int x;
    int y;
} *ZuiPoint, ZPoint;
/**大小*/
typedef struct _ZSize
{
    int Width;	///宽度
    int Height;	///高度
} *ZuiSize, ZSize;
/**点(浮点)*/
typedef struct _ZPointR
{
    ZuiReal x;
    ZuiReal y;
} *ZuiPointR, ZPointR;

#define MAKEZRECT(r, L, T, W, H) \
    r.Left = L; \
    r.Top = T;\
    r.Width = W;\
    r.Height = H;
#define MAKEZRECTP(r, L, T, W, H) \
    r->Left = L; \
    r->Top = T;\
    r->Width = W;\
    r->Height = H;

#ifdef LINUX 
typedef long LONG;
typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *PRECT, *NPRECT, *LPRECT;
typedef struct tagSIZE
{
    LONG        cx;
    LONG        cy;
} SIZE, *PSIZE, *LPSIZE;
#endif


#include "core/memory.h"
#include "core/debug.h"

/*基础辅助函数*/
#include "core/carray.h"
#include "core/string.h"
#include "core/function.h"
#include "core/tree.h"
#include "zlib/unzip.h"
#include "core/mxml.h"
#include "platform/platform.h"
#include "core/resdb.h"
#include "core/global.h"
/*内核*/

#include "core/manager.h"       //绘制管理器
#include "core/animation.h"     //动画处理器
#include "core/control.h"       //控件基类
#include "core/template.h"
#include "core/builder.h"

/*控件*/
#include "control/Register.h"
#include "control/window.h"
#include "control/Html.h"
#include "control/Virtual.h"    //虚拟控件,用来管理原生控件
#include "control/Menu.h"
#include "control/MenuBar.h"

#include "control/ScrollBar.h"
#include "control/Label.h"
#include "control/Button.h"
#include "control/SplitterBar.h"
#ifdef WIN32
#include "control/Browser.h"
#endif
#include "control/Edit.h"
#include "control/Option.h"
#include "control/List.h"
#include "control/TreeView.h"

/*布局*/
#include "layout/Layout.h"              //容器基类
#include "layout/VerticalLayout.h"      //纵向布局
#include "layout/HorizontalLayout.h"    //横向布局
#include "layout/TileLayout.h"          //列表布局
#include "layout/TabLayout.h"           //选择夹布局





#if defined(__cplusplus)
}
#endif

#endif
