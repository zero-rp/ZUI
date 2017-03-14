/**
* @file     Menu.h
* @brief    标签控件实现.
* @author   [Zero](22249030)
* @version  1.0
* @date     $date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __MENU_H__
#define __MENU_H__
#define Type_Menu           0xa6be6715
#define Type_MenuItem       0x43d66f6d
/**菜单结构*/
typedef struct _ZMenu
{
    BOOL bExit;
    HWND m_hWnd;
    WNDPROC m_OldWndProc;
    struct _ZMenu *m_aSubMenu;
    struct _ZMenu *m_aParentMenu;
    ZuiPaintManager m_pm;
    ZuiControl root;
    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiMenu, ZMenu;
typedef struct _ZMenuItem
{

    mxml_node_t *node;//子菜单节点
    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiMenuItem, ZMenuItem;
ZuiMenu ZuiAddMenu(mxml_node_t *node, ZuiControl win);
ZEXPORT ZuiVoid ZCALL ZuiPopupMenu(ZuiPaintManager mp, ZuiText name, ZuiPoint pt);
ZEXPORT ZuiAny ZCALL ZuiMenuProc(ZuiInt ProcId, ZuiControl cp, ZuiMenu p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZEXPORT ZuiAny ZCALL ZuiMenuItemProc(ZuiInt ProcId, ZuiControl cp, ZuiMenuItem p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif //__MENU_H__
