/**
* @file		Button.h
* @brief	按钮控件实现.
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
#ifndef __BUTTON_H__
#define __BUTTON_H__
#define	Proc_Button_SetResNormal	1001	//
#define	Proc_Button_SetResHot		1002	//高亮状态
#define	Proc_Button_SetResPushed	1003	//按下状态
#define	Proc_Button_SetResFocused	1004	//焦点图片
#define	Proc_Button_SetResDisabled	1005	//非激活状态
/**按钮控件结构*/
typedef struct _ZButton
{
	ZuiRes m_ResNormal;		//正常状态
	ZuiRes m_ResHot;		//高亮状态
	ZuiRes m_ResPushed;		//按下状态
	ZuiRes m_ResFocused;	//焦点图片
	ZuiRes m_ResDisabled;	//非激活状态

	int type;
	ZCtlProc old_call;
	ZuiAny old_udata;
}*ZuiButton, ZButton;
ZEXPORT ZuiAny ZCALL ZuiButtonProc(ZuiInt ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__BUTTON_H__
