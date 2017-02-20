/**
* @file		Register.h
* @brief	控件类名注册接口.
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
#ifndef __Zui_CONTROLS_REGISTER_H__
#define __Zui_CONTROLS_REGISTER_H__
#include <ZUI.h>

/**
* 此函数用作注册系统默认控件.
* @return 此函数没有返回值.
*/
ZuiBool ZuiControlRegister();
ZuiVoid ZuiControlUnRegister();
/**
* 此函数用作向系统注册控件.
* @param name 控件类名 请使用全小写注册
* @param Proc 回调地址
* @return 此函数没有返回值.
*/
ZEXPORT ZuiBool ZCALL ZuiControlRegisterAdd(ZuiText name, ZCtlProc Proc);
/**
* 此函数用作删除系统中注册的控件.
* @param name 控件类名 请使用全小写注册
* @return 此函数没有返回值.
*/
ZEXPORT ZuiBool ZCALL ZuiControlRegisterDel(ZuiText name);

#endif	//__Zui_CONTROLS_REGISTER_H__