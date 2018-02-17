#include "TreeView.h"
#include <core/control.h>
#include <core/resdb.h>
#include <control/List.h>
#include "Register.h"
ZuiAny ZCALL ZuiTreeViewNotifyProc(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    if (wcscmp(L"selectchanged", msg) == 0) {
        ////选择
        //TNotifyUI* pMsg = (TNotifyUI*)param;
        //if (pMsg->sType == DUI_MSGTYPE_SELECTCHANGED)
        //{
        //    CCheckBoxUI* pCheckBox = (CCheckBoxUI*)pMsg->pSender;
        //    CTreeNodeUI* pItem = (CTreeNodeUI*)pCheckBox->GetParent()->GetParent();
        //    SetItemCheckBox(pCheckBox->GetCheck(), pItem);
        //    return TRUE;
        //}
        //return TRUE;
        ////展开
        //TNotifyUI* pMsg = (TNotifyUI*)param;
        //if (pMsg->sType == DUI_MSGTYPE_SELECTCHANGED) {
        //    CCheckBoxUI* pFolder = (CCheckBoxUI*)pMsg->pSender;
        //    CTreeNodeUI* pItem = (CTreeNodeUI*)pFolder->GetParent()->GetParent();
        //    pItem->SetVisibleTag(!pFolder->GetCheck());
        //    SetItemExpand(!pFolder->GetCheck(), pItem);
        //    return TRUE;
        //}
        return (ZuiAny)TRUE;
    }
	return 0;
}
ZEXPORT ZuiAny ZCALL ZuiTreeViewProc(ZuiInt ProcId, ZuiControl cp, ZuiTreeView p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_TreeView_Add: {
        if (!Param1)
            return FALSE;

        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) != (ZuiAny)Type_TreeNode)
            return FALSE;

        ZuiControlRegNotify(Param1, ZuiTreeViewNotifyProc);
        ZuiControlRegNotify(ZuiControlCall(Proc_TreeNode_GetFolderButton, Param1, NULL, NULL, NULL), ZuiTreeViewNotifyProc);
        ZuiControlRegNotify(ZuiControlCall(Proc_TreeNode_GetCheckBox, Param1, NULL, NULL, NULL), ZuiTreeViewNotifyProc);

        ZuiControlCall(Proc_TreeNode_SetVisibleFolderBtn, Param1, (ZuiAny)p->m_bVisibleFolderBtn, NULL, NULL);
        ZuiControlCall(Proc_TreeNode_SetVisibleCheckBtn, Param1, (ZuiAny)p->m_bVisibleCheckBtn, NULL, NULL);
        if (p->m_uItemMinWidth > 0)
            ZuiControlCall(Proc_SetMinWidth, Param1, (ZuiAny)p->m_uItemMinWidth, NULL, NULL);

        ZuiListProc(Proc_List_Add, cp, p->old_udata, Param1, NULL, NULL);

        ZuiControlCall(Proc_TreeNode_SetTreeView, Param1, cp, NULL, NULL);
        return (ZuiAny)TRUE;
    }
    case Proc_TreeView_AddAt: {
        if (!Param1)
            return (ZuiAny)-1;

        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) != (ZuiAny)Type_TreeNode)
            return (ZuiAny)-1;

        //ZuiControl* pParent = ZuiControlCall(Proc_List_GetItemAt, cp, Param2, NULL, NULL);
        //if (!pParent)
        //    return -1;

        //继承事件
        ZuiControlRegNotify(Param1, ZuiTreeViewNotifyProc);
        ZuiControlRegNotify(ZuiControlCall(Proc_TreeNode_GetFolderButton, Param1, NULL, NULL, NULL), ZuiTreeViewNotifyProc);
        ZuiControlRegNotify(ZuiControlCall(Proc_TreeNode_GetCheckBox, Param1, NULL, NULL, NULL), ZuiTreeViewNotifyProc);
        

        ZuiControlCall(Proc_TreeNode_SetVisibleFolderBtn, Param1, (ZuiAny)p->m_bVisibleFolderBtn, NULL, NULL);
        ZuiControlCall(Proc_TreeNode_SetVisibleCheckBtn, Param1, (ZuiAny)p->m_bVisibleCheckBtn, NULL, NULL);

        if (p->m_uItemMinWidth > 0)
            ZuiControlCall(Proc_SetMinWidth, Param1, (ZuiAny)p->m_uItemMinWidth, NULL, NULL);


        ZuiListProc(Proc_List_AddAt, cp, p->old_udata, Param1, Param2, NULL);

        if (ZuiControlCall(Proc_TreeNode_GetCountChild, Param1, NULL, NULL, NULL) > 0)
        {
            int nCount = (int)ZuiControlCall(Proc_TreeNode_GetCountChild, Param1, NULL, NULL, NULL);
            for (int nIndex = 0; nIndex < nCount; nIndex++)
            {
                ZuiControl pNode = ZuiControlCall(Proc_TreeNode_GetChildNode, Param1, (ZuiAny)nIndex, NULL, NULL);
                if (pNode)
                    return ZuiControlCall(Proc_TreeView_AddAt, cp, pNode, (ZuiAny)((ZuiInt)Param2 + 1), NULL);
            }
        }
        else
            return (ZuiAny)((ZuiInt)Param2 + 1);

        return (ZuiAny)-1;
    }
    case Proc_TreeView_Remove: {
        //if (pControl->GetCountChild() > 0)
        //{
        //    int nCount = pControl->GetCountChild();
        //    for (int nIndex = 0; nIndex < nCount; nIndex++)
        //    {
        //        CTreeNodeUI* pNode = pControl->GetChildNode(nIndex);
        //        if (pNode) {
        //            pControl->Remove(pNode);
        //        }
        //    }
        //}
        //CListUI::Remove(pControl);
        return (ZuiAny)TRUE;
    }
    case Proc_TreeView_RemoveAt: {
        //ZuiControl* pItem = (CTreeNodeUI*)GetItemAt(iIndex);
        //if (pItem->GetCountChild())
        //    Remove(pItem);
        return (ZuiAny)TRUE;
    }
    case Proc_TreeView_RemoveAll: {
        return ZuiListProc(Proc_List_RemoveAll, cp, p->old_udata, NULL, NULL, NULL);
    }
    case Proc_TreeView_SetItemExpand: {
        //if (Param1) {
        //    if (ZuiControlCall(Proc_TreeNode_GetCountChild, Param1, NULL, NULL, NULL) > 0) {
        //        int nCount = ZuiControlCall(Proc_TreeNode_GetCountChild, Param1, NULL, NULL, NULL);
        //        for (int nIndex = 0; nIndex < nCount; nIndex++) {
        //            ZuiControl pItem = _TreeNode->GetChildNode(nIndex);
        //            pItem->SetVisible(Param2);
        //            if (ZuiControlCall(Proc_TreeNode_GetCountChild, pItem, NULL, NULL, NULL) && !pItem->GetFolderButton()->IsSelected()) {
        //                ZuiControlCall(Proc_TreeView_SetItemExpand, cp, pItem, Param2, NULL);
        //            }
        //        }
        //    }
        //}
        //else {
        //    //没有指定控件,设置全部
        //    int nIndex = 0;
        //    int nCount = GetCount();
        //    while (nIndex < nCount) {
        //        ZuiControl pItem = (CTreeNodeUI*)GetItemAt(nIndex);
        //        pItem->SetVisible(_Expanded);
        //        if (ZuiControlCall(Proc_TreeNode_GetCountChild, pItem, NULL, NULL, NULL) && !pItem->GetFolderButton()->IsSelected()) {
        //            ZuiControlCall(Proc_TreeView_SetItemExpand, cp, pItem, Param2, NULL);
        //        }
        //        nIndex++;
        //    }
        //}
        return 0;
    }
    case Proc_TreeView_SetItemCheckBox: {
        //if (Param1) {
        //    if (_TreeNode->GetCountChild() > 0) {
        //        int nCount = _TreeNode->GetCountChild();
        //        for (int nIndex = 0; nIndex < nCount; nIndex++) {
        //            ZuiControl pItem = _TreeNode->GetChildNode(nIndex);
        //            pItem->GetCheckBox()->Selected(_Selected);
        //            if (pItem->GetCountChild())
        //                SetItemCheckBox(_Selected, pItem);
        //        }
        //    }
        //    return TRUE;
        //}
        //else {
        //    //没有指定控件,设置全部
        //    int nIndex = 0;
        //    int nCount = GetCount();
        //    while (nIndex < nCount) {
        //        ZuiControl pItem = (CTreeNodeUI*)GetItemAt(nIndex);
        //        pItem->GetCheckBox()->Selected(_Selected);
        //        if (pItem->GetCountChild())
        //            SetItemCheckBox(_Selected, pItem);

        //        nIndex++;
        //    }
        //    return TRUE;
        //}
        return FALSE;
    }
    case Proc_OnCreate: {
        p = (ZuiTreeView)malloc(sizeof(ZTreeView));
        memset(p, 0, sizeof(ZTreeView));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiListProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiListProc;
        ZuiControl pHeader = ZuiListProc(Proc_List_GetHeader, cp, p->old_udata, NULL, NULL, NULL);
        ZuiControlCall(Proc_SetVisible, pHeader, FALSE, NULL, NULL);

        p->m_bVisibleFolderBtn = TRUE;
        p->m_bVisibleCheckBtn = TRUE;
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
        free(p);

        return 0;
    }
    case Proc_GetObject:
        if (Param1 == (ZuiAny)Type_TreeView)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_TreeView;
    case Proc_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(L"treenode", (ZCtlProc)&ZuiTreeNodeProc);
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}
ZEXPORT ZuiAny ZCALL ZuiTreeNodeProc(ZuiInt ProcId, ZuiControl cp, ZuiTreeNode p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_TreeNode_Add: {
        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) == (ZuiAny)Type_TreeNode)
            return ZuiControlCall(Proc_TreeNode_AddChildNode, cp, Param1, NULL, NULL);//作为子节点插入
        return ZuiControlCall(Proc_TreeNode_Add, p->pHoriz, Param1, NULL, NULL);
        break;//普通插入,直接交给原型处理
    }
    case Proc_TreeNode_AddAt: {
        break;
    }
    case Proc_TreeNode_Remove: {
        break;
    }
    case Proc_TreeNode_AddChildNode: {
        if (!Param1)
            return FALSE;

        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) != (ZuiAny)Type_TreeNode)
            return FALSE;
        //计算缩进
        Param1 = ZuiControlCall(Proc_TreeNode_CalLocation, cp, Param1, NULL, NULL);

        ZuiBool nRet = TRUE;

        if (p->pTreeView) {
            ZuiControl pNode = darray_getat(p->mTreeNodes, p->mTreeNodes->count-1);

            if (!pNode || !ZuiControlCall(Proc_TreeNode_GetLastNode, cp, NULL, NULL, NULL))
                nRet = ZuiControlCall(Proc_TreeView_AddAt, p->pTreeView, Param1, (ZuiAny)((ZuiInt)ZuiControlCall(Proc_TreeNode_GetTreeIndex, cp, NULL, NULL, NULL) + 1), NULL) >= 0;
            else
                nRet = ZuiControlCall(Proc_TreeView_AddAt, p->pTreeView, Param1, (ZuiAny)((ZuiInt)ZuiControlCall(Proc_TreeNode_GetTreeIndex, ZuiControlCall(Proc_TreeNode_GetLastNode, pNode, NULL, NULL, NULL), NULL, NULL, NULL) + 1), NULL) >= 0;
        }

        if (nRet)
            darray_append(p->mTreeNodes, Param1);

        return (ZuiAny)nRet;
    }
    case Proc_TreeNode_GetChildNode: {
        return (ZuiAny)darray_getat(p->mTreeNodes, (int)Param1);
    }
    case Proc_TreeNode_CalLocation: {
        if (!Param1)
            return 0;
        ZuiControlCall(Proc_SetVisible, ZuiControlCall(Proc_TreeNode_GetDottedLine, Param1, NULL, NULL, NULL), (ZuiAny)TRUE, NULL, NULL);
        ZuiControlCall(Proc_SetFixedWidth,
            ZuiControlCall(Proc_TreeNode_GetDottedLine, Param1, NULL, NULL, NULL),
            (ZuiAny)((ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->pDottedLine, NULL, NULL, NULL) + 16), NULL, NULL);
        ZuiControlCall(Proc_TreeNode_SetParentNode, Param1, cp, NULL, NULL);
        //ZuiControlCall(Proc_TreeNode_GetItemButton, Param1, NULL, NULL, NULL)
        //_pTreeNodeUI->GetItemButton()->SetGroup(pItemButton->GetGroup());
        ZuiControlCall(Proc_TreeNode_SetTreeView, Param1, p->pTreeView, NULL, NULL);
        return Param1;
    }
    case Proc_TreeNode_GetLastNode: {
        if (p->mTreeNodes->count == 0)
            return cp;

        ZuiControl nRetNode = NULL;

        for (int nIndex = 0; nIndex < p->mTreeNodes->count; nIndex++) {
            ZuiControl pNode = p->mTreeNodes->data[nIndex];
            if (!pNode)
                continue;

            if (ZuiControlCall(Proc_TreeNode_IsHasChild, pNode, NULL, NULL, NULL))
                nRetNode = ZuiControlCall(Proc_TreeNode_GetLastNode, pNode, NULL, NULL, NULL);
            else
                nRetNode = pNode;
        }

        return nRetNode;
    }
    case Proc_TreeNode_GetTreeIndex: {
        if (!p->pTreeView)
            return (ZuiAny)-1;

        for (int nIndex = 0; nIndex < (int)(ZuiControlCall(Proc_List_GetCount, p->pTreeView, NULL, NULL, NULL)); nIndex++) {
            if (cp == ZuiControlCall(Proc_List_GetItemAt, p->pTreeView, (ZuiAny)nIndex, NULL, NULL))
                return (ZuiAny)nIndex;
        }

        return (ZuiAny)-1;
    }
    case Proc_TreeNode_GetDottedLine: {
        return p->pDottedLine;
    }
    case Proc_TreeNode_SetParentNode: {
        p->pParentTreeNode = Param1;
        return 0;
    }
    case Proc_TreeNode_GetItemButton: {
        return p->pItemButton;
    }
    case Proc_TreeNode_SetTreeView: {
        p->pTreeView = Param1;
        return 0;
    }
    case Proc_TreeNode_IsHasChild: {
        return (ZuiAny)(p->mTreeNodes->count != 0);
    }
    case Proc_TreeNode_SetVisibleFolderBtn: {
        return ZuiControlCall(Proc_SetVisible, p->pFolderButton, Param1, NULL, NULL);
    }
    case Proc_TreeNode_GetVisibleFolderBtn: {
        return ZuiControlCall(Proc_GetVisible, p->pFolderButton, NULL, NULL, NULL);
    }
    case Proc_TreeNode_SetVisibleCheckBtn: {
        return ZuiControlCall(Proc_SetVisible, p->pCheckBox, Param1, NULL, NULL);
    }
    case Proc_TreeNode_GetVisibleCheckBtn: {
        return ZuiControlCall(Proc_GetVisible, p->pCheckBox, NULL, NULL, NULL);
    }
    case Proc_TreeNode_GetCountChild: {
        return (ZuiAny)p->mTreeNodes->count;
    }
    case Proc_TreeNode_GetFolderButton: {
        return p->pFolderButton;
    }
    case Proc_TreeNode_GetCheckBox: {
        return p->pCheckBox;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"text") == 0)return ZuiControlCall(Proc_SetText, p->pItemButton, Param2, NULL, NULL);
        break;
    }
    case Proc_OnInit: {
        //p->pDottedLine->SetVisible(_ParentNode->IsVisible());
        //ZuiControlCall(Proc_SetFixedWidth, p->pDottedLine, (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, ZuiControlCall(Proc_TreeNode_GetDottedLine, p->pParentTreeNode, NULL, NULL, NULL), NULL, NULL, NULL)+16, NULL, NULL);
        //->SetFixedWidth(pParentTreeNode->GetDottedLine()->GetFixedWidth() + 16);
        //this->SetParentNode(_ParentNode);
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiTreeNode)malloc(sizeof(ZTreeNode));
        memset(p, 0, sizeof(ZTreeNode));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiListElementProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiListElementProc;

        p->m_dwItemTextColor = 0x00000000;
        p->m_dwItemHotTextColor = 0;
        p->m_dwSelItemTextColor = 0;
        p->m_dwSelItemHotTextColor = 0;

        p->pTreeView = NULL;
        p->m_bIsVisable = TRUE;
        p->m_bIsCheckBox = FALSE;
        p->pParentTreeNode = NULL;

        p->pHoriz = NewZuiControl(L"HorizontalLayout", NULL, NULL, NULL);
        p->pFolderButton = NewZuiControl(L"CheckBox", NULL, NULL, NULL);
        p->pDottedLine = NewZuiControl(L"Label", NULL, NULL, NULL);
        p->pCheckBox = NewZuiControl(L"CheckBox", NULL, NULL, NULL);
        p->pItemButton = NewZuiControl(L"Option", NULL, NULL, NULL);

        //加载默认资源
        ZuiControlCall(Proc_Button_SetRes, p->pFolderButton, (ZuiAny)Button_N_Res, ZuiResDBGetRes(L"default:default/treeview_a.png:src='0,0,16,16'", ZREST_IMG), NULL);
        ZuiControlCall(Proc_Button_SetRes, p->pFolderButton, (ZuiAny)Button_H_Res, ZuiResDBGetRes(L"default:default/treeview_a.png:src='16,0,16,16'", ZREST_IMG), NULL);

        ZuiControlCall(Proc_Button_SetRes, p->pCheckBox, (ZuiAny)Button_N_Res, ZuiResDBGetRes(L"default:default/unchecked.png", ZREST_IMG), NULL);
        ZuiControlCall(Proc_Button_SetRes, p->pCheckBox, (ZuiAny)Button_H_Res, ZuiResDBGetRes(L"default:default/unchecked.png", ZREST_IMG), NULL);
        ZuiControlCall(Proc_Button_SetRes, p->pCheckBox, (ZuiAny)Button_P_Res, ZuiResDBGetRes(L"default:default/unchecked.png", ZREST_IMG), NULL);
        ZuiControlCall(Proc_Option_SetRes, p->pCheckBox, (ZuiAny)Option_SN_Res, ZuiResDBGetRes(L"default:default/checked.png", ZREST_IMG), NULL);
        ZuiControlCall(Proc_Option_SetRes, p->pCheckBox, (ZuiAny)Option_SH_Res, ZuiResDBGetRes(L"default:default/checked.png", ZREST_IMG), NULL);
        ZuiControlCall(Proc_Option_SetRes, p->pCheckBox, (ZuiAny)Option_SP_Res, ZuiResDBGetRes(L"default:default/checked.png", ZREST_IMG), NULL);

        ZuiControlCall(Proc_SetFixedHeight, cp, (ZuiAny)18, NULL, NULL);
        ZuiControlCall(Proc_SetFixedWidth, cp, (ZuiAny)250, NULL, NULL);

        ZuiControlCall(Proc_SetFixedWidth, p->pFolderButton, ZuiControlCall(Proc_GetFixedHeight, cp, NULL, NULL, NULL), NULL, NULL);
        ZuiControlCall(Proc_SetFixedWidth, p->pDottedLine, (ZuiAny)2, NULL, NULL);
        ZuiControlCall(Proc_SetFixedWidth, p->pCheckBox, ZuiControlCall(Proc_GetFixedHeight, cp, NULL, NULL, NULL), NULL, NULL);
        //p->pItemButton->SetAttribute(_T("align"), _T("left"));

        ZuiControlCall(Proc_SetVisible, p->pDottedLine, FALSE, NULL, NULL);
        ZuiControlCall(Proc_SetVisible, p->pCheckBox, FALSE, NULL, NULL);

        //p->pItemButton->SetMouseEnabled(false);

        ZuiControlCall(Proc_Layout_Add, p->pHoriz, p->pDottedLine, NULL, NULL);
        ZuiControlCall(Proc_Layout_Add, p->pHoriz, p->pFolderButton, NULL, NULL);
        ZuiControlCall(Proc_Layout_Add, p->pHoriz, p->pCheckBox, NULL, NULL);
        ZuiControlCall(Proc_Layout_Add, p->pHoriz, p->pItemButton, NULL, NULL);
        ZuiListElementProc(Proc_TreeNode_Add, cp, p->old_udata, p->pHoriz, NULL, NULL);

        p->mTreeNodes = darray_create();
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
        if (p->mTreeNodes) darray_destroy(p->mTreeNodes);
        free(p);

        return 0;
    }
    case Proc_GetObject:
        if (Param1 == (ZuiAny)Type_TreeNode)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_TreeNode;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}



