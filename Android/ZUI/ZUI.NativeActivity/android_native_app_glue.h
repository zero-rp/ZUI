/*
 * 版权所有 (C) 2010 Android 开放源代码项目
 *
 * 按照 Apache 许可 2.0 版本(称为“许可”)授予许可；
 * 要使用此文件，必须遵循“许可”中的说明。
 * 你可以从以下位置获取“许可”的副本
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * 除非适用法律要求或书面同意，根据
 * “许可”分配的软件“按原样”分配，
 * 不提供任何形式(无论是明示还是默示)的担保和条件。
 * 参见“许可”了解“许可”中管理权限和
 * 限制的指定语言。
 *
 */

#ifndef _ANDROID_NATIVE_APP_GLUE_H
#define _ANDROID_NATIVE_APP_GLUE_H

#include <poll.h>
#include <pthread.h>
#include <sched.h>

#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_activity.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 由 <android/native_activity.h> 提供的本地活动界面
 * 基于一组应用程序提供的回调，
 * 发生某些事件时，活动的主线程将调用这些回调。
 *
 * 这意味着，这些回调中的每个回调不应堵塞，否则会有系统
 * 强制关闭应用程序的风险。此编程
 * 模型直接、轻型，但是有约束。
 *
 *“threaded_native_app”静态库用于提供不同的
 * 执行模型，其中应用程序可在其他
 * 线程中实施它自己的主要事件循环。其工作方式如下:
 *
 * 1/ 应用程序必须提供名为“android_main()”的函数，
 *    当在不同于活动的主线程的
 *    新线程中创建活动时，将调用该函数。
 *
 * 2/ android_main() 接收到指向有效的“android_app”结构的指针，
 *    该结构包含对其他重要对象(例如，
 *    应用程序正在其中运行的 ANativeActivity 对象实例)的引用。
 *
 * 3/“android_app”对象保留已经
 *    在侦听以下两项重大事件的 ALooper 实例:
 *
 *      - 活动生命周期事件(例如“暂停”、“恢复”)。参见下文的 APP_CMD_XXX
 *        声明。
 *
 *      - 来自附加到活动的 AInputQueue 的输入事件。
 *
 *    每个事件都对应于一个由值分别为 LOOPER_ID_MAIN 和 LOOPER_ID_INPUT 的
 *    ALooper_pollOnce 返回的 ALooper
 *    标识符。
 *
 *    你的应用程序可使用相同的 ALooper 侦听其他
 *    文件描述符。它们可基于回调或带有以
 *    LOOPER_ID_USER 开头的返回标识符。
 *
 * 4/ 无论在何时收到 LOOPER_ID_MAIN 或 LOOPER_ID_INPUT 事件，
 *    返回的数据都将指向 android_poll_source 结构。你
 *    可对它调用 process() 函数，并填入要调用的 android_app->onAppCmd
 *    和 android_app->onInputEvent，以处理你自己的
 *    事件。
 *
 *    另外，你还可调用低级别函数直接
 *    读取和处理数据...  查看附加的 process_cmd() 和 process_input()
 *    实现，以了解如何进行此操作。
 *
 * 查看包含完整用例的 NDK 中提供的名为 
 *“native-activity”的示例。另请查看 NativeActivity 的 JavaDoc。
 */

struct android_app;

/**
 * 当该源的数据准备就绪后，
 * 与 ALooper fd 相关的数据将作为“outData”返回。
 */
struct android_poll_source {
    //此源的标识符。  可能是 LOOPER_ID_MAIN 或
    // LOOPER_ID_INPUT。
    int32_t id;

    //此 ident 关联的 android_app。
    struct android_app* app;

    //要从此源调用以执行标准数据处理的
    //函数。
    void (*process)(struct android_app* app, struct android_poll_source* source);
};

