#include <ZUI.h>




ZEXPORT ZuiAny ZCALL ZuiMenuBarProc(ZuiInt ProcId, ZuiControl cp, ZuiMenuBar p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_CoreInit:
        return TRUE;
    case Proc_OnCreate: {
        p = (ZuiMenuBar)ZuiMalloc(sizeof(ZMenuBar));
        memset(p, 0, sizeof(ZMenuBar));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_udata = ZuiHorizontalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiHorizontalLayoutProc;


        return p;
    }
                        break;
    case Proc_OnPaintText: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;
        ZRect r;
        MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
		//ZuiDrawString(gp, Global_Font, cp->m_sText, wcslen(cp->m_sText), &r);
        break;
    }
    case Proc_CoreUnInit: {
        return NULL;
        break;
    }
    default:
        break;
    }
    return p->old_call(ProcId, p->old_call, p->old_udata, Param1, Param2, Param3);
}




