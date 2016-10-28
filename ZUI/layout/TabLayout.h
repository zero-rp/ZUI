/**
* @file		TabLayout.h
* @brief	选择夹布局类.
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
#ifndef __TABLAYOUT_H__
#define __TABLAYOUT_H__


#define	Proc_Layout_Tab_SelectItem			1001	//当前Tab


/**选择夹布局结构*/
typedef struct _ZuiTabLayout
{
	int m_iCurSel;
	ZCtlProc old_call;
	void *old_udata;
}*ZuiTabLayout, ZTabLayout;

void* CALLBACK ZuiTabLayoutProc(int ProcId, ZuiControl cp, ZuiTabLayout p, void* Param1, void* Param2, void* Param3);




#endif // __UITABLAYOUT_H__
