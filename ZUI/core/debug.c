#include <ZUI.h>
#include <io.h>
#include <fcntl.h>  
#include <stdio.h>  
#include "../resource.h"
#include "commctrl.h" //高级控件都要加该头文件
#include "duk_trans_dvalue.h"

#if RUN_DEBUG
extern MArray *mem;
extern DArray *m_window_array;
extern ZuiControl ShowDebugRect;
extern ZuiPaintManager ShowDebugRectManager;
HWND hDlg_intab[6]; //两个要载入到TAB控件中的对话框句柄
HWND MemList;
HWND ControlTree;
HWND AttList;
ZuiControl SelectControl;
BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI tab3_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI tab4_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI tab5_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI tab6_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateControlTree();
void SetSelectControl(ZuiControl p);
typedef BOOL(WINAPI *DIALOGPROC)(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam); //定义一个 函数指针  
DIALOGPROC DlgProc[6] = { tab1_dlg_proc,tab2_dlg_proc,tab3_dlg_proc,tab4_dlg_proc,tab5_dlg_proc,tab6_dlg_proc };


static void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    default:
        break;
    }
}
static void Main_OnClose(HWND hwnd)
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
                for (size_t i = 0; i < 6; i++)
                {
                    if(i==sel)
                        ShowWindow(hDlg_intab[i], TRUE); //显示窗口用ShowWindow函数
                    else
                        ShowWindow(hDlg_intab[i], FALSE); //显示窗口用ShowWindow函数
                }
            }
        }
        break;
    }
    }
    return FALSE;
}
//Tab回调
static BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
static BOOL WINAPI tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
static BOOL WINAPI tab3_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
static BOOL WINAPI tab4_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
static BOOL WINAPI tab5_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
static BOOL WINAPI tab6_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

static void SetSelectControl(ZuiControl p) {
    wchar_t buf[255];
    ZRect *pos = ZuiControlCall(Proc_GetPos, p, NULL, NULL, NULL);
    swprintf(buf, L"%d,%d,%d,%d", pos->left, pos->top, pos->right, pos->bottom);
    SetWindowText(GetDlgItem(hDlg_intab[2], IDC_EDIT3), buf);

    SetWindowText(GetDlgItem(hDlg_intab[2], IDC_EDIT2), p->m_sText);



}
static void InsertColumn(void)
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

//刷新控件树
static ZuiControl ZCALL __FindControlFromAll(ZuiControl pThis, LPVOID pData)
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
    TreeView_Expand(ControlTree, pis.hParent, TVE_EXPAND);
    return NULL;
}
static void UpdateControlTree() {

    TreeView_DeleteAllItems(ControlTree);
    if (!m_window_array)
        return;
    for (size_t i = 0; i < m_window_array->count; i++)
    {
        ZuiControlCall(Proc_FindControl, m_window_array->data[i], __FindControlFromAll, NULL, (void *)(ZFIND_ME_FIRST));
    }
}

//刷新数据时钟 1s
static VOID CALLBACK UPDATETIME(HWND H, UINT U, UINT_PTR Pt, DWORD D) {
    
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
    
    for (size_t i = mem->count-1; i > mem->count-14; i--)
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
    wsprintf(buf, L"%d", mem->count);
    SetWindowText(GetDlgItem(hDlg_intab[0], IDC_STATIC1), buf);
    wsprintf(buf, L"%d", mem->msize);
    SetWindowText(GetDlgItem(hDlg_intab[0], IDC_STATIC2), buf);
}
ZuiVoid ZuiDebugLog(ZuiInt type, const wchar_t *fmt) {

    /* 以下两条语句为在edit中追加字符串 */
    SendMessage(GetDlgItem(hDlg_intab[4], IDC_EDIT2), EM_SETSEL, -2, -1);
    SendMessage(GetDlgItem(hDlg_intab[4], IDC_EDIT2), EM_REPLACESEL, TRUE, (long)fmt);

    /* 设置滚轮到末尾，这样就可以看到最新信息 */
    SendMessage(GetDlgItem(hDlg_intab[4], IDC_EDIT2), WM_VSCROLL, SB_BOTTOM, 0);
}

