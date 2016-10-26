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

/**按钮控件结构*/
typedef struct _ZOption
{

	ZCtlProc old_call;
}*ZuiOption, ZOption;
ZEXPORT ZuiAny ZCALL ZuiOptionProc(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__OPTION_H__
