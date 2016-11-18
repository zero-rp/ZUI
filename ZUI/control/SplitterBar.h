/**
* @file		SplitterBar.h
* @brief	分割条实现.
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
#ifndef __SplitterBar_H__
#define __SplitterBar_H__

/**按钮控件结构*/
typedef struct _ZSplitterBar
{
	BOOL mousedown;
	ZuiRes font;	//字体控制
	ZCtlProc old_call;
}*ZuiSplitterBar, ZSplitterBar;
ZEXPORT ZuiAny ZCALL ZuiSplitterBarProc(ZuiInt ProcId, ZuiControl cp, ZuiSplitterBar p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__SplitterBar_H__
