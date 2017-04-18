/**
* @file		template.h
* @brief	控件模版.
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
#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

ZuiBool ZuiTemplateInit();
ZuiVoid ZuiTemplateUnInit();
ZuiVoid ZuiAddTemplate(mxml_node_t *node);
ZuiVoid ZuiLoadTemplate(mxml_node_t *n, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
#endif //__TEMPLATE_H__
