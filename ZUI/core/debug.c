#include <ZUI.h>
#include "../resource.h"
#include "commctrl.h" //高级控件都要加该头文件
#if RUN_DEBUG
extern MArray *mem;
extern DArray *m_window_array;
extern ZuiControl ShowDebugRect;
extern ZuiPaintManager ShowDebugRectManager;
HWND hDlg_intab[3]; //两个要载入到TAB控件中的对话框句柄
HWND MemList;
HWND ControlTree;
HWND AttList;
ZuiControl SelectControl;
BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//两个子窗口的窗口处理过程函数申明  
BOOL WINAPI tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI tab3_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateControlTree();
void SetSelectControl(ZuiControl p);
typedef BOOL(WINAPI *DIALOGPROC)(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam); //定义一个 函数指针  
DIALOGPROC DlgProc[3] = { tab1_dlg_proc,tab2_dlg_proc,tab3_dlg_proc };


void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    default:
        break;
    }
}
void Main_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}
static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);//命令接收函数  
        HANDLE_MSG(hWnd, WM_CLOSE, Main_OnClose);//关闭对话框函数  
    case WM_NOTIFY:      //TAB控件切换发生时传送的消息  
    {
        if ((INT)wParam == IDC_TAB1) //这里也可以用一个NMHDR *nm = (NMHDR *)lParam这个指针来获取 句柄和事件  
        {
            if (((LPNMHDR)lParam)->code == TCN_SELCHANGE) //当TAB标签转换的时候发送TCN_SELCHANGE消息  
            {

                int sel = TabCtrl_GetCurSel(GetDlgItem(hWnd, IDC_TAB1));
                switch (sel)   //根据索引值查找相应的标签值，干相应的事情  
                {

                case 0: //TAB1标签时，我们要显示 tab1标签页面  
                {
                    ShowWindow(hDlg_intab[0], TRUE); //显示窗口用ShowWindow函数  
                    ShowWindow(hDlg_intab[1], FALSE);
                    ShowWindow(hDlg_intab[2], FALSE);
                    break;
                }
                case 1://TAB2标签时，我们要显示 tab2标签页面  
                {
                    ShowWindow(hDlg_intab[0], FALSE);
                    ShowWindow(hDlg_intab[1], TRUE);
                    ShowWindow(hDlg_intab[2], FALSE);
                    break;
                }
                case 2://TAB2标签时，我们要显示 tab2标签页面  
                {
                    ShowWindow(hDlg_intab[0], FALSE);
                    ShowWindow(hDlg_intab[1], FALSE);
                    ShowWindow(hDlg_intab[2], TRUE);
                    break;
                }
                }

            }
        }
        break;
    }
    }
    return FALSE;
}
BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case 0:
        {
            //MessageBox(hwnd, TEXT("你按了tab1标签问好"), TEXT(""), MB_OK);
            break;
        }
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return FALSE;
    }
    return FALSE;
}
BOOL WINAPI tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case 0:
        {
            //MessageBox(htabctrl, TEXT("你按了tab2标签问好"), TEXT(""), MB_OK);
            break;
        }
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return FALSE;
    }
    return FALSE;
}
BOOL WINAPI tab3_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case IDC_BUTTON1:
        {
            UpdateControlTree();
            break;
        }
        }
        break;
    case WM_NOTIFY:      //TAB控件切换发生时传送的消息  
    {
        if ((INT)wParam == IDC_TREE1)
        {
            LPNMTREEVIEW pTree = (NM_TREEVIEW *)lParam;
            //pTree->hdr.code = abs(pTree->hdr.code + TVN_FIRST);
            //pTree->hdr.code++;
            if (pTree->hdr.code == TVN_ITEMCHANGINGW) {
                wchar_t buf[15];
                NMTVITEMCHANGE *pnm = (NMTVITEMCHANGE *)lParam;
                TV_ITEM tvi;
                tvi.mask = TVIF_PARAM;
                tvi.hItem = pnm->hItem;
                TreeView_GetItem(ControlTree, &tvi);
                wsprintf(buf, L"%p", tvi.lParam);
                SetWindowText(GetDlgItem(hwnd, IDC_EDIT1), buf);
                SetSelectControl(tvi.lParam);
            }
            else if (pTree->hdr.code == 0xffffffef) {
                TVHITTESTINFO tsi;
                GetCursorPos(&tsi.pt);
                ScreenToClient(ControlTree, &tsi.pt);
                tsi.flags = TVHT_ONITEMLABEL;
                HTREEITEM hItem = SendMessage(ControlTree, TVM_HITTEST, 0, &tsi);
                if (hItem) {
                    TV_ITEM tvi;
                    tvi.mask = TVIF_PARAM;
                    tvi.hItem = hItem;
                    TreeView_GetItem(ControlTree, &tvi);
                    if (ShowDebugRect != tvi.lParam && tvi.lParam) {
                        ShowDebugRect = tvi.lParam;
                        ShowDebugRectManager = ShowDebugRect->m_pManager;
                        ZuiPaintManagerInvalidate(ShowDebugRect->m_pManager);
                    }
                }
                else
                {
                    if (ShowDebugRectManager) {
                        ZuiPaintManager oldShowDebugRectManager = ShowDebugRectManager;
                        ShowDebugRect = NULL;
                        ShowDebugRectManager = NULL;
                        ZuiPaintManagerInvalidate(oldShowDebugRectManager);
                        break;
                    }
                    ShowDebugRect = NULL;
                    ShowDebugRectManager = NULL;
                }

            }
            break;
        }
        break;
    }
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return FALSE;
    }
    return FALSE;
}
void SetSelectControl(ZuiControl p) {
    wchar_t buf[255];
    RECT *pos = ZuiControlCall(Proc_GetPos, p, NULL, NULL, NULL);
    swprintf(buf, L"%d,%d,%d,%d", pos->left, pos->top, pos->right, pos->bottom);
    SetWindowText(GetDlgItem(hDlg_intab[2], IDC_EDIT3), buf);

    SetWindowText(GetDlgItem(hDlg_intab[2], IDC_EDIT2), p->m_sText);



}
void InsertColumn(void)
{
    LV_COLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.pszText = L"指针";
    lvc.cx = 80;
    SendMessage(MemList, LVM_INSERTCOLUMN, 0, (long)&lvc);
    lvc.pszText = L"大小";
    lvc.cx = 40;
    SendMessage(MemList, LVM_INSERTCOLUMN, 1, (long)&lvc);
    lvc.pszText = L"时间";
    lvc.cx = 40;
    SendMessage(MemList, LVM_INSERTCOLUMN, 2, (long)&lvc);
    lvc.pszText = L"函数";
    lvc.cx = 180;
    SendMessage(MemList, LVM_INSERTCOLUMN, 3, (long)&lvc);
    lvc.pszText = L"文件";
    lvc.cx = 280;
    SendMessage(MemList, LVM_INSERTCOLUMN, 4, (long)&lvc);
    lvc.pszText = L"行号";
    lvc.cx = 50;
    SendMessage(MemList, LVM_INSERTCOLUMN, 5, (long)&lvc);

    lvc.pszText = L"属性";
    lvc.cx = 100;
    SendMessage(AttList, LVM_INSERTCOLUMN, 0, (long)&lvc);
    lvc.pszText = L"数据";
    lvc.cx = 100;
    SendMessage(AttList, LVM_INSERTCOLUMN, 1, (long)&lvc);
}


