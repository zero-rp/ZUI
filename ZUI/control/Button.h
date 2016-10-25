#ifndef __BUTTON_H__
#define __BUTTON_H__
/**按钮控件结构*/
typedef struct _ZButton
{
	int type;
	ZCtlProc old_call;
}*ZuiButton, ZButton;
ZEXPORT ZuiAny ZCALL ZuiButtonProc(ZuiInt ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__BUTTON_H__
