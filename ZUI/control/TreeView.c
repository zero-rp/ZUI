#include <ZUI.h>

ZEXPORT ZuiAny ZCALL ZuiTreeViewProc(ZuiInt ProcId, ZuiControl cp, ZuiTreeView p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3){
    switch (ProcId)
    {
    case Proc_TreeView_Add: {
        if (!Param1)
            return FALSE;

        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) != Type_TreeNode)
            return FALSE;


        if (p->m_uItemMinWidth > 0)
            ZuiControlCall(Proc_SetMinWidth, Param1, p->m_uItemMinWidth, NULL, NULL);

        ZuiListProc(Proc_List_Add, cp, p->old_udata, Param1, NULL, NULL);
        if (ZuiControlCall(Proc_TreeNode_GetCountChild, Param1, NULL, NULL, NULL) > 0)
        {
            int nCount = ZuiControlCall(Proc_TreeNode_GetCountChild, Param1, NULL, NULL, NULL);
            for (int nIndex = 0; nIndex < nCount; nIndex++)
            {
                ZuiControl pNode = ZuiControlCall(Proc_TreeNode_GetChildNode, Param1, nIndex, NULL, NULL);
                if (pNode)
                    ZuiControlCall(Proc_TreeView_Add, cp, pNode, NULL, NULL);
            }
        }

        ZuiControlCall(Proc_TreeNode_SetTreeView, Param1, cp, NULL, NULL);
        return TRUE;
    }
    case Proc_TreeView_AddAt: {

    }
    case Proc_TreeView_Remove: {

    }
    case Proc_TreeView_RemoveAt: {
    
    }
    case Proc_TreeView_RemoveAll: {
    
    }
    case Proc_OnCreate: {
        p = (ZuiTreeView)ZuiMalloc(sizeof(ZTreeView));
        memset(p, 0, sizeof(ZTreeView));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiListProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiListProc;
        ZuiControl pHeader = ZuiListProc(Proc_List_GetHeader, cp, p->old_udata, NULL, NULL, NULL);
        ZuiControlCall(Proc_SetVisible, pHeader, FALSE, NULL, NULL);



        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject:
        if (Param1 == Type_TreeView)
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
    case Proc_OnCreate: {
        p = (ZuiTreeNode)ZuiMalloc(sizeof(ZTreeNode));
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
        //p->pFolderButton = NewZuiControl(L"ListHeader", NULL, NULL, NULL); new CCheckBoxUI();
        p->pDottedLine = NewZuiControl(L"Label", NULL, NULL, NULL);
        //p->pCheckBox = NewZuiControl(L"ListHeader", NULL, NULL, NULL); new CCheckBoxUI();
        p->pItemButton = NewZuiControl(L"Option", NULL, NULL, NULL);
        
        ZuiControlCall(Proc_SetFixedHeight, cp, 18, NULL, NULL, NULL);
        ZuiControlCall(Proc_SetFixedWidth, cp, 250, NULL, NULL, NULL);

        //p->pFolderButton->SetFixedWidth(GetFixedHeight());
        ZuiControlCall(Proc_SetFixedWidth, p->pDottedLine, 2, NULL, NULL, NULL);
        //p->pCheckBox->SetFixedWidth(GetFixedHeight());
        //p->pItemButton->SetAttribute(_T("align"), _T("left"));

        ZuiControlCall(Proc_SetVisible, p->pDottedLine, FALSE, NULL, NULL);
        ZuiControlCall(Proc_SetVisible, p->pCheckBox, FALSE, NULL, NULL);

        //p->pItemButton->SetMouseEnabled(false);

        ZuiControlCall(Proc_Layout_Add, p->pHoriz, p->pDottedLine, NULL, NULL);
        ZuiControlCall(Proc_Layout_Add, p->pHoriz, p->pFolderButton, NULL, NULL);
        ZuiControlCall(Proc_Layout_Add, p->pHoriz, p->pCheckBox, NULL, NULL);
        ZuiControlCall(Proc_Layout_Add, p->pHoriz, p->pItemButton, NULL, NULL);
        ZuiControlCall(Proc_TreeNode_Add, cp, p->pHoriz, NULL, NULL);

        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject:
        if (Param1 == Type_TreeNode)
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



