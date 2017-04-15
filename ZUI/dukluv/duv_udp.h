#ifndef DUV_UDP_H
#define DUV_UDP_H

#include "duv.h"

duk_ret_t duv_new_udp(duk_context *ctx);
duk_ret_t duv_udp_open(duk_context *ctx);
duk_ret_t duv_udp_bind(duk_context *ctx);

#endif
