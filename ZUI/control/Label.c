#include <ZUI.h>




ZEXPORT ZuiAny ZCALL ZuiLabelProc(ZuiInt ProcId, ZuiControl cp, ZuiLabel p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_CoreInit:
        return TRUE;
    case Proc_OnCreate: {
        p = (ZuiLabel)malloc(sizeof(ZLabel));
        memset(p, 0, sizeof(ZLabel));
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
        ZRect r;
        MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
        ZuiDrawString(gp, Global_StringFormat, cp->m_sText, &r);
        break;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}




