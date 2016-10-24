#include <ZUI.h>

extern rb_root *Global_ControlClass;
//创建控件
ZuiControl NewZuiControl(ZuiText classname, void* Param1, void* Param2, void* Param3){
	ZuiControl p = (ZuiControl)malloc(sizeof(ZControl));
	if (p){
		memset(p, 0, sizeof(ZControl));
		p->m_pManager = NULL;
		p->m_pParent = NULL;
		p->m_bUpdateNeeded = TRUE;
		p->m_bMenuUsed = FALSE;
		p->m_bVisible = TRUE;
		p->m_bFocused = FALSE;
		p->m_bEnabled = TRUE;
		p->m_bMouseEnabled = TRUE;
		p->m_bKeyboardEnabled = TRUE;
		p->m_bFloat = FALSE;
		p->m_bSetPos = FALSE;
		p->m_chShortcut = '\0';
		p->m_nTooltipWidth = 300;

		p->m_cXY.cx = p->m_cXY.cy = 0;
		p->m_cxyFixed.cx = p->m_cxyFixed.cy = 0;
		p->m_cxyMin.cx = p->m_cxyMin.cy = 0;
		p->m_cxyMax.cx = p->m_cxyMax.cy = 9999;


		p->m_piFloatPercent.left = p->m_piFloatPercent.top = p->m_piFloatPercent.right = p->m_piFloatPercent.bottom = 0.0f;

		p->call = &ZuiDefaultControlProc;

		//通知重载的对象开始创建
		//没有重载的
		/*查找类名*/
		wchar_t name[256];
		int l = wcslen(classname);
		if (l > 255)
			return p;
		memset(name, 0, 256 * sizeof(wchar_t));
		memcpy(name, classname, l*sizeof(wchar_t));
		wcslwr(name);
		rb_node * node = rb_search((key_t)Zui_Hash(name), Global_ControlClass);
		if (node) {
			p->m_sUserData = ((ZCtlProc)node->data)(Proc_OnCreate, p, 0, Param1, Param2, Param3);
			p->call = ((ZCtlProc)node->data);
		}

		return p;
	}
	return NULL;
}
//销毁控件
void FreeCControlUI(ZuiControl p)
{
	ZuiControlCall(Proc_OnDestroy, p, NULL, NULL, NULL);
	if (p->m_pManager != NULL)
		ZuiPaintManagerReapObjects(p->m_pManager, p);
	if (p->m_sText)
		free(p->m_sText);
	if (p->m_sToolTip)
		free(p->m_sToolTip);
}
//-------------------------------------------------------------------------------------------------

