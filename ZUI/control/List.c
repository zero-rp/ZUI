#include <ZUI.h>




ZEXPORT ZuiAny ZCALL ZuiListProc(ZuiInt ProcId, ZuiControl cp, ZuiList p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnCreate: {
        p = (ZuiList)ZuiMalloc(sizeof(ZList));
        memset(p, 0, sizeof(ZList));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;
        //创建表头
        p->m_pHeader = NewZuiControl(L"ListHeader", NULL, NULL, NULL);
        ZuiVerticalLayoutProc(Proc_Layout_Add, cp, p->old_udata, p->m_pList, NULL, NULL);




        return p;
    }
                        break;
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        ZuiFree(p);

        return old_call(ProcId, cp, 0, Param1, Param2, Param3);
        break;
    }
    case Proc_OnPaintText: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = &cp->m_rcItem;

        break;
    }
    case Proc_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(L"listbody", (ZCtlProc)&ZuiListBodyProc);
        ZuiControlRegisterAdd(L"listheader", (ZCtlProc)&ZuiListHeaderProc);
        return TRUE;
    case Proc_CoreUnInit: {
        return NULL;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}

ZEXPORT ZuiAny ZCALL ZuiListBodyProc(ZuiInt ProcId, ZuiControl cp, ZuiListBody p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnCreate: {
        p = (ZuiList)ZuiMalloc(sizeof(ZList));
        memset(p, 0, sizeof(ZList));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;


        return p;
    }
                        break;
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        ZuiFree(p);

        return old_call(ProcId, cp, 0, Param1, Param2, Param3);
        break;
    }
    case Proc_OnPaintText: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = &cp->m_rcItem;

        break;
    }
    case Proc_CoreInit:
        return TRUE;
    case Proc_CoreUnInit: {
        return NULL;
        break;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}
ZEXPORT ZuiAny ZCALL ZuiListHeaderProc(ZuiInt ProcId, ZuiControl cp, ZuiListHeader p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {

    switch (ProcId)
    {
    case Proc_OnCreate: {
        p = (ZuiList)ZuiMalloc(sizeof(ZList));
        memset(p, 0, sizeof(ZList));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiHorizontalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiHorizontalLayoutProc;


        return p;
    }
                        break;
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        ZuiFree(p);

        return old_call(ProcId, cp, 0, Param1, Param2, Param3);
        break;
    }
    case Proc_OnPaintText: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = &cp->m_rcItem;

        break;
    }
    case Proc_CoreInit:
        return TRUE;
    case Proc_CoreUnInit: {
        return NULL;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}