VOID CALLBACK UPDATEMEMTIME(HWND H, UINT U, UINT_PTR Pt, DWORD D) {
    return;
    LVITEM lvItem = { 0 };
    WCHAR buf[1024];
    int hash = 0;
    static int oldhash = 0;
    for (size_t i = 0; i < mem->count; i++)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ (int)mem->data[i] ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ (int)mem->data[i] ^ (hash >> 5)));
        }
    }
    if (oldhash == hash) {
        //未变化就只刷新时间
        for (size_t i = 0; i < ListView_GetItemCount(MemList); i++)
        {
            lvItem.mask = LVIF_PARAM;    //说明pszText是有效的
            lvItem.iItem = i;        //行号
            lvItem.iSubItem = 0;        //列号
            ListView_GetItem(MemList, (LPARAM)(LV_ITEM *)&lvItem);

            lvItem.iSubItem = 2;
            wsprintf(buf, L"%d", (GetTickCount() - lvItem.lParam) / 1000);
            lvItem.pszText = buf;
            SendMessageA(MemList, LVM_SETITEMTEXT, i, (LPARAM)(LV_ITEM *)&lvItem);
        }
        return;
    }
    oldhash = hash;
    SendMessage(MemList, LVM_DELETEALLITEMS, 0, 0);

    for (size_t i = 0; i < mem->count; i++)
    {
        MEM* p = ((MEM *)((char *)mem->data[i] - sizeof(MEM)));
        //先需要插入一行，才能对这行输入内容
        lvItem.mask = LVIF_PARAM;
        lvItem.iSubItem = 0;
        lvItem.lParam = p->timer;
        ListView_InsertItem(MemList, (LPARAM)&lvItem);

        lvItem.iSubItem = 0;
        wsprintf(buf, L"0x%p", p->ptr);
        lvItem.pszText = buf;
        SendMessageA(MemList, LVM_SETITEMTEXT, 0, (LPARAM)(LV_ITEM *)&lvItem);
        lvItem.iSubItem = 1;
        wsprintf(buf, L"%d", p->_Size);
        lvItem.pszText = buf;
        SendMessageA(MemList, LVM_SETITEMTEXT, 0, (LPARAM)(LV_ITEM *)&lvItem);
        lvItem.iSubItem = 2;
        wsprintf(buf, L"%d", (GetTickCount() - p->timer) / 1000);
        lvItem.pszText = buf;
        SendMessageA(MemList, LVM_SETITEMTEXT, 0, (LPARAM)(LV_ITEM *)&lvItem);
        lvItem.iSubItem = 3;
        ZuiAsciiToUnicode(p->_Func, -1, buf, 1024);
        lvItem.pszText = buf;
        SendMessageA(MemList, LVM_SETITEMTEXT, 0, (LPARAM)(LV_ITEM *)&lvItem);
        lvItem.iSubItem = 4;
        ZuiAsciiToUnicode(p->_File, -1, buf, 1024);
        lvItem.pszText = buf;
        SendMessageA(MemList, LVM_SETITEMTEXT, 0, (LPARAM)(LV_ITEM *)&lvItem);
        lvItem.iSubItem = 5;
        wsprintf(buf, L"%d", p->_Line);
        lvItem.pszText = buf;
        SendMessageA(MemList, LVM_SETITEMTEXT, 0, (LPARAM)(LV_ITEM *)&lvItem);
    }
}
//刷新控件树
ZuiControl CALLBACK __FindControlFromAll(ZuiControl pThis, LPVOID pData)
{
    TVINSERTSTRUCT pis;
    WCHAR buf[1024];
    wsprintf(buf, L"%s|%s", pThis->m_sClassName, pThis->m_sText ? pThis->m_sText : L"NULL");
    pis.hParent = pThis->m_pParent ? pThis->m_pParent->m_aTreeHwndl : TVI_ROOT;
    pis.hInsertAfter = TVI_LAST;
    pis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
    pis.item.pszText = buf;
    pis.item.lParam = pThis;
    pis.item.state = TVIS_EXPANDED;
    pThis->m_aTreeHwndl = TreeView_InsertItem(ControlTree, &pis);
    return NULL;
}

