/**
* @file     Layout.h
* @brief    布局容器类.
* @author   [Zero](22249030)
* @version  1.0
* @date     $date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __LAYOUT_H__
#define __LAYOUT_H__
#define Type_Layout                     0xbebc5b1a

#define Proc_Layout_Add                 501 ///添加控件
#define Proc_Layout_AddAt               502 ///插入控件
#define Proc_Layout_Remove              503 ///移除指定控件, 参数1为真由调用方销毁成员
#define Proc_Layout_RemoveAt            504 ///移除指定位置控件
#define Proc_Layout_RemoveAll           505 ///移除全部控件
#define Proc_Layout_GetCount            506 ///取容器控件数量
#define Proc_Layout_GetItemIndex        507 ///取指定控件索引
#define Proc_Layout_SetItemIndex        508 ///设置控件所在位置
#define Proc_Layout_GetItemAt           509 ///索引取控件
#define Proc_Layout_SetFloatPos         510 ///设置浮动控件位置
#define Proc_Layout_SetInset            512 ///设置内边距
//--------滚动条
#define Proc_Layout_GetScrollPos        513
#define Proc_Layout_GetScrollRange      514
#define Proc_Layout_SetScrollPos        515
#define Proc_Layout_SetScrollStepSize   516
#define Proc_Layout_GetScrollStepSize   517
#define Proc_Layout_LineUp              518
#define Proc_Layout_LineDown            519
#define Proc_Layout_PageUp              520
#define Proc_Layout_PageDown            521
#define Proc_Layout_HomeUp              522
#define Proc_Layout_EndDown             523
#define Proc_Layout_LineLeft            524
#define Proc_Layout_LineRight           525
#define Proc_Layout_PageLeft            526
#define Proc_Layout_PageRight           527
#define Proc_Layout_HomeLeft            528
#define Proc_Layout_EndRight            529
#define Proc_Layout_EnableScrollBar     530
#define Proc_Layout_GetVerticalScrollBar    531
#define Proc_Layout_GetHorizontalScrollBar  532
#define Proc_Layout_ProcessScrollBar    533 //调整滚动条位置

//------Js

#define Js_Id_Layout_Add                100
#define Js_Id_Layout_AddAt              101
#define Js_Id_Layout_GetItemIndex       102
#define Js_Id_Layout_GetItemAt          103
#define Js_Id_Layout_GetItemName        104

#define Js_Id_Layout_count              105
#define Js_Id_Layout_inset              106

//------宏封装
#define ZuiLayoutAdd(Control, ...) ZProc(Control, Layout_Add, __VA_ARGS__);
#define ZuiLayoutAddAt(Control, ...) ZProc(Control, Layout_AddAt, __VA_ARGS__);
#define ZuiLayoutRemove(Control, ...) ZProc(Control, Layout_Remove, __VA_ARGS__);
#define ZuiLayoutRemoveAt(Control, ...) ZProc(Control, Layout_RemoveAt, __VA_ARGS__);
#define ZuiLayoutRemoveAll(Control, ...) ZProc(Control, Layout_RemoveAll, __VA_ARGS__);
#define ZuiLayoutGetCount(Control, ...) ZProc(Control, Layout_GetCount, __VA_ARGS__);
#define ZuiLayoutGetItemIndex(Control, ...) ZProc(Control, Layout_GetItemIndex, __VA_ARGS__);
#define ZuiLayoutSetItemIndex(Control, ...) ZProc(Control, Layout_SetItemIndex, __VA_ARGS__);
#define ZuiLayoutGetItemAt(Control, ...) ZProc(Control, Layout_GetItemAt, __VA_ARGS__);
#define ZuiLayoutSetFloatPos(Control, ...) ZProc(Control, Layout_SetFloatPos, __VA_ARGS__);
#define ZuiLayoutSetInset(Control, ...) ZProc(Control, Layout_SetInset, __VA_ARGS__);
#define ZuiLayoutGetScrollPos(Control, ...) ZProc(Control, Layout_GetScrollPos, __VA_ARGS__);
#define ZuiLayoutGetScrollRange(Control, ...) ZProc(Control, Layout_GetScrollRange, __VA_ARGS__);
#define ZuiLayoutSetScrollPos(Control, ...) ZProc(Control, Layout_SetScrollPos, __VA_ARGS__);
#define ZuiLayoutSetScrollStepSize(Control, ...) ZProc(Control, Layout_SetScrollStepSize, __VA_ARGS__);
#define ZuiLayoutGetScrollStepSize(Control, ...) ZProc(Control, Layout_GetScrollStepSize, __VA_ARGS__);
#define ZuiLayoutLineUp(Control, ...) ZProc(Control, Layout_LineUp, __VA_ARGS__);
#define ZuiLayoutLineDown(Control, ...) ZProc(Control, Layout_LineDown, __VA_ARGS__);
#define ZuiLayoutPageUp(Control, ...) ZProc(Control, Layout_PageUp, __VA_ARGS__);
#define ZuiLayoutPageDown(Control, ...) ZProc(Control, Layout_PageDown, __VA_ARGS__);
#define ZuiLayoutHomeUp(Control, ...) ZProc(Control, Layout_HomeUp, __VA_ARGS__);
#define ZuiLayoutEndDown(Control, ...) ZProc(Control, Layout_EndDown, __VA_ARGS__);
#define ZuiLayoutLineLeft(Control, ...) ZProc(Control, Layout_LineLeft, __VA_ARGS__);
#define ZuiLayoutLineRight(Control, ...) ZProc(Control, Layout_LineRight, __VA_ARGS__);
#define ZuiLayoutPageLeft(Control, ...) ZProc(Control, Layout_PageLeft, __VA_ARGS__);
#define ZuiLayoutPageRight(Control, ...) ZProc(Control, Layout_PageRight, __VA_ARGS__);
#define ZuiLayoutHomeLeft(Control, ...) ZProc(Control, Layout_HomeLeft, __VA_ARGS__);
#define ZuiLayoutEndRight(Control, ...) ZProc(Control, Layout_EndRight, __VA_ARGS__);
#define ZuiLayoutEnableScrollBar(Control, ...) ZProc(Control, Layout_EnableScrollBar, __VA_ARGS__);
#define ZuiLayoutGetVerticalScrollBar(Control, ...) ZProc(Control, Layout_GetVerticalScrollBar, __VA_ARGS__);
#define ZuiLayoutGetHorizontalScrollBar(Control, ...) ZProc(Control, Layout_GetHorizontalScrollBar, __VA_ARGS__);
#define ZuiLayoutProcessScrollBar(Control, ...) ZProc(Control, Layout_ProcessScrollBar, __VA_ARGS__);


/**容器基类结构*/
typedef struct _ZuiLayout
{
    DArray *m_items;            /// 控件数组
    RECT m_rcInset;             /// 内边距
    ZuiBool m_bMouseChildEnabled;
    ZuiInt m_iChildPadding;
    ZuiBool m_bFocused;            //是焦点状态
    ZuiBool m_bScrollProcess;      //  防止SetPos循环调用
    ZuiInt  m_nScrollStepSize;
    ZuiControl m_pVerticalScrollBar;    //纵向滚动条
    ZuiControl m_pHorizontalScrollBar;  //横向滚动条

    ZCtlProc old_call;
}*ZuiLayout, ZLayout;

void* ZCALL ZuiLayoutProc(int ProcId, ZuiControl cp, ZuiLayout p, void* Param1, void* Param2, void* Param3);


#endif // __LAYOUT_H__
