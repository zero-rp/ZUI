/**
* @file		global.h
* @brief	全局对象.
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
#ifndef __ZUI_CORE_GLOBAL_H__
#define __ZUI_CORE_GLOBAL_H__

extern  ZuiImage Global_Res_Ctl[4][3];		//窗口控制按钮资源
extern  ZuiImage Global_Res_ScrollBar[4];	//滚动条
extern  ZuiStringFormat Global_StringFormat;//默认字体风格
extern  rb_root *Global_ControlClass;		//全局控件类
extern	rb_root *Global_ControlArray;		//全局Control数组
extern	rb_root *Global_ExportList;		//全局导出表
extern HINSTANCE m_hInstance;		//模块句柄
extern DArray *m_aPreMessages;	//所有ZuiPaintManager实例数组
/**
* 此函数用作初始化全局对象.
* @return 成功返回TRUE.
*/
ZuiBool ZuiGlobalInit();


#endif	//__ZUI_CORE_GLOBAL_H__