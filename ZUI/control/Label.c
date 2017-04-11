#include <ZUI.h>

ZEXPORT ZuiAny ZCALL ZuiLabelProc(ZuiInt ProcId, ZuiControl cp, ZuiLabel p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnPaintText: {
		if (!cp->m_sText)
			return;
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;
        ZRect r;
        MAKEZRECT(r, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
		if (p->font)
			ZuiDrawString(gp, p->font->p, cp->m_sText, wcslen(cp->m_sText), &r);
		else
			ZuiDrawString(gp, Global_StringFormat, cp->m_sText, wcslen(cp->m_sText), &r);
        return;
    }
    case Proc_Label_SetFont: {
        if (p->font)
            ZuiResDBDelRes(p->font);
        p->font = Param1;
        break;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"font") == 0) ZuiControlCall(Proc_Label_SetFont, cp, ZuiResDBGetRes(Param2, ZREST_FONT), NULL, NULL);
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiLabel)ZuiMalloc(sizeof(ZLabel));
        memset(p, 0, sizeof(ZLabel));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        ZuiFree(p);

        return old_call(ProcId, cp, 0, Param1, Param2, Param3);
    }
    case Proc_GetObject:
        if (Param1 == Type_Label)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_Label;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}




