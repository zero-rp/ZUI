#include <ZUI.h>



void* CALLBACK ZuiVerticalLayoutProc(int ProcId, ZuiControl cp, ZuiVerticalLayout p, void* Param1, void* Param2, void* Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return TRUE;
	case Proc_OnCreate: {
		p = (ZuiVerticalLayout)malloc(sizeof(ZVerticalLayout));
		memset(p, 0, sizeof(ZVerticalLayout));
		//创建继承的控件 保存数据指针
		p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
		p->old_call = (ZCtlProc)&ZuiLayoutProc;
		
		return p;
		break;
	}
	case Proc_SetPos: {
		ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2, Param3);
		RECT rc = cp->m_rcItem;
		ZuiLayout op = (ZuiLayout)p->old_udata;
		// Adjust for inset
		rc.left += op->m_rcInset.left;
		rc.top += op->m_rcInset.top;
		rc.right -= op->m_rcInset.right;
		rc.bottom -= op->m_rcInset.bottom;
		if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) rc.right -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, op->m_pVerticalScrollBar, NULL, NULL, NULL);
		if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) rc.bottom -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
		if (darray_len(op->m_items) == 0) {
			ZuiControlCall(Proc_Layout_ProcessScrollBar, cp, &rc, 0, 0);
			return 0;
		}

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

		int nAdjustables = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for (int it1 = 0; it1 < darray_len(op->m_items); it1++) {
			ZuiControl pControl = (ZuiControl)(op->m_items->data[it1]);
			if (!pControl->m_bVisible) continue;
			if (pControl->m_bFloat) continue;
			SIZE sz;
			SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szAvailable, 0, 0);
			sz.cx = psz->cx;
			sz.cy = psz->cy;
			if (sz.cy == 0) {
				nAdjustables++;
			}
			else {
				if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
				if (sz.cy >(LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
			}
			cyFixed += sz.cy + ((RECT *)(ZuiControlCall(Proc_GetPadding, pControl,0,0,0)))->top + ((RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0)))->bottom;
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * op->m_iChildPadding;

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		if (nAdjustables > 0) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
			iPosY -= (LONG)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL, NULL);
		}
		int iPosX = rc.left;
		if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
			iPosX -= (LONG)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
		}
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for (int it2 = 0; it2 < darray_len(op->m_items); it2++) {
			ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
			if (!pControl->m_bVisible) continue;
			if (pControl->m_bFloat) {
				ZuiControlCall(Proc_Layout_SetFloatPos, cp, (void *)it2, 0, 0);
				continue;
			}

			RECT *rcPadding = (RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));
			szRemaining.cy -= rcPadding->top;
			SIZE sz;
			SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szRemaining, 0, 0);
			sz.cx = psz->cx;
			sz.cy = psz->cy;
			if (sz.cy == 0) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if (iAdjustable == nAdjustables) {
					sz.cy = MAX(0, szRemaining.cy - rcPadding->bottom - cyFixedRemaining);
				}
				if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
				if (sz.cy > (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
			}
			else {
				if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
				if (sz.cy > (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = (LONG)ZuiControlCall(Proc_GetFixedWidth, pControl, 0, 0, 0);
			if (sz.cx == 0) sz.cx = szAvailable.cx - rcPadding->left - rcPadding->right;
			if (sz.cx < 0) sz.cx = 0;
			if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
			if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);

			RECT rcCtrl = { iPosX + rcPadding->left, iPosY + rcPadding->top, iPosX + rcPadding->left + sz.cx, iPosY + sz.cy + rcPadding->top + rcPadding->bottom };
			ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);

			iPosY += sz.cy + op->m_iChildPadding + rcPadding->top + rcPadding->bottom;
			cyNeeded += sz.cy + rcPadding->top + rcPadding->bottom;
			szRemaining.cy -= sz.cy + op->m_iChildPadding + rcPadding->bottom;
		}
		cyNeeded += (nEstimateNum - 1) * op->m_iChildPadding;

		// Process the scrollbar

		ZuiControlCall(Proc_Layout_ProcessScrollBar, cp, &rc, 0, cyNeeded);
		return 0;
		break;
	}
	case Proc_OnPaint: {


		break;
	}
	default:
		break;
	}
	return p->old_call(ProcId, cp,p->old_udata, Param1, Param2, Param3);
}




