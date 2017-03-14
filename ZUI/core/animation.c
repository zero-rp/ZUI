#include <ZUI.h>
ZuiAny ZCALL OnPaint(ZuiControl p, ZuiAny Param1, ZuiAny Param2) {
    if (!p->m_aAnime->steup) {
        //动画开始
        ZuiPaintManagerKillTimer_Id(p, 1);
        p->m_aAnime->steup += 1;
        RECT rc = { 0,0,p->m_rcItem.right - p->m_rcItem.left, p->m_rcItem.bottom - p->m_rcItem.top };
        ZuiControlCall(Proc_OnPaint, p, p->m_aAnime->m_hDcOffscreen, &rc, NULL);
        ZuiPaintManagerSetTimer(p, 1, 10);
        
    }
    else
    {
        RECT *rc = &p->m_rcItem;
        if (p->m_aAnime->steup * 3 > 255) {
            ZuiPaintManagerKillTimer_Id(p, 1);
            ZuiAlphaBlend(Param1, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, p->m_aAnime->m_hDcOffscreen, 0, 0, 255);
            return 0;
        }
        ZuiAlphaBlend(Param1, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, p->m_aAnime->m_hDcOffscreen, 0, 0, p->m_aAnime->steup * 3);
        
    }
    
}
ZuiVoid ZCALL OnSize(ZuiControl p, ZuiAny w, ZuiAny h) {
    if (p->m_aAnime->m_hDcOffscreen)
        ZuiDestroyGraphics(p->m_aAnime->m_hDcOffscreen);
   p->m_aAnime->m_hDcOffscreen = ZuiCreateGraphicsInMemory(w, h);
}
ZuiVoid ZCALL OnEvent(ZuiControl p, TEventUI *event) {
    if (event->Type == ZEVENT_TIMER && event->wParam==1) {
        p->m_aAnime->steup += 1;
        ZuiControlInvalidate(p, FALSE);
    }
}

ZuiAnimation ZuiAnimationNew() {
    ZuiAnimation p = (ZuiAnimation)ZuiMalloc(sizeof(ZAnimation));
    p->steup = 0;
    p->OnPaint = OnPaint;
    p->OnSize = OnSize;
    p->OnEvent = OnEvent;
    p->m_hDcOffscreen = NULL;
    return p;
}