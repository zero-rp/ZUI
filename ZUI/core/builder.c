#include <ZUI.h>
#include "../dukluv/src/refs.h"

duk_context *Global_ctx;
static DArray *ctx_array = NULL;
ZEXPORT ZuiControl ZCALL ZuiLayoutLoadNode(mxml_node_t *tree, ZuiControl win) {
    mxml_node_t *node;
    ZuiText ClassName = NULL;
    ZuiStringFormat StringFormat = NULL;
    ZuiBool Visible = FALSE, Enabled = TRUE;
    ZuiControl Control;
    for (node = mxmlFindElement(tree, tree, NULL, NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlWalkNext(node, NULL, MXML_DESCEND)/*node = mxmlFindElement(node, tree, NULL,NULL,NULL,MXML_DESCEND)*/) {
        {
        LoadNodeBedin:
            ClassName = node->value.name;
            LOG_DEGUB(L"layout创建控件: 类名:%ls\r\n", ClassName);
            if (wcscmp(ClassName, L"Template") == 0) {//模版类
                ZuiAddTemplate(node);
                node = node->next;
                if (node) {
                    ClassName = node->value.name;
                    goto LoadNodeBedin;
                }
                else
                    continue;
            }
            if (wcscmp(ClassName, L"Menu") == 0) {//菜单类
                ZuiAddMenu(node, NULL);
                node = node->next;
                if (node) {
                    ClassName = node->value.name;
                    goto LoadNodeBedin;
                }
                else
                    continue;
            }
            if (wcscmp(ClassName, L"Include") == 0) {//包含文件
                ZuiText src = NULL;
                for (size_t i = 0; i < node->value.num_attrs; i++)
                {
                    if (wcscmp(node->value.attrs[i].name, L"src") == 0) {
                        src = node->value.attrs[i].value;
                    }
                }
                ZuiRes res = ZuiResDBGetRes(src, NULL);
                if (res) {
                    mxml_node_t *new_node = ZuiLayoutLoad(res->p, res->plen);
                    mxmlAdd(node->parent ? node->parent : node, MXML_ADD_BEFORE, node, new_node);
                    ZuiResDBDelRes(res);
                }
            }
            else if (wcscmp(ClassName, L"LoadScript") == 0) {
                ZuiText src = NULL;
                for (size_t i = 0; i < node->value.num_attrs; i++)
                {
                    if (wcscmp(node->value.attrs[i].name, L"src") == 0) {
                        src = node->value.attrs[i].value;
                    }
                }
                //ZuiRes res = ZuiResDBGetRes(src, ZREST_TXT);
                //if (res)
                //{
                //    ZuiBuilderJsLoad(win->m_pManager->m_ctx, res->p, res->plen);
                //    ZuiResDBDelRes(res);
                //}
                ZuiBuilderJsLoad(win->m_pManager->m_ctx, src);
            }
            else if (!node->user_data) {//当前节点还未创建
                Control = NewZuiControl(ClassName, NULL, NULL, NULL);
                if (node->parent->user_data && wcsicmp(ClassName, L"window") != 0) {
                    //上级控件已存在且当前欲创建的子窗口不为窗口对象
                    if (Control) {
                        node->user_data = Control;//保存控件到节点
                                                  /*添加到容器*/
                        ZuiControlCall(Proc_Layout_Add, node->parent->user_data, Control, NULL, NULL);
                    }
                    else {
                        /*当前控件创建失败 子控件肯定创建不了 删除节点*/
                        mxmlDelete(node);
                        /*再次从头处理*/
                        node = tree;
                        continue;//窗口创建失败就没必要继续下去了
                    }
                }
                else if (!node->parent->user_data && wcsicmp(ClassName, L"window") == 0) {
                    //上级控件已存在且当前欲创建的子窗口为窗口对象
                    if (Control) {
                        node->user_data = Control;//保存控件到节点
                        win = Control;
                    }
                    else
                        break;//窗口创建失败就没必要继续下去了
                }
                /*解析属性*/
                for (size_t i = 0; i < node->value.num_attrs; i++)
                {
                    ZuiControlCall(Proc_SetAttribute, Control, node->value.attrs[i].name, node->value.attrs[i].value, NULL);
                }
            }
        }
    }
    return win;
}
ZEXPORT ZuiControl ZCALL ZuiLayoutLoad(ZuiAny xml, ZuiInt len) {
    mxml_node_t *tree;
    tree = mxmlLoadString(NULL, xml, len);
    ZuiControl win = ZuiLayoutLoadNode(tree, NULL);
    /*解析完成后释放xml树*/
    mxmlDelete(tree);
    return win;
}

//----------------------------------------------------------------------------JNI
wchar_t duk_temp[2048];
const wchar_t *duk_get_string_w(duk_context *ctx, duk_idx_t idx) {
    char *str = duk_get_string(ctx, idx);
    duk_temp[MultiByteToWideChar(CP_UTF8, 0, str, -1, duk_temp, 2048 * 2)] = 0;
    return duk_temp;
}
const wchar_t *duk_to_string_w(duk_context *ctx, duk_idx_t idx) {
    char *str = duk_to_string(ctx, idx);
    duk_temp[MultiByteToWideChar(CP_UTF8, 0, str, -1, duk_temp, 2048 * 2)] = 0;
    return duk_temp;
}
void duk_push_string_w(duk_context *ctx, wchar_t *str) {
    int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, NULL, NULL);
    char*buf = ZuiMalloc(len + 1);
    buf[WideCharToMultiByte(CP_UTF8, 0, str, -1, buf, len, NULL, NULL)] = 0;
    duk_push_string(ctx, buf);
    ZuiFree(buf);
}

