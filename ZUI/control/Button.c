#include <ZUI.h>




ZEXPORT ZuiAny ZCALL ZuiButtonProc(ZuiInt ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return TRUE;
	case Proc_OnDestroy: {
		if (p->m_ResNormal)
			ZuiResDBDelRes(p->m_ResNormal);
		if (p->m_ResHot)
			ZuiResDBDelRes(p->m_ResHot);
		if (p->m_ResPushed)
			ZuiResDBDelRes(p->m_ResPushed);
		if (p->m_ResFocused)
			ZuiResDBDelRes(p->m_ResFocused);
		if (p->m_ResDisabled)
			ZuiResDBDelRes(p->m_ResDisabled);
		break;
	}
	case Proc_OnCreate: {
		p = (ZuiButton)malloc(sizeof(ZButton));
		memset(p, 0, sizeof(ZButton));

		//保存原来的回调地址,创建成功后回调地址指向当前函数
		p->old_udata = ZuiLabelProc(Proc_OnCreate, cp, 0, 0, 0, 0);
		p->old_call = (ZCtlProc)&ZuiLabelProc;
		return p;
	}
		break;
	case Proc_OnEvent: {
		TEventUI *event= (TEventUI *)Param1;
		switch (event->Type)
		{
		case ZEVENT_MOUSELEAVE: {
			p->type = 0;
			ZuiControlInvalidate(cp);
		}
			break;
		case ZEVENT_MOUSEENTER: {
			p->type = 1;
			ZuiControlInvalidate(cp);
		}
			break;
		case ZEVENT_LBUTTONDOWN: {
			p->type = 2;
			ZuiControlInvalidate(cp);
		}
			break;
		case ZEVENT_LBUTTONUP: {
			p->type = 1;
			ZuiControlInvalidate(cp);
			return ZuiControlNotify(L"onclick", cp, event->ptMouse.x, JS_TNUMBER, event->ptMouse.y, JS_TNUMBER, NULL, NULL);
		}
		   break;
		default:
			break;
		}
	}
		break;
	case Proc_OnPaintStatusImage: {
		ZuiGraphics gp = (ZuiGraphics)Param1;
		RECT *rc = &cp->m_rcItem;
		HPEN hPen = 0;
		ZuiImage img;
		if (p->type == 0) {
			if (p->m_ResNormal) {
				img = p->m_ResNormal->p;
				ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
			}
			else {
				ZuiDrawFillRect(gp, ARGB(200, 0, 3, 255), rc->left, rc->top, rc->right - rc->left - 1, rc->bottom - rc->top - 1);
			}
		}
		else if (p->type == 1) {
			if (p->m_ResHot) {
				img = p->m_ResHot->p;
				ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
			}
			else {
				ZuiDrawFillRect(gp, ARGB(200, 0, 255, 255), rc->left, rc->top, rc->right - rc->left - 1, rc->bottom - rc->top - 1);
			}
		}
		else if (p->type == 2) {
			if (p->m_ResPushed) {
				img = p->m_ResPushed->p;
				ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
			}
			else {
				ZuiDrawFillRect(gp, ARGB(200, 255, 255, 255), rc->left, rc->top, rc->right - rc->left - 1, rc->bottom - rc->top - 1);
			}
		}
		break;
	}
	case Proc_Button_SetResNormal: {
		if (p->m_ResNormal)
			ZuiResDBDelRes(p->m_ResNormal);
		p->m_ResNormal = Param1;
		break;
	}
	case Proc_Button_SetResHot: {
		if (p->m_ResHot)
			ZuiResDBDelRes(p->m_ResHot);
		p->m_ResHot = Param1;
		break;
	}
	case Proc_Button_SetResPushed: {
		if (p->m_ResPushed)
			ZuiResDBDelRes(p->m_ResPushed);
		p->m_ResPushed = Param1;
		break;
	}
	case Proc_Button_SetResFocused: {
		if (p->m_ResFocused)
			ZuiResDBDelRes(p->m_ResFocused);
		p->m_ResFocused = Param1;
		break;
	}
	case Proc_Button_SetResDisabled: {
		if (p->m_ResDisabled)
			ZuiResDBDelRes(p->m_ResDisabled);
		p->m_ResDisabled = Param1;
		break;
	}
	case Proc_SetAttribute: {
		if (wcscmp(Param1, L"normalimage") == 0) ZuiControlCall(Proc_Button_SetResNormal, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
		else if (wcscmp(Param1, L"hotimage") == 0) ZuiControlCall(Proc_Button_SetResHot, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
		else if (wcscmp(Param1, L"pushedimage") == 0) ZuiControlCall(Proc_Button_SetResPushed, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
		else if (wcscmp(Param1, L"focusedimage") == 0) ZuiControlCall(Proc_Button_SetResFocused, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
		else if (wcscmp(Param1, L"disabledimage") == 0) ZuiControlCall(Proc_Button_SetResDisabled, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
		break;
	}
	case Proc_JsPut: {
		js_State *J = Param2;
		if (wcscmp(Param1, L"normalimage") == 0) {
			ZuiText str = js_tostring(J, -1);
			ZuiRes res = ZuiResDBGetRes(str, ZREST_IMG);
			ZuiControlCall(Proc_Button_SetResNormal, cp,
				res,
				NULL, NULL);
		}
		else if (wcscmp(Param1, L"hotimage") == 0) ZuiControlCall(Proc_Button_SetResHot, cp, ZuiResDBGetRes(js_tostring(J, -1), ZREST_IMG), NULL, NULL);
		else if (wcscmp(Param1, L"pushedimage") == 0) ZuiControlCall(Proc_Button_SetResPushed, cp, ZuiResDBGetRes(js_tostring(J, -1), ZREST_IMG), NULL, NULL);
		else if (wcscmp(Param1, L"focusedimage") == 0) ZuiControlCall(Proc_Button_SetResFocused, cp, ZuiResDBGetRes(js_tostring(J, -1), ZREST_IMG), NULL, NULL);
		else if (wcscmp(Param1, L"disabledimage") == 0) ZuiControlCall(Proc_Button_SetResDisabled, cp, ZuiResDBGetRes(js_tostring(J, -1), ZREST_IMG), NULL, NULL);
		break;
	}
	case Proc_OnInit:{

	}
		break;
	default:
		break;
	}
	return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}




