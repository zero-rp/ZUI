#ifndef __CONFIG_H__
#define __CONFIG_H__

//config

//运行平台配置
#ifdef _WIN32
    #define PLATFORM_OS_WIN
#elif defined _WIN64
    #define PLATFORM_OS_WIN
#elif defined __linux__
    #define PLATFORM_OS_LINUX
#elif defined MACX
    #define PLATFORM_OS_MACX
#elif defined __ANDROID__
    #define PLATFORM_OS_ANDROID
#endif

#define HAVE_JS 0
#define HAVE_UV 0
#define HAVE_DUV 0

#if (defined HAVE_JS) && (HAVE_JS == 1)
#undef HAVE_UV
#define HAVE_UV 1
#endif

#if (defined HAVE_DUV) && (HAVE_DUV == 1)
#undef HAVE_JS
#define HAVE_JS 1
#undef HAVE_UV
#define HAVE_UV 1
#endif

#define JS_GCTIMER  10      //js内存回收间隔 s

#endif
