#include <ZUI.h>




ZEXPORT ZuiAny ZCALL ZuiButtonProc(ZuiInt ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return 0;
	case Proc_OnCreate: {
		p = (ZuiButton)malloc(sizeof(ZButton));
		memset(p, 0, sizeof(ZButton));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		p->old_call = cp->call;
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
			ZuiControlNotify(L"onclick", cp, &event->ptMouse, NULL, NULL);
		}
		   break;
		default:
			break;
		}
	}
		break;
	case Proc_OnPaint:{
		ZuiGraphics gp = (ZuiGraphics)Param1;
		RECT *rc = &cp->m_rcItem;
		HPEN hPen=0;
		ZuiImage img;
		if (p->type == 0) {
			if(p->m_ResNormal){
				img = p->m_ResNormal->p;
				ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, img->Width, img->Height, 255);
			}
			else {
				ZuiDrawFillRect(gp, ARGB(200, 0, 3, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top );
			}
		}
		else if(p->type==1) {
			if (p->m_ResHot) {
				img = p->m_ResHot->p;
				ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, img->Width, img->Height, 255);
			}
			else {
				ZuiDrawFillRect(gp, ARGB(200, 0, 255, 255), rc->left, rc->top, rc->right - rc->left , rc->bottom - rc->top);
			}
		}
		else if (p->type == 2) {
			if (p->m_ResPushed) {
				img = p->m_ResPushed->p;
				ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, img->Width, img->Height, 255);
			}
			else {
				ZuiDrawFillRect(gp, ARGB(200, 255, 255, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
			}
		}
		ZRect r;
		MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
		ZuiDrawString(gp, Global_StringFormat, cp->m_sText, &r);
		return 0;
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
	case Proc_OnInit:{

	}
		break;
	default:
		break;
	}
	return p->old_call(ProcId, cp,0, Param1, Param2, Param3);
}




