/**
* @file		VerticalLayout.h
* @brief	纵向布局容器.
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
#ifndef __VERTICALLAYOUT_H__
#define __VERTICALLAYOUT_H__
/**纵向布局结构*/
typedef struct _ZuiVerticalLayout
{
	ZCtlProc old_call;
	void *old_udata;
}*ZuiVerticalLayout, ZVerticalLayout;

void* ZCALL ZuiVerticalLayoutProc(int ProcId, ZuiControl cp, ZuiVerticalLayout p, void* Param1, void* Param2, void* Param3);
#endif // __UIVERTICALLAYOUT_H__
