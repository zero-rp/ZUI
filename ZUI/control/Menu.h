/**
* @file		Menu.h
* @brief	标签控件实现.
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
#ifndef __MENU_H__
#define __MENU_H__

ZuiVoid ZuiAddMenu(mxml_node_t *node, ZuiControl win);

/**按钮控件结构*/
typedef struct _ZMenu
{
	ZuiControl win;
}*ZuiMenu, ZMenu;


#endif	//__MENU_H__
