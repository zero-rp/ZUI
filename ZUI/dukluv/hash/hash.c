#if (defined DUV_HAVE_HASH) && (DUV_HAVE_HASH == 1)
#include <duktape.h>
#include "md5.h"
#include "sha1.h"
#include "sha2.h"

#define DECLARE_HASH(name, HASH_CTX, HASH_Init, HASH_Update, HASH_Final, HASH_DIGEST_LENGTH) \
    static duk_ret_t hash_##name##_new(duk_context* ctx) {                                   \
        HASH_CTX* hash_ctx;                                                                  \
        hash_ctx = malloc(sizeof(*hash_ctx));                                                \
        if (hash_ctx == NULL) {                                                              \
            return -42;                                                                      \
        } else {                                                                             \
            HASH_Init(hash_ctx);                                                             \
            duk_push_pointer(ctx, (void*) hash_ctx);                                         \
            return 1;                                                                        \
        }                                                                                    \
    }                                                                                        \
                                                                                             \
    static duk_ret_t hash_##name##_update(duk_context* ctx) {                                \
        HASH_CTX* hash_ctx;                                                                  \
        const char* data;                                                                    \
        size_t len;                                                                          \
        hash_ctx = duk_require_pointer(ctx, 0);                                              \
        data = duk_require_lstring(ctx, 1, &len);                                            \
        HASH_Update(hash_ctx, (const uint8_t*) data, len);                                   \
        duk_push_undefined(ctx);                                                             \
        return 1;                                                                            \
    }                                                                                        \
                                                                                             \
    static duk_ret_t hash_##name##_digest(duk_context* ctx) {                                \
        HASH_CTX* hash_ctx;                                                                  \
        HASH_CTX tmp;                                                                        \
        char* digest;                                                                        \
        hash_ctx = duk_require_pointer(ctx, 0);                                              \
        memcpy(&tmp, hash_ctx, sizeof(*hash_ctx));                                           \
        digest = duk_push_fixed_buffer(ctx, HASH_DIGEST_LENGTH);                             \
        HASH_Final((uint8_t*) digest, &tmp);                                                 \
        return 1;                                                                            \
    }                                                                                        \


DECLARE_HASH(md5, MD5_CTX, MD5Init, MD5Update, MD5Final, MD5_DIGEST_LENGTH)
DECLARE_HASH(sha1, SHA1_CTX, SHA1Init, SHA1Update, SHA1Final, SHA1_DIGEST_LENGTH)
DECLARE_HASH(sha256, SHA256_CTX, SHA256_Init, SHA256_Update, SHA256_Final, SHA256_DIGEST_LENGTH)
DECLARE_HASH(sha512, SHA512_CTX, SHA512_Init, SHA512_Update, SHA512_Final, SHA512_DIGEST_LENGTH)

#undef DECLARE_HASH


static duk_ret_t hash_dealloc(duk_context* ctx) {
    void* hash_ctx;
    hash_ctx = duk_require_pointer(ctx, 0);
    free(hash_ctx);
    duk_push_undefined(ctx);
    return 1;
}


#define EXPORT_HASH(name)                        \
    { #name"_new",    hash_##name##_new,    0 }, \
    { #name"_update", hash_##name##_update, 2 }, \
    { #name"_digest", hash_##name##_digest, 1 }, \

static const duk_function_list_entry module_funcs[] = {
    EXPORT_HASH(md5)
    EXPORT_HASH(sha1)
    EXPORT_HASH(sha256)
    EXPORT_HASH(sha512)
    { "dealloc", hash_dealloc, 1 /*nargs*/ },
    { NULL, NULL, 0 }
};

#undef EXPORT_HASH


DUK_EXTERNAL duk_ret_t hash_mod_init(duk_context* ctx) {
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, module_funcs);
    return 1;
}

#endif
