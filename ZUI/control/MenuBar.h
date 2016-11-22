/**
* @file		MenuBar.h
* @brief	标签控件实现.
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
#ifndef __MENUBAR_H__
#define __MENUBAR_H__

/**按钮控件结构*/
typedef struct _ZMenuBar
{

	ZCtlProc old_call;
}*ZuiMenuBar, ZMenuBar;
ZEXPORT ZuiAny ZCALL ZuiMenuBarProc(ZuiInt ProcId, ZuiControl cp, ZuiMenuBar p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__MENUBAR_H__
