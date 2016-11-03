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

/**模版结构*/
typedef struct _ZTemplate
{
	int type;
	ZCtlProc old_call;
}*ZuiTemplate, ZTemplate;

ZEXPORT ZuiAny ZCALL ZuiTemplateProc(ZuiInt ProcId, ZuiControl cp, ZuiTemplate p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZuiVoid ZuiAddTemplate(mxml_node_t *node);
#endif //__TEMPLATE_H__
