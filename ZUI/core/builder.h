/**
* @file		builder.h
* @brief	xml绑定.
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
#ifndef __ZUI_CORE_BUILDER_H__
#define __ZUI_CORE_BUILDER_H__
ZEXPORT ZuiControl ZCALL ZuiLayoutLoadNode(mxml_node_t *tree, ZuiControl win);
ZEXPORT ZuiControl ZCALL ZuiLayoutLoad(ZuiAny xml, ZuiInt len);
//绑定js对象
ZuiVoid ZuiBuilderJs_pushControl(js_State *J, ZuiControl cp);
ZuiControl ZuiBuilderJs_toControl(js_State *J, ZuiInt idx);
ZuiBool ZuiBuilderJs(js_State *J);
ZuiBool ZuiBuilderJsUn(js_State *J);
ZuiBool ZuiBuilderJsPM(js_State *J, ZuiPaintManager p);
ZEXPORT ZuiBool ZCALL ZuiBuilderJsLoad(js_State *J, ZuiText str, ZuiInt len);
ZuiBool ZuiBuilderInit();
ZuiVoid ZuiBuilderUnInit();
#endif	//__ZUI_CORE_BUILDER_H__