/**
* @file     CheckBox.h
* @brief    虚拟控件实现.
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
#ifndef __CheckBox_H__
#define __CheckBox_H__
#define Type_CheckBox            0x251a8a44

/**按钮控件结构*/
typedef struct _ZCheckBox
{

    ZCtlProc old_call;
}*ZuiCheckBox, ZCheckBox;
ZEXPORT ZuiAny ZCALL ZuiCheckBoxProc(ZuiInt ProcId, ZuiControl cp, ZuiCheckBox p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__CheckBox_H__
