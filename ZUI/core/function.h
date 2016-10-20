#ifndef __ZUI_CORE_FUNCTION_H__
#define __ZUI_CORE_FUNCTION_H__

void Rect_Join(RECT *rc, RECT *rc1);
/*字符串HASH函数*/
void * ZCALL Zui_Hash(char* str);
/**
* 此函数用作初始化zui.
* @此函数没有返回值.
*/
ZAPI(BOOL) ZuiInit();

/**
* 此函数用作Zui消息循环.
* @param Returns int.
*/
ZAPI(ZuiInt) ZuiMsgLoop();
/**
* 此函数用作退出Zui消息循环.
* @param Returns int.
*/
ZAPI(ZuiVoid) ZuiMsgLoop_exit();
#endif //__ZUI_CORE_FUNCTION_H__
