#include <ZUI.h>


void* CALLBACK ZuiTileLayoutProc(int ProcId, ZuiControl cp, ZuiTileLayout p, void* Param1, void* Param2, void* Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return TRUE;
	case Proc_OnCreate: {
		p = (ZuiTileLayout)malloc(sizeof(ZTileLayout));
		memset(p, 0, sizeof(ZTileLayout));
		//创建继承的控件 保存数据指针
		p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
		p->old_call = (ZCtlProc)&ZuiLayoutProc;

		return p;
		break;
	}
	case Proc_SetPos: {
		ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2, Param3);
		RECT rc = cp->m_rcItem;

		// Adjust for inset
		ZuiLayout op = (ZuiLayout)p->old_udata;
		rc.left += op->m_rcInset.left;
		rc.top += op->m_rcInset.top;
		rc.right -= op->m_rcInset.right;
		rc.bottom -= op->m_rcInset.bottom;

		if (darray_len(op->m_items) == 0) {
			return;
		}

		// Position the elements
		if (p->m_szItem.cx > 0) p->m_nColumns = (rc.right - rc.left) / p->m_szItem.cx;
		if (p->m_nColumns == 0) p->m_nColumns = 1;

		int cyNeeded = 0;
		int cxWidth = (rc.right - rc.left) / p->m_nColumns;


		int cyHeight = 0;
		int iCount = 0;
		POINT ptTile = { rc.left, rc.top };

		int iPosX = rc.left;

		for (int it1 = 0; it1 < darray_len(op->m_items); it1++) {
			ZuiControl pControl = (ZuiControl)(op->m_items->data[it1]);
			if (!pControl->m_bVisible) continue;
			if (pControl->m_bFloat) {
				ZuiControlCall(Proc_Layout_SetFloatPos, cp, (void *)it1, 0, 0);
				continue;
			}

			// Determine size
			RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
			if ((iCount % p->m_nColumns) == 0)
			{
				int iIndex = iCount;
				for (int it2 = it1; it2 < darray_len(op->m_items); it2++) {
					ZuiControl pLineControl = (ZuiControl)(op->m_items->data[it2]);
					if (!pLineControl->m_bVisible) continue;
					if (pLineControl->m_bFloat) continue;
					
					RECT *rcPadding = (RECT *)(ZuiControlCall(Proc_GetPadding, pLineControl, 0, 0, 0));
					SIZE szAvailable = { rcTile.right - rcTile.left - rcPadding->left - rcPadding->right, 9999 };

					if (szAvailable.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) szAvailable.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
					if (szAvailable.cx >(LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) szAvailable.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);

					
					SIZE *pszTile = (SIZE *)ZuiControlCall(Proc_EstimateSize, pLineControl, (void *)&szAvailable, 0, 0);
					SIZE szTile;
					szTile.cx = pszTile->cx;
					szTile.cy = pszTile->cy;
					if (szTile.cx == 0) szTile.cx = p->m_szItem.cx;
					if (szTile.cy == 0) szTile.cy = p->m_szItem.cy;
					if (szTile.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) szTile.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
					if (szTile.cx >(LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) szTile.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
					if (szTile.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) szTile.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
					if (szTile.cy >(LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) szTile.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);

					cyHeight = MAX(cyHeight, szTile.cy + rcPadding->top + rcPadding->bottom);
					if ((++iIndex % p->m_nColumns) == 0) break;
				}
			}

			RECT *rcPadding = (RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));

			rcTile.left += rcPadding->left;
			rcTile.right -= rcPadding->right;

			// Set position
			rcTile.top = ptTile.y + rcPadding->top;
			rcTile.bottom = ptTile.y + cyHeight;

			SIZE szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };

			SIZE *pszTile = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szAvailable, 0, 0);
			SIZE szTile;
			szTile.cx = pszTile->cx;
			szTile.cy = pszTile->cy;
			if (szTile.cx == 0) szTile.cx = p->m_szItem.cx;
			if (szTile.cy == 0) szTile.cy = p->m_szItem.cy;
			if (szTile.cx == 0) szTile.cx = szAvailable.cx;
			if (szTile.cy == 0) szTile.cy = szAvailable.cy;
			if (szTile.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) szTile.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
			if (szTile.cx >(LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) szTile.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
			if (szTile.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) szTile.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
			if (szTile.cy >(LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) szTile.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
			RECT rcPos = { (rcTile.left + rcTile.right - szTile.cx) / 2, (rcTile.top + rcTile.bottom - szTile.cy) / 2,
				(rcTile.left + rcTile.right - szTile.cx) / 2 + szTile.cx, (rcTile.top + rcTile.bottom - szTile.cy) / 2 + szTile.cy };
			ZuiControlCall(Proc_SetPos, pControl, &rcPos, FALSE, 0);
			if ((++iCount % p->m_nColumns) == 0) {
				ptTile.x = iPosX;
				ptTile.y += cyHeight + op->m_iChildPadding;
				cyHeight = 0;
			}
			else {
				ptTile.x += cxWidth;
			}
			cyNeeded = rcTile.bottom - rc.top;
			//if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) cyNeeded += m_pVerticalScrollBar->GetScrollPos();
		}
		return 0;
		break;
	}
	case Proc_TileLayout_SetColumns: {
		if (Param1 <= 0) return;
		p->m_nColumns = Param1;
		ZuiControlNeedUpdate(cp);
		break;
	}
	case Proc_TileLayout_SetItemSize: {
		if (p->m_szItem.cx != (LONG)Param1 || p->m_szItem.cy != (LONG)Param2) {
			p->m_szItem.cx = (LONG)Param1;
			p->m_szItem.cy = (LONG)Param2;
			ZuiControlNeedUpdate(cp);
		}
		break;
	}
	case Proc_SetAttribute: {
		if (wcscmp(Param1, L"itemsize") == 0) {
			LPTSTR pstr = NULL;
			ZuiInt x = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
			ZuiInt y = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);
			ZuiControlCall(Proc_TileLayout_SetItemSize, cp, x, y, NULL);
		}
		else if (wcscmp(Param1, L"columns") == 0) ZuiControlCall(Proc_TileLayout_SetColumns, cp, _ttoi(Param2), NULL, NULL);
		break;
	}
	default:
		break;
	}
	return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}