#ifndef __VERTICALLAYOUT_H__
#define __VERTICALLAYOUT_H__

#include <ZUI.h>

/**纵向布局结构*/
typedef struct _ZuiVerticalLayout
{
    ZCtlProc old_call;
    void *old_udata;
}*ZuiVerticalLayout, ZVerticalLayout;

void* ZCALL ZuiVerticalLayoutProc(int ProcId, ZuiControl cp, ZuiVerticalLayout p, void* Param1, void* Param2, void* Param3);
#endif // __UIVERTICALLAYOUT_H__
