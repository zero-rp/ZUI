/**
* @file		control.h
* @brief	控件基类.
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
#ifndef __CONTROL_H__
#define __CONTROL_H__

/////////////////////////////////////////////////////////////////////////////////////
//
typedef struct _ZControl *ZuiControl, ZControl;

typedef ZuiControl(ZCALL* FINDCONTROLPROC)(ZuiControl, LPVOID);
typedef ZuiAny(ZCALL *ZCtlProc)(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
typedef ZuiAny(ZCALL *ZNotifyProc)(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

#undef  MODULE
#define MODULE  Control

#define ZTYLE_BOX				1	//单线边框
#define ZTYLE_BKGColor			2	//具有背景色
//-----控件事件
#define	Proc_CoreInit			1	//用于控件内核初始化
#define	Proc_CoreUnInit			2	//用于控件内核反初始化
#define	Proc_OnCreate			3	//内核创建
#define	Proc_OnInit				4	//用户初始化
#define	Proc_OnDestroy			5	//销毁
#define	Proc_OnSize				6	//
#define	Proc_OnEvent			7	//
#define	Proc_OnNotify			8	//
#define	Proc_OnPaint			9	// 绘制循序：背景颜色->背景图->状态图->文本->边框
#define	Proc_OnPaintBkColor		10	//背景色
#define	Proc_OnPaintBkImage		11	//背景图片
#define	Proc_OnPaintStatusImage	12	//状态图片
#define	Proc_OnPaintText		13	//文本
#define	Proc_OnPaintBorder		14	//边框
#define	Proc_OnPostPaint		15	//

#define	Proc_FindControl		16	//查找控件
#define	Proc_JsCall				17	//函数调用
#define	Proc_JsPut				18	//
#define	Proc_JsHas				19	//处理js对象 返回0则是变量,返回1则是call

//----控件属性
#define	Proc_SetAttribute		20	//解析属性
#define	Proc_GetAttribute		21	//取属性
#define	Proc_GetControlFlags	22	//
#define	Proc_Activate			23	//活动
#define	Proc_SetVisible			24	//设置是否可视
#define	Proc_SetText			25	//设置文本
#define	Proc_GetText			26	//设置文本
#define	Proc_SetName			27	//设置名字
#define	Proc_SetTooltip			28	//设置提示文本
#define	Proc_GetPos				29	//得到控件位置
#define	Proc_SetPos				30	//设置控件位置并重绘
#define	Proc_SetManager			31	//设置控件的绘制管理者
//设置大小的限制值
#define	Proc_GetMinWidth		32	//
#define	Proc_SetMinWidth		33	//
#define	Proc_GetMaxWidth		34	//
#define	Proc_SetMaxWidth		35	//
#define	Proc_GetMinHeight		36	//
#define	Proc_SetMinHeight		37	//
#define	Proc_GetMaxHeight		38	//
#define	Proc_SetMaxHeight		39	//
#define	Proc_GetWidth			40	//
#define	Proc_GetHeight			41	//
#define	Proc_GetX				42	//
#define	Proc_GetY				43
#define	Proc_EstimateSize		44	//获取自适应大小
#define	Proc_SetFloatPercent	45	//
#define	Proc_GetPadding			46
#define	Proc_SetPadding			47	// 设置外边距，由上层窗口绘制
#define	Proc_GetFixedXY			48	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedXY         49	// 仅float为true时有效
#define	Proc_GetFixedWidth		50	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedWidth		51	// 预设的参考值
#define	Proc_GetFixedHeight		52	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedHeight		53	// 预设的参考值
#define	Proc_GetRelativePos		54	// 相对(父控件)位置
#define Proc_GetImePoint		55	// 获取输入法位置
#define	Proc_SetFloat			56	//设置为浮动控件
#define	Proc_SetEnabled			57	//设置可用状态
#define	Proc_SetFocus			58	//设置焦点
#define	Proc_SetDrag			59	//设置拖拽控件

//-------绘图资源
#define	Proc_SetBkColor			60	//设置背景色
#define	Proc_SetBkImage			61	//设置背景图片
#define	Proc_SetBorderColor		62	//设置边框颜色

#define Proc_SetAnimationType   63  //设置动画类型

//-------控件状态
#define ZSTATE_FOCUSED      0x00000001
#define ZSTATE_SELECTED     0x00000002
#define ZSTATE_DISABLED     0x00000004
#define ZSTATE_HOT          0x00000008
#define ZSTATE_PUSHED       0x00000010
#define ZSTATE_READONLY     0x00000020
#define ZSTATE_CAPTURED     0x00000040

//------宏封装
#define ZuiControlCoreInit(Control, ...) ZProc(Control, CoreInit, __VA_ARGS__);
#define ZuiControlOnCreate(Control, ...) ZProc(Control, OnCreate, __VA_ARGS__);
#define ZuiControlOnInit(Control, ...) ZProc(Control, OnInit, __VA_ARGS__);
#define ZuiControlOnDestroy(Control, ...) ZProc(Control, OnDestroy, __VA_ARGS__);
#define ZuiControlOnSize(Control, ...) ZProc(Control, OnSize, __VA_ARGS__);
#define ZuiControlOnEvent(Control, ...) ZProc(Control, OnEvent, __VA_ARGS__);
#define ZuiControlOnNotify(Control, ...) ZProc(Control, OnNotify, __VA_ARGS__);
#define ZuiControlOnPaint(Control, ...) ZProc(Control, OnPaint, __VA_ARGS__);
#define ZuiControlOnPaintBkColor(Control, ...) ZProc(Control, OnPaintBkColor, __VA_ARGS__);
#define ZuiControlOnPaintBkImage(Control, ...) ZProc(Control, OnPaintBkImage, __VA_ARGS__);
#define ZuiControlOnPaintStatusImage(Control, ...) ZProc(Control, OnPaintStatusImage, __VA_ARGS__);
#define ZuiControlOnPaintText(Control, ...) ZProc(Control, OnPaintText, __VA_ARGS__);
#define ZuiControlOnPaintBorder(Control, ...) ZProc(Control, OnPaintBorder, __VA_ARGS__);
#define ZuiControlOnPostPaint(Control, ...) ZProc(Control, OnPostPaint, __VA_ARGS__);
#define ZuiControlFindControl(Control, ...) ZProc(Control, FindControl, __VA_ARGS__);
#define ZuiControlJsCall(Control, ...) ZProc(Control, JsCall, __VA_ARGS__);
#define ZuiControlJsPut(Control, ...) ZProc(Control, JsPut, __VA_ARGS__);
#define ZuiControlJsHas(Control, ...) ZProc(Control, JsHas, __VA_ARGS__);
#define ZuiControlSetAttribute(Control, ...) ZProc(Control, SetAttribute, __VA_ARGS__);
#define ZuiControlGetAttribute(Control, ...) ZProc(Control, GetAttribute, __VA_ARGS__);
#define ZuiControlGetControlFlags(Control, ...) ZProc(Control, GetControlFlags, __VA_ARGS__);
#define ZuiControlActivate(Control, ...) ZProc(Control, Activate, __VA_ARGS__);
#define ZuiControlSetVisible(Control, ...) ZProc(Control, SetVisible, __VA_ARGS__);
#define ZuiControlSetText(Control, ...) ZProc(Control, SetText, __VA_ARGS__);
#define ZuiControlGetText(Control, ...) ZProc(Control, GetText, __VA_ARGS__);
#define ZuiControlSetName(Control, ...) ZProc(Control, SetName, __VA_ARGS__);
#define ZuiControlSetTooltip(Control, ...) ZProc(Control, SetTooltip, __VA_ARGS__);
#define ZuiControlGetPos(Control, ...) ZProc(Control, GetPos, __VA_ARGS__);
#define ZuiControlSetPos(Control, ...) ZProc(Control, SetPos, __VA_ARGS__);
#define ZuiControlSetManager(Control, ...) ZProc(Control, SetManager, __VA_ARGS__);
#define ZuiControlGetMinWidth(Control, ...) ZProc(Control, GetMinWidth, __VA_ARGS__);
#define ZuiControlSetMinWidth(Control, ...) ZProc(Control, SetMinWidth, __VA_ARGS__);
#define ZuiControlGetMaxWidth(Control, ...) ZProc(Control, GetMaxWidth, __VA_ARGS__);
#define ZuiControlSetMaxWidth(Control, ...) ZProc(Control, SetMaxWidth, __VA_ARGS__);
#define ZuiControlGetMinHeight(Control, ...) ZProc(Control, GetMinHeight, __VA_ARGS__);
#define ZuiControlSetMinHeight(Control, ...) ZProc(Control, SetMinHeight, __VA_ARGS__);
#define ZuiControlGetMaxHeight(Control, ...) ZProc(Control, GetMaxHeight, __VA_ARGS__);
#define ZuiControlSetMaxHeight(Control, ...) ZProc(Control, SetMaxHeight, __VA_ARGS__);
#define ZuiControlGetWidth(Control, ...) ZProc(Control, GetWidth, __VA_ARGS__);
#define ZuiControlGetHeight(Control, ...) ZProc(Control, GetHeight, __VA_ARGS__);
#define ZuiControlGetX(Control, ...) ZProc(Control, GetX, __VA_ARGS__);
#define ZuiControlEstimateSize(Control, ...) ZProc(Control, EstimateSize, __VA_ARGS__);
#define ZuiControlSetFloatPercent(Control, ...) ZProc(Control, SetFloatPercent, __VA_ARGS__);
#define ZuiControlSetPadding(Control, ...) ZProc(Control, SetPadding, __VA_ARGS__);
#define ZuiControlGetFixedXY(Control, ...) ZProc(Control, GetFixedXY, __VA_ARGS__);
#define ZuiControlSetFixedXY(Control, ...) ZProc(Control, SetFixedXY, __VA_ARGS__);
#define ZuiControlGetFixedWidth(Control, ...) ZProc(Control, GetFixedWidth, __VA_ARGS__);
#define ZuiControlSetFixedWidth(Control, ...) ZProc(Control, SetFixedWidth, __VA_ARGS__);
#define ZuiControlGetFixedHeight(Control, ...) ZProc(Control, GetFixedHeight, __VA_ARGS__);
#define ZuiControlSetFixedHeight(Control, ...) ZProc(Control, SetFixedHeight, __VA_ARGS__);
#define ZuiControlGetRelativePos(Control, ...) ZProc(Control, GetRelativePos, __VA_ARGS__);
#define ZuiControlGetImePoint(Control, ...) ZProc(Control, GetImePoint, __VA_ARGS__);
#define ZuiControlSetFloat(Control, ...) ZProc(Control, SetFloat, __VA_ARGS__);
#define ZuiControlSetEnabled(Control, ...) ZProc(Control, SetEnabled, __VA_ARGS__);
#define ZuiControlSetFocus(Control, ...) ZProc(Control, SetFocus, __VA_ARGS__);
#define ZuiControlSetDrag(Control, ...) ZProc(Control, SetDrag, __VA_ARGS__);
#define ZuiControlSetBkColor(Control, ...) ZProc(Control, SetBkColor, __VA_ARGS__);
#define ZuiControlSetBkImage(Control, ...) ZProc(Control, SetBkImage, __VA_ARGS__);
#define ZuiControlSetBorderColor(Control, ...) ZProc(Control, SetBorderColor, __VA_ARGS__);

enum ZAttType
{
    ZAttType_JsCall = 1,
    ZAttType_String
};
/**属性结构*/
typedef struct _ZAttribute
{
    ZuiAny v;
    ZuiInt vlen;
    ZuiInt type;
}*ZuiAttribute, ZAttribute;
typedef struct _ZControl
{
    //消息处理函数指针
    ZCtlProc call;
    //通知接口地址
    ZNotifyProc m_pNotify;

    ZuiPaintManager m_pManager;		//关联的管理器
    struct _ZControl* m_pParent;	//父控件
    ZuiBool m_bUpdateNeeded;		//是否需要更新布局
    RECT m_rcItem;					//控件矩形
    RECT m_rcPadding;				//控件
    SIZE m_cXY;						//预设的左上角坐标
    SIZE m_cxyFixed;				//预设的控件大小
    SIZE m_cxyMin;					//控件最小大小
    SIZE m_cxyMax;					//控件最大大小
    ZuiBool m_bVisible;				//可视
    ZuiBool m_bEnabled;				//激活
    ZuiBool m_bMouseEnabled;		//鼠标激活
    ZuiBool m_bKeyboardEnabled;		//键盘激活
    ZuiBool m_bFocused;				//焦点
    ZuiBool m_bFloat;				//浮动控件
    TPercentInfo m_piFloatPercent;
    ZuiBool m_bSetPos;				// 防止SetPos循环调用
    ZuiBool m_drag;					//用于窗口拖拽
    ZuiText m_sText;				//
    ZuiText m_sName;				//控件名 主要用于查找xml对象
    ZuiText m_sToolTip;				//提示文本
    _ZuiText m_chShortcut;			//快捷键
    void *m_sUserData;				//

    rb_root *m_rAttribute;			//属性map

    //控件默认样式-------------------
    ZuiInt m_tyle;					//控件风格
    ZuiColor m_BkgColor;			//背景颜色
    ZuiRes m_BkgImg;				//背景图片
    ZuiColor m_dwBorderColor;		//边框颜色
    //控件默认样式-------------------

    ZuiAnimation m_aAnime;          //控件动画

    int m_nTooltipWidth;			//多行ToolTip单行最长宽度
}*ZuiControl, ZControl;

