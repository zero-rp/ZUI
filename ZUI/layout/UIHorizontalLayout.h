#ifndef __UIHORIZONTALLAYOUT_H__
#define __UIHORIZONTALLAYOUT_H__

//横向布局
typedef struct _CHorizontalLayoutUI
{
	ZCtlProc old_call;
	void *old_udata;
}*CHorizontalLayoutUI, IHorizontalLayoutUI;

void* CALLBACK CControlUI_Call_CHorizontalLayoutUI(int ProcId, ZuiControl cp, CHorizontalLayoutUI p, void* Param1, void* Param2, void* Param3);

#endif // __UIHORIZONTALLAYOUT_H__