void UpdateControlTree() {

    TreeView_DeleteAllItems(ControlTree);

    for (size_t i = 0; i < m_window_array->count; i++)
    {
        ZuiControlCall(Proc_FindControl, m_window_array->data[i], __FindControlFromAll, NULL, (void *)(ZFIND_ME_FIRST));
    }
}
ZuiVoid ZuiStartDebug() {
    InitCommonControls();
    HWND hwnd = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DEBUG), GetDesktopWindow(), __WndProc);

    HWND htabctrl = GetDlgItem(hwnd, IDC_TAB1);
    TCITEM tie;//设置tab标签的属性  
    LPWSTR tabname[5] = { L"内存跟踪",L"资源管理",L"控件Spy",L"脚本调试",L"日志" };
    tie.mask = TCIF_TEXT | TCIF_IMAGE;//psztext字段有效  
    tie.iImage = -1;
    for (INT i = 0; i < 3; i++)
    {
        tie.pszText = tabname[i];
        TabCtrl_InsertItem(htabctrl, i, &tie);
    }
    //在tab标签中显示 别的控件  
    RECT rect;//存放tab控件的区域位置  
    GetClientRect(htabctrl, &rect);
    // 将两个窗口往 tab控件位置移动  
    for (int i = 0; i < 3; i++)
    {
        hDlg_intab[i] = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_FORMVIEW + i), htabctrl, DlgProc[i]);
        MoveWindow(hDlg_intab[i], 2, 23, rect.right - rect.left - 6, rect.bottom - rect.top - 26, FALSE);
    }
    if (hDlg_intab[0]) {
        MemList = GetDlgItem(hDlg_intab[0], IDC_LIST1);
        SendMessage(MemList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_UNDERLINEHOT);
        ShowWindow(MemList, SW_SHOW);
        SetTimer(0, 0, 1000, UPDATEMEMTIME);
    }
    if (hDlg_intab[2]) {
        ControlTree = GetDlgItem(hDlg_intab[2], IDC_TREE1);
        AttList = GetDlgItem(hDlg_intab[2], IDC_LIST1);
        SendMessage(AttList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_UNDERLINEHOT);
        UpdateControlTree();
    }
    InsertColumn();
    ShowWindow(hDlg_intab[0], SW_SHOW);
    UpdateWindow(hwnd);

}

#endif