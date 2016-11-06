#include <ZUI.h>
void Rect_Join(RECT *rc, RECT *rc1)
{
	if (rc1->left < rc->left) rc->left = rc1->left;
	if (rc1->top < rc->top) rc->top = rc1->top;
	if (rc1->right > rc->right) rc->right = rc1->right;
	if (rc1->bottom > rc->bottom) rc->bottom = rc1->bottom;
}

void * ZCALL Zui_Hash(wchar_t* str){
	__asm{
		mov esi, str
			xor edx, edx
		hash_loop :
		movsx eax, byte ptr ds : [esi]
		movsx eax, byte ptr ds : [esi]
			cmp al, ah
			je compare_hash
			ror edx, 0x7
			add edx, eax
			inc esi
			inc esi
			jmp hash_loop
		compare_hash :
		mov eax, edx
	}
}

ZEXPORT ZuiBool ZCALL ZuiInit() {
	/*初始化图形层*/
	if (!ZuiGraphInitialize()) {
		return FALSE;
	}
	/*初始化全局变量*/
	if (!ZuiGlobalInit()) {
		return FALSE;
	}
	/*初始化绘制管理器*/
	if (!ZuiPaintManagerInitialize()) {
		return FALSE;
	}
	/*注册全局控件*/
	if (!ZuiControlRegister())
	{
		return FALSE;
	}
	/*初始化模版管理器*/
	if (!ZuiTemplateInit())
	{
		return FALSE;
	}
	/*初始化绑定器*/
	if (!ZuiBuilderInit())
	{
		return FALSE;
	}
	/*初始化资源池*/
	if (!ZuiResDBInit()) {
		return FALSE;
	}
	return TRUE;
}
ZEXPORT ZuiInt ZCALL ZuiMsgLoop() {
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
ZEXPORT ZuiVoid ZCALL ZuiMsgLoop_exit() {
	PostQuitMessage(0);
}
ZuiControl MsgBox_pRoot;
ZuiAny ZCALL MsgBox_Notify_ctl(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(p->m_sName, L"clos") == 0) {
		ZuiMsgLoop_exit();
	}
	else if (wcscmp(p->m_sName, L"min") == 0) {
		ZuiControlCall(Proc_Window_SetWindowMin, MsgBox_pRoot, NULL, NULL, NULL);
	}
	return 0;
}

ZEXPORT ZuiVoid ZCALL ZuiMsgBox() {
	MsgBox_pRoot = NewZuiControl(L"window", NULL, NULL, NULL);
	ZuiControlRegNotify(MsgBox_pRoot, MsgBox_Notify_ctl);



	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return;
}