static duk_ret_t ZuiJsBind_Call_exit(duk_context *ctx) {
    ZuiMsgLoop_exit();
    return 0;
}
duk_ret_t ZuiJsBind_Call_print(duk_context *ctx) {
    for (size_t i = 0; i < duk_get_top(ctx); i++)
    {
        LOG_DEGUB(L"%ls", duk_to_string_w(ctx, i));
    }
    LOG_DEGUB(L"\r\n");
    return 0;
}


//--------------------------------------------------Control_Begin
//js控件成员函数回调
static duk_ret_t ZuiBuilderJs_Control_Call(duk_context *ctx) {
    int id = duk_get_current_magic(ctx);
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "_ptr");
    ZuiControl p = duk_to_pointer(ctx, -1);
    return ZuiControlCall(Proc_JsCall, p, ctx, id, NULL);
}
//js控件成员属性回调
static duk_ret_t ZuiBuilderJs_Control_Getter(duk_context *ctx) {
    int id = duk_get_current_magic(ctx);
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "_ptr");
    ZuiControl p = duk_to_pointer(ctx, -1);
    return ZuiControlCall(Proc_JsGet, p, ctx, id, NULL);
}
static duk_ret_t ZuiBuilderJs_Control_Setter(duk_context *ctx) {
    int id = duk_get_current_magic(ctx);
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "_ptr");
    ZuiControl p = duk_to_pointer(ctx, -1);
    return ZuiControlCall(Proc_JsSet, p, ctx, id, NULL);
}
//----------------辅助函数
//查找控件
static duk_ret_t ZuiJsBind_Call_GetByName(duk_context *ctx) {
    if (duk_is_string(ctx, 0)) {
        ZuiContext c = ZuiBuilderContext(ctx);
        if (c && c->mp && c->mp->m_pRoot)
        {
            ZuiControl cp = ZuiControlFindName(c->mp->m_pRoot, duk_get_string_w(ctx, 0));
            if (!cp)
                LOG_ERROR("GetByName失败: Name:%ls\r\n", duk_get_string_w(ctx, 0));
            if (cp)
            {
                duk_get_global_string(ctx, "Control");
                duk_push_pointer(ctx, cp);
                duk_new(ctx, 1);
                return 1;
            }
        }
    }
    duk_push_null(ctx);
    return 1;
}
static duk_ret_t ZuiJsBind_Call_ClientToScreen(duk_context *ctx) {
    //if (js_isuserdata(J, 1, TAG) && js_isobject(J, 2)) {
    //    ZuiControl p = js_touserdata(J, 1, TAG);
    //    if (p)
    //    {
    //        js_getproperty(J, 2, "x");
    //        int x = js_toint32(J, -1);
    //        js_getproperty(J, 2, "y");
    //        int y = js_toint32(J, -1);
    //        ZPoint pt = { x,y };
    //        ZuiClientToScreen(p, &pt);
    //        js_newobject(J);
    //        js_pushnumber(J, pt.x);
    //        js_setproperty(J, -2, L"x");
    //        js_pushnumber(J, pt.y);
    //        js_setproperty(J, -2, L"y");
    //        return;
    //    }
    //}
    //js_pushundefined(J);
}
//----------------辅助函数结束
ZuiVoid ZuiBuilderJs_pushControl(duk_context *ctx, ZuiControl cp) {
    if (!cp)
        return;
    duk_get_global_string(ctx, "Control");
    duk_push_pointer(ctx, cp);
    duk_new(ctx, 1);
    duk_dup(ctx, -1);
    return;
}
ZuiControl ZuiBuilderJs_toControl(duk_context *ctx, ZuiInt idx) {
    if (!ctx)
        return;
    duk_get_prop_string(ctx, idx, "_ptr");
    ZuiControl p = duk_to_pointer(ctx, -1);
    return p;
}
//Control对象构造函数
static duk_ret_t ZuiBuilderJs_Control_constructor(duk_context *ctx) {
    if (!duk_is_constructor_call(ctx)) {
        return DUK_RET_TYPE_ERROR;
    }

    duk_push_this(ctx);
    ZuiControl p = NULL;
    if (duk_is_pointer(ctx, 0)) {
        p = duk_to_pointer(ctx, 0);
    }
    else if (duk_is_string(ctx, 0)) {
        p = NewZuiControl(duk_get_string_w(ctx, 0), NULL, NULL, NULL);
    }
    //绑定控件指针
    duk_push_pointer(ctx, p);
    duk_put_prop_string(ctx, -2, "_ptr");

    //初始化控件对象成员
    ZuiControlCall(Proc_JsInit, p, ctx, NULL, NULL);

    return 1;
}
//Control对象成员绑定辅助函数
ZuiVoid ZuiBuilderControlInit(duk_context *ctx, char *name, int id, int ttr) {
    if (ttr) {
        duk_push_string(ctx, name);
        duk_push_c_function(ctx, ZuiBuilderJs_Control_Getter, 0 /*nargs*/);
        duk_set_magic(ctx, -1, id);  /* magic=0: trace */
        duk_push_c_function(ctx, ZuiBuilderJs_Control_Setter, 1 /*nargs*/);
        duk_set_magic(ctx, -1, id);  /* magic=0: trace */
        duk_def_prop(ctx,
            -4,
            DUK_DEFPROP_HAVE_GETTER |
            DUK_DEFPROP_HAVE_SETTER |
            DUK_DEFPROP_HAVE_CONFIGURABLE |  /* clear */
            DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE);
    }
    else {
        duk_push_c_function(ctx, ZuiBuilderJs_Control_Call, DUK_VARARGS /*nargs*/);
        duk_set_magic(ctx, -1, id);  /* magic=0: trace */
        duk_put_prop_string(ctx, -2, name);
    }
}
static void ZuiBuilderJs_Control(duk_context *ctx)
{
    duk_push_c_function(ctx, ZuiBuilderJs_Control_constructor, DUK_VARARGS /*nargs*/);

    /* Push MyObject.prototype object. */
    duk_push_object(ctx);  /* -> stack: [ MyObject proto ] */

    /* Set MyObject.prototype = proto */
    duk_put_prop_string(ctx, -2, "prototype");  /* -> stack: [ MyObject ] */

                                                /* Finally, register MyObject to the global object */
    duk_put_global_string(ctx, "Control");  /* -> stack: [ ] */
}

