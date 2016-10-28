/**
* @file		Option.h
* @brief	选择框控件实现.
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
#ifndef __OPTION_H__
#define __OPTION_H__

#define	Proc_Option_SetSelected				1031	//
#define	Proc_Option_GetSelected				1032	//
#define	Proc_Option_SetResSelected			1033			//选中的普通状态
#define	Proc_Option_SetResSelectedHot		1034		//选中的点燃状态
#define	Proc_Option_SetResSelectedPushed	1035	//选中的按下状态
#define	Proc_Option_SetGroup				1036
/**按钮控件结构*/
typedef struct _ZOption
{
	ZuiBool m_bSelected;//是否选择
	ZuiBool m_bGroup;	//
	//控件继承自按钮控件
	ZuiRes		m_ResSelected;			//选中的普通状态
	ZuiRes		m_ResSelectedHot;		//选中的点燃状态
	ZuiRes		m_ResSelectedPushed;	//选中的按下状态

	ZCtlProc old_call;
	ZuiAny old_udata;
}*ZuiOption, ZOption;
ZEXPORT ZuiAny ZCALL ZuiOptionProc(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__OPTION_H__
