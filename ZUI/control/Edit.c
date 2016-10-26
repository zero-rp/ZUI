#include <ZUI.h>




ZEXPORT ZuiAny ZCALL ZuiEditProc(ZuiInt ProcId, ZuiControl cp, ZuiEdit p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return 0;
	case Proc_OnCreate: {
		p = (ZuiEdit)malloc(sizeof(ZEdit));
		memset(p, 0, sizeof(ZEdit));
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

			ZuiControlInvalidate(cp);
		}
			break;
		case ZEVENT_MOUSEENTER: {

			ZuiControlInvalidate(cp);
		}
			break;
		case ZEVENT_LBUTTONDOWN: {

			ZuiControlInvalidate(cp);
		}
			break;
		case ZEVENT_LBUTTONUP: {

			ZuiControlInvalidate(cp);
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
	
		ZRect r;
		MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
		ZuiDrawString(gp, Global_StringFormat, cp->m_sText, &r);
	}
		return 0;
	case Proc_SetAttribute: {
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




