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
#define Type_Label                  0xe538ac26
#define Proc_Label_SetFont          151     //设置字体
#define Proc_Label_SetTextColor     152     //设置文本颜色
#define Proc_Label_SetTextPadding   153     //字体边距

//------JSID
#define Js_Id_Label_font              50
#define Js_Id_Label_align             51
#define Js_Id_Label_valign            52
#define Js_Id_Label_textcolor         53
#define Js_Id_Label_textpadding       54
#define Js_Id_Label_wordbreak         55
#define Js_Id_Label_endellipsis       56

/**按钮控件结构*/
typedef struct _ZLabel
{
    ZuiRes      m_rFont;         //字体
    ZuiUInt     m_uTextStyle;    //字体控制
    ZuiColor    m_cTextColor;   //字体颜色
    ZCtlProc old_call;
}*ZuiLabel, ZLabel;
ZEXPORT ZuiAny ZCALL ZuiLabelProc(ZuiInt ProcId, ZuiControl cp, ZuiLabel p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__LABEL_H__
