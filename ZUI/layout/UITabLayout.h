#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__


#define	Proc_CContainerUI_SelectItem			1001	//添加控件


//选择夹布局
typedef struct _CTabLayoutUI
{
	int m_iCurSel;
	ZCtlProc old_call;
	void *old_udata;
}*CTabLayoutUI, ITabLayoutUI;

void* CALLBACK CControlUI_Call_CTabLayoutUI(int ProcId, ZuiControl cp, CTabLayoutUI p, void* Param1, void* Param2, void* Param3);




#endif // __UITABLAYOUT_H__