//--------------------------------------------------Control_End
//--------------------------------------------------PaintManager_Begin


//--------------------------------------------------PaintManager_End
//--------------------------------------------------Graphic_Begin



//ZuiBool ZuiBuilderJs_Graphic(js_State *J) {
//    js_newcfunction(J, ZuiJsBind_Call_exit, L"DrawFillRect", 0);
//    js_setglobal(J, L"DrawFillRect");
//
//    js_newcfunction(J, ZuiJsBind_Call_exit, L"DrawRect", 0);
//    js_setglobal(J, L"DrawFillRect");
//
//    js_newcfunction(J, ZuiJsBind_Call_exit, L"DrawLine", 0);
//    js_setglobal(J, L"DrawFillRect");
//
//    js_newcfunction(J, ZuiJsBind_Call_exit, L"DrawString", 0);
//    js_setglobal(J, L"DrawString");
//    return TRUE;
//}
//--------------------------------------------------Graphic_End
static duk_ret_t ZuiJsBind_Call_LayoutLoad(duk_context *ctx) {
    if (duk_is_string(ctx, 0)) {
        ZuiRes res = ZuiResDBGetRes(duk_get_string_w(ctx, 0), ZREST_STREAM);
        if (res)
        {
            ZuiControl p = ZuiLayoutLoad(res->p, res->plen);
            ZuiResDBDelRes(res);
            if (p) {
                duk_get_global_string(ctx, "Control");
                duk_push_pointer(ctx, p);
                duk_new(ctx, 1);
                return 1;
            }
        }
    }
    return 0;
}

