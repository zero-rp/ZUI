#ifndef __LAYOUT_H__
#define __LAYOUT_H__
#include <ZUI.h>
#include <core/carray.h>
//------Js

#define Js_Id_Layout_Add                100
#define Js_Id_Layout_AddAt              101
#define Js_Id_Layout_GetItemIndex       102
#define Js_Id_Layout_GetItemAt          103
#define Js_Id_Layout_GetItemName        104

#define Js_Id_Layout_count              105
#define Js_Id_Layout_inset              106


/**容器基类结构*/
typedef struct _ZuiLayout
{
    DArray *m_items;                /// 控件数组
    ZRect m_rcInset;                /// 内边距
    ZuiBool m_bMouseChildEnabled;
    ZuiInt m_iChildPadding;
    ZuiUInt m_iChildAlign;
    ZuiUInt m_iChildVAlign;
    ZuiBool m_bFocused;             //是焦点状态
    ZuiBool m_bScrollProcess;       //  防止SetPos循环调用
    ZuiInt  m_nScrollStepSize;

    ZuiControl m_pVerticalScrollBar;    //纵向滚动条
    ZuiControl m_pHorizontalScrollBar;  //横向滚动条

    ZCtlProc old_call;
}*ZuiLayout, ZLayout;

void* ZCALL ZuiLayoutProc(ZuiInt ProcId, ZuiControl cp, ZuiLayout p, ZuiAny Param1, ZuiAny Param2);


#endif // __LAYOUT_H__