/**
 * 这是针对线程化应用程序的标准粘附代码的
 * 界面。 在此模型中，应用程序的代码在
 * 其自己的线程中运行，该线程独立于进程的主线程。
 * 此线程不必与 Java
 * VM 关联，尽管它们需要相互关联以使 JNI 调用任何
 *  Java 对象。
 */
struct android_app {
    //如果愿意，应用程序可将指向其自己的状态对象的指针
    //放在这里。
    void* userData;

    //在此处填充用于处理主应用命令(APP_CMD_*)的函数
    void (*onAppCmd)(struct android_app* app, int32_t cmd);

    //在此处填充用于处理输入事件的函数。此时，
    //事件已经提前分派，一经返回将立即
    //完成。如果你已处理该事件，则返回 1，对于任何默认
    //分派返回 0。
    int32_t (*onInputEvent)(struct android_app* app, AInputEvent* event);

    //此应用在其中运行的 ANativeActivity 对象实例。
    ANativeActivity* activity;

    //正在运行的应用采用的当前配置。
    AConfiguration* config;

    //这是创建时提供的上一个实例的已保存状态。
    //如果没有状态，则为 NULL。你可根据需要使用；
    //内存将保持可用，直到你为 APP_CMD_RESUME 调用 android_app_exec_cmd()，
    //此时，将释放内存，并且 savedState 设置为 NULL。
    //仅当处理 APP_CMD_SAVE_STATE 时才可更改这些变量，
    //此时，这些变量将初始化为 NULL，并且你能够为你的
    //状态分配内存并将信息放在此处。这种情况下，将稍后
    //为你释放内存。
    void* savedState;
    size_t savedStateSize;

    //与应用的线程关联的 ALooper。
    ALooper* looper;

    //不为 NULL 时，这是一个输入队列，应用将通过它
    //接收用户输入序列。
    AInputQueue* inputQueue;

    //不为 NIULL 时，这是应用可在其中进行绘制的窗口界面。
    ANativeWindow* window;

    //窗口的当前内容矩形；这是
    //应放置窗口的内容以供用户查看的区域。
    ARect contentRect;

    //应用的活动的当前状态。可能是 APP_CMD_START、
    //APP_CMD_RESUME、APP_CMD_PAUSE 或 APP_CMD_STOP；请参见下文。
    int activityState;

    //应用程序的 NativeActivity 被破坏并且正在
    //等待应用线程完成时，这为非零值。
    int destroyRequested;

    // -------------------------------------------------
    //以下是粘附代码的“私有”实现。

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int msgread;
    int msgwrite;

    pthread_t thread;

    struct android_poll_source cmdPollSource;
    struct android_poll_source inputPollSource;

    int running;
    int stateSaved;
    int destroyed;
    int redrawNeeded;
    AInputQueue* pendingInputQueue;
    ANativeWindow* pendingWindow;
    ARect pendingContentRect;
};

enum {
    /**
     *命令的 Looper 数据 ID，其中的命令来自应用的主线程，该数据 ID 作为一个标识符从
     *ALooper_pollOnce() 返回。  此
     * 标识符的数据是指向 android_poll_source structure 结构的指针。
     * 可使用 android_app_read_cmd()
     * 和 android_app_exec_cmd() 检索和处理这些数据。
     */
    LOOPER_ID_MAIN = 1,

    /**
     * 事件的 Looper 数据 ID，其中的事件来自应用程序窗口的 AInputQueue，
     * 该数据 ID 作为一个标识符从
     * ALooper_pollOnce() 返回。此标识符的数据是指向
     * android_poll_source 结构的指针。这些数据可通过
     *android_app 的 inputQueue 对象读取。
     */
    LOOPER_ID_INPUT = 2,

    /**
     * 用户定义的 ALooper 标识符的开头。
     */
    LOOPER_ID_USER = 3,
};

enum {
    /**
     * 来自主线程的命令: AInputQueue 已更改。  处理
     * 此命令时，android_app->inputQueue 将更新到新的队列
     * (或 NULL)。
     */
    APP_CMD_INPUT_CHANGED,

