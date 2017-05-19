#ifndef __HORIZONTALLAYOUT_H__
#define __HORIZONTALLAYOUT_H__
#include <ZUI.h>
/**横向布局结构*/
typedef struct _ZuiHorizontalLayout
{
    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiHorizontalLayout, ZHorizontalLayout;

void* ZCALL ZuiHorizontalLayoutProc(int ProcId, ZuiControl cp, ZuiHorizontalLayout p, void* Param1, void* Param2, void* Param3);

#endif // __HORIZONTALLAYOUT_H__
