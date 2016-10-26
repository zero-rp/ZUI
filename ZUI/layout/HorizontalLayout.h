/**
* @file		HorizontalLayout.h
* @brief	横向布局容器.
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
#ifndef __HORIZONTALLAYOUT_H__
#define __HORIZONTALLAYOUT_H__

/**横向布局结构*/
typedef struct _ZuiHorizontalLayout
{
	ZCtlProc old_call;
	void *old_udata;
}*ZuiHorizontalLayout, ZHorizontalLayout;

void* CALLBACK ZuiHorizontalLayoutProc(int ProcId, ZuiControl cp, ZuiHorizontalLayout p, void* Param1, void* Param2, void* Param3);

#endif // __HORIZONTALLAYOUT_H__
