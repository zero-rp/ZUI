#ifndef __CONTROL_H__
#define __CONTROL_H__
#include <ZUI.h>
#include <platform/platform.h>
#include "animation.h"
#include "function.h"

#define CONTROL_SETSTR(STRNAME,VALUE,TYPE)			\
		if (STRNAME) {								\
			if (_tcscmp(STRNAME, (TYPE)VALUE) == 0)	\
				return 0;							\
			free(STRNAME);							\
		}											\
		STRNAME = _tcsdup((TYPE)VALUE);				\

#define ZTYLE_BOX               1   //单线边框
#define ZTYLE_BKGColor          2   //具有背景色


//-------控件状态
#define ZSTATE_FOCUSED      0x00000001
#define ZSTATE_SELECTED     0x00000002
#define ZSTATE_DISABLED     0x00000004
#define ZSTATE_HOT          0x00000008
#define ZSTATE_PUSHED       0x00000010
#define ZSTATE_READONLY     0x00000020
#define ZSTATE_CAPTURED     0x00000040

//------JSID
#define Js_Id_close             1

#define Js_Id_root              2
#define Js_Id_parent            3
#define Js_Id_text              4
#define Js_Id_tooltip           5
#define Js_Id_width             6
#define Js_Id_height            7
#define Js_Id_minwidth          8
#define Js_Id_minheight         9
#define Js_Id_maxwidth          10
#define Js_Id_maxheight         11
#define Js_Id_bkcolor           12
#define Js_Id_drag              13
#define Js_Id_rect              14
#define Js_Id_enabled           15
#define Js_Id_visible           16
#define Js_Id_name              17
#define Js_Id_padding           18
#define Js_Id_left              19
#define Js_Id_top               20

#define Js_Id_onmouseleave      21
#define Js_Id_onmouseenter      22
#define Js_Id_onlbuttondown     23
#define Js_Id_onclick           24
#define Js_Id_onchar            25
#define Js_Id_onsize            26


typedef struct tagFINDTABINFO
{
    ZuiControl pFocus;
    ZuiControl pLast;
    ZuiBool bForward;
    ZuiBool bNextIsIt;
} FINDTABINFO;
typedef struct tagFINDSHORTCUT
{
    ZText ch;
    ZuiBool bPickNext;
} FINDSHORTCUT;

typedef struct _ZControl
{
    //消息处理函数指针
    ZCtlProc call;
    //通知接口地址
    ZNotifyProc m_pNotify;

    struct _ZControl* m_pParent;    //父控件
    ZuiOsWindow m_pOs;              //绑定的系统窗口
    ZuiBool m_bUpdateNeeded;        //是否需要更新布局
    ZRect m_rcItem;                 //控件矩形
    ZRect m_rcPadding;              //控件
    ZSize m_cXY;                     //预设的左上角坐标
    ZSize m_cxyFixed;                //预设的控件大小
    ZSize m_cxyMin;                  //控件最小大小
    ZSize m_cxyMax;                  //控件最大大小
    ZSize m_rRound;                   //控件圆角
    ZuiBool m_bVisible;             //可视
    ZuiBool m_bEnabled;             //激活
    ZuiBool m_bMouseEnabled;        //鼠标激活
    ZuiBool m_bKeyboardEnabled;     //键盘激活
    ZuiBool m_bFocused;             //焦点
    ZRectR m_piFloatPercent;
    ZuiBool m_bFloat;               //浮动控件
    ZuiBool m_bSetPos;              // 防止SetPos循环调用
    ZuiBool m_drag;                 //用于窗口拖拽
    ZuiText m_sText;                //
    ZuiText m_sName;                //控件名 主要用于查找xml对象
    ZuiText m_sToolTip;             //提示文本
    _ZuiText m_chShortcut;          //快捷键
    ZuiAny m_sUserData;              //

#if (defined HAVE_JS) && (HAVE_JS == 1)
    //js_ref
    ZuiInt m_rOnclick;
    ZuiInt m_rOnmouseleave;
    ZuiInt m_rOnmouseenter;
    ZuiInt m_rOnlbuttondown;
    ZuiInt m_rOnchar;
    ZuiInt m_rOnsize;
    //js_refend
#endif
    //控件默认样式-------------------
    ZuiInt m_tyle;                  //控件风格
    ZuiColor m_BkgColor;            //背景颜色
    ZuiRes m_BkgImg;                //背景图片
    ZuiColor m_dwBorderColor;       //边框颜色
	ZuiInt m_dwBorderWidth;
    //控件默认样式-------------------

    ZuiAnimation m_aAnime;          //控件动画

    int m_nTooltipWidth;            //多行ToolTip单行最长宽度
#if RUN_DEBUG
    ZuiText m_sClassName;           //控件类名
    ZuiAny  m_aTreeHwndl;           //在调试器中的树控件句柄
#endif
}*ZuiControl, ZControl;

//控件默认处理函数
ZEXPORT ZuiAny ZCALL ZuiDefaultControlProc(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);




//发送用户通知
ZEXPORT ZuiAny ZCALL ZuiControlNotify(ZuiText msg, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
//注册通知函数
ZEXPORT ZuiVoid ZCALL ZuiControlRegNotify(ZuiControl p, ZNotifyProc pNotify);

ZEXPORT ZuiControl ZCALL ZuiControlFindName(ZuiControl p, ZuiText Name);

//客户坐标转屏幕坐标
ZuiVoid ZuiClientToScreen(ZuiControl p, ZuiPoint pt);
ZuiVoid ZuiScreenToClient(ZuiControl p, ZuiPoint pt);

ZEXPORT ZuiVoid ZCALL ZuiControlInvalidate(ZuiControl p, ZuiBool ResetAnimation);           //刷新显示
ZEXPORT ZuiVoid ZCALL ZuiControlNeedUpdate(ZuiControl p);                                   //更新布局
ZEXPORT ZuiVoid ZCALL ZuiControlNeedParentUpdate(ZuiControl p);                             //更新父控件布局
ZEXPORT ZuiVoid ZCALL ZuiControlEvent(ZuiControl p, TEventUI *event);                       //发送事件

                                                                                             //-------------------------------------------------------------------------------------------------
ZuiControl ZCALL __FindControlFromCount(ZuiControl pThis, ZuiAny pData);//计算控件数量
ZuiControl ZCALL __FindControlFromPoint(ZuiControl pThis, ZuiAny pData);//根据点是否在区域中，查询控件
ZuiControl ZCALL __FindControlFromTab(ZuiControl pThis, ZuiAny pData);//通过Tab信息查询控件
ZuiControl ZCALL __FindControlFromShortcut(ZuiControl pThis, ZuiAny pData);//从快捷键查询控件
ZuiControl ZCALL __FindControlFromName(ZuiControl pThis, ZuiAny pData);// 通过名称比较查询控件
ZuiControl ZCALL __FindControlsFromUpdate(ZuiControl pThis, ZuiAny pData);

#endif // __CONTROL_H__
