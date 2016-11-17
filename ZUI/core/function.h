/**
* @file		function.h
* @brief	公共函数接口.
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
#ifndef __ZUI_CORE_FUNCTION_H__
#define __ZUI_CORE_FUNCTION_H__

void Rect_Join(RECT *rc, RECT *rc1);
ZEXPORT ZuiBool ZCALL ZuiIsPointInRect(ZuiRect Rect, ZuiPoint pt);
/*字符串HASH函数*/
void * ZCALL Zui_Hash(wchar_t* str);
/**
* 此函数用作初始化zui.
* @return 此函数没有返回值.
*/
ZEXPORT ZuiBool ZCALL ZuiInit();

/**
* 此函数用作Zui消息循环.
* @return int.
*/
ZEXPORT ZuiInt ZCALL ZuiMsgLoop();
/**
* 此函数用作退出Zui消息循环.
* @return int.
*/
ZEXPORT ZuiVoid ZCALL ZuiMsgLoop_exit();
ZEXPORT ZuiVoid ZCALL ZuiMsgBox();
#endif //__ZUI_CORE_FUNCTION_H__
