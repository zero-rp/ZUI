#if (defined HAVE_DUV) && (HAVE_DUV == 1)
#include "duv_module.h"
#include <duktape.h>
#include <ZUI.h>
void duv_add_module(duk_c_function f);

static ZuvFuncs *func;

ZuiBool ZuiInitZuvFunc() {
    ZuvFuncs *p = func = malloc(sizeof(*p));
    p->size = sizeof(*p);
    p->version = 1;


    p->zuv_push_undefined = duk_push_undefined;
    p->zuv_push_null = duk_push_null;
    p->zuv_push_boolean = duk_push_boolean;
    p->zuv_push_true = duk_push_true;
    p->zuv_push_false = duk_push_false;
    p->zuv_push_number = duk_push_number;
    p->zuv_push_nan = duk_push_nan;
    p->zuv_push_int = duk_push_int;
    p->zuv_push_uint = duk_push_uint;
    p->zuv_push_string = duk_push_string;
    p->zuv_push_lstring = duk_push_lstring;
    p->zuv_push_pointer = duk_push_pointer;

    p->zuv_push_this = duk_push_this;
    p->zuv_push_global_object = duk_push_global_object;

    p->zuv_push_object = duk_push_object;
    p->zuv_push_array = duk_push_array;
    p->zuv_push_c_function = duk_push_c_function;

    p->zuv_push_buffer_raw = duk_push_buffer_raw;

    p->zuv_put_function_list = duk_put_function_list;
    p->zuv_put_number_list = duk_put_number_list;

    p->zuv_is_undefined = duk_is_undefined;
    p->zuv_is_null = duk_is_null;
    p->zuv_is_boolean = duk_is_boolean;
    p->zuv_is_number = duk_is_number;
    p->zuv_is_nan = duk_is_nan;
    p->zuv_is_string = duk_is_string;
    p->zuv_is_object = duk_is_object;
    p->zuv_is_buffer = duk_is_buffer;
    p->zuv_is_buffer_data = duk_is_buffer_data;
    p->zuv_is_pointer = duk_is_pointer;
    p->zuv_is_lightfunc = duk_is_lightfunc;
    p->zuv_is_array = duk_is_array;
    p->zuv_is_function = duk_is_function;
    p->zuv_is_c_function = duk_is_c_function;

    p->zuv_get_boolean = duk_get_boolean;
    p->zuv_get_number = duk_get_number;
    p->zuv_get_int = duk_get_int;
    p->zuv_get_uint = duk_get_uint;
    p->zuv_get_string = duk_get_string;
    p->zuv_get_lstring = duk_get_lstring;
    p->zuv_get_buffer = duk_get_buffer;
    p->zuv_get_buffer_data = duk_get_buffer_data;
    p->zuv_get_pointer = duk_get_pointer;
    p->zuv_get_c_function = duk_get_c_function;
    p->zuv_get_length = duk_get_length;
    p->zuv_set_length = duk_set_length;

    p->zuv_call = duk_call;
    p->zuv_call_method = duk_call_method;
    p->zuv_call_prop = duk_call_prop;
    p->zuv_pcall = duk_pcall;
    p->zuv_pcall_method = duk_pcall_method;
    p->zuv_pcall_prop = duk_pcall_prop;
    p->zuv_new = duk_new;
    p->zuv_pnew = duk_pnew;
    p->zuv_safe_call = duk_safe_call;

    p->zuv_put_prop_string = duk_put_prop_string;
    p->zuv_put_prop_index = duk_put_prop_index;
    p->zuv_get_top = duk_get_top;
    p->zuv_add_module = duv_add_module;
    return TRUE;
}
ZuvFuncs *ZuiGetZuvFunc() {
    return func;
}
#endif
