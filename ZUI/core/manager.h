/**
* @file		manager.h
* @brief	绘制管理器.
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
#ifndef __MANAGER_H__
#define __MANAGER_H__

/////////////////////////////////////////////////////////////////////////////////////
//
typedef struct _ZControl *ZuiControl, ZControl;

/////////////////////////////////////////////////////////////////////////////////////
//
//事件类型定义
typedef enum EVENTTYPE_UI
{
	ZEVENT__FIRST = 1,
	ZEVENT__KEYBEGIN,
	ZEVENT_KEYDOWN,
	ZEVENT_KEYUP,
	ZEVENT_CHAR,
	ZEVENT_SYSKEY,
	ZEVENT__KEYEND,
	ZEVENT__MOUSEBEGIN,
	ZEVENT_MOUSEMOVE,//鼠标移动
	ZEVENT_MOUSELEAVE,//鼠标离开
	ZEVENT_MOUSEENTER,//鼠标进入
	ZEVENT_MOUSEHOVER,
	ZEVENT_LBUTTONDOWN,
	ZEVENT_LBUTTONUP,
	ZEVENT_RBUTTONDOWN,
	ZEVENT_LDBLCLICK,
	ZEVENT_CONTEXTMENU,
	ZEVENT_SCROLLWHEEL,
	ZEVENT__MOUSEEND,
	ZEVENT_KILLFOCUS,
	ZEVENT_SETFOCUS,
	ZEVENT_WINDOWSIZE,
	ZEVENT_SETCURSOR,
	ZEVENT_TIMER,
	ZEVENT_NOTIFY,
	ZEVENT_COMMAND,
	ZEVENT__LAST,
};

/////////////////////////////////////////////////////////////////////////////////////
//

#define ZFLAG_TABSTOP       0x00000001
#define ZFLAG_SETCURSOR     0x00000002
#define ZFLAG_WANTRETURN    0x00000004


#define ZFIND_ALL           0x00000000
#define ZFIND_VISIBLE       0x00000001
#define ZFIND_ENABLED       0x00000002
#define ZFIND_HITTEST       0x00000004
#define ZFIND_UPDATETEST    0x00000008
#define ZFIND_TOP_FIRST     0x00000010
#define ZFIND_ME_FIRST      0x80000000


/////////////////////////////////////////////////////////////////////////////////////
//

typedef struct tagTPercentInfo
{
	double left;
	double top;
	double right;
	double bottom;
} TPercentInfo;

//事件结构
typedef struct tagTEventUI
{
	int Type;			//事件类型
	ZuiControl pSender;	//事件对应的控件
	DWORD dwTimestamp;	//时间戳
	POINT ptMouse;		//
	TCHAR chKey;		//
	WORD wKeyState;		//
	WPARAM wParam;		//
	LPARAM lParam;		//
} TEventUI;




/**绘制管理器结构*/
typedef struct _ZuiPaintManager
{
	HWND m_hWndPaint;				///要ZuiPaintManager进行Direct绘图操作的窗体句柄
	HDC m_hDcPaint;					///直接绘制到窗体的DC(为窗体的整个区域包括费客户区)
	ZuiGraphics m_hDcOffscreen;		///内存缓冲区绘图DC

	COLORREF* m_pOffscreenBits;		///

	int m_iTooltipWidth;			///
	HWND m_hwndTooltip;				///提示窗口句柄
	TOOLINFO m_ToolTip;				///提示消息
	int m_iHoverTime;				///鼠标悬停世间
	BOOL m_bShowUpdateRect;			///是否显示更新区域 调试使用
	//
	ZuiControl m_pRoot;				///控件树根节点
	ZuiControl m_pFocus;			///处于获得焦点状态的控件
	ZuiControl m_pEventHover;		///处于鼠标悬停状态的控件
	ZuiControl m_pEventClick;		///被鼠标点击的控件
	ZuiControl m_pEventKey;			///接收键盘输入的控件
	//
	POINT m_ptLastMousePos;
	SIZE m_szMinWindow;				///窗口最小大小
	SIZE m_szMaxWindow;				///窗口最大大小
	SIZE m_szInitWindowSize;		///窗体初始化时的大小
	UINT m_uTimerID;				///当前定时器ID
	BOOL m_bFirstLayout;			///是否是第一次布局 相当于窗口初始化
	BOOL m_bUpdateNeeded;			///是否需要更新界面布局
	BOOL m_bFocusNeeded;			///是否需要焦点
	BOOL m_bOffscreenPaint;			///是否需要开双缓存绘图

	BYTE m_nOpacity;				///窗口透明度
	BOOL m_bLayered;				///是否分层
	RECT m_rcLayeredInset;			///
	BOOL m_bLayeredChanged;			///
	RECT m_rcLayeredUpdate;			///

	BOOL m_bMouseTracking;			///是否需要支持鼠标追踪
	BOOL m_bMouseCapture;			///是否需要支持鼠标捕获
	BOOL m_bIsPainting;				///是否正在绘制
	BOOL m_bUsedVirtualWnd;

	//
	DArray *m_aTimers;				///时钟句柄数组
	DArray *m_aPostPaintControls;	///发送绘制请求的控件集合
	DArray *m_aFoundControls;		///
} *ZuiPaintManager, IZuiPaintManager;

