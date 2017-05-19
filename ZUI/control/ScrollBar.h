#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__
#include <ZUI.h>


#define DEFAULT_SCROLLBAR_SIZE 16 //默认滚动条大小
#define DEFAULT_TIMERID 10 //时钟ID
typedef struct _ZScrollBar
{
    ZuiBool m_bHorizontal; //是否横向
    int m_nRange;
    int m_nScrollPos;
    int m_nLineSize;
    ZuiControl m_pOwner;
    ZPoint ptLastMouse;
    int m_nLastScrollPos;
    int m_nLastScrollOffset;
    int m_nScrollRepeatDelay;

    //头按钮
    ZuiBool m_bShowButton1;
    ZRect m_rcButton1;
    ZuiUInt m_uButton1State;
    ZuiRes m_sButton1NormalImage;
    ZuiRes m_sButton1HotImage;
    ZuiRes m_sButton1PushedImage;
    ZuiRes m_sButton1DisabledImage;

    //尾按钮
    ZuiBool m_bShowButton2;
    ZRect m_rcButton2;
    ZuiUInt m_uButton2State;
    ZuiRes m_sButton2NormalImage;
    ZuiRes m_sButton2HotImage;
    ZuiRes m_sButton2PushedImage;
    ZuiRes m_sButton2DisabledImage;

    //滑块按钮
    ZRect m_rcThumb;
    ZuiUInt m_uThumbState;
    ZuiRes m_sThumbNormalImage;
    ZuiRes m_sThumbHotImage;
    ZuiRes m_sThumbPushedImage;
    ZuiRes m_sThumbDisabledImage;

    //滑块轨道

    ZCtlProc old_call;
}*ZuiScrollBar, ZScrollBar;
ZEXPORT ZuiAny ZCALL ZuiScrollBarProc(ZuiInt ProcId, ZuiControl cp, ZuiScrollBar p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

#endif // __UISCROLLBAR_H__
