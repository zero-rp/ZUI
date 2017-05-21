#ifndef __ZUI_CORE_BUILDER_H__
#define __ZUI_CORE_BUILDER_H__
#include <ZUI.h>
#include "mxml.h"
#if (defined HAVE_JS) && (HAVE_JS == 1)
#include <duktape.h>
#include <platform/platform.h>
extern duk_context *Global_ctx;
typedef struct _ZuiContext
{
    ZuiOsWindow m_pOs;

}*ZuiContext, ZContext;
#endif


#if (defined HAVE_JS) && (HAVE_JS == 1)
int duv_ref(duk_context *ctx);
void duv_push_ref(duk_context *ctx, int ref);
void duv_unref(duk_context *ctx, int ref);

const wchar_t *duk_get_string_w(duk_context *ctx, duk_idx_t idx);
const wchar_t *duk_to_string_w(duk_context *ctx, duk_idx_t idx);
void duk_push_string_w(duk_context *ctx, wchar_t *str);
//绑定js对象
ZuiVoid ZuiBuilderJs_pushControl(duk_context *ctx, ZuiControl cp);
ZuiControl ZuiBuilderJs_toControl(duk_context *ctx, ZuiInt idx);
ZuiVoid ZuiBuilderControlInit(duk_context *ctx, char *name, int id, int ttr);
//创建js环境
duk_context *ZuiBuilderJs(ZuiOsWindow p);
ZuiBool ZuiBuilderJsUn(duk_context *ctx);
ZuiContext ZuiBuilderContext(duk_context *ctx);
ZEXPORT ZuiBool ZCALL ZuiBuilderJsLoad(duk_context *ctx, ZuiText str);
#endif

ZuiBool ZuiBuilderInit();
ZuiVoid ZuiBuilderUnInit();
#endif	//__ZUI_CORE_BUILDER_H__