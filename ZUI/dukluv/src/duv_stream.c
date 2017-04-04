#include "duv.h"


duk_ret_t duv_shutdown(duk_context *ctx) {
    uv_stream_t* handle;
    uv_shutdown_t* req;

    dschema_check(ctx, (const duv_schema_entry[]) {
        {"stream", duv_is_stream},
        { "next", dschema_is_continuation },
        { NULL }
    });

    handle = duk_get_buffer(ctx, 0, NULL);
    req = duk_push_fixed_buffer(ctx, sizeof(*req));
    duv_check(ctx, uv_shutdown(req, handle, duv_shutdown_cb));
    req->data = duv_setup_req(ctx, 1);
    return 1;
}

duk_ret_t duv_listen(duk_context *ctx) {
    uv_stream_t* handle;
    int backlog;

    dschema_check(ctx, (const duv_schema_entry[]) {
        {"stream", duv_is_stream},
        { "backlog", duk_is_number },
        { "onConnection", duk_is_function },
        { NULL }
    });

    handle = duk_get_buffer(ctx, 0, NULL);
    backlog = duk_get_number(ctx, 1);
    duv_check(ctx, uv_listen(handle, backlog, duv_connection_cb));
    duv_store_handler(ctx, handle->data, DUV_CONNECTION, 2);
    return 0;
}

duk_ret_t duv_accept(duk_context *ctx) {
    uv_stream_t* server;
    uv_stream_t* client;

    dschema_check(ctx, (const duv_schema_entry[]) {
        {"server", duv_is_stream},
        { "client", duv_is_stream },
        { NULL }
    });

    server = duk_get_buffer(ctx, 0, NULL);
    client = duk_get_buffer(ctx, 1, NULL);
    duv_check(ctx, uv_accept(server, client));
    return 0;
}
//拆包函数
static void duv_alloc_unpack_cb(struct task_client * handle, size_t suggested_size, uv_buf_t* buf) {
    unsigned short *len = (unsigned short *)((char *)handle + sizeof(uv_tcp_t));
    char *head = (char *)((char *)handle + sizeof(uv_tcp_t) + sizeof(unsigned short));
    if (*len == 0) {
        buf->len = 2;
        buf->base = len;
        *head = 0;
    }
    else {
        buf->base = (char*)malloc(*len);
        buf->len = *len;
    }
}
static void duv_read_unpack_cb(uv_stream_t* handle, ssize_t nread, uv_buf_t* buf) {
    unsigned short *len = (unsigned short *)((char *)handle + sizeof(uv_tcp_t));
    char *head = (char *)((char *)handle + sizeof(uv_tcp_t) + sizeof(unsigned short));
    if (nread >= 0) {
        if (!*head) {
            if (nread == 2) {
                *head = 1;
            }
        }
        else if (*len == nread) {
            duv_read_cb(handle, nread, buf);
            *len = 0;
            *head = 0;
        }
        else {
            //erro
            duv_read_cb(handle, -1, buf);
        }
        return;
    }
    else {
        if (!*head)
            buf->base = 0;
    }
    duv_read_cb(handle, nread, buf);
}

duk_ret_t duv_read_start(duk_context *ctx) {
    uv_stream_t* handle;
    char *unpack;
    dschema_check(ctx, (const duv_schema_entry[]) {
        {"stream", duv_is_stream},
        { "onread", duk_is_function },
        { "unpack", duk_is_boolean },
        { NULL }
    });

    handle = duk_get_buffer(ctx, 0, NULL);
    unpack = (char *)((char *)handle + sizeof(uv_tcp_t) + sizeof(unsigned short) + sizeof(char));
    if (duk_get_boolean(ctx, 2)) {
        *unpack = 1;
        duv_check(ctx, uv_read_start(handle, duv_alloc_unpack_cb, duv_read_unpack_cb));
    }
    else {
        *unpack = 0;
        duv_check(ctx, uv_read_start(handle, duv_alloc_cb, duv_read_cb));
    }
    duv_store_handler(ctx, handle->data, DUV_READ, 1);
    return 0;
}

