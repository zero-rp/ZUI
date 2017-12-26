#include "animation.h"
#include "control.h"
#include <platform/platform.h>
ZuiAny ZCALL OnPaint(ZuiControl p, ZuiAny Param1, ZuiAny Param2) {
    if (!p->m_aAnime->steup) {
        //动画开始
        ZuiOsKillTimer_Id(p, 1);
        p->m_aAnime->steup += 1;
        ZRect rc = { 0,0,p->m_rcItem.right - p->m_rcItem.left, p->m_rcItem.bottom - p->m_rcItem.top };
        ZuiControlCall(Proc_OnPaint, p, p->m_aAnime->m_hDcOffscreen, &rc, NULL);
        ZuiOsSetTimer(p, 1, 50);
        
    }
    else
    {
        ZRect *rc = &p->m_rcItem;
        if (p->m_aAnime->steup * 10 > 255) {
            ZuiOsKillTimer_Id(p, 1);
            //ZuiAlphaBlend(Param1, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, p->m_aAnime->m_hDcOffscreen, 0, 0, 255);
            return 0;
        }
        //ZuiAlphaBlend(Param1, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, p->m_aAnime->m_hDcOffscreen, 0, 0, p->m_aAnime->steup * 10);
        
    }
	return 0;
}
ZuiVoid ZCALL OnSize(ZuiControl p, ZuiAny w, ZuiAny h) {
    if (p->m_aAnime->m_hDcOffscreen)
        ZuiDestroyGraphics(p->m_aAnime->m_hDcOffscreen);
   p->m_aAnime->m_hDcOffscreen = ZuiCreateGraphicsInMemory((ZuiInt)w, (ZuiInt)h);
}
ZuiVoid ZCALL OnEvent(ZuiControl p, TEventUI *event) {
    if (event->Type == ZEVENT_TIMER && event->wParam==1) {
        p->m_aAnime->steup += 1;
        ZuiControlInvalidate(p, FALSE);
    }
}

ZuiAnimation ZuiAnimationNew(ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    ZuiAnimation p = (ZuiAnimation)malloc(sizeof(ZAnimation));
    p->steup = 0;
    p->OnPaint = OnPaint;
    p->OnSize = OnSize;
    p->OnEvent = OnEvent;
    p->m_hDcOffscreen = NULL;
    return p;
}
ZuiVoid ZuiAnimationFree(ZuiAnimation p) {
    if (!p)
        return 0;
    //ZuiOsKillTimer_Id(p, 1);
    free(p);
}