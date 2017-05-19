#include "duv_udp.h"

duk_ret_t duv_new_udp(duk_context *ctx) {
  uv_udp_t* handle;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {NULL}
  });

  handle = duk_push_fixed_buffer(ctx, sizeof(*handle));
  duv_check(ctx, uv_udp_init(duv_loop(ctx), handle));
  handle->data = duv_setup_handle(ctx);
  return 1;
}

duk_ret_t duv_udp_open(duk_context *ctx) {
  uv_udp_t* handle;
  uv_os_sock_t sock;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"udp", duv_is_udp},
    {"socket", duk_is_number},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  sock = duk_get_number(ctx, 1);
  duv_check(ctx, uv_udp_open(handle, sock));
  return 0;
}

duk_ret_t duv_udp_bind(duk_context *ctx) {
  uv_udp_t* handle;
  const char* host;
  int port, flags;
  struct sockaddr_storage addr;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"udp", duv_is_udp},
    {"host", duk_is_string},
    {"port", duk_is_number},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  host = duk_get_string(ctx, 1);
  port = duk_get_number(ctx, 2);
  flags = 0;
  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
  }
  duv_check(ctx, uv_udp_bind(handle, (struct sockaddr*)&addr, flags));
  return 0;
}

