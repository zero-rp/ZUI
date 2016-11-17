/**
* @file		Layout.h
* @brief	布局容器类.
* @author	[Zero](22249030)
* @version	1.0
* @date		$date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#define	Proc_Layout_Add				501	///添加控件
#define	Proc_Layout_AddAt			502	///插入控件
#define	Proc_Layout_Remove			503	///移除指定控件
#define	Proc_Layout_RemoveAt		504	///移除指定位置控件
#define	Proc_Layout_RemoveAll		505	///移除全部控件
#define	Proc_Layout_GetCount		506	///取容器控件数量
#define	Proc_Layout_GetItemIndex	507	///取指定控件索引
#define	Proc_Layout_SetItemIndex	508	///设置控件所在位置
#define	Proc_Layout_GetItemAt		509	///索引取控件
#define	Proc_Layout_SetFloatPos		510	///设置浮动控件位置
#define	Proc_Layout_SetInset		512	///设置内边距
//--------滚动条
#define	Proc_Layout_GetScrollPos	513
#define	Proc_Layout_GetScrollRange	514
#define	Proc_Layout_SetScrollPos	515
#define	Proc_Layout_SetScrollStepSize	516
#define	Proc_Layout_GetScrollStepSize	517
#define	Proc_Layout_LineUp			518
#define	Proc_Layout_LineDown		519
#define	Proc_Layout_PageUp			520
#define	Proc_Layout_PageDown		521
#define	Proc_Layout_HomeUp			522
#define	Proc_Layout_EndDown			523
#define	Proc_Layout_LineLeft		524
#define	Proc_Layout_LineRight		525
#define	Proc_Layout_PageLeft		526
#define	Proc_Layout_PageRight		527
#define	Proc_Layout_HomeLeft		528
#define	Proc_Layout_EndRight		529
#define	Proc_Layout_EnableScrollBar	530
#define	Proc_Layout_GetVerticalScrollBar	531
#define	Proc_Layout_GetHorizontalScrollBar	532

/**容器基类结构*/
typedef struct _ZuiLayout
{
	DArray *m_items;			///控件数组
	RECT m_rcInset;				///
	BOOL m_bMouseChildEnabled;
	int m_iChildPadding;
	int	 m_nScrollStepSize;
	ZuiControl m_pVerticalScrollBar;//纵向滚动条
	ZuiControl m_pHorizontalScrollBar;//横向滚动条

	ZCtlProc old_call;
}*ZuiLayout, ZLayout;

void* CALLBACK ZuiLayoutProc(int ProcId, ZuiControl cp, ZuiLayout p, void* Param1, void* Param2, void* Param3);


#endif // __LAYOUT_H__