//控件默认处理函数
ZAPI(ZuiAny) ZuiDefaultControlProc(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {

	switch (ProcId)
	{
	case Proc_GetControlFlags:{
								  return 0;
	}
	case Proc_Activate: {
							if (!p->m_bVisible) return FALSE;
							if (!p->m_bEnabled) return FALSE;
	}
	case Proc_SetVisible: {
							  if (p->m_bVisible == (BOOL)Param1)
								  return 0;
							  BOOL v = p->m_bVisible;
							  p->m_bVisible = (BOOL)Param1;
							  if (p->m_bFocused) p->m_bFocused = FALSE;
							  if (!(BOOL)Param1 && p->m_pManager && p->m_pManager->m_pFocus == p) {
								  ZuiPaintManagerSetFocus(p->m_pManager, NULL, TRUE);
							  }
							  if (p->m_bVisible != v) {
								  ZuiControlNeedParentUpdate(p);
							  }
							  break;
	}
	case Proc_SetText:{
		if(!p->m_sText)
			p->m_sText = _wcsdup((ZuiText)Param1);
		if (wcscmp(p->m_sText, (ZuiText)Param1) == 0)
			return 0;
		p->m_sText = _wcsdup((ZuiText)Param1);
		ZuiControlInvalidate(p);
		break;
	}
	case Proc_SetTooltip: {
		if(!p->m_sToolTip)
			p->m_sToolTip = _wcsdup((ZuiText)Param1);
		if (wcscmp(p->m_sToolTip, (ZuiText)Param1) == 0)
			return 0;
		p->m_sToolTip = _wcsdup((ZuiText)Param1);
		break;
	}
	case Proc_GetPos:{
						 return (void *)&p->m_rcItem;
						 break;
	}
	case Proc_SetPos: {// 只有控件为float的时候，外部调用SetPos和Move才是有效的，位置参数是相对父控件的位置
		RECT *rc=(RECT *)Param1;
		BOOL bNeedInvalidate= (BOOL)Param2;
			if (rc->right < rc->left) rc->right = rc->left;
			if (rc->bottom < rc->top) rc->bottom = rc->top;

			RECT invalidateRc = p->m_rcItem;
			if (IsRectEmpty(&invalidateRc)) invalidateRc = *rc;

			if (p->m_bFloat) {
				ZuiControl pParent = p->m_pParent;
				if (pParent != NULL) {
					RECT *rcParentPos = (RECT *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
					RECT rcCtrl = { rcParentPos->left + rc->left, rcParentPos->top + rc->top,
						rcParentPos->left + rc->right, rcParentPos->top + rc->bottom };
					p->m_rcItem = rcCtrl;

					LONG width = rcParentPos->right - rcParentPos->left;
					LONG height = rcParentPos->bottom - rcParentPos->top;
					RECT rcPercent = { (LONG)(width*p->m_piFloatPercent.left), (LONG)(height*p->m_piFloatPercent.top),
						(LONG)(width*p->m_piFloatPercent.right), (LONG)(height*p->m_piFloatPercent.bottom) };
					p->m_cXY.cx = rc->left - rcPercent.left;
					p->m_cXY.cy = rc->top - rcPercent.top;
					p->m_cxyFixed.cx = rc->right - rcPercent.right - p->m_cXY.cx;
					p->m_cxyFixed.cy = rc->bottom - rcPercent.bottom - p->m_cXY.cy;
				}
			}
			else {
				memcpy(&p->m_rcItem, rc, sizeof(RECT));
			}
			if (p->m_pManager == NULL) 
				return 0;

			if (!p->m_bSetPos) {
				p->m_bSetPos = TRUE;
				ZuiControlCall(Proc_OnSize, p, p->m_rcItem.right - p->m_rcItem.left, p->m_rcItem.bottom - p->m_rcItem.top, NULL);
				p->m_bSetPos = FALSE;
			}

			p->m_bUpdateNeeded = FALSE;

			if (bNeedInvalidate && p->m_bVisible) {
				Rect_Join(&invalidateRc, &p->m_rcItem);
				ZuiControl pParent = p;
				RECT rcTemp;
				RECT *rcParent;
				while (pParent = pParent->m_pParent) {
					if (!pParent->m_bVisible)
						return 0;
					rcTemp = invalidateRc;
					rcParent = (RECT *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
					if (!IntersectRect(&invalidateRc, &rcTemp, rcParent)) 
						return 0;
				}
				ZuiPaintManagerInvalidateRect(p->m_pManager, &invalidateRc);
			}
		break;
	}
	case Proc_SetManager:{
							 p->m_pManager = (ZuiPaintManager)Param1;
							 p->m_pParent = (ZuiControl)Param2;
							 if (Param3 && p->m_pParent)
								 ZuiControlCall(Proc_OnInit, p, NULL, NULL, NULL);
							 break;
	}
	case Proc_OnEvent:{
						  if (((TEventUI *)Param1)->Type == ZEVENT_SETCURSOR)
						  {
							  SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
							  return 0;
						  }
						  if (((TEventUI *)Param1)->Type == ZEVENT_SETFOCUS)
						  {
							  p->m_bFocused = TRUE;
							  ZuiControlInvalidate(p);
							  return 0;
						  }
						  if (((TEventUI *)Param1)->Type == ZEVENT_KILLFOCUS)
						  {
							  p->m_bFocused = FALSE;
							  ZuiControlInvalidate(p);
							  return 0;
						  }
						  if (p->m_pParent != NULL)
							  ZuiControlCall(Proc_OnEvent, p->m_pParent, Param1, NULL, NULL);
						  break;
	}
	case Proc_GetMinWidth: {
		return (void *)p->m_cxyMin.cx;
		break;
	}
	case Proc_SetMinWidth:{
		if (p->m_cxyMin.cx == (LONG)Param1) 
			return 0;
		if ((LONG)Param1 < 0) 
			return 0;
		p->m_cxyMin.cx = (LONG)Param1;
		ZuiControlNeedParentUpdate(p);
		break;
	}
	case Proc_GetMaxWidth: {
		return (void *)p->m_cxyMax.cx;
		break;
	}
	case Proc_SetMaxWidth:{
		if (p->m_cxyMax.cx == (LONG)Param1)
			return 0;
		if ((LONG)Param1 < 0) 
			return 0;
		p->m_cxyMax.cx = (LONG)Param1;
		ZuiControlNeedParentUpdate(p);
		break;
	}
	case Proc_GetMinHeight: {
		return (void *)p->m_cxyMin.cy;
		break;
	}
	case Proc_SetMinHeight:{
		if (p->m_cxyMin.cy == (LONG)Param1)
			return 0;
		if ((LONG)Param1 < 0)
			return 0;
		p->m_cxyMin.cy = (LONG)Param1;
		ZuiControlNeedParentUpdate(p);
		break;
	}
	case Proc_GetMaxHeight: {
		return (void *)p->m_cxyMax.cy;
		break;
	}
	case Proc_SetMaxHeight: {
		if (p->m_cxyMax.cy == (LONG)Param1)
			return 0;
		if ((LONG)Param1 < 0)
			return 0;
		p->m_cxyMax.cy = (LONG)Param1;
		ZuiControlNeedParentUpdate(p);
		break;
	}
	case Proc_GetWidth: {
		return (void *)(p->m_rcItem.right - p->m_rcItem.left);
		break;
	}
	case Proc_GetHeight: {
		return (void *)(p->m_rcItem.bottom - p->m_rcItem.top);
		break;
	}
	case Proc_GetX: {
		return (void *)p->m_rcItem.left;
		break;
	}
	case Proc_GetY: {
		return (void *)p->m_rcItem.top;
		break;
	}
	case Proc_GetPadding: {
		return (void *)&p->m_rcPadding;
		break;
	}
	case Proc_SetPadding: {
		memcpy(&p->m_rcPadding, Param1, sizeof(RECT));
		ZuiControlNeedParentUpdate(p);
		break;
	}
	case Proc_GetFixedXY: {
		return (void *)&p->m_cXY;
		break;
	}
	case Proc_SetFixedXY: {
		p->m_cXY.cx = (LONG)Param1;
		p->m_cXY.cy = (LONG)Param2;
		ZuiControlNeedParentUpdate(p);
		break;
	}
	case Proc_GetFixedWidth: {
		return (void *)p->m_cxyFixed.cx;
		break;
	}
	case Proc_SetFixedWidth: {
		if ((LONG)Param1 < 0)
			return 0;
		p->m_cxyFixed.cx = (LONG)Param1;
		ZuiControlNeedParentUpdate(p);
		break;
	}
	case Proc_GetFixedHeight: {
		return (void *)p->m_cxyFixed.cy;
		break;
	}
	case Proc_SetFixedHeight: {
		if ((LONG)Param1 < 0)
			return 0;
		p->m_cxyFixed.cy = (LONG)Param1;
		ZuiControlNeedParentUpdate(p);
		break;
	}
	case Proc_GetRelativePos: {
		ZuiControl pParent = p->m_pParent;
		if (pParent != NULL) {
			RECT *rcParentPos = (RECT *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
			RECT rcRelativePos = p->m_rcItem;
			OffsetRect(&rcRelativePos, -rcParentPos->left, -rcParentPos->top);
			return &rcRelativePos;
		}
		break;
	}
	case Proc_SetFloat: {
		if (p->m_bFloat == (BOOL)Param1)
			return 0;
		p->m_bFloat = (BOOL)Param1;
		ZuiControlNeedParentUpdate(p);
		break;
	}
	case Proc_SetEnabled: {
		if (p->m_bEnabled == Param1)
			return 0;

		p->m_bEnabled = Param1;
		ZuiControlInvalidate(p);
		break;
	}
	case Proc_SetFocus: {
		if (p->m_pManager != NULL)
			ZuiPaintManagerSetFocus(p->m_pManager, p, FALSE);
		break;
	}
	case Proc_EstimateSize: {
		return (void *)&p->m_cxyFixed;
		break;
	}
	case Proc_FindControl: {
		if (((UINT)Param3 & ZFIND_VISIBLE) != 0 && !p->m_bVisible)
			return NULL;
		if (((UINT)Param3 & ZFIND_ENABLED) != 0 && !p->m_bEnabled)
			return NULL;//激活
		if (((UINT)Param3 & ZFIND_HITTEST) != 0 && (!p->m_bMouseEnabled || !PtInRect(&p->m_rcItem, *(LPPOINT)(Param2))))
			return NULL;
		if (((UINT)Param3 & ZFIND_UPDATETEST) != 0 && ((FINDCONTROLPROC)Param1)(p, Param2) != NULL)
			return NULL;
		return ((FINDCONTROLPROC)Param1)(p, Param2);
		break;
	}
	case Proc_SetBkColor: {
		p->m_BkgColor = Param1;
		ZuiControlInvalidate(p);
		break;
	}
	case Proc_OnPaint: {
		ZuiGraphics gp = (ZuiGraphics)Param1;
		RECT *rc = &p->m_rcItem;
		if (p->m_BkgColor)
			ZuiDrawFillRect(gp, p->m_BkgColor, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
		break;
	}
	case Proc_SetAttribute: {
		if (wcscmp(Param1, L"text") == 0) ZuiControlCall(Proc_SetText, p, Param2, NULL, NULL);
		else if (wcscmp(Param1, L"tooltip") == 0) ZuiControlCall(Proc_SetTooltip, p, Param2, NULL, NULL);
		else if (wcscmp(Param1, L"width") == 0) ZuiControlCall(Proc_SetFixedWidth, p, _wtoi(Param2), NULL, NULL);
		else if (wcscmp(Param1, L"height") == 0) ZuiControlCall(Proc_SetFixedHeight, p, _wtoi(Param2), NULL, NULL);
		else if (wcscmp(Param1, L"minwidth") == 0) ZuiControlCall(Proc_SetMinWidth, p, _wtoi(Param2), NULL, NULL);
		else if (wcscmp(Param1, L"minheight") == 0) ZuiControlCall(Proc_SetMinHeight, p, _wtoi(Param2), NULL, NULL);
		else if (wcscmp(Param1, L"maxwidth") == 0) ZuiControlCall(Proc_SetMaxWidth, p, _wtoi(Param2), NULL, NULL);
		else if (wcscmp(Param1, L"maxheight") == 0) ZuiControlCall(Proc_SetMaxHeight, p, _wtoi(Param2), NULL, NULL);
		else if (wcscmp(Param1, L"bkcolor") == 0) {
			while (*(wchar_t *)Param2 > L'\0' && *(wchar_t *)Param2 <= L' ') (wchar_t *)Param2 = CharNext((wchar_t *)Param2);
			if (*(wchar_t *)Param2 == L'#') (wchar_t *)Param2 = CharNext((wchar_t *)Param2);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul((wchar_t *)Param2, &pstr, 16);
			ZuiControlCall(Proc_SetBkColor, p, clrColor, NULL, NULL);
		}
	}
		break;
	default:
		break;
	}
	return 0;
}

//调用控件函数
ZAPI(ZuiAny) ZuiControlCall(ZuiInt ProcId, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (p->call)
	{
		return p->call(ProcId, p, p->m_sUserData, Param1, Param2, Param3);
	}
	return NULL;
}
//-------------------------------------------------------------------------------------------------


ZAPI(ZuiVoid) ZuiControlInvalidate(ZuiControl p)
{
	if (!p->m_bVisible) return;

	RECT invalidateRc = p->m_rcItem;

	ZuiControl pParent = p;
	RECT rcTemp;
	RECT *rcParent;
	while (pParent = pParent->m_pParent)
	{
		rcTemp = invalidateRc;
		rcParent = (RECT *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
		if (!IntersectRect(&invalidateRc, &rcTemp, rcParent))
		{
			return;
		}
	}

	if (p->m_pManager != NULL) ZuiPaintManagerInvalidateRect(p->m_pManager, &invalidateRc);
}

ZAPI(ZuiVoid) ZuiControlNeedUpdate(ZuiControl p)
{
	if (!p->m_bVisible) return;
	p->m_bUpdateNeeded = TRUE;
	ZuiControlInvalidate(p);

	if (p->m_pManager != NULL)
		p->m_pManager->m_bUpdateNeeded = TRUE;
}

ZAPI(ZuiVoid) ZuiControlNeedParentUpdate(ZuiControl p)
{
	if (p->m_pParent) {
		ZuiControlNeedUpdate(p->m_pParent);
		ZuiControlInvalidate(p->m_pParent);
	}
	else {
		ZuiControlNeedUpdate(p);
	}

	if (p->m_pManager != NULL)
		p->m_pManager->m_bUpdateNeeded = TRUE;
}

ZAPI(ZuiVoid) ZuiControlEvent(ZuiControl p, TEventUI *event)
{
	ZuiControlCall(Proc_OnEvent, p, event, NULL, NULL);
}

ZAPI(ZuiVoid) ZuiControlMove(ZuiControl p, SIZE szOffset, BOOL bNeedInvalidate)
{
	RECT invalidateRc = p->m_rcItem;
	p->m_rcItem.left += szOffset.cx;
	p->m_rcItem.top += szOffset.cy;
	p->m_rcItem.right += szOffset.cx;
	p->m_rcItem.bottom += szOffset.cy;

	if (bNeedInvalidate && p->m_pManager == NULL && p->m_bVisible) {
		Rect_Join(&invalidateRc, &p->m_rcItem);
		ZuiControl pParent = p;
		RECT rcTemp;
		RECT *rcParent;
		while (pParent = pParent->m_pParent) {
			if (!pParent->m_bVisible) return;
			rcTemp = invalidateRc;
			rcParent = (RECT *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
			if (!IntersectRect(&invalidateRc, &rcTemp, rcParent)) return;
		}
		ZuiPaintManagerInvalidateRect(p->m_pManager, &invalidateRc);
	}
}

//-------------------------------------------------------------------------------------------------











