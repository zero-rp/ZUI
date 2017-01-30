#include <ZUI.h>



ZEXPORT ZuiAny ZCALL ZuiVirtualProc(ZuiInt ProcId, ZuiControl cp, ZuiVirtual p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return TRUE;
	case Proc_OnCreate: {
		p = (ZuiVirtual)malloc(sizeof(ZVirtual));
		memset(p, 0, sizeof(ZVirtual));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		p->old_call = cp->call;

		return p;
	}
		break;
	case Proc_SetPos: {
		p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
		if (p->m_hwnd)
			MoveWindow(p->m_hwnd,
				cp->m_rcItem.left, cp->m_rcItem.top,
				cp->m_rcItem.right - cp->m_rcItem.left, cp->m_rcItem.bottom - cp->m_rcItem.top, TRUE);
		return;
	}
	case Proc_SetVisible: {
		if (p->m_hwnd)
			ShowWindow(p->m_hwnd, Param1);
	}
		break;
	case Proc_OnPaint: {
		if (p->m_hwnd)
			InvalidateRect(p->m_hwnd, NULL, TRUE);
		return 0;
	}
		break;
	case Proc_OnDestroy: {
		if (p->m_hwnd)
			DestroyWindow(p->m_hwnd);
		ZCtlProc old_call = p->old_call;

		free(p);

		return old_call(ProcId, cp, 0, Param1, Param2, Param3);
	}
		break;
	case Proc_Virtual_SetHost: {
		if (p->m_hwnd)
			DestroyWindow(p->m_hwnd);
		p->m_hwnd = Param1;
	}
		break;
	case Proc_SetText: {
		SetWindowText(p->m_hwnd, Param1);

	}
		break;
	default:
		break;
	}
	return p->old_call(ProcId, cp,0, Param1, Param2, Param3);
}




