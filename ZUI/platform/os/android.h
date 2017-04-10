#ifndef __ZUI_PLATFORM_OS_LINUX_H__
#define __ZUI_PLATFORM_OS_LINUX_H__

#include "../platform.h"
#include "../../zui.h"
#if (defined PLATFORM_OS_ANDROID)



typedef struct _ZOsWindow
{
	void* xwindow;
} *ZuiOsWindow, ZOsWindow;

#endif //PLATFORM_OS_ANDROID
#endif //__ZUI_PLATFORM_OS_LINUX_H__