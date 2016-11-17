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

#define ZTYLE_BOX				1	//单线边框
#define ZTYLE_BKGColor			2	//具有背景色
//-----控件事件
#define	Proc_CoreInit			1	//用于控件内核初始化
#define	Proc_OnCreate			2	//内核创建
#define	Proc_OnInit				3	//用户初始化
#define	Proc_OnDestroy			4	//销毁
#define	Proc_OnSize				5	//
#define	Proc_OnEvent			6	//
#define	Proc_OnNotify			7	//
#define	Proc_OnPaint			8	// 绘制循序：背景颜色->背景图->状态图->文本->边框
#define	Proc_OnPaintBkColor		9	//背景色
#define	Proc_OnPaintBkImage		10	//背景图片
#define	Proc_OnPaintStatusImage	11	//状态图片
#define	Proc_OnPaintText		12	//文本
#define	Proc_OnPaintBorder		13	//边框
#define	Proc_OnPostPaint		14	//

#define	Proc_FindControl		15	//查找控件
#define	Proc_JsCall				16	//函数调用
#define	Proc_JsPut				17	//
#define	Proc_JsHas				18	//处理js对象 返回0则是变量,返回1则是call

//----控件属性
#define	Proc_SetAttribute		19	//解析属性
#define	Proc_GetAttribute		20	//取属性
#define	Proc_GetControlFlags	21	//
#define	Proc_Activate			22	//活动
#define	Proc_SetVisible			23	//设置是否可视
#define	Proc_SetText			24	//设置文本
#define	Proc_SetName			25	//设置名字
#define	Proc_SetTooltip			26	//设置提示文本
#define	Proc_GetPos				27	//得到控件位置
#define	Proc_SetPos				28	//设置控件位置并重绘
#define	Proc_SetManager			29	//设置控件的绘制管理者
//设置大小的限制值
#define	Proc_GetMinWidth		30	//
#define	Proc_SetMinWidth		31	//
#define	Proc_GetMaxWidth		32	//
#define	Proc_SetMaxWidth		33	//
#define	Proc_GetMinHeight		34	//
#define	Proc_SetMinHeight		35	//
#define	Proc_GetMaxHeight		36	//
#define	Proc_SetMaxHeight		37	//

#define	Proc_GetWidth			38	//
#define	Proc_GetHeight			39	//
#define	Proc_GetX				40	//
#define	Proc_GetY				41
#define	Proc_EstimateSize		42	//获取自适应大小
#define	Proc_SetFloatPercent	43	//
#define	Proc_GetPadding			44
#define	Proc_SetPadding			45	// 设置外边距，由上层窗口绘制
#define	Proc_GetFixedXY			46	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedXY			47	// 仅float为true时有效
#define	Proc_GetFixedWidth		48	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedWidth		49	// 预设的参考值
#define	Proc_GetFixedHeight		50	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedHeight		51	// 预设的参考值
#define	Proc_GetRelativePos		52	// 相对(父控件)位置
#define Proc_GetImePoint		53	
#define	Proc_SetFloat			54	//设置为浮动控件
#define	Proc_SetEnabled			55	//设置可用状态
#define	Proc_SetFocus			56	//设置焦点
#define	Proc_SetDrag			57	//设置拖拽控件

//-------绘图资源
#define	Proc_SetBkColor			58	//设置背景色
#define	Proc_SetBkImage			59	//设置背景图片
#define	Proc_SetBorderColor		60	//设置边框颜色


//-------控件状态
#define ZSTATE_FOCUSED      0x00000001
#define ZSTATE_SELECTED     0x00000002
#define ZSTATE_DISABLED     0x00000004
#define ZSTATE_HOT          0x00000008
#define ZSTATE_PUSHED       0x00000010
#define ZSTATE_READONLY     0x00000020
#define ZSTATE_CAPTURED     0x00000040


typedef enum ZAttType
{
	ZAttType_JsCall = 1,
	ZAttType_String,
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
	ZuiBool m_bMenuUsed;			//
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
	ZuiColor m_dwBorderColor;				//边框颜色
	//控件默认样式-------------------

	int m_nTooltipWidth;			//多行ToolTip单行最长宽度
	RECT m_rcPaint;					//
}*ZuiControl, ZControl;

//控件默认处理函数
ZEXPORT ZuiAny ZCALL ZuiDefaultControlProc(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

//创建控件
ZuiControl NewZuiControl(ZuiText classname, void* Param1, void* Param2, void* Param3);
//销毁控件
void FreeCControlUI(ZuiControl p);

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

ZEXPORT ZuiVoid ZCALL ZuiControlInvalidate(ZuiControl p);				//刷新显示
ZEXPORT ZuiVoid ZCALL ZuiControlNeedUpdate(ZuiControl p);				//更新布局
ZEXPORT ZuiVoid ZCALL ZuiControlNeedParentUpdate(ZuiControl p);			//更新父控件布局
ZEXPORT ZuiVoid ZCALL ZuiControlEvent(ZuiControl p, TEventUI *event);	//发送事件
ZEXPORT ZuiVoid ZCALL ZuiControlMove(ZuiControl p, SIZE szOffset, BOOL bNeedInvalidate); //移动位置


#endif // __CONTROL_H__
