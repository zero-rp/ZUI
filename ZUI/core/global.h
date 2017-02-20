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

extern  ZuiStringFormat Global_StringFormat;	//默认字体风格
extern  rb_root *Global_ControlClass;			//全局控件类
extern  rb_root *Global_TemplateClass;			//全局模版类
extern  rb_root *Global_MenuClass;				//全局菜单类
extern js_State *Global_Js;						//
extern  ZuiResDBPool Global_ResDB;				//全局资源池
extern HINSTANCE m_hInstance;					//模块句柄
extern DArray *m_aPreMessages;					//所有ZuiPaintManager实例数组
extern ZuiReal	Global_DPI_X;					//DPI
extern ZuiReal	Global_DPI_Y;					//DPI
/**
* 此函数用作初始化全局对象.
* @return 成功返回TRUE.
*/
ZuiBool ZuiGlobalInit();
ZuiVoid ZuiGlobalUnInit();

#endif	//__ZUI_CORE_GLOBAL_H__