    /**
     * 来自主线程的命令: 新的 ANativeWindow 已准备就绪可供使用。  接收到
     * 此命令后，android_app->window 将包含新的窗口
     * 界面。
     */
    APP_CMD_INIT_WINDOW,

    /**
     * 来自主线程的命令: 需要终止
     * 目前的 ANativeWindow。  收到此命令后，android_app->window 仍
     * 包含现有窗口；调用 android_app_exec_cmd 后，
     * 它将被设置为 NULL。
     */
    APP_CMD_TERM_WINDOW,

    /**
     * 来自主线程的命令: 已调整当前 ANativeWindow 的大小。
     * 请根据它的新大小重新绘制。
     */
    APP_CMD_WINDOW_RESIZED,

    /**
     * 来自主线程的命令: 系统需要重新绘制
     * 当前 ANativeWindow。  你应该在进行相关处理之前将窗口重新绘制到
     * android_app_exec_cmd()，以避免短暂的绘制故障。
     */
    APP_CMD_WINDOW_REDRAW_NEEDED,

    /**
     *来自主线程的命令: 窗口的内容区域已更改，
     * 例如，从显示或隐藏的软输入窗口更改为其他区域。  你可在
     *android_app::contentRect 中找到新的内容矩形。
     */
    APP_CMD_CONTENT_RECT_CHANGED,

    /**
     * 来自主线程的命令: 应用的活动窗口已获得
     * 输入焦点。
     */
    APP_CMD_GAINED_FOCUS,

    /**
     * 来自主线程的命令: 应用的活动窗口已丢失
     *输入焦点。
     */
    APP_CMD_LOST_FOCUS,

    /**
     * 来自主线程的命令: 当前设备配置已更改。
     */
    APP_CMD_CONFIG_CHANGED,

    /**
     * 来自主线程的命令: 系统在运行时内存不足。
     * 尝试减少你的内存使用。
     */
    APP_CMD_LOW_MEMORY,

    /**
     * 来自主线程的命令: 应用的活动已启动。
     */
    APP_CMD_START,

    /**
     * 来自主线程的命令: 应用的活动已恢复。
     */
    APP_CMD_RESUME,

    /**
     * 来自主线程的命令: 应用应为它自己生成新的已保存状态，
     * 以便今后在需要时从其进行还原。  如果你有已保存状态，
     * 通过分配内存对其进行分配并将其以
     * android_app.savedStateSize 中的大小放在 android_app.savedState 中。  这样会稍后为你
     * 释放内存。
     */
    APP_CMD_SAVE_STATE,

    /**
     * 来自主线程的命令: 应用的活动已暂停。
     */
    APP_CMD_PAUSE,

    /**
     * 来自主线程的命令: 应用的活动已停止。
     */
    APP_CMD_STOP,

    /**
     * 来自主线程的命令: 应用的活动已被破坏，
     * 正在等待应用程清理并退出，然后继续。
     */
    APP_CMD_DESTROY,
};

/**
 * ALooper_pollAll() 返回 LOOPER_ID_MAIN 时调用，读取下一
 * 应用命令消息。
 */
int8_t android_app_read_cmd(struct android_app* android_app);

/**
 * 用 android_app_read_cmd() 返回的命令调用，以对
 * 给定的命令进行初始预处理。调用此函数后，你可对
 * 命令执行你自己的操作。
 */
void android_app_pre_exec_cmd(struct android_app* android_app, int8_t cmd);

/**
 * 用 android_app_read_cmd() 返回的命令调用，以对
 * 给定的命令进行最终后处理。调用此函数前，必须先对
 * 命令执行你自己的操作。
 */
void android_app_post_exec_cmd(struct android_app* android_app, int8_t cmd);

/**
 * 这是应用程序代码必须实现的函数，表示
 * 应用的主索引项。
 */
extern void android_main(struct android_app* app);

#ifdef __cplusplus
}
#endif

#endif /* _ANDROID_NATIVE_APP_GLUE_H */
