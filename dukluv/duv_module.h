#ifndef DUV_MODULE_H
#define DUV_MODULE_H
#include <stdint.h>

typedef void * zuv_context;
typedef int zuv_bool_t;
typedef double zuv_double_t;
typedef int zuv_ret_t;
typedef int zuv_int_t;
typedef unsigned int zuv_uint_t;
typedef int zuv_size_t;
typedef int zuv_bool_t;
typedef zuv_int_t zuv_idx_t;
typedef zuv_uint_t zuv_uarridx_t;
typedef signed int zuv_int32_t;
typedef unsigned short zuv_uint16_t;
typedef unsigned int zuv_uint32_t;
typedef zuv_ret_t(*zuv_c_function)(zuv_context ctx);
typedef zuv_ret_t(*zuv_safe_call_function) (zuv_context *ctx, void *udata);

struct zuv_function_list_entry {
    const char *key;
    zuv_c_function value;
    zuv_idx_t nargs;
};

struct zuv_number_list_entry {
    const char *key;
    zuv_double_t value;
};

//压栈操作
typedef void (*pzuv_push_undefined)(zuv_context ctx);
typedef void(*pzuv_push_null)(zuv_context ctx);
typedef void(*pzuv_push_boolean)(zuv_context ctx, zuv_bool_t val);
typedef void(*pzuv_push_true)(zuv_context ctx);
typedef void(*pzuv_push_false)(zuv_context ctx);
typedef void(*pzuv_push_number)(zuv_context ctx, zuv_double_t val);
typedef void(*pzuv_push_nan)(zuv_context ctx);
typedef void(*pzuv_push_int)(zuv_context ctx, zuv_int_t val);
typedef void(*pzuv_push_uint)(zuv_context ctx, zuv_uint_t val);
typedef const char *(*pzuv_push_string)(zuv_context ctx, const char *str);
typedef const char *(*pzuv_push_lstring)(zuv_context ctx, const char *str, zuv_size_t len);
typedef void(*pzuv_push_pointer)(zuv_context ctx, void *p);

typedef void(*pzuv_push_this)(zuv_context ctx);
typedef void(*pzuv_push_global_object)(zuv_context ctx);

typedef zuv_idx_t(*pzuv_push_object)(zuv_context ctx);
typedef zuv_idx_t(*pzuv_push_array)(zuv_context ctx);
typedef zuv_idx_t(*pzuv_push_c_function)(zuv_context ctx, zuv_c_function func, zuv_idx_t nargs);

typedef void *(*pzuv_push_buffer_raw)(zuv_context ctx, zuv_size_t size, zuv_uint_t flags);

typedef void(*pzuv_put_function_list)(zuv_context ctx, zuv_idx_t obj_idx, const struct zuv_function_list_entry *funcs);
typedef void(*pzuv_put_number_list)(zuv_context ctx, zuv_idx_t obj_idx, const struct zuv_number_list_entry *numbers);

//---类型判断操作
typedef zuv_bool_t(*pzuv_is_undefined)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_null)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_boolean)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_number)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_nan)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_string)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_object)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_buffer)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_buffer_data)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_pointer)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_lightfunc)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_array)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_function)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_bool_t(*pzuv_is_c_function)(zuv_context ctx, zuv_idx_t idx);

zuv_bool_t zuv_is_ecmascript_function(zuv_context ctx, zuv_idx_t idx);
zuv_bool_t zuv_is_bound_function(zuv_context ctx, zuv_idx_t idx);
zuv_bool_t zuv_is_thread(zuv_context ctx, zuv_idx_t idx);

//---参数获取操作
typedef zuv_bool_t(*pzuv_get_boolean)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_double_t(*pzuv_get_number)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_int_t(*pzuv_get_int)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_uint_t(*pzuv_get_uint)(zuv_context ctx, zuv_idx_t idx);
typedef const char *(*pzuv_get_string)(zuv_context ctx, zuv_idx_t idx);
typedef const char *(*pzuv_get_lstring)(zuv_context ctx, zuv_idx_t idx, zuv_size_t *out_len);
typedef void *(*pzuv_get_buffer)(zuv_context ctx, zuv_idx_t idx, zuv_size_t *out_size);
typedef void *(*pzuv_get_buffer_data)(zuv_context ctx, zuv_idx_t idx, zuv_size_t *out_size);
typedef void *(*pzuv_get_pointer)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_c_function(*pzuv_get_c_function)(zuv_context ctx, zuv_idx_t idx);
typedef zuv_size_t(*pzuv_get_length)(zuv_context ctx, zuv_idx_t idx);
typedef void(*pzuv_set_length)(zuv_context ctx, zuv_idx_t idx, zuv_size_t len);

//转换参数操作
void zuv_to_undefined(zuv_context ctx, zuv_idx_t idx);
void zuv_to_null(zuv_context ctx, zuv_idx_t idx);
zuv_bool_t zuv_to_boolean(zuv_context ctx, zuv_idx_t idx);
zuv_double_t zuv_to_number(zuv_context ctx, zuv_idx_t idx);
zuv_int_t zuv_to_int(zuv_context ctx, zuv_idx_t idx);
zuv_uint_t zuv_to_uint(zuv_context ctx, zuv_idx_t idx);
zuv_int32_t zuv_to_int32(zuv_context ctx, zuv_idx_t idx);
zuv_uint32_t zuv_to_uint32(zuv_context ctx, zuv_idx_t idx);
zuv_uint16_t zuv_to_uint16(zuv_context ctx, zuv_idx_t idx);
const char *zuv_to_string(zuv_context ctx, zuv_idx_t idx);
const char *zuv_to_lstring(zuv_context ctx, zuv_idx_t idx, zuv_size_t *out_len);
void *zuv_to_buffer_raw(zuv_context ctx, zuv_idx_t idx, zuv_size_t *out_size, zuv_uint_t flags);
void *zuv_to_pointer(zuv_context ctx, zuv_idx_t idx);
void zuv_to_object(zuv_context ctx, zuv_idx_t idx);
void zuv_to_primitive(zuv_context ctx, zuv_idx_t idx, zuv_int_t hint);

