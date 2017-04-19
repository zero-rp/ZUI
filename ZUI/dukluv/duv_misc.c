#if (defined HAVE_DUV) && (HAVE_DUV == 1)
#include "duv.h"
#include <ZUI.h>
duk_ret_t duv_guess_handle(duk_context *ctx) {
    uv_file file;

    dschema_check(ctx, (const duv_schema_entry[]) {
        {"fd", duv_is_fd},
        { NULL }
    });

    file = duk_get_uint(ctx, 0);
    switch (uv_guess_handle(file)) {
#define XX(uc, lc) case UV_##uc: duk_push_string(ctx, #uc); break;
        UV_HANDLE_TYPE_MAP(XX)
#undef XX
    case UV_FILE: duk_push_string(ctx, "FILE"); break;
    default: return 0;
    }
    return 1;
}


duk_ret_t duv_get_process_title(duk_context *ctx) {
    char title[MAX_TITLE_LENGTH];
    duv_check(ctx, uv_get_process_title(title, MAX_TITLE_LENGTH));
    duk_push_string(ctx, title);
    return 1;
}

duk_ret_t duv_set_process_title(duk_context *ctx) {
    const char* title;

    dschema_check(ctx, (const duv_schema_entry[]) {
        {"title", duk_is_string},
        { NULL }
    });

    title = duk_get_string(ctx, 0);
    duv_check(ctx, uv_set_process_title(title));
    return 0;
}

duk_ret_t duv_resident_set_memory(duk_context *ctx) {
    size_t rss;
    duv_check(ctx, uv_resident_set_memory(&rss));
    duk_push_number(ctx, rss);
    return 1;
}

duk_ret_t duv_uptime(duk_context *ctx) {
    double uptime;
    duv_check(ctx, uv_uptime(&uptime));
    duk_push_number(ctx, uptime);
    return 1;
}

static void duv_push_timeval_table(duk_context *ctx, const uv_timeval_t* t) {
    duk_push_object(ctx);
    duk_push_uint(ctx, t->tv_sec);
    duk_put_prop_string(ctx, -2, "sec");
    duk_push_uint(ctx, t->tv_usec);
    duk_put_prop_string(ctx, -2, "usec");
}

duk_ret_t duv_getrusage(duk_context *ctx) {
    uv_rusage_t rusage;
    duv_check(ctx, uv_getrusage(&rusage));
    duk_push_object(ctx);
    // user CPU time used
    duv_push_timeval_table(ctx, &rusage.ru_utime);
    duk_put_prop_string(ctx, -2, "utime");
    // system CPU time used
    duv_push_timeval_table(ctx, &rusage.ru_stime);
    duk_put_prop_string(ctx, -2, "stime");
    // maximum resident set size
    duk_push_uint(ctx, rusage.ru_maxrss);
    duk_put_prop_string(ctx, -2, "maxrss");
    // integral shared memory size
    duk_push_uint(ctx, rusage.ru_ixrss);
    duk_put_prop_string(ctx, -2, "ixrss");
    // integral unshared data size
    duk_push_uint(ctx, rusage.ru_idrss);
    duk_put_prop_string(ctx, -2, "idrss");
    // integral unshared stack size
    duk_push_uint(ctx, rusage.ru_isrss);
    duk_put_prop_string(ctx, -2, "isrss");
    // page reclaims (soft page faults)
    duk_push_uint(ctx, rusage.ru_minflt);
    duk_put_prop_string(ctx, -2, "minflt");
    // page faults (hard page faults)
    duk_push_uint(ctx, rusage.ru_majflt);
    duk_put_prop_string(ctx, -2, "majflt");
    // swaps
    duk_push_uint(ctx, rusage.ru_nswap);
    duk_put_prop_string(ctx, -2, "nswap");
    // block input operations
    duk_push_uint(ctx, rusage.ru_inblock);
    duk_put_prop_string(ctx, -2, "inblock");
    // block output operations
    duk_push_uint(ctx, rusage.ru_oublock);
    duk_put_prop_string(ctx, -2, "oublock");
    // IPC messages sent
    duk_push_uint(ctx, rusage.ru_msgsnd);
    duk_put_prop_string(ctx, -2, "msgsnd");
    // IPC messages received
    duk_push_uint(ctx, rusage.ru_msgrcv);
    duk_put_prop_string(ctx, -2, "msgrcv");
    // signals received
    duk_push_uint(ctx, rusage.ru_nsignals);
    duk_put_prop_string(ctx, -2, "nsignals");
    // voluntary context switches
    duk_push_uint(ctx, rusage.ru_nvcsw);
    duk_put_prop_string(ctx, -2, "nvcsw");
    // involuntary context switches
    duk_push_uint(ctx, rusage.ru_nivcsw);
    duk_put_prop_string(ctx, -2, "nivcsw");
    return 1;
}

