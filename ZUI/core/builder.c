#include <ZUI.h>

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
                ZuiRes res = ZuiResDBGetRes(src, ZREST_TXT);
                if (res)
                {
                    ZuiBuilderJsLoad(win->m_pManager->m_ctx, res->p, res->plen);
                    ZuiResDBDelRes(res);
                }
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

//----------------------------------------------------------------------------JS
wchar_t duk_temp[2048];
const wchar_t *duk_to_string_w(duk_context *ctx, duk_idx_t idx) {
    char *str = duk_to_string(ctx, idx);
    duk_temp[MultiByteToWideChar(CP_UTF8, 0, str, -1, duk_temp, 2048 * 2)] = 0;
    return duk_temp;
}

static duk_ret_t ZuiJsBind_Call_exit(duk_context *ctx) {
    ZuiMsgLoop_exit();
    return 0;
}
static duk_ret_t ZuiJsBind_Call_print(duk_context *ctx) {
    for (size_t i = 0; i < duk_gettop(ctx); i++)
    {
        LOG_DEGUB(L"%ls", duk_to_string_w(ctx, i));
    }
    LOG_DEGUB(L"\r\n");
    return 0;
}


//--------------------------------------------------Control_Begin

//----------------辅助函数
//查找控件
static duk_ret_t ZuiJsBind_Call_GetByName(duk_context *ctx) {
   // if (js_isstring(J, 1)) {
   //     ZuiPaintManager p = js_getcontext(J);
   //     if (p && p->m_pRoot)
   //     {
   //         ZuiControl cp = ZuiControlFindName(p->m_pRoot, js_tostring(J, 1));
			//if (!cp)
   //             LOG_ERROR("GetByName失败: Name:%ls\r\n", js_tostring(J, 1));
			//if (cp)
   //         {
   //             js_newobject(J);
   //             js_getproperty(J, -1, L"prototype");
   //             js_newuserdatax(J, TAG, cp, Control_has, Control_put, NULL, NULL);
   //             return;
   //         }
   //     }
   // }
   // js_pushundefined(J);
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
//ZuiVoid ZuiBuilderJs_pushControl(js_State *J, ZuiControl cp) {
//    if (!cp)
//        return;
//    js_newobject(J);
//    js_getproperty(J, -1, L"prototype");
//    js_newuserdatax(J, TAG, cp, Control_has, Control_put, NULL, NULL);
//    return;
//}
//ZuiControl ZuiBuilderJs_toControl(js_State *J, ZuiInt idx) {
//    if (!J)
//        return;
//    return js_touserdata(J, idx, TAG);;
//}

static duk_ret_t ZuiBuilderJs_Control_constructor(duk_context *ctx) {
    duk_idx_t nargs;

    if (!duk_is_constructor_call(ctx)) {
        return DUK_RET_TYPE_ERROR;
    }

    nargs = duk_get_top(ctx);
    duk_set_top(ctx, 1);

    duk_push_this(ctx);

    if (duk_is_pointer(ctx, 0)) {
        duk_push_pointer(ctx, duk_to_pointer(ctx, 0));
        duk_put_prop_string(ctx, -2, "_ptr");
    }
    else if (duk_is_string(ctx, 0)) {
        ZuiControl p = NewZuiControl(duk_to_string_w(ctx, 0), NULL, NULL, NULL);
        duk_push_pointer(ctx, p);
        duk_put_prop_string(ctx, -2, "_ptr");
    }

    duk_compact(ctx, 1);
    return 0;
}
void ZuiBuilderJs_Control(duk_context *ctx)
{
    duk_push_c_function(ctx, ZuiBuilderJs_Control_constructor, DUK_VARARGS /*nargs*/);

    /* Push MyObject.prototype object. */
    duk_push_object(ctx);  /* -> stack: [ MyObject proto ] */

                           /* Set MyObject.prototype.printName. */
    //duk_push_c_function(ctx, myobject_print_name, 0 /*nargs*/);
    //duk_put_prop_string(ctx, -2, "printName");

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
        ZuiRes res = ZuiResDBGetRes(duk_to_string_w(ctx, 0), ZREST_STREAM);
        if (res)
        {
            ZuiControl p = ZuiLayoutLoad(res->p, res->plen);
            ZuiResDBDelRes(res);
            if (p) {
                duk_get_global_string(ctx, "Control");
                duk_push_pointer(ctx, p);
                duk_new(ctx, 1);
    //            js_newobject(J);
    //            js_getproperty(J, -1, L"prototype");
    //            js_newuserdatax(J, TAG, p, Control_has, Control_put, NULL, NULL);
    //            return;
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
//---------------------------------------------------------
ZuiBool ZuiBuilderJs(duk_context *ctx) {
    duk_push_c_function(ctx, ZuiJsBind_Call_exit, 1 /*nargs*/);
    duk_put_global_string(ctx, "exit");

    //js_newcfunction(J, ZuiJsBind_Call_print, L"print", 0);
    //js_setglobal(J, L"print");

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


    darray_append(ctx_array, ctx);
    return TRUE;
}
ZuiBool ZuiBuilderJsUn(duk_context *ctx) {
    darray_delete(ctx_array, darray_find(ctx_array, ctx));
}
//ZuiBool ZuiBuilderJsPM(js_State *J, ZuiPaintManager p) {
//
//}


typedef struct _jsbuf
{
    const wchar_t *buf;
    int len;
}jsbuf;
static duk_ret_t eval_raw(duk_context *ctx, jsbuf *udata) {
    int len = WideCharToMultiByte(CP_UTF8, 0, udata->buf, -1, 0, 0, NULL, NULL);
    char*buf = malloc(len+1);
    buf[WideCharToMultiByte(CP_UTF8, 0, udata->buf, -1, buf, len, NULL, NULL)] = 0;
    duk_eval_string(ctx, buf);
    return 1;
}
ZEXPORT ZuiBool ZCALL ZuiBuilderJsLoad(duk_context *ctx, ZuiText str, ZuiInt len) {
    duk_int_t rc;
    jsbuf buf;
    buf.buf = str;
    buf.len = len;
    rc = duk_safe_call(ctx, eval_raw, &buf, NULL /*nargs*/, 1 /*nrets*/);
    if (rc) {
        LOG_ERROR(L"ERROR: %s\n", duk_to_string_w(ctx, -1));
    }
}
VOID ZCALL ZuiGcTimerProc(HWND h, UINT u, UINT_PTR p, DWORD d) {
    for (size_t i = 0; i < ctx_array->count; i++)
    {
        //js_gc(js_array->data[i], LOG_DEBUG);
    }
}
ZuiBool ZuiBuilderInit() {
    ctx_array = darray_create();
    Global_ctx = duk_create_heap_default();
    ZuiBuilderJs(Global_ctx);
    SetTimer(0, 0, 1000 * JS_GCTIMER, ZuiGcTimerProc);
    return TRUE;
}
ZuiVoid ZuiBuilderUnInit() {
    ZuiBuilderJsUn(Global_ctx);
    duk_destroy_heap(Global_ctx);
    darray_destroy(ctx_array);
}