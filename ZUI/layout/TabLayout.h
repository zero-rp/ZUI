/**
* @file     TabLayout.h
* @brief    选择夹布局类.
* @author   [Zero](22249030)
* @version  1.0
* @date     $date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __TABLAYOUT_H__
#define __TABLAYOUT_H__
#define Type_TabLayout              0x61b7311c

#define Proc_TabLayout_SelectItem  151    //当前Tab

//------Js

#define Js_Id_TabLayout_selectitem    200


/**选择夹布局结构*/
typedef struct _ZuiTabLayout
{
    ZuiInt m_iCurSel;//当前选择夹
    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiTabLayout, ZTabLayout;

void* ZCALL ZuiTabLayoutProc(int ProcId, ZuiControl cp, ZuiTabLayout p, void* Param1, void* Param2, void* Param3);




#endif // __UITABLAYOUT_H__
