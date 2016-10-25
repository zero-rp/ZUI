#ifndef __CONTROL_H__
#define __CONTROL_H__

/////////////////////////////////////////////////////////////////////////////////////
//
typedef struct _ZControl *ZuiControl, ZControl;

typedef ZuiControl(ZCALL* FINDCONTROLPROC)(ZuiControl, LPVOID);
typedef ZuiAny (ZCALL *ZCtlProc)(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

#define ZTYLE_BOX				1	//单线边框
#define ZTYLE_BKGColor			2	//具有背景色

#define	Proc_CoreInit			1	//用于控件内核初始化
#define	Proc_SetAttribute		2	//解析属性
#define	Proc_GetControlFlags	3	//
#define	Proc_Activate			4	//活动
#define	Proc_SetVisible			5	//设置是否可视
#define	Proc_SetText			6	//设置文本
#define	Proc_SetTooltip			44	//设置提示文本
#define	Proc_GetPos				7	//得到控件位置
#define	Proc_SetPos				8	//设置控件位置并重绘
#define	Proc_SetManager			9	//设置控件的绘制管理者
//设置大小的限制值
#define	Proc_GetMinWidth		10	//
#define	Proc_SetMinWidth		11	//
#define	Proc_GetMaxWidth		12	//
#define	Proc_SetMaxWidth		13	//
#define	Proc_GetMinHeight		14	//
#define	Proc_SetMinHeight		15	//
#define	Proc_GetMaxHeight		16	//
#define	Proc_SetMaxHeight		17	//

#define	Proc_GetWidth			18	//
#define	Proc_GetHeight			19	//
#define	Proc_GetX				20	//
#define	Proc_GetY				21
#define	Proc_GetPadding			22
#define	Proc_SetPadding			23	// 设置外边距，由上层窗口绘制
#define	Proc_GetFixedXY			24	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedXY			25	// 仅float为true时有效
#define	Proc_GetFixedWidth		26	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedWidth		27	// 预设的参考值
#define	Proc_GetFixedHeight		28	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedHeight		29	// 预设的参考值
#define	Proc_GetRelativePos		30	// 相对(父控件)位置

#define	Proc_SetFloat			31	//设置为浮动控件
#define	Proc_SetEnabled			32	//设置可用状态
#define	Proc_SetFocus			33	//设置焦点
#define	Proc_SetDrag			46	//设置拖拽控件

#define	Proc_SetBkColor			45	//设置背景色
#define	Proc_SetBkImage			47	//设置背景图片

#define	Proc_EstimateSize		34	//获取自适应大小
#define	Proc_FindControl		35	//查找控件

#define	Proc_OnCreate			36	//内核创建
#define	Proc_OnInit				37	//用户初始化
#define	Proc_OnDestroy			38	//销毁
#define	Proc_OnSize				39	//
#define	Proc_OnEvent			40	//
#define	Proc_OnNotify			41	//
#define	Proc_OnPaint			42	//
#define	Proc_OnPostPaint		43	//
/**控件基类结构*/
typedef struct _ZControl
{
	//消息处理函数指针
	ZCtlProc call;

	ZuiPaintManager m_pManager;		//关联的管理器
	struct _ZControl* m_pParent;	//父控件
	ZuiBool m_bUpdateNeeded;		//是否需要更新布局
	ZuiBool m_bMenuUsed;			//
	RECT m_rcItem;					//
	RECT m_rcPadding;				//
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
	_ZuiText m_chShortcut;				//快捷键
	void *m_sUserData;				//

	//控件默认样式-------------------
	ZuiInt m_tyle;					//控件风格
	ZuiColor m_BkgColor;			//背景颜色
	ZuiRes m_BkgImg;				//背景图片
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


ZEXPORT ZuiVoid ZCALL ZuiControlInvalidate(ZuiControl p);				//刷新显示
ZEXPORT ZuiVoid ZCALL ZuiControlNeedUpdate(ZuiControl p);				//更新布局
ZEXPORT ZuiVoid ZCALL ZuiControlNeedParentUpdate(ZuiControl p);			//更新父控件布局
ZEXPORT ZuiVoid ZCALL ZuiControlEvent(ZuiControl p, TEventUI *event);	//发送事件
ZEXPORT ZuiVoid ZCALL ZuiControlMove(ZuiControl p, SIZE szOffset, BOOL bNeedInvalidate); //移动位置


#endif // __CONTROL_H__
