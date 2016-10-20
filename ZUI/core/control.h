#ifndef __UICONTROL_H__
#define __UICONTROL_H__

/////////////////////////////////////////////////////////////////////////////////////
//
typedef struct _ZControl *ZuiControl, ZControl;

typedef ZuiControl(ZCALL* FINDCONTROLPROC)(ZuiControl, LPVOID);
typedef ZuiAny (ZCALL *ZCtlProc)(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

#define	Proc_GetControlFlags	1	//
#define	Proc_Activate			2	//活动
#define	Proc_SetVisible			3	//设置是否可视
#define	Proc_SetText			4	//设置文本
#define	Proc_GetPos				5	//得到控件位置
#define	Proc_SetPos				6	//设置控件位置并重绘
#define	Proc_SetManager			7	//设置控件的绘制管理者
//设置大小的限制值
#define	Proc_GetMinWidth		8	//
#define	Proc_SetMinWidth		9	//
#define	Proc_GetMaxWidth		10	//
#define	Proc_SetMaxWidth		11	//
#define	Proc_GetMinHeight		12	//
#define	Proc_SetMinHeight		13	//
#define	Proc_GetMaxHeight		14	//
#define	Proc_SetMaxHeight		15	//

#define	Proc_GetWidth			16	//
#define	Proc_GetHeight			17	//
#define	Proc_GetX				18	//
#define	Proc_GetY				19
#define	Proc_GetPadding			20
#define	Proc_SetPadding			21	// 设置外边距，由上层窗口绘制
#define	Proc_GetFixedXY			22	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedXY			23	// 仅float为true时有效
#define	Proc_GetFixedWidth		24	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedWidth		25	// 预设的参考值
#define	Proc_GetFixedHeight		26	// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define	Proc_SetFixedHeight		27	// 预设的参考值
#define	Proc_GetRelativePos		40	// 相对(父控件)位置

#define	Proc_SetFloat			37	//设置为浮动控件
#define	Proc_SetEnabled			38	//设置可用状态
#define	Proc_SetFocus			39	//设置焦点


#define	Proc_EstimateSize		28	//获取自适应大小
#define	Proc_FindControl		29	//查找控件

#define	Proc_OnCreate			30	//内核创建
#define	Proc_OnInit				31	//用户初始化
#define	Proc_OnDestroy			32	//销毁
#define	Proc_OnSize				32	//
#define	Proc_OnEvent			33	//
#define	Proc_OnNotify			34	//
#define	Proc_OnPaint			35	//
#define	Proc_OnPostPaint		36	//

typedef struct _ZControl
{
	//消息处理函数指针
	ZCtlProc call;

	ZuiPaintManager m_pManager;		//关联的管理器
	struct _ZControl* m_pParent;	//父控件
	char *m_sName;					//
	BOOL m_bUpdateNeeded;			//是否需要更新布局
	BOOL m_bMenuUsed;				//
	RECT m_rcItem;					//
	RECT m_rcPadding;				//
	SIZE m_cXY;						//预设的左上角坐标
	SIZE m_cxyFixed;				//预设的控件大小
	SIZE m_cxyMin;					//控件最小大小
	SIZE m_cxyMax;					//控件最大大小
	BOOL m_bVisible;				//可视
	BOOL m_bEnabled;				//激活
	BOOL m_bMouseEnabled;			//鼠标激活
	BOOL m_bKeyboardEnabled;		//键盘激活
	BOOL m_bFocused;				//焦点
	BOOL m_bFloat;					//浮动控件
	TPercentInfo m_piFloatPercent;
	BOOL m_bSetPos;					// 防止SetPos循环调用

	LPCTSTR m_sText;				//
	LPCTSTR m_sToolTip;				//提示文本
	TCHAR m_chShortcut;				//快捷键
	void *m_sUserData;				//

	int m_nTooltipWidth;			//多行ToolTip单行最长宽度
	RECT m_rcPaint;					//
}*ZuiControl, ZControl;

//控件默认处理函数
ZAPI(ZuiAny) ZuiDefaultControlProc(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

//创建控件
ZuiControl NewCControlUI(char *classname, void* Param1, void* Param2, void* Param3);
//销毁控件
void FreeCControlUI(ZuiControl p);

//调用控件处理函数
ZAPI(ZuiAny) ZuiControlCall(ZuiInt ProcId, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


ZAPI(ZuiVoid) ZuiControlInvalidate(ZuiControl p);				//刷新显示
ZAPI(ZuiVoid) ZuiControlNeedUpdate(ZuiControl p);				//更新布局
ZAPI(ZuiVoid) ZuiControlNeedParentUpdate(ZuiControl p);			//更新父控件布局
ZAPI(ZuiVoid) ZuiControlEvent(ZuiControl p, TEventUI *event);	//发送事件
ZAPI(ZuiVoid) ZuiControlMove(ZuiControl p, SIZE szOffset, BOOL bNeedInvalidate); //移动位置


#endif // __UICONTROL_H__
