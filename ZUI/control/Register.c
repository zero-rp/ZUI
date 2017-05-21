#include "Register.h"
#include <core/tree.h>
#include <core/function.h>
#include <layout/Layout.h>
#include <layout/VerticalLayout.h>
#include <layout/HorizontalLayout.h>
#include <layout/TabLayout.h>
#include <layout/TileLayout.h>

#include <control/Window.h>
#include <control/ScrollBar.h>
#include <control/Label.h>
#include <control/Button.h>
#include <control/Option.h>
#include <control/CheckBox.h>
#include <control/ProgressBar.h>
#include <control/SplitterBar.h>
#include <control/List.h>
#include <control/TreeView.h>
extern rb_root *Global_ControlClass;
static ZuiBool ZuiCoreInit(void *data) {
    return ((ZCtlProc)data)(Proc_CoreInit, NULL, NULL, NULL, NULL, NULL);
}
ZuiBool ZuiControlRegister()
{
    Global_ControlClass = rb_new();
    /*核心组件 不可卸载*/
    ZuiControlRegisterAdd(L"layout", (ZCtlProc)&ZuiLayoutProc);
    ZuiControlRegisterAdd(L"verticallayout", (ZCtlProc)&ZuiVerticalLayoutProc);
    ZuiControlRegisterAdd(L"horizontallayout", (ZCtlProc)&ZuiHorizontalLayoutProc);
    ZuiControlRegisterAdd(L"tablayout", (ZCtlProc)&ZuiTabLayoutProc);
    ZuiControlRegisterAdd(L"tilelayout", (ZCtlProc)&ZuiTileLayoutProc);
    //ZuiControlRegisterAdd(L"virtual", (ZCtlProc)&ZuiVirtualProc);
    //ZuiControlRegisterAdd(L"menubar", (ZCtlProc)&ZuiMenuBarProc);
    //ZuiControlRegisterAdd(L"menu", (ZCtlProc)&ZuiMenuProc);


    ZuiControlRegisterAdd(L"window", (ZCtlProc)&ZuiWindowProc);
    //ZuiControlRegisterAdd(L"html", (ZCtlProc)&ZuiHtmlProc);
    ZuiControlRegisterAdd(L"scrollbar", (ZCtlProc)&ZuiScrollBarProc);
    ZuiControlRegisterAdd(L"splitterbar", (ZCtlProc)&ZuiSplitterBarProc);
    ZuiControlRegisterAdd(L"label", (ZCtlProc)&ZuiLabelProc);
    ZuiControlRegisterAdd(L"ProgressBar", (ZCtlProc)&ZuiProgressBarProc);
    ZuiControlRegisterAdd(L"button", (ZCtlProc)&ZuiButtonProc);
    //ZuiControlRegisterAdd(L"browser", (ZCtlProc)&ZuiBrowserProc);
    //ZuiControlRegisterAdd(L"edit", (ZCtlProc)&ZuiEditProc);
    //ZuiControlRegisterAdd(L"singleedit", (ZCtlProc)&ZuiSingleEditProc);
    ZuiControlRegisterAdd(L"option", (ZCtlProc)&ZuiOptionProc);
    ZuiControlRegisterAdd(L"checkbox", (ZCtlProc)&ZuiCheckBoxProc);
    ZuiControlRegisterAdd(L"list", (ZCtlProc)&ZuiListProc);
    ZuiControlRegisterAdd(L"treeview", (ZCtlProc)&ZuiTreeViewProc);
    
    return TRUE;
}
static ZuiVoid ZuiControlUnRegisterCallBack(void *data) {
    ((ZCtlProc)data)(Proc_CoreUnInit, NULL, NULL, NULL, NULL, NULL);
}
ZuiVoid ZuiControlUnRegister() {
    //通知全部控件释放数据
    rb_foreach(Global_ControlClass, ZuiControlUnRegisterCallBack);
    rb_free(Global_ControlClass);
}
ZuiBool ZuiControlRegisterAdd(ZuiText name, ZCtlProc Proc)
{
    if (ZuiCoreInit(Proc))
    {
        return rb_insert((key_t)Zui_Hash(name), Proc, Global_ControlClass);
    }
    return FALSE;
}
ZuiBool ZuiControlRegisterDel(ZuiText name)
{
    rb_delete((key_t)Zui_Hash(name), Global_ControlClass);
    return 0;
}
