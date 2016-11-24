#include <ZUI.h>




ZEXPORT ZuiAny ZCALL ZuiMenuProc(ZuiInt ProcId, ZuiControl cp, ZuiMenu p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return TRUE;
	case Proc_OnCreate: {
		p = (ZuiMenu)malloc(sizeof(ZMenu));
		memset(p, 0, sizeof(ZMenu));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		p->old_udata = ZuiWindowProc(Proc_OnCreate, cp, 0, 0, 0, 0);
		p->old_call = (ZCtlProc)&ZuiWindowProc;
		ZuiControlCall(Proc_Window_SetComBo, cp, TRUE, NULL, NULL);
		ZuiControlCall(Proc_SetVisible, cp, FALSE, NULL, NULL);
		return p;
	}
		break;
	case Proc_OnPaintText: {
		ZuiGraphics gp = (ZuiGraphics)Param1;
		RECT *rc = &cp->m_rcItem;
		ZRect r;
		MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
		ZuiDrawString(gp, Global_StringFormat, cp->m_sText, &r);
		break;
	}
	default:
		break;
	}
	return p->old_call(ProcId, p->old_call, p->old_udata, Param1, Param2, Param3);
}




