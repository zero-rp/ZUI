#include <ZUI.h>
#include "CodeEdit.h"



ZEXPORT ZuiAny ZCALL ZuiCodeEditProc(ZuiInt ProcId, ZuiControl cp, ZuiCodeEdit p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return LoadLibrary(_T("SciLexer.dll"));
	case Proc_OnCreate: {
		p = (ZuiCodeEdit)malloc(sizeof(ZCodeEdit));
		memset(p, 0, sizeof(ZCodeEdit));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		p->old_udata = ZuiVirtualProc(Proc_OnCreate, cp, 0, 0, 0, 0);
		p->old_call = (ZCtlProc)&ZuiVirtualProc;
		
		return p;
	}
		break;
	case Proc_OnInit: {
		ZuiVirtual op = (ZuiVirtual)p->old_udata;
		op->m_hwnd = CreateWindow(_T("Scintilla"), NULL, 1342177280,
			((SIZE *)ZuiControlCall(Proc_GetFixedXY, cp, 0, 0, 0))->cx, ((SIZE *)ZuiControlCall(Proc_GetFixedXY, cp, 0, 0, 0))->cy,
			(LONG)ZuiControlCall(Proc_GetFixedWidth, cp, 0, 0, 0), (LONG)ZuiControlCall(Proc_GetFixedHeight, cp, 0, 0, 0),
			cp->m_pManager->m_hWndPaint,
			NULL, m_hInstance, NULL);
		if (cp->m_bVisible)
			ShowWindow(op->m_hwnd, TRUE);
		p->fnDirect = (SciFnDirect)SendMessage(op->m_hwnd, SCI_GETDIRECTFUNCTION, 0, 0);
		p->ptrDirect = (sptr_t)SendMessage(op->m_hwnd, SCI_GETDIRECTPOINTER, 0, 0);
		p->fnDirect(p->ptrDirect, SCI_SETCODEPAGE, SC_CP_UTF8, NULL);
		p->fnDirect(p->ptrDirect, SCI_SETTABWIDTH, 4, NULL);//TAB宽度
		p->fnDirect(p->ptrDirect, SCI_SETCARETLINEVISIBLE, TRUE, NULL);//高亮当前行
		p->fnDirect(p->ptrDirect, SCI_SETCARETLINEBACK, 0xb0ffff, NULL);

		p->fnDirect(p->ptrDirect, SCI_SETMARGINTYPEN, 2, SC_MARGIN_NUMBER);//显示行号
		p->fnDirect(p->ptrDirect, SCI_SETMARGINWIDTHN, 2, 20);
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
	return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}




