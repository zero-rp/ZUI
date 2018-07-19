#include "export.h"
#include <stdlib.h>
static ZuiFuncs *func;

ZuiBool ZuiInitZuiFunc() {
    ZuiFuncs *p = func = (ZuiFuncs *)malloc(sizeof(*p));
    p->size = sizeof(*p);
    p->version = ZuiFuncsVersion;
     
    p->ZuiMsgLoop = &ZuiMsgLoop;
    p->ZuiMsgLoop_exit = &ZuiMsgLoop_exit;
    p->ZuiPostTask = &ZuiPostTask;
    p->NewZuiControl = &NewZuiControl;
    p->FreeZuiControl = &FreeZuiControl;
    p->ZuiControlCall = &ZuiControlCall;

}
