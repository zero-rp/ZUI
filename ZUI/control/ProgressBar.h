#ifndef __ProgressBar_H__
#define __ProgressBar_H__
#include <ZUI.h>

/**按钮控件结构*/
typedef struct _ZProgressBar
{

    ZCtlProc old_call;
}*ZuiProgressBar, ZProgressBar;
ZEXPORT ZuiAny ZCALL ZuiProgressBarProc(ZuiInt ProcId, ZuiControl cp, ZuiProgressBar p, ZuiAny Param1, ZuiAny Param2);


#endif	//__ProgressBar_H__
