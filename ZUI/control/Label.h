#ifndef __LABEL_H__
#define __LABEL_H__
#include <ZUI.h>

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
	ZRect		m_rcPadding;
    ZuiRes      m_rFont;         //字体
    ZuiUInt     m_uTextStyle;    //字体控制
    ZuiColor    m_cTextColor;   //字体颜色
    ZCtlProc old_call;
}*ZuiLabel, ZLabel;
ZEXPORT ZuiAny ZCALL ZuiLabelProc(ZuiInt ProcId, ZuiControl cp, ZuiLabel p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__LABEL_H__