ZuiBool ZuiPaintManagerInitialize();

//创建消息管理器
ZuiPaintManager NewCPaintManagerUI();
//销毁消息管理器
void FreeCPaintManagerUI(ZuiPaintManager p);

ZEXPORT ZuiVoid ZCALL ZuiPaintManagerInit(ZuiPaintManager p, HWND hWnd);// 绘图管理器的初始化(m_hWndPaint,m_hDcPaint赋值，在预处理消息中加入管理器)
ZEXPORT ZuiVoid ZCALL ZuiPaintManagerInvalidate(ZuiPaintManager p);//指定区域失效
ZEXPORT ZuiVoid ZCALL ZuiPaintManagerInvalidateRect(ZuiPaintManager p, RECT *rcItem);//指定区域失效


ZEXPORT ZuiVoid ZCALL ZuiPaintManagerSetOpacity(ZuiPaintManager p, BYTE nOpacity);//设置窗口透明度 会开启分层窗口

ZEXPORT ZuiVoid ZCALL ZuiPaintManagerSetLayered(ZuiPaintManager p, BOOL bLayered);//设置分层窗口

ZEXPORT ZuiVoid ZCALL ZuiPaintManagerSetLayeredInset(ZuiPaintManager p, RECT *rcLayeredInset);

ZEXPORT ZuiVoid ZCALL ZuiPaintManagerSetLayeredOpacity(ZuiPaintManager p, BYTE nOpacity);//设置分层窗口透明度

ZEXPORT ZuiBool ZCALL ZuiPaintManagerAttachDialog(ZuiPaintManager p, ZuiControl pControl);//附加控件树
ZEXPORT ZuiBool ZCALL ZuiPaintManagerInitControls(ZuiPaintManager p, ZuiControl pControl, ZuiControl pParent);//初始化控件
ZEXPORT ZuiVoid ZCALL ZuiPaintManagerReapObjects(ZuiPaintManager p, ZuiControl pControl);


ZEXPORT ZuiVoid ZCALL ZuiPaintManagerSetFocus(ZuiPaintManager p, ZuiControl pControl, BOOL bFocusWnd /*默认TRUE*/);//设置焦点控件
ZEXPORT ZuiVoid ZCALL ZuiPaintManagerSetFocusNeeded(ZuiPaintManager p, ZuiControl pControl);

ZEXPORT ZuiBool ZCALL ZuiPaintManagerSetTimer(ZuiControl pControl, UINT nTimerID, UINT uElapse);//创建时钟
ZEXPORT ZuiBool ZCALL ZuiPaintManagerKillTimer_Id(ZuiControl pControl, UINT nTimerID);//销毁时钟
ZEXPORT ZuiVoid ZCALL ZuiPaintManagerKillTimer(ZuiControl pControl);//销毁时钟
ZEXPORT ZuiVoid ZCALL ZuiPaintManagerRemoveAllTimers(ZuiPaintManager p);//销毁全部时钟

ZEXPORT ZuiVoid ZCALL ZuiPaintManagerSetCapture(ZuiPaintManager p);//俘获输入
ZEXPORT ZuiVoid ZCALL ZuiPaintManagerReleaseCapture(ZuiPaintManager p);//释放俘获


ZEXPORT ZuiBool ZCALL ZuiPaintManagerSetNextTabControl(ZuiPaintManager p, BOOL bForward /*默认TRUE*/);


ZEXPORT ZuiBool ZCALL ZuiPaintManagerAddPostPaint(ZuiPaintManager p, ZuiControl pControl);
ZEXPORT ZuiBool ZCALL ZuiPaintManagerRemovePostPaint(ZuiPaintManager p, ZuiControl pControl);
ZEXPORT ZuiBool ZCALL ZuiPaintManagerSetPostPaintIndex(ZuiPaintManager p, ZuiControl pControl, int iIndex);


ZEXPORT ZuiControl ZCALL ZuiPaintManagerFindControl(ZuiPaintManager p, POINT pt);
ZEXPORT ZuiControl ZCALL ZuiPaintManagerFindSubControlByPoint(ZuiPaintManager p, ZuiControl pParent, POINT pt);

ZEXPORT ZuiBool ZCALL ZuiPaintManagerMessageHandler(ZuiPaintManager p, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lRes);


//静态函数

//-------------------------------------------------------------------------------------------------
ZuiControl CALLBACK __FindControlFromCount(ZuiControl pThis, LPVOID pData);//计算控件数量
ZuiControl CALLBACK __FindControlFromPoint(ZuiControl pThis, LPVOID pData);//根据点是否在区域中，查询控件
ZuiControl CALLBACK __FindControlFromTab(ZuiControl pThis, LPVOID pData);//通过Tab信息查询控件
ZuiControl CALLBACK __FindControlFromShortcut(ZuiControl pThis, LPVOID pData);//从快捷键查询控件
ZuiControl CALLBACK __FindControlFromName(ZuiControl pThis, LPVOID pData);// 通过名称比较查询控件
ZuiControl CALLBACK __FindControlsFromUpdate(ZuiControl pThis, LPVOID pData);



#endif // __MANAGER_H__
