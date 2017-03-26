/**
* @file     ProgressBar.h
* @brief    进度条实现.
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
#ifndef __ProgressBar_H__
#define __ProgressBar_H__
#define Type_ProgressBar            0x251a8a44

/**按钮控件结构*/
typedef struct _ZProgressBar
{

    ZCtlProc old_call;
}*ZuiProgressBar, ZProgressBar;
ZEXPORT ZuiAny ZCALL ZuiProgressBarProc(ZuiInt ProcId, ZuiControl cp, ZuiProgressBar p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__ProgressBar_H__
