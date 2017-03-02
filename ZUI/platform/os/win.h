#ifndef __ZUI_PLATFORM_OS_WIN_H__
#define __ZUI_PLATFORM_OS_WIN_H__

#include <ZUI.h>
#if (defined PLATFORM_OS_WIN)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Imm.h>
#include <Mmsystem.h>
#include <tchar.h>
#include <ImageHlp.h>

typedef struct _ZOsWindow
{
    HWND hWnd;				//窗口句柄
    HDC hDC;				//窗口场景 GetDC得到
    int Layered;			//分层窗口
    HIMC   hIMC;			//输入法句柄
} *ZuiOsWindow, ZOsWindow;

#endif //PLATFORM_OS_WIN
#endif //__ZUI_PLATFORM_OS_WIN_H__