duk_ret_t duv_cpu_info(duk_context *ctx) {
    uv_cpu_info_t* cpu_infos;
    int count, i;
    duv_check(ctx, uv_cpu_info(&cpu_infos, &count));
    duk_push_array(ctx);

    for (i = 0; i < count; i++) {
        duk_push_object(ctx);
        duk_push_string(ctx, cpu_infos[i].model);
        duk_put_prop_string(ctx, -2, "model");
        duk_push_uint(ctx, cpu_infos[i].speed);
        duk_put_prop_string(ctx, -2, "speed");
        duk_push_object(ctx);
        duk_push_uint(ctx, cpu_infos[i].cpu_times.user);
        duk_put_prop_string(ctx, -2, "user");
        duk_push_uint(ctx, cpu_infos[i].cpu_times.nice);
        duk_put_prop_string(ctx, -2, "nice");
        duk_push_uint(ctx, cpu_infos[i].cpu_times.sys);
        duk_put_prop_string(ctx, -2, "sys");
        duk_push_uint(ctx, cpu_infos[i].cpu_times.idle);
        duk_put_prop_string(ctx, -2, "idle");
        duk_push_uint(ctx, cpu_infos[i].cpu_times.irq);
        duk_put_prop_string(ctx, -2, "irq");
        duk_put_prop_string(ctx, -2, "times");
        duk_put_prop_index(ctx, -2, i);
    }

    uv_free_cpu_info(cpu_infos, count);
    return 1;
}

duk_ret_t duv_interface_addresses(duk_context *ctx) {
    uv_interface_address_t* interfaces;
    int count, i;
    char ip[INET6_ADDRSTRLEN];

    uv_interface_addresses(&interfaces, &count);

    duk_push_object(ctx);

    for (i = 0; i < count; i++) {

        duk_get_prop_string(ctx, -1, interfaces[i].name);
        if (!duk_is_object(ctx, -1)) {
            duk_pop(ctx);
            duk_push_array(ctx);
            duk_dup(ctx, -1);
            duk_put_prop_string(ctx, -3, interfaces[i].name);
        }

        duk_push_object(ctx);

        duk_push_boolean(ctx, interfaces[i].is_internal);
        duk_put_prop_string(ctx, -2, "internal");

        if (interfaces[i].address.address4.sin_family == AF_INET) {
            uv_ip4_name(&interfaces[i].address.address4, ip, sizeof(ip));
        }
        else if (interfaces[i].address.address4.sin_family == AF_INET6) {
            uv_ip6_name(&interfaces[i].address.address6, ip, sizeof(ip));
        }
        else {
            strncpy(ip, "<unknown sa family>", INET6_ADDRSTRLEN);
        }
        duk_push_string(ctx, ip);
        duk_put_prop_string(ctx, -2, "ip");

        duk_push_string(ctx, duv_protocol_to_string(interfaces[i].address.address4.sin_family));
        duk_put_prop_string(ctx, -2, "family");

        duk_put_prop_index(ctx, -2, duk_get_length(ctx, -2));

        duk_pop(ctx);
    }

    uv_free_interface_addresses(interfaces, count);
    return 1;
}

