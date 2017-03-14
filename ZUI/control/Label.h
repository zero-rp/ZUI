/**
* @file     Label.h
* @brief    标签控件实现.
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
#ifndef __LABEL_H__
#define __LABEL_H__
#define Type_Label        0xe538ac26
/**按钮控件结构*/
typedef struct _ZLabel
{
    ZuiRes font;    //字体控制
    ZCtlProc old_call;
}*ZuiLabel, ZLabel;
ZEXPORT ZuiAny ZCALL ZuiLabelProc(ZuiInt ProcId, ZuiControl cp, ZuiLabel p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__LABEL_H__
