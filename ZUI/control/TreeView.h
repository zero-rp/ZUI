/**
* @file     Tree.h
* @brief    树控件实现.
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
#ifndef __TREE_H__
#define __TREE_H__
#define Type_TreeView               0x0
#define Type_TreeNode               0x0

#define Proc_TreeView_Add           Proc_Layout_Add  
#define Proc_TreeView_AddAt         Proc_Layout_AddAt
#define Proc_TreeView_Remove        Proc_Layout_Remove
#define Proc_TreeView_RemoveAt      Proc_Layout_RemoveAt
#define Proc_TreeView_RemoveAll     Proc_Layout_RemoveAll

#define Proc_TreeNode_GetCountChild 1000
#define Proc_TreeNode_GetChildNode  1001
#define Proc_TreeNode_SetTreeView   1002
#define Proc_TreeNode_Add           Proc_Layout_Add  
#define Proc_TreeNode_AddAt         Proc_Layout_AddAt
#define Proc_TreeNode_Remove        Proc_Layout_Remove

/**树形框结构*/
typedef struct _ZTreeNode
{
    long	m_iTreeLavel;
    ZuiBool	m_bIsVisable;
    ZuiBool	m_bIsCheckBox;
    DWORD	m_dwItemTextColor;
    DWORD	m_dwItemHotTextColor;
    DWORD	m_dwSelItemTextColor;
    DWORD	m_dwSelItemHotTextColor;

    ZuiControl          pTreeView;
    ZuiControl          pHoriz;
    ZuiControl          pFolderButton;
    ZuiControl          pDottedLine;
    ZuiControl          pCheckBox;
    ZuiControl          pItemButton;

    ZuiControl          pParentTreeNode;    //父节点

    DArray              *mTreeNodes;        //子节点数组

    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiTreeNode, ZTreeNode;
/**树形框结构*/
typedef struct _ZTreeView
{
    ZuiUInt m_uItemMinWidth;
    ZuiBool m_bVisibleFolderBtn;
    ZuiBool m_bVisibleCheckBtn;

    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiTreeView, ZTreeView;
ZEXPORT ZuiAny ZCALL ZuiTreeViewProc(ZuiInt ProcId, ZuiControl cp, ZuiTreeView p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZEXPORT ZuiAny ZCALL ZuiTreeNodeProc(ZuiInt ProcId, ZuiControl cp, ZuiTreeNode p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

#endif	//__TREE_H__
