#ifndef __TABLAYOUT_H__
#define __TABLAYOUT_H__


#define	Proc_Layout_Tab_SelectItem			1001	//添加控件


//选择夹布局
typedef struct _ZuiTabLayout
{
	int m_iCurSel;
	ZCtlProc old_call;
	void *old_udata;
}*ZuiTabLayout, ZTabLayout;

void* CALLBACK ZuiTabLayoutProc(int ProcId, ZuiControl cp, ZuiTabLayout p, void* Param1, void* Param2, void* Param3);




#endif // __UITABLAYOUT_H__
