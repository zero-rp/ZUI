#include <ZUI.h>




ZEXPORT ZuiAny ZCALL ZuiListProc(ZuiInt ProcId, ZuiControl cp, ZuiList p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return TRUE;
	case Proc_OnCreate: {
		p = (ZuiList)malloc(sizeof(ZList));
		memset(p, 0, sizeof(ZList));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		p->old_call = cp->call;
		return p;
	}
		break;
	case Proc_OnDestroy: {
		ZCtlProc old_call = p->old_call;

		free(p);

		return old_call(ProcId, cp, 0, Param1, Param2, Param3);
		break;
	}
	case Proc_OnPaintText: {
		ZuiGraphics gp = (ZuiGraphics)Param1;
		RECT *rc = &cp->m_rcItem;

		break;
	}
	default:
		break;
	}
	return p->old_call(ProcId, cp,0, Param1, Param2, Param3);
}

ZEXPORT ZuiAny ZCALL ZuiListItemProc(ZuiInt ProcId, ZuiControl cp, ZuiList p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {

}


