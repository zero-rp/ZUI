#ifndef __HORIZONTALLAYOUT_H__
#define __HORIZONTALLAYOUT_H__

/**横向布局结构*/
typedef struct _ZuiHorizontalLayout
{
	ZCtlProc old_call;
	void *old_udata;
}*ZuiHorizontalLayout, ZHorizontalLayout;

void* CALLBACK ZuiHorizontalLayoutProc(int ProcId, ZuiControl cp, ZuiHorizontalLayout p, void* Param1, void* Param2, void* Param3);

#endif // __HORIZONTALLAYOUT_H__
