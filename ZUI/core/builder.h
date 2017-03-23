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

typedef struct _ZuiContext
{
    ZuiPaintManager mp;

}*ZuiContext, ZContext;

ZEXPORT ZuiControl ZCALL ZuiLayoutLoadNode(mxml_node_t *tree, ZuiControl win);
ZEXPORT ZuiControl ZCALL ZuiLayoutLoad(ZuiAny xml, ZuiInt len);

const wchar_t *duk_to_string_w(duk_context *ctx, duk_idx_t idx);
void duk_push_string_w(duk_context *ctx, wchar_t *str);
//绑定js对象
ZuiVoid ZuiBuilderJs_pushControl(duk_context *ctx, ZuiControl cp);
ZuiControl ZuiBuilderJs_toControl(duk_context *ctx, ZuiInt idx);
ZuiVoid ZuiBuilderControlInit(duk_context *ctx, char *name, int id, int ttr);
//创建js环境
duk_context *ZuiBuilderJs(ZuiPaintManager p);
ZuiBool ZuiBuilderJsUn(duk_context *ctx);
ZuiContext ZuiBuilderContext(duk_context *ctx);
ZEXPORT ZuiBool ZCALL ZuiBuilderJsLoad(duk_context *ctx, ZuiText str);
ZuiBool ZuiBuilderInit();
ZuiVoid ZuiBuilderUnInit();
#endif	//__ZUI_CORE_BUILDER_H__