duk_ret_t duv_read_stop(duk_context *ctx) {
    uv_stream_t* handle;

    dschema_check(ctx, (const duv_schema_entry[]) {
        {"stream", duv_is_stream},
        { NULL }
    });

    handle = duk_get_buffer(ctx, 0, NULL);
    duv_check(ctx, uv_read_stop(handle));
    return 0;
}
//打包函数
static void duv_get_unpack_data(duk_context *ctx, int index, uv_buf_t* buf) {
    duk_size_t len;
    char *data;
    if (duk_is_string(ctx, index)) {
        data = (char*)duk_get_lstring(ctx, index, &len);
    }
    else {
        data = duk_get_buffer_data(ctx, index, &len);
    }
    buf->base = malloc(len+2);
    *((unsigned short *)buf->base) = len;
    memcpy(buf->base + 2, data, len);
    buf->len = len + 2;
}
static void duv_write_unpack_cb(uv_write_t* req, int status) {
    duk_context *ctx = ((duv_handle_t*)req->handle->data)->ctx;
    duv_push_status(ctx, status);
    duv_fulfill_req(ctx, (uv_req_t*)req, 1);
    duv_req_t *data = req->data;
    duv_unref(ctx, data->req_ref);
    duv_unref(ctx, data->context);
    duv_unref(ctx, data->callback_ref);
    duv_unref(ctx, data->data_ref);
    free(data->data);
    duk_free(ctx, data);
}

duk_ret_t duv_write(duk_context *ctx) {
    uv_stream_t* handle;
    uv_buf_t buf;
    uv_write_t* req;
    char *unpack;
    dschema_check(ctx, (const duv_schema_entry[]) {
        {"stream", duv_is_stream},
        { "data", dschema_is_data },
        { "next", dschema_is_continuation },
        { NULL }
    });

    handle = duk_get_buffer(ctx, 0, NULL);
    unpack = (char *)((char *)handle + sizeof(uv_tcp_t) + sizeof(unsigned short) + sizeof(char));
    if (*unpack) {
        duv_get_unpack_data(ctx, 1, &buf);
        req = duk_push_fixed_buffer(ctx, sizeof(*req));
        duv_check(ctx, uv_write(req, handle, &buf, 1, duv_write_unpack_cb));
        req->data = duv_setup_req(ctx, 2);
        ((duv_req_t *)req->data)->data = buf.base;
    }
    else {
        duv_get_data(ctx, 1, &buf);
        req = duk_push_fixed_buffer(ctx, sizeof(*req));
        duv_check(ctx, uv_write(req, handle, &buf, 1, duv_write_cb));
        req->data = duv_setup_req(ctx, 2);
    }
    return 1;
}

duk_ret_t duv_is_readable(duk_context *ctx) {
    uv_stream_t* handle;

    dschema_check(ctx, (const duv_schema_entry[]) {
        {"stream", duv_is_stream},
        { NULL }
    });

    handle = duk_get_buffer(ctx, 0, NULL);
    duk_push_boolean(ctx, uv_is_readable(handle));
    return 1;
}

duk_ret_t duv_is_writable(duk_context *ctx) {
    dschema_check(ctx, (const duv_schema_entry[]) {
        {"stream", duv_is_stream},
        { NULL }
    });

    uv_stream_t* handle = duk_get_buffer(ctx, 0, NULL);
    duk_push_boolean(ctx, uv_is_writable(handle));
    return 1;
}

duk_ret_t duv_stream_set_blocking(duk_context *ctx) {
    uv_stream_t* handle;
    int blocking;

    dschema_check(ctx, (const duv_schema_entry[]) {
        {"stream", duv_is_stream},
        { "isblocking", duk_is_boolean },
        { NULL }
    });

    handle = duk_get_buffer(ctx, 0, NULL);
    blocking = duk_get_boolean(ctx, 1);
    duv_check(ctx, uv_stream_set_blocking(handle, blocking));
    return 0;
}
