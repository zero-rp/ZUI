#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#define	Proc_ScrollBar_SetHorizontal	1000	//设置为横向滚动条
#define	Proc_ScrollBar_SetScrollPos		1001	//设置位置
#define	Proc_ScrollBar_GetScrollPos		1002	//获取位置
#define	Proc_ScrollBar_GetScrollRange	1003
#define	Proc_ScrollBar_SetOwner			1004
#define	Proc_ScrollBar_SetScrollRange	1005

//------宏封装
#define ZuiScrollBarSetHorizontal(Control, ...) ZProc(Control, ScrollBar_SetHorizontal, __VA_ARGS__);
#define ZuiScrollBarSetScrollPos(Control, ...) ZProc(Control, ScrollBar_SetScrollPos, __VA_ARGS__);
#define ZuiScrollBarGetScrollPos(Control, ...) ZProc(Control, ScrollBar_GetScrollPos, __VA_ARGS__);
#define ZuiScrollBarGetScrollRange(Control, ...) ZProc(Control, ScrollBar_GetScrollRange, __VA_ARGS__);
#define ZuiScrollBarSetOwner(Control, ...) ZProc(Control, ScrollBar_SetOwner, __VA_ARGS__);
#define ZuiScrollBarSetScrollRange(Control, ...) ZProc(Control, ScrollBar_SetScrollRange, __VA_ARGS__);


#define	DEFAULT_SCROLLBAR_SIZE			16		//默认滚动条大小
#define	DEFAULT_TIMERID					10		//时钟ID
typedef struct _ZScrollBar
{
	ZuiBool m_bHorizontal; //是否横向
	int m_nRange;
	int m_nScrollPos;
	int m_nLineSize;
	ZuiControl* m_pOwner;
	ZPoint ptLastMouse;
	int m_nLastScrollPos;
	int m_nLastScrollOffset;
	int m_nScrollRepeatDelay;

    ZuiBool m_bShowButton1;
	RECT m_rcButton1;
    ZuiUInt m_uButton1State;


    ZuiBool m_bShowButton2;
	RECT m_rcButton2;
	ZuiUInt m_uButton2State;


	RECT m_rcThumb;
    ZuiUInt m_uThumbState;

	ZCtlProc old_call;
}*ZuiScrollBar, ZScrollBar;
ZEXPORT ZuiAny ZCALL ZuiScrollBarProc(ZuiInt ProcId, ZuiControl cp, ZuiScrollBar p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

#endif // __UISCROLLBAR_H__