//---------------------------------------------------------------------------Debug
static void duv_cooperate(duk_trans_dvalue_ctx *ctx, int block) {
    static int first_blocked = 1;

    if (!block) {
        /* Duktape is not blocked; you can cooperate with e.g. a user
        * interface here and send dvalues to Duktape, but don't block.
        */
        return;
    }

    //duk_trans_dvalue_send_req_cmd(ctx, 0x14);  /* 0x14 = StepOver */
    duk_trans_dvalue_send_eom(ctx);
}
static void duv_received(duk_trans_dvalue_ctx *ctx, duk_dvalue *dv) {
    char buf[DUK_DVALUE_TOSTRING_BUFLEN];
    (void)ctx;


    duk_dvalue_free(dv);
}
static void duv_handshake(duk_trans_dvalue_ctx *ctx, const char *line) {
    (void)ctx;

}
static void duv_detached(duk_trans_dvalue_ctx *ctx) {
    (void)ctx;

}
static void duv_debug(duk_context *ctx) {
    duk_trans_dvalue_ctx *trans_ctx;
    trans_ctx = duk_trans_dvalue_init();
    if (!trans_ctx) {

    }
    trans_ctx->cooperate = duv_cooperate;
    trans_ctx->received = duv_received;
    trans_ctx->handshake = duv_handshake;
    trans_ctx->detached = duv_detached;

    /* Attach debugger; this will fail with a fatal error here unless
    * debugger support is compiled in.  To fail more gracefully, call
    * this under a duk_safe_call() to catch the error.
    */
    duk_debugger_attach(ctx,
        duk_trans_dvalue_read_cb,
        duk_trans_dvalue_write_cb,
        duk_trans_dvalue_peek_cb,
        duk_trans_dvalue_read_flush_cb,
        duk_trans_dvalue_write_flush_cb,
        NULL,  /* app request cb */
        duk_trans_dvalue_detached_cb,
        (void *)trans_ctx);
}

//---------------------------------------------------------------------------Debug_End

ZuiVoid ZuiStartDebug() {
    InitCommonControls();
    HWND hwnd = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DEBUG), GetDesktopWindow(), __WndProc);

    HWND htabctrl = GetDlgItem(hwnd, IDC_TAB1);
    TCITEM tie;//设置tab标签的属性  
    LPWSTR tabname[6] = { L"内存跟踪",L"资源管理",L"控件Spy",L"脚本调试",L"日志",L"工具" };
    tie.mask = TCIF_TEXT | TCIF_IMAGE;//psztext字段有效  
    tie.iImage = -1;
    for (INT i = 0; i < 6; i++)
    {
        tie.pszText = tabname[i];
        TabCtrl_InsertItem(htabctrl, i, &tie);
    }
    //在tab标签中显示 别的控件  
    RECT rect;//存放tab控件的区域位置  
    GetClientRect(htabctrl, &rect);
    // 将两个窗口往 tab控件位置移动  
    for (int i = 0; i < 6; i++)
    {
        hDlg_intab[i] = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_MEM + i), htabctrl, DlgProc[i]);
        MoveWindow(hDlg_intab[i], 2, 23, rect.right - rect.left - 6, rect.bottom - rect.top - 26, FALSE);
    }
    if (hDlg_intab[0]) {
        MemList = GetDlgItem(hDlg_intab[0], IDC_LIST1);
        SendMessage(MemList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_UNDERLINEHOT);
        ShowWindow(MemList, SW_SHOW);
        SetTimer(0, 0, 1000, UPDATETIME);
    }
    if (hDlg_intab[2]) {
        ControlTree = GetDlgItem(hDlg_intab[2], IDC_TREE1);
        AttList = GetDlgItem(hDlg_intab[2], IDC_LIST1);
        SendMessage(AttList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_UNDERLINEHOT);
        UpdateControlTree();
    }
    if (hDlg_intab[3]) {
        //js调试窗口
        if (LoadLibrary(_T("SciLexer.dll"))) {
            CreateWindow(_T("Scintilla"), NULL, 1342177280,
                0, 0,
                620, 480,
                hDlg_intab[3],
                NULL, m_hInstance, NULL);

        }
    }
    InsertColumn();
    ShowWindow(hDlg_intab[0], SW_SHOW);
    UpdateWindow(hwnd);

}

#endif