void on_resolved(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res) {
    duv_req_t* req = resolver->data;
    if (status == -1) {
        duv_push_status(req->ctx, status);//error
        duk_push_undefined(req->ctx);

    }
    else {
        struct addrinfo *addr = res;
        duk_push_null(req->ctx);
        duk_idx_t obj_idx = duk_push_array(req->ctx);
        int i = 0;
        while (addr)
        {
            duv_push_sockaddr(req->ctx, (struct sockaddr_in*) addr->ai_addr, sizeof(struct sockaddr_in));
            duk_put_prop_index(req->ctx, obj_idx, i);
            addr = addr->ai_next;
            i++;
        }
    }

    duv_fulfill_req(req->ctx, resolver, 2);
    uv_freeaddrinfo(res);
    duv_cleanup_req(req->ctx, req);
    ZuiFree(resolver);
}

duk_ret_t duv_getaddrinfo(duk_context *ctx) {
    char*host;
    char*name;
    dschema_check(ctx, (const duv_schema_entry[]) {
        {"host", duk_is_string},
        { "name", duk_is_string },
        { "callback", dschema_is_continuation },
        { NULL }
    });
    host = duk_get_string(ctx, 0);
    name = duk_get_string(ctx, 1);

    uv_getaddrinfo_t *resolver = (uv_getaddrinfo_t*)ZuiMalloc(sizeof(uv_getaddrinfo_t));
    resolver->data = duv_setup_req(ctx, 2);
    int r = uv_getaddrinfo(duv_loop(ctx), resolver, on_resolved, host, name, NULL);
    if (r) {
        duk_push_boolean(ctx, FALSE);
    }
    else {
        duk_push_boolean(ctx, TRUE);
    }
    return 1;
}


duk_ret_t duv_loadavg(duk_context *ctx) {
    double avg[3];
    int i;
    uv_loadavg(avg);
    duk_push_array(ctx);
    for (i = 0; i < 3; i++) {
        duk_push_number(ctx, avg[i]);
        duk_put_prop_index(ctx, -2, i);
    }
    return 1;
}

duk_ret_t duv_exepath(duk_context *ctx) {
    size_t size = 2 * PATH_MAX;
    char exe_path[2 * PATH_MAX];
    duv_check(ctx, uv_exepath(exe_path, &size));
    duk_push_lstring(ctx, exe_path, size);
    return 1;
}

duk_ret_t duv_os_homedir(duk_context *ctx) {
    size_t size = 2 * PATH_MAX;
    char path[2 * PATH_MAX];
    duv_check(ctx, uv_os_homedir(path, &size));
    duk_push_lstring(ctx, path, size);
    return 1;
}

duk_ret_t duv_chdir(duk_context *ctx) {
    const char* path;

    dschema_check(ctx, (const duv_schema_entry[]) {
        {"path", duk_is_string},
        { NULL }
    });

    path = duk_get_string(ctx, 0);
    duv_check(ctx, uv_chdir(path));
    return 0;
}

duk_ret_t duv_get_total_memory(duk_context *ctx) {
    duk_push_number(ctx, uv_get_total_memory());
    return 1;
}

duk_ret_t duv_hrtime(duk_context *ctx) {
    duk_push_number(ctx, uv_hrtime());
    return 1;
}

duk_ret_t duv_update_time(duk_context *ctx) {
    uv_update_time(duv_loop(ctx));
    return 0;
}

duk_ret_t duv_now(duk_context *ctx) {
    uint64_t now = uv_now(duv_loop(ctx));
    duk_push_uint(ctx, now);
    return 1;
}

duk_ret_t duv_process_exec(duk_context *ctx) {
    dschema_check(ctx, (const duv_schema_entry[]) {
        {"opt", duk_is_object},
        { NULL }
    });

    uv_process_options_t opt;
    uv_process_t *p = duk_push_fixed_buffer(ctx, sizeof(uv_process_t));



    opt.file =
        uv_spawn(duv_loop(ctx), p, &opt);
}

duk_ret_t duv_process_kill(duk_context *ctx) {

    //uv_process_kill()
}
#endif
