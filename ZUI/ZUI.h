#define __ZUI_H__

#define WIN32_LEAN_AND_MEAN
#include <olectl.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>

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
#define ZAPI(Type) extern "C" __declspec(dllexport) Type __stdcall
#else
#define ZAPI(Type) __declspec(dllexport) Type __stdcall
#endif

#define ZCALL __stdcall

#else
#define ZAPI(Type) extern "C" Type __attribute__((__stdcall__))
#endif

#define ARGB(A,R,G,B) ((uint32_t)((((A)&0xff)<<24)|(((R)&0xff)<<16)|(((G)&0xff)<<8)|((B)&0xff)))
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


typedef float		ZuiReal;
#ifdef _WIN64
typedef int64_t		ZuiInt;
#else
typedef int32_t		ZuiInt;
#endif
typedef int			ZuiBool;
typedef void		ZuiVoid;
typedef void*		ZuiAny;
typedef uint32_t	ZuiColor;
typedef unsigned	ZuiByte;

typedef struct _ZRect
{
} *ZuiRect, ZRect;
typedef struct _ZRectR
{
} *ZuiRectR, ZRectR;
typedef struct _ZPoint
{
	ZuiInt x;
	ZuiInt y;
} *ZuiPoint, ZPoint;
typedef struct _ZSize
{
} *ZuiSize, ZSize;
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

#include "Core/carray.h"
#include "Core/function.h"
#include "Core/tree.h"
#include "Core/unzip.h"
#include "Core/gdi.h"
#include "Core/global.h"


#include "Control/Register.h"
#include "Control/window.h"


#include "Control/Button.h"





#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )
#pragma comment(lib, "Msimg32.lib")  
#pragma comment(lib, "Gdiplus.lib")

#endif
