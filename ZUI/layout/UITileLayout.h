#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

//横向布局
typedef struct _CTileLayoutUI
{
	SIZE m_szItem;
	int m_nColumns;
	ZCtlProc old_call;
	void *old_udata;
}*CTileLayoutUI, ITileLayoutUI;

void* CALLBACK CControlUI_Call_CTileLayoutUI(int ProcId, ZuiControl cp, CTileLayoutUI p, void* Param1, void* Param2, void* Param3);


#endif // __UITILELAYOUT_H__
