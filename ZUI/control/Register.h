#ifndef __Zui_CONTROLS_REGISTER_H__
#define __Zui_CONTROLS_REGISTER_H__
#include <ZUI.h>

/**
* 此函数用作注册系统默认控件.
* @此函数没有返回值.
*/
BOOL ZuiControlRegister();
/**
* 此函数用作向系统注册控件.
* @param name 控件类名 请使用全小写注册
* @此函数没有返回值.
*/
ZAPI(BOOL) ZuiControlRegisterAdd(char *name, ZCtlProc Proc);
/**
* 此函数用作删除系统中注册的控件.
* @此函数没有返回值.
*/
ZAPI(BOOL) ZuiControlRegisterDel(char *name);

#endif	//__Zui_CONTROLS_REGISTER_H__