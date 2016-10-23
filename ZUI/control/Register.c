#include <ZUI.h>

rb_root *Global_ControlClass;
BOOL ZuiControlRegister()
{
	Global_ControlClass = rb_new();
	/*核心组件 不可卸载*/
	ZuiControlRegisterAdd("layout", (ZCtlProc)&ZuiLayoutProc);
	ZuiControlRegisterAdd("verticallayout", (ZCtlProc)&ZuiVerticalLayoutProc);
	ZuiControlRegisterAdd("horizontallayout", (ZCtlProc)&ZuiHorizontalLayoutProc);
	ZuiControlRegisterAdd("tablayout", (ZCtlProc)&ZuiTabLayoutProc);
	ZuiControlRegisterAdd("tilelayout", (ZCtlProc)&ZuiTileLayoutProc);

	ZuiControlRegisterAdd("window", (ZCtlProc)&ZuiWindowProc);
	ZuiControlRegisterAdd("button", (ZCtlProc)&ZuiButtonProc);


	return TRUE;
}
ZAPI(BOOL) ZuiControlRegisterAdd(char *name, ZCtlProc Proc)
{
	return rb_insert((key_t)Zui_Hash(name), Proc, Global_ControlClass);
}
ZAPI(BOOL) ZuiControlRegisterDel(char *name)
{
	rb_delete((key_t)Zui_Hash(name), Global_ControlClass);
	return 0;
}
