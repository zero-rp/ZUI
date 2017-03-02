#ifndef __ZUI_PLATFORM_OS_LINUX_H__
#define __ZUI_PLATFORM_OS_LINUX_H__

#include "../platform.h"
#include "../../zui.h"
#if (defined PLATFORM_OS_LINUX)

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <cairo/cairo-xcb.h>


typedef struct _ZOsWindow
{
	xcb_window_t xwindow;
} *ZuiOsWindow, ZOsWindow;

#endif //PLATFORM_OS_LINUX
#endif //__ZUI_PLATFORM_OS_LINUX_H__