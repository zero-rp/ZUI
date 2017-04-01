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
#define Type_TreeView                   0xa1526942
#define Type_TreeNode                   0xa78958f4

#define Proc_TreeView_Add                   Proc_Layout_Add  
#define Proc_TreeView_AddAt                 Proc_Layout_AddAt
#define Proc_TreeView_Remove                Proc_Layout_Remove
#define Proc_TreeView_RemoveAt              Proc_Layout_RemoveAt
#define Proc_TreeView_RemoveAll             Proc_Layout_RemoveAll
#define Proc_TreeView_SetItemExpand         1000    //展开关闭
#define Proc_TreeView_SetItemCheckBox       1001    //选中反选

#define Proc_TreeNode_GetCountChild         1000
#define Proc_TreeNode_GetChildNode          1001
#define Proc_TreeNode_SetTreeView           1002
#define Proc_TreeNode_Add                   Proc_Layout_Add  
#define Proc_TreeNode_AddAt                 Proc_Layout_AddAt
#define Proc_TreeNode_Remove                Proc_Layout_Remove
#define Proc_TreeNode_AddChildNode          1003
#define Proc_TreeNode_CalLocation           1004    //计算缩进
#define Proc_TreeNode_GetLastNode           1005
#define Proc_TreeNode_GetTreeIndex          1006    //取得全局树视图的索引
#define Proc_TreeNode_GetDottedLine         1007
#define Proc_TreeNode_SetParentNode         1008    //设置父节点
#define Proc_TreeNode_GetItemButton         1009
#define Proc_TreeNode_IsHasChild            1010    //是否有子节点
#define Proc_TreeNode_SetVisibleFolderBtn   1011
#define Proc_TreeNode_GetVisibleFolderBtn   1012
#define Proc_TreeNode_SetVisibleCheckBtn    1013
#define Proc_TreeNode_GetVisibleCheckBtn    1014
#define Proc_TreeNode_GetFolderButton       1015
#define Proc_TreeNode_GetCheckBox           1016
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

    ZuiControl          pTreeView;          //所属树形框
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
