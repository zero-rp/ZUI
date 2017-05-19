#include "ProgressBar.h"
#include <core/control.h>
#include <core/resdb.h>
ZEXPORT ZuiAny ZCALL ZuiProgressBarProc(ZuiInt ProcId, ZuiControl cp, ZuiProgressBar p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {

    case Proc_OnCreate: {
        p = (ZuiProgressBar)malloc(sizeof(ZProgressBar));
        memset(p, 0, sizeof(ZProgressBar));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;

        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        old_call(ProcId, cp, 0, Param1, Param2, Param3);
        free(p);

        return 0;
    }
    case Proc_GetObject:
        if (Param1 == Type_ProgressBar)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_ProgressBar;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}




