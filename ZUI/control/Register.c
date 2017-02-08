#include <ZUI.h>

rb_root *Global_ControlClass;
BOOL ZuiCoreInit(void *data) {
    return ((ZCtlProc)data)(Proc_CoreInit, NULL, NULL, NULL, NULL, NULL);
}
BOOL ZuiControlRegister()
{
    Global_ControlClass = rb_new();
    /*核心组件 不可卸载*/
    ZuiControlRegisterAdd(L"layout", (ZCtlProc)&ZuiLayoutProc);
    ZuiControlRegisterAdd(L"verticallayout", (ZCtlProc)&ZuiVerticalLayoutProc);
    ZuiControlRegisterAdd(L"horizontallayout", (ZCtlProc)&ZuiHorizontalLayoutProc);
    ZuiControlRegisterAdd(L"tablayout", (ZCtlProc)&ZuiTabLayoutProc);
    ZuiControlRegisterAdd(L"tilelayout", (ZCtlProc)&ZuiTileLayoutProc);
    ZuiControlRegisterAdd(L"virtual", (ZCtlProc)&ZuiVirtualProc);
    ZuiControlRegisterAdd(L"menubar", (ZCtlProc)&ZuiMenuBarProc);
    ZuiControlRegisterAdd(L"menu", (ZCtlProc)&ZuiMenuProc);
    ZuiControlRegisterAdd(L"menuitem", (ZCtlProc)&ZuiMenuItemProc);


    ZuiControlRegisterAdd(L"window", (ZCtlProc)&ZuiWindowProc);
    ZuiControlRegisterAdd(L"html", (ZCtlProc)&ZuiHtmlProc);
    ZuiControlRegisterAdd(L"scrollbar", (ZCtlProc)&ZuiScrollBarProc);
    ZuiControlRegisterAdd(L"splitterbar", (ZCtlProc)&ZuiSplitterBarProc);
    ZuiControlRegisterAdd(L"label", (ZCtlProc)&ZuiLabelProc);
    ZuiControlRegisterAdd(L"button", (ZCtlProc)&ZuiButtonProc);
    ZuiControlRegisterAdd(L"browser", (ZCtlProc)&ZuiBrowserProc);
    ZuiControlRegisterAdd(L"edit", (ZCtlProc)&ZuiEditProc);
    ZuiControlRegisterAdd(L"option", (ZCtlProc)&ZuiOptionProc);
    ZuiControlRegisterAdd(L"list", (ZCtlProc)&ZuiListProc);
    ZuiControlRegisterAdd(L"listitem", (ZCtlProc)&ZuiListItemProc);

    /*初始化全部控件*/
    //rb_foreach(Global_ControlClass, ZuiCoreInit);
    return TRUE;
}
ZEXPORT ZuiBool ZCALL ZuiControlRegisterAdd(ZuiText name, ZCtlProc Proc)
{
    if (ZuiCoreInit(Proc))
    {
        return rb_insert((key_t)Zui_Hash(name), Proc, Global_ControlClass);
    }
    return FALSE;
}
ZEXPORT ZuiBool ZCALL ZuiControlRegisterDel(ZuiText name)
{
    rb_delete((key_t)Zui_Hash(name), Global_ControlClass);
    return 0;
}
