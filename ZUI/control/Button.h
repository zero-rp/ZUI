/**
* @file     Button.h
* @brief    按钮控件实现.
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
#ifndef __BUTTON_H__
#define __BUTTON_H__
#define Type_Button                 0x5e7331d7

#define Proc_Button_SetResNormal    201    //
#define Proc_Button_SetResHot       202    //高亮状态
#define Proc_Button_SetResPushed    203    //按下状态
#define Proc_Button_SetResFocused   204    //焦点图片
#define Proc_Button_SetResDisabled  205    //非激活状态

#define Proc_Button_SetColorNormal    206    //
#define Proc_Button_SetColorHot       207    //高亮状态
#define Proc_Button_SetColorPushed    208    //按下状态
#define Proc_Button_SetColorFocused   209    //焦点图片
#define Proc_Button_SetColorDisabled  210    //非激活状态

//------Js

#define Js_Id_Button_normalimage    100
#define Js_Id_Button_hotimage       101
#define Js_Id_Button_pushedimage    102
#define Js_Id_Button_focusedimage   103
#define Js_Id_Button_disabledimage  104

/**按钮控件结构*/
typedef struct _ZButton
{
    ZuiRes m_ResNormal;     //正常状态
    ZuiRes m_ResHot;        //高亮状态
    ZuiRes m_ResPushed;     //按下状态
    ZuiRes m_ResFocused;    //焦点图片
    ZuiRes m_ResDisabled;   //非激活状态

    ZuiColor m_ColorNormal;     //正常状态
    ZuiColor m_ColorHot;        //高亮状态
    ZuiColor m_ColorPushed;     //按下状态
    ZuiColor m_ColorFocused;    //焦点图片
    ZuiColor m_ColorDisabled;   //非激活状态

    int type;
    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiButton, ZButton;
ZEXPORT ZuiAny ZCALL ZuiButtonProc(ZuiInt ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__BUTTON_H__
