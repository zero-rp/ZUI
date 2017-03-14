#ifndef __ZUI_PLATFORM_OS_WIN_H__
#define __ZUI_PLATFORM_OS_WIN_H__

#include <ZUI.h>
#if (defined PLATFORM_OS_WIN)

/*系统头文件*/
#define WIN32_LEAN_AND_MEAN
#include <olectl.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <crtdbg.h>

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )
#pragma comment(lib, "Msimg32.lib")  
#pragma comment(lib, "Gdiplus.lib")

#if defined _M_IX86
#   pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#   pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#   pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#   pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

typedef struct _ZOsWindow
{
    HWND hWnd;				//窗口句柄
    HDC hDC;				//窗口场景 GetDC得到
    int Layered;			//分层窗口
    HIMC   hIMC;			//输入法句柄
} *ZuiOsWindow, ZOsWindow;

#endif //PLATFORM_OS_WIN
#endif //__ZUI_PLATFORM_OS_WIN_H__