//控件默认处理函数
ZEXPORT ZuiAny ZCALL ZuiDefaultControlProc(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

//创建控件
ZuiControl NewZuiControl(ZuiText classname, void* Param1, void* Param2, void* Param3);
//销毁控件
void FreeZuiControl(ZuiControl p, ZuiBool Delayed);

//调用控件处理函数
ZEXPORT ZuiAny ZCALL ZuiControlCall(ZuiInt ProcId, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
//发送用户通知
ZEXPORT ZuiAny ZCALL ZuiControlNotify(ZuiText msg, ZuiControl p, ZuiAny Param1, ZuiInt p1Type, ZuiAny Param2, ZuiInt p2Type, ZuiAny Param3, ZuiInt p3Type);
//注册通知函数
ZEXPORT ZuiVoid ZCALL ZuiControlRegNotify(ZuiControl p, ZNotifyProc pNotify);

//客户坐标转屏幕坐标
ZEXPORT ZuiVoid ZCALL ZuiClientToScreen(ZuiControl p, ZuiPoint pt);
ZEXPORT ZuiVoid ZCALL ZuiScreenToClient(ZuiControl p, ZuiPoint pt);

ZEXPORT ZuiControl ZCALL ZuiControlFindName(ZuiControl p, ZuiText Name);

ZEXPORT ZuiVoid ZCALL ZuiControlInvalidate(ZuiControl p, ZuiBool ResetAnimation);				//刷新显示
ZEXPORT ZuiVoid ZCALL ZuiControlNeedUpdate(ZuiControl p);				//更新布局
ZEXPORT ZuiVoid ZCALL ZuiControlNeedParentUpdate(ZuiControl p);			//更新父控件布局
ZEXPORT ZuiVoid ZCALL ZuiControlEvent(ZuiControl p, TEventUI *event);	//发送事件
ZEXPORT ZuiVoid ZCALL ZuiControlMove(ZuiControl p, SIZE szOffset, BOOL bNeedInvalidate); //移动位置


#endif // __CONTROL_H__
