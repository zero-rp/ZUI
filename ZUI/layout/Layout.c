#include <ZUI.h>




void* CALLBACK ZuiLayoutProc(int ProcId, ZuiControl cp, ZuiLayout p, void* Param1, void* Param2, void* Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return TRUE;
	case Proc_OnCreate: {
		p = (ZuiLayout)malloc(sizeof(ZLayout));
		memset(p, 0, sizeof(ZLayout));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		p->old_call = cp->call;
		p->m_bMouseChildEnabled = TRUE;
		p->m_iChildPadding = 0;
		p->m_items = darray_create();
		return p;
		break;
	}
	case Proc_Layout_Add: {
		if (Param1 == NULL) return FALSE;

		if (cp->m_pManager != NULL)
			ZuiPaintManagerInitControls(cp->m_pManager, (ZuiControl)Param1, cp);
		if (cp->m_bVisible)
			ZuiControlNeedUpdate(cp);

		return (void *)darray_append(p->m_items,Param1);
		break;
	}
	case Proc_Layout_AddAt: {
		if (Param1 == NULL) return FALSE;

		if (cp->m_pManager != NULL)
			ZuiPaintManagerInitControls(cp->m_pManager, (ZuiControl)Param1, cp);
		if (cp->m_bVisible)
			ZuiControlNeedUpdate(cp);

		return (void *)darray_insert(p->m_items,(int)Param2,(ZuiControl)Param1);
		break;
	}
	case Proc_Layout_Remove: {
		if (Param1 == NULL)
			return FALSE;

		for (int it = 0; it < darray_len(p->m_items); it++) {
			if ((ZuiControl)(p->m_items->data[it]) == (ZuiControl)Param1) {
				ZuiControlNeedUpdate(cp);
				if(!Param2)
					FreeCControlUI((ZuiControl)Param1);
				return darray_delete(p->m_items,it);
			}
		}
		return FALSE;
		break;
	}
	case Proc_Layout_RemoveAt: {
		ZuiControl pControl = ZuiLayoutProc(Proc_Layout_GetItemAt, cp, p, Param1, 0, 0);
		if (pControl != NULL) {
			return ZuiLayoutProc(Proc_Layout_Remove, cp, p, pControl, 0, 0);
		}

		return FALSE;
		break;
	}
	case Proc_Layout_RemoveAll: {
		for (int it = 0; it < darray_len(p->m_items); it++) {
			FreeCControlUI((ZuiControl)(p->m_items->data[it]));
		}
		darray_empty(p->m_items);
		ZuiControlNeedUpdate(cp);
		break;
	}
	case Proc_Layout_GetCount: {
		return darray_len(p->m_items);
		break;
	}
	case Proc_Layout_GetItemIndex: {
		for (int it = 0; it < darray_len(p->m_items); it++) {
			if (p->m_items->data[it] == Param1) {
				return it;
			}
		}
		return -1;
		break;
	}
	case Proc_Layout_SetItemIndex: {
		for (int it = 0; it < darray_len(p->m_items); it++) {
			if (p->m_items->data[it] == Param1) {
				ZuiControlNeedUpdate(cp);
				darray_delete(p->m_items,it);
				return darray_insert(p->m_items,Param2, Param1);
			}
		}

		return FALSE;
		break;
	}
	case Proc_Layout_GetItemAt: {
		if (Param1 < 0 || Param1 >= darray_len(p->m_items)) return NULL;
		return p->m_items->data[(LONG)Param1];
		break;
	}
	case Proc_Layout_SetFloatPos: {
		if ((int)Param1 < 0 || (int)Param1 >= darray_len(p->m_items))
			return 0;

		ZuiControl pControl = (ZuiControl)(p->m_items->data[(int)Param1]);

		if (!pControl->m_bVisible)
			return 0;
		if (!pControl->m_bFloat)
			//不是浮动控件
			return 0;

		SIZE *szXY = (SIZE *)ZuiControlCall(Proc_GetFixedXY, pControl, 0, 0, 0);
		SIZE sz = { (LONG)ZuiControlCall(Proc_GetFixedWidth, pControl, 0, 0, 0), (LONG)ZuiControlCall(Proc_GetFixedHeight, pControl, 0, 0, 0) };
		TPercentInfo rcPercent = {0};// pControl->GetFloatPercent();
		LONG width = cp->m_rcItem.right - cp->m_rcItem.left;
		LONG height = cp->m_rcItem.bottom - cp->m_rcItem.top;
		RECT rcCtrl = { 0 };
		rcCtrl.left = (LONG)(width*rcPercent.left) + szXY->cx;
		rcCtrl.top = (LONG)(height*rcPercent.top) + szXY->cy;
		rcCtrl.right = (LONG)(width*rcPercent.right) + szXY->cx + sz.cx;
		rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY->cy + sz.cy;
		ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);
		break;
	}
	case Proc_SetPos: {
		p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
		if (darray_isempty(p->m_items))
			return 0;

		RECT rc = cp->m_rcItem;
		rc.left += p->m_rcInset.left;
		rc.top += p->m_rcInset.top;
		rc.right -= p->m_rcInset.right;
		rc.bottom -= p->m_rcInset.bottom;

		for (int it = 0; it < darray_len(p->m_items); it++) {
			ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
			if (!pControl->m_bVisible)
				continue;
			if (pControl->m_bFloat) {
				ZuiControlCall(Proc_Layout_SetFloatPos, cp, (void *)it, 0, 0);
			}
			else {
				SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
				if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl,0,0,0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
				if (sz.cx >(LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
				if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
				if (sz.cy >(LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
				RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
				ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);
			}
		}
		break;
	}
	case Proc_FindControl: {
		// Check if this guy is valid
		if (((UINT)Param3 & ZFIND_VISIBLE) != 0 && !cp->m_bVisible) return NULL;
		if (((UINT)Param3 & ZFIND_ENABLED) != 0 && !cp->m_bEnabled) return NULL;
		if (((UINT)Param3 & ZFIND_HITTEST) != 0 && !PtInRect(&cp->m_rcItem, *(LPPOINT)(Param2))) return NULL;
		if (((UINT)Param3 & ZFIND_UPDATETEST) != 0 && ((FINDCONTROLPROC)Param1)(cp, Param2) != NULL) return NULL;

		ZuiControl pResult = NULL;
		if (((UINT)Param3 & ZFIND_ME_FIRST) != 0) {
			if (((UINT)Param3 & ZFIND_HITTEST) == 0 || cp->m_bMouseEnabled) pResult = ((FINDCONTROLPROC)Param1)(cp, Param2);
		}
		if (pResult != NULL) return pResult;

		if (((UINT)Param3 & ZFIND_HITTEST) == 0 || p->m_bMouseChildEnabled) {
			RECT rc = cp->m_rcItem;
			rc.left += p->m_rcInset.left;
			rc.top += p->m_rcInset.top;
			rc.right -= p->m_rcInset.right;
			rc.bottom -= p->m_rcInset.bottom;
			if (((UINT)Param3 & ZFIND_TOP_FIRST) != 0) {
				for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
					pResult = (ZuiControl)ZuiControlCall(Proc_FindControl, (ZuiControl)(p->m_items->data[it]), Param1, Param2, Param3);
					if (pResult != NULL) {
						if (((UINT)Param3 & ZFIND_HITTEST) != 0 && !pResult->m_bFloat && !PtInRect(&rc, *((LPPOINT)(Param2))))
							continue;
						else
							return pResult;
					}
				}
			}
			else {
				for (int it = 0; it < darray_len(p->m_items); it++) {
					pResult = (ZuiControl)ZuiControlCall(Proc_FindControl, (ZuiControl)(p->m_items->data[it]), Param1, Param2, Param3);
					if (pResult != NULL) {
						if (((UINT)Param3 & ZFIND_HITTEST) != 0 && !pResult->m_bFloat && !PtInRect(&rc, *((LPPOINT)(Param2))))
							continue;
						else
							return pResult;
					}
				}
			}
		}

		pResult = NULL;
		if (pResult == NULL && ((UINT)Param3 & ZFIND_ME_FIRST) == 0) {
			if (((UINT)Param3 & ZFIND_HITTEST) == 0 || cp->m_bMouseEnabled) pResult = ((FINDCONTROLPROC)Param1)(cp, Param2);
		}
		return pResult;
		break;
	}
	case Proc_OnPaint: {
		RECT rcTemp = { 0 };
		if (!IntersectRect(&rcTemp, (RECT *)Param2, &cp->m_rcItem))
			//不在绘制区域
			return 0;
		//保存原始剪裁区
		ZuiRegion OldRgn = ZuiCreateRegion();
		ZuiGraphicsGetClipRegion((ZuiGraphics)Param1, OldRgn);
		//设置新剪裁区
		
		ZRect rcClip;
		MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top);
		ZuiRegion rgn = ZuiCreateRegionRect(&rcClip);
		ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, rgn,1);


		p->old_call(Proc_OnPaint, cp, 0, Param1, Param2, 0);
		if (darray_len(p->m_items) > 0) {
			RECT rc = cp->m_rcItem;
			rc.left += p->m_rcInset.left;
			rc.top += p->m_rcInset.top;
			rc.right -= p->m_rcInset.right;
			rc.bottom -= p->m_rcInset.bottom;

			if (!IntersectRect(&rcTemp, (RECT *)Param2, &rc)) {
				for (int it = 0; it < darray_len(p->m_items); it++) {
					ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
					if (!pControl->m_bVisible) continue;
					if (!IntersectRect(&rcTemp, (RECT *)Param2, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
					if (pControl->m_bFloat) {
						if (!IntersectRect(&rcTemp, &cp->m_rcItem, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
						ZuiControlCall(Proc_OnPaint, pControl, Param1, Param2, Param3);
					}
				}
			}
			else {
				//保存原始剪裁区
				ZuiRegion OldRgn_child = ZuiCreateRegion();
				ZuiGraphicsGetClipRegion((ZuiGraphics)Param1, OldRgn_child);
				//设置子剪裁区
				MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top);
				ZuiRegion rgn_child = ZuiCreateRegionRect(&rcClip);
				ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, rgn_child, 1);

				for (int it = 0; it < darray_len(p->m_items); it++) {
					ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
					if (!pControl->m_bVisible) continue;
					if (!IntersectRect(&rcTemp, (RECT *)Param2, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
					if (pControl->m_bFloat) {
						if (!IntersectRect(&rcTemp, (RECT *)Param2, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
						ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, OldRgn_child, 0);
						ZuiControlCall(Proc_OnPaint, pControl, Param1, Param2, Param3);
						ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, rgn_child, 0);
					}
					else {
						if (!IntersectRect(&rcTemp, &rc, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
						ZuiControlCall(Proc_OnPaint, pControl, Param1, Param2, Param3);
					}
				}

				//恢复剪裁区
				ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, OldRgn_child, 0);
				ZuiDestroyRegion(OldRgn_child);
				ZuiDestroyRegion(rgn_child);
			}
		}
		
		//恢复剪裁区

		ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, OldRgn, 0);
		ZuiDestroyRegion(OldRgn);
		ZuiDestroyRegion(rgn);
		return 0;//绘图完毕,不需要默认
	}
	case Proc_Layout_SetInset: {
		memcpy(&p->m_rcInset, Param1, sizeof(RECT));
		ZuiControlNeedParentUpdate(cp);
		break;
	}
	case Proc_SetAttribute: {
		if (wcscmp(Param1, _T("inset")) == 0) {
			RECT rcInset = { 0 };
			LPTSTR pstr = NULL;
			rcInset.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
			rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			ZuiControlCall(Proc_Layout_SetInset, cp, &rcInset, NULL, NULL);
		}
		break;
	}
	case Proc_JsPut: {
		js_State *J = Param2;
		if (wcscmp(Param1, L"inset") == 0) {
			RECT rcPadding = { 0 };
			LPTSTR pstr = NULL;
			rcPadding.left = _tcstol(js_tostring(J, -1), &pstr, 10);  ASSERT(pstr);
			rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			ZuiControlCall(Proc_Layout_SetInset, p, &rcPadding, NULL, NULL);
		}

		break;
	}
	case Proc_JsHas: {
		js_State *J = Param2;
		if (wcscmp(Param1, L"Add") == 0)
			return 1;
		else if (wcscmp(Param1, L"AddAt") == 0)
			return 1;
		else if (wcscmp(Param1, L"GetItemIndex") == 0)
			return 1;
		else if (wcscmp(Param1, L"GetItemAt") == 0)
			return 1;
		else if (wcscmp(Param1, L"count") == 0) js_pushnumber(J, (int)ZuiControlCall(Proc_Layout_GetCount, cp, NULL, NULL, NULL));
		else {
			for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
				if (((ZuiControl)(p->m_items->data[it]))->m_sName)
				{
					if (wcscmp(Param1, ((ZuiControl)(p->m_items->data[it]))->m_sName) == 0) {
						ZuiBuilderJs_pushControl(Param2, p->m_items->data[it]);
						continue;
					}
				}
			}
		}
		break;
	}
	case Proc_JsCall: {
		if (wcscmp(Param1, L"Add") == 0) {
			ZuiControl ite = ZuiBuilderJs_toControl(Param2, -1);
			ZuiControlCall(Proc_Layout_Add, cp, ite, NULL, NULL);
		}
		else if (wcscmp(Param1, L"AddAt") == 0) {
			ZuiControl ite = ZuiBuilderJs_toControl(Param2, 1);
			ZuiControlCall(Proc_Layout_AddAt, cp, ite, js_toint32(Param2, 2), NULL);
		}
		else if (wcscmp(Param1, L"GetItemIndex") == 0) {
			ZuiControl ite = ZuiBuilderJs_toControl(Param2, -1);
			ZuiInt index = ZuiControlCall(Proc_Layout_GetItemIndex, cp, ite, NULL, NULL);
			js_pushnumber(Param2, index);
		}
		else if (wcscmp(Param1, L"GetItemAt") == 0) {
			ZuiControl ite = ZuiControlCall(Proc_Layout_GetItemAt, cp, js_toint32(Param2, -1), NULL, NULL);
			if (ite)
				ZuiBuilderJs_pushControl(Param2, ite);
			else
				js_pushundefined(Param2);
		}
		break;
	}
	case Proc_SetManager: {
		for (int it = 0; it < darray_len(p->m_items); it++) {
			ZuiControlCall(Proc_SetManager, (ZuiControl)(p->m_items->data[it]),Param1, cp, Param3);
		}
		break;
	}
	default:
		break;
	}
	return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}


