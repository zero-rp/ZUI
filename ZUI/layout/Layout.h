#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#define	Proc_Layout_Add			1101	//添加控件
#define	Proc_Layout_AddAt			1102	//插入控件
#define	Proc_Layout_Remove		1103	//移除指定控件
#define	Proc_Layout_RemoveAt		1104	//移除指定位置控件
#define	Proc_Layout_RemoveAll		1105	//移除全部控件
#define	Proc_Layout_GetCount		1106	//取容器控件数量
#define	Proc_Layout_GetItemIndex	1107	//取指定控件索引
#define	Proc_Layout_SetItemIndex	1108	//设置控件所在位置
#define	Proc_Layout_GetItemAt		1109	//索引取控件
#define	Proc_Layout_SetFloatPos	1110	//设置浮动控件位置

//容器基类
typedef struct _ZuiLayout
{
	DArray *m_items;			//控件数组
	RECT m_rcInset;
	BOOL m_bMouseChildEnabled;
	int m_iChildPadding;
	ZCtlProc old_call;
}*ZuiLayout, ZLayout;

void* CALLBACK ZuiLayoutProc(int ProcId, ZuiControl cp, ZuiLayout p, void* Param1, void* Param2, void* Param3);


#endif // __LAYOUT_H__