static duk_ret_t ZuiJsBind_Call_ZuiPopupMenu(duk_context *ctx) {
    //if (js_isstring(J, 1) && js_isobject(J, 2)) {
    //    js_getproperty(J, 2, "x");
    //    int x = js_toint32(J, -1);
    //    js_getproperty(J, 2, "y");
    //    int y = js_toint32(J, -1);
    //    ZPoint pt = { x,y };
    //    ZuiPopupMenu(js_getcontext(J), js_tostring(J, 1), &pt);
    //}
    //js_pushundefined(J);
}
//---------------------------------------------------------------------------JNI_End
//致命错误
static void duv_fatal(void *udata, const char *msg) {
    (void)udata;  /* ignored in this case, silence warning */
    msg = (msg ? msg : "no message");
    duk_temp[MultiByteToWideChar(CP_UTF8, 0, msg, -1, duk_temp, 2048 * 2)] = 0;
    LOG_ERROR(L"FATAL ERROR: %s\n", duk_temp);
}
ZuiContext ZuiBuilderContext(duk_context *ctx) {
    duk_memory_functions funcs;
    duk_get_memory_functions(ctx, &funcs);
    return funcs.udata;
}
duk_context *ZuiBuilderJs(ZuiPaintManager p) {
    ZuiContext c = (ZuiContext)ZuiMalloc(sizeof(ZContext));
    c->mp = p;
    duk_context *ctx = duk_create_heap(NULL, NULL, NULL, c, duv_fatal);
    duk_push_c_function(ctx, ZuiJsBind_Call_exit, 1 /*nargs*/);
    duk_put_global_string(ctx, "exit");

    duk_push_c_function(ctx, ZuiJsBind_Call_print, DUK_VARARGS /*nargs*/);
    duk_put_global_string(ctx, "print");

    duk_push_c_function(ctx, ZuiJsBind_Call_GetByName, 1 /*nargs*/);
    duk_put_global_string(ctx, "GetByName");

    //js_newcfunction(J, ZuiJsBind_Call_ClientToScreen, L"ClientToScreen", 0);
    //js_setglobal(J, L"ClientToScreen");

    duk_push_c_function(ctx, ZuiJsBind_Call_LayoutLoad, 1 /*nargs*/);
    duk_put_global_string(ctx, "LayoutLoad");

    //js_newcfunction(J, ZuiJsBind_Call_ZuiPopupMenu, L"PopupMenu", 0);
    //js_setglobal(J, L"PopupMenu");

    ZuiBuilderJs_Control(ctx);
    //ZuiBuilderJs_Graphic(J);

    //duv_ref_setup(ctx);
    duv_bind(ctx);
    darray_append(ctx_array, ctx);
    return ctx;
}
ZuiBool ZuiBuilderJsUn(duk_context *ctx) {
    ZuiContext c = ZuiBuilderContext(ctx);
    darray_delete(ctx_array, darray_find(ctx_array, ctx));
    duk_destroy_heap(ctx);
    ZuiFree(c);
}
duk_ret_t duv_load(duk_context *ctx);
ZEXPORT ZuiBool ZCALL ZuiBuilderJsLoad(duk_context *ctx, ZuiText src) {
    duk_int_t rc;
    duk_push_c_function(ctx, duv_load, 1);
    duk_push_string_w(ctx, src);
    if (duk_pcall(ctx, 1)) {
        LOG_DUK(ctx);
        return 1;
    }
}
VOID ZCALL ZuiGcTimerProc(HWND h, UINT u, UINT_PTR p, DWORD d) {
    for (size_t i = 0; i < ctx_array->count; i++)
    {
        duk_gc(ctx_array->data[i], 0);
    }
}
ZuiBool ZuiBuilderInit() {
    ctx_array = darray_create();
    Global_ctx = ZuiBuilderJs(NULL);
    SetTimer(0, 0, 10 * JS_GCTIMER, ZuiGcTimerProc);
    return TRUE;
}
ZuiVoid ZuiBuilderUnInit() {
    ZuiBuilderJsUn(Global_ctx);
    darray_destroy(ctx_array);
}