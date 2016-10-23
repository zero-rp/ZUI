#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

typedef struct ZButton
{
	int type;
	ZCtlProc old_call;
}*ZuiButton, ZButton;
ZAPI(ZuiAny) ZuiButtonProc(ZuiInt ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__UIBUTTON_H__
