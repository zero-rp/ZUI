#ifndef __ZUI_PLATFORM_OS_H__
#define __ZUI_PLATFORM_OS_H__

#include <ZUI.h>

/**
* 此函数用作初始化Os.
* @param Returns 成功返回true.
*/
ZuiBool ZuiOsInitialize();
/**
* 此函数用作反初始化Os.
* @param Returns 成功返回true.
*/
ZuiBool ZuiOsUnInitialize();

/**
* 此函数用作创建ZuiOsWindow对象.
* @param Rect 窗口矩形
* @param Title 窗口标题
* @param ShowInTaskbar 是否在任务栏显示
* @param PosMiddle 是否居中
* @param Window 框架层ZuiWindow指针
* @param Returns 成功返回ZuiOsWindow对象,不成功返回NULL.
*/
ZuiOsWindow ZuiOsCreateWindow(ZuiControl root,ZuiBool show);
/**
* 此函数用作销毁由ZuiOsCreateWindow创建的对象.
* @param OsWindow ZuiOsWindow对象
* @此函数没有返回值.
*/
ZuiVoid ZuiOsDestroyWindow(ZuiOsWindow OsWindow);
/**
* 此函数用作设置窗口标题.
* @param OsWindow ZuiOsWindow对象
* @param Title 标题
* @此函数没有返回值.
*/
ZuiBool ZuiOsSetWindowTitle(ZuiOsWindow OsWindow, ZuiText Title);
/**
* 此函数用做最小化窗口
* @param OsWindow ZuiOsWindow对象
* @此函数没有返回值.
*/
ZuiBool ZuiOsSetWindowMin(ZuiOsWindow OsWindow);
/**
* 此函数用做最大化窗口
* @param OsWindow ZuiOsWindow对象
* @此函数没有返回值.
*/
ZuiBool ZuiOsSetWindowMax(ZuiOsWindow OsWindow);
/**
* 此函数用做向下还原窗口
* @param OsWindow ZuiOsWindow对象
* @此函数没有返回值.
*/
ZuiBool ZuiOsSetWindowRestor(ZuiOsWindow OsWindow);
ZuiBool ZuiOsSetWindowSize(ZuiOsWindow OsWindow, ZuiUInt w, ZuiUInt h);
ZuiBool ZuiOsSetWindowNoBox(ZuiOsWindow OsWindow, ZuiBool b);
ZuiBool ZuiOsSetWindowComBo(ZuiOsWindow OsWindow, ZuiBool b);
ZuiBool ZuiOsSetWindowTool(ZuiOsWindow OsWindow, ZuiBool b);
/**
* 此函数用作设置窗口可视状态.
* @param OsWindow ZuiOsWindow对象
* @param Visible 可视状态
* @此函数没有返回值.
*/
ZuiVoid ZuiOsSetWindowVisible(ZuiOsWindow OsWindow, ZuiBool Visible);
ZuiVoid ZuiOsWindowPopup(ZuiOsWindow OsWindow, ZuiPoint pt);
ZuiVoid ZuiOsSetWindowCenter(ZuiOsWindow OsWindow);
#endif //__ZUI_PLATFORM_OS_H__