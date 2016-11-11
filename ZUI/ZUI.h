/**
* @file		ZUI.h
* @brief	公共头文件.
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
#ifndef __ZUI_H__
#define __ZUI_H__

/*系统头文件*/
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <olectl.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )
#pragma comment(lib, "Msimg32.lib")  
#pragma comment(lib, "Gdiplus.lib")
#else
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#endif

#if defined _M_IX86
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


#ifdef WIN32

#ifdef __cplusplus
#define ZEXPORT extern "C" __declspec(dllexport)
#else
#define ZEXPORT __declspec(dllexport)
#endif

#define ZCALL __stdcall

#else
#define ZAPI(Type) extern "C" Type __attribute__((__stdcall__))
#endif

#define ARGB(A,R,G,B) ((int32_t)((((A)&0xff)<<24)|(((R)&0xff)<<16)|(((G)&0xff)<<8)|((B)&0xff)))
#define RGBA(R,G,B,A) ARGB(A,R,G,B)
#define RGB2ARGB(COLOR,A) RGBA(((COLOR) >> 16 & 0xFF), ((COLOR) >> 8 & 0xFF), ((COLOR) & 0xFF), (A))
#ifdef LINUX
#define RGB(r,g,b) ((ZuiInt)(((ZuiByte)(r)|((short)((ZuiByte)(g))<<8))|(((ZuiInt)(ZuiByte)(b))<<16)))
#endif

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif


typedef wchar_t*	ZuiText, _ZuiText;	//内核默认Unicode存储字符
typedef float		ZuiReal;
#ifdef _WIN64
typedef int64_t		ZuiInt;
#else
typedef int32_t		ZuiInt;
#endif
typedef int			ZuiBool;
typedef void		ZuiVoid;
typedef void*		ZuiAny;
typedef int32_t		ZuiColor;
typedef unsigned	ZuiByte;
/**矩形*/
typedef struct _ZRect
{
	ZuiInt Left;	///左边
	ZuiInt Top;		///顶边
	ZuiInt Width;	///宽度
	ZuiInt Height;	///高度
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
	ZuiInt x;
	ZuiInt y;
} *ZuiPoint, ZPoint;
/**大小*/
typedef struct _ZSize
{
	ZuiInt Width;	///宽度
	ZuiInt Height;	///高度
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


#define	DEBUG_BORDER	1

#if defined(__cplusplus)
extern "C"
{
#endif

#include "mujs/mujs.h"
/*基础辅助函数*/
#include "core/carray.h"
#include "core/string.h"
#include "core/function.h"
#include "core/tree.h"
#include "zlib/unzip.h"
#include "core/mxml.h"
#include "core/graphic.h"
#include "core/resdb.h"
#include "core/global.h"
/*内核*/
#include "core/manager.h"		//绘制管理器
#include "core/control.h"		//控件基类
#include "core/template.h"
#include "core/builder.h"

/*控件*/
#include "control/Register.h"
#include "control/window.h"

#include "control/Label.h"
#include "control/Button.h"
#include "control/Browser.h"
#include "control/Edit.h"
#include "control/Option.h"

/*布局*/
#include "cayout/Layout.h"				//容器基类
#include "cayout/VerticalLayout.h"		//纵向布局
#include "cayout/HorizontalLayout.h"	//横向布局
#include "cayout/TileLayout.h"			//列表布局
#include "cayout/TabLayout.h"			//选择夹布局





#if defined(__cplusplus)
}
#endif

#endif