const char *zuv_safe_to_lstring(zuv_context ctx, zuv_idx_t idx, zuv_size_t *out_len);

//js函数调用
typedef void(*pzuv_call)(zuv_context ctx, zuv_idx_t nargs);
typedef void(*pzuv_call_method)(zuv_context ctx, zuv_idx_t nargs);
typedef void(*pzuv_call_prop)(zuv_context ctx, zuv_idx_t obj_idx, zuv_idx_t nargs);
typedef zuv_int_t(*pzuv_pcall)(zuv_context ctx, zuv_idx_t nargs);
typedef zuv_int_t(*pzuv_pcall_method)(zuv_context ctx, zuv_idx_t nargs);
typedef zuv_int_t(*pzuv_pcall_prop)(zuv_context ctx, zuv_idx_t obj_idx, zuv_idx_t nargs);
typedef void(*pzuv_new)(zuv_context ctx, zuv_idx_t nargs);
typedef zuv_int_t(*pzuv_pnew)(zuv_context ctx, zuv_idx_t nargs);
typedef zuv_int_t(*pzuv_safe_call)(zuv_context ctx, zuv_safe_call_function func, void *udata, zuv_idx_t nargs, zuv_idx_t nrets);


//杂项
typedef zuv_bool_t(*pzuv_put_prop_string)(zuv_context ctx, zuv_idx_t obj_idx, const char *key);
typedef zuv_bool_t(*pzuv_put_prop_index)(zuv_context ctx, zuv_idx_t obj_idx, zuv_uarridx_t arr_idx);
typedef zuv_idx_t(*pzuv_get_top)(zuv_context ctx);
typedef void(*pzuv_add_module)(zuv_c_function f);
//
typedef struct _ZuvFuncs {
    uint16_t size;      //结构大小
    uint16_t version;   //结构版本
//---压栈操作
    pzuv_push_undefined zuv_push_undefined;
    pzuv_push_null zuv_push_null;
    pzuv_push_boolean zuv_push_boolean;
    pzuv_push_true zuv_push_true;
    pzuv_push_false zuv_push_false;
    pzuv_push_number zuv_push_number;
    pzuv_push_nan zuv_push_nan;
    pzuv_push_int zuv_push_int;
    pzuv_push_uint zuv_push_uint;
    pzuv_push_string zuv_push_string;
    pzuv_push_lstring zuv_push_lstring;
    pzuv_push_pointer zuv_push_pointer;

    pzuv_push_this zuv_push_this;
    pzuv_push_global_object zuv_push_global_object;

    pzuv_push_object zuv_push_object;
    pzuv_push_array zuv_push_array;
    pzuv_push_c_function zuv_push_c_function;

    pzuv_push_buffer_raw zuv_push_buffer_raw;

    pzuv_put_function_list zuv_put_function_list;
    pzuv_put_number_list zuv_put_number_list;

    //---类型判断操作
    pzuv_is_undefined zuv_is_undefined;
    pzuv_is_null zuv_is_null;
    pzuv_is_boolean zuv_is_boolean;
    pzuv_is_number zuv_is_number;
    pzuv_is_nan zuv_is_nan;
    pzuv_is_string zuv_is_string;
    pzuv_is_object zuv_is_object;
    pzuv_is_buffer zuv_is_buffer;
    pzuv_is_buffer_data zuv_is_buffer_data;
    pzuv_is_pointer zuv_is_pointer;
    pzuv_is_lightfunc zuv_is_lightfunc;
    pzuv_is_array zuv_is_array;
    pzuv_is_function zuv_is_function;
    pzuv_is_c_function zuv_is_c_function;


    //---参数获取操作
    pzuv_get_boolean zuv_get_boolean;
    pzuv_get_number zuv_get_number;
    pzuv_get_int zuv_get_int;
    pzuv_get_uint zuv_get_uint;
    pzuv_get_string zuv_get_string;
    pzuv_get_lstring zuv_get_lstring;
    pzuv_get_buffer zuv_get_buffer;
    pzuv_get_buffer_data zuv_get_buffer_data;
    pzuv_get_pointer zuv_get_pointer;
    pzuv_get_c_function zuv_get_c_function;
    pzuv_get_length zuv_get_length;
    pzuv_set_length zuv_set_length;

    //---js函数调用
    pzuv_call zuv_call;
    pzuv_call_method zuv_call_method;
    pzuv_call_prop zuv_call_prop;
    pzuv_pcall zuv_pcall;
    pzuv_pcall_method zuv_pcall_method;
    pzuv_pcall_prop zuv_pcall_prop;
    pzuv_new zuv_new;
    pzuv_pnew zuv_pnew;
    pzuv_safe_call zuv_safe_call;

    //---杂项
    pzuv_put_prop_string zuv_put_prop_string;
    pzuv_put_prop_index zuv_put_prop_index;
    pzuv_get_top zuv_get_top;
    pzuv_add_module zuv_add_module;
}ZuvFuncs;

#endif
