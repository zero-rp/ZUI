#ifndef __OPTION_H__
#define __OPTION_H__
#include <ZUI.h>

//------JSID
#define Js_Id_Option_selected               100
#define Js_Id_Option_group                  101
#define Js_Id_Option_onselected             102


/**按钮控件结构*/
typedef struct _ZOption
{
    ZuiBool m_bSelected;        //是否选择
    ZuiBool m_bGroup;           //
    //控件继承自按钮控件
    ZuiRes      m_OptRes;
    ZRect       m_rcOpt[4];

    ZuiColor    m_ColorSelected;        //选中的普通状态
    ZuiColor    m_ColorSelectedHot;     //选中的点燃状态
    ZuiColor    m_ColorSelectedPushed;  //选中的按下状态
    ZuiColor    m_ColorSelectedFocused;
    ZuiColor    m_ColorSelectedDisabled;
#if (defined HAVE_JS) && (HAVE_JS == 1)
    ZuiInt      m_rOnselected;          //js回调
#endif
    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiOption, ZOption;
ZEXPORT ZuiAny ZCALL ZuiOptionProc(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif  //__OPTION_H__
