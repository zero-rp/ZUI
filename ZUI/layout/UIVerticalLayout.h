#ifndef __UIVERTICALLAYOUT_H__
#define __UIVERTICALLAYOUT_H__
//纵向布局
typedef struct _CVerticalLayoutUI
{
	ZCtlProc old_call;
	void *old_udata;
}*CVerticalLayoutUI, IVerticalLayoutUI;

void* CALLBACK CControlUI_Call_CVerticalLayoutUI(int ProcId, ZuiControl cp, CVerticalLayoutUI p, void* Param1, void* Param2, void* Param3);
#endif // __UIVERTICALLAYOUT_H__
