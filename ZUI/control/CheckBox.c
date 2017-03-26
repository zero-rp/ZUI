#include <ZUI.h>

ZEXPORT ZuiAny ZCALL ZuiCheckBoxProc(ZuiInt ProcId, ZuiControl cp, ZuiCheckBox p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {

    case Proc_OnCreate: {
        p = (ZuiCheckBox)ZuiMalloc(sizeof(ZCheckBox));
        memset(p, 0, sizeof(ZCheckBox));
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
        if (Param1 == Type_CheckBox)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_CheckBox;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}




