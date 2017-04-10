/*
 * 版权所有 (C) 2010 Android 开放源码代码项目
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

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

/**
* 我们的保存状态数据。
*/
struct saved_state {
	float angle;
	int32_t x;
	int32_t y;
};

/**
* 我们应用程序的共享状态。
*/
struct engine {
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	struct saved_state state;
};

/**
* 初始化当前显示的 EGL 上下文。
*/
static int engine_init_display(struct engine* engine) {
	//初始化 OpenGL ES 和 EGL

	/*
	* 此处指定了所需配置的属性。
	*下面，我们选择与屏上窗口
	* 兼容的至少每个颜色有 8 个位的 EGLConfig 
	*/
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/*此处，应用程序选择了所需的配置。 在本
	*示例中，我们有非常简化的选择流程，
	*其中我们选取了与我们的标准匹配的第一个 EGLConfig */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID 是
	*保证会被 ANativeWindow_setBuffersGeometry() 接受的 EGLConfig 的属性。
	* 只要我们选取 EGLConfig，就可使用 EGL_NATIVE_VISUAL_ID 安全地重新配置
	* ANativeWindow 缓冲区以进行匹配。*/
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
	context = eglCreateContext(display, config, NULL, NULL);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;
	engine->state.angle = 0;

	//初始化 GL 状态。
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	return 0;
}

/**
* 仅显示中的当前帧。
*/
static void engine_draw_frame(struct engine* engine) {
	if (engine->display == NULL) {
		//无显示。
		return;
	}

	//只使用一种颜色填充屏幕。
	glClearColor(((float)engine->state.x) / engine->width, engine->state.angle,
		((float)engine->state.y) / engine->height, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	eglSwapBuffers(engine->display, engine->surface);
}

/**
* 关闭当前与显示关联的 EGL 上下文。
*/
static void engine_term_display(struct engine* engine) {
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
*处理下一输入事件。
*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		engine->state.x = AMotionEvent_getX(event, 0);
		engine->state.y = AMotionEvent_getY(event, 0);
		return 1;
	}
	return 0;
}

/**
*处理下一主命令。
*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		//系统已经要求我们保存当前状态。就这样做。
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		//正在显示窗口，让其准备就绪。
		if (engine->app->window != NULL) {
			engine_init_display(engine);
			engine_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		//正在隐藏或关闭窗口，请其进行清理。
		engine_term_display(engine);
		break;
	case APP_CMD_GAINED_FOCUS:
		//当我们的应用获得焦点时，我们开始监控加速计。
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
			//我们想要每秒获得 60 个事件(在美国)。
			ASensorEventQueue_setEventRate(engine->sensorEventQueue,
				engine->accelerometerSensor, (1000L / 60) * 1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		//当我们的应用程序失去焦点时，我们会停止监控加速计。
		//这可在不使用时避免使用电池。
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
		}
		//另外，停止动画。
		engine->animating = 0;
		engine_draw_frame(engine);
		break;
	}
}

/**
* 这是使用 android_native_app_glue
* 的本地应用程序的主要入口点。它在其自己的线程中运行，具有自己的
* 事件循环用于接收输入事件并执行其他操作。
*/
void android_main(struct android_app* state) {
	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	//准备监控加速器
	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
		ASENSOR_TYPE_ACCELEROMETER);
	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
		state->looper, LOOPER_ID_USER, NULL, NULL);

	if (state->savedState != NULL) {
		//我们从之前保存的状态开始；从它还原。
		engine.state = *(struct saved_state*)state->savedState;
	}

	engine.animating = 1;

	//循环等待事情以进行处理。

	while (1) {
		//读取所有挂起的事件。
		int ident;
		int events;
		struct android_poll_source* source;

		//如果没有动态效果，我们将一直阻止等待事件。
		//如果有动态效果，我们进行循环，直到读取所有事件，然后继续
		//绘制动画的下一帧。
		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
			(void**)&source)) >= 0) {

			//处理此事件。
			if (source != NULL) {
				source->process(state, source);
			}

			//如果传感器有数据，立即处理。
			if (ident == LOOPER_ID_USER) {
				if (engine.accelerometerSensor != NULL) {
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
						&event, 1) > 0) {
						LOGI("accelerometer: x=%f y=%f z=%f",
							event.acceleration.x, event.acceleration.y,
							event.acceleration.z);
					}
				}
			}

			//检查，我们是否存在。
			if (state->destroyRequested != 0) {
				engine_term_display(&engine);
				return;
			}
		}

		if (engine.animating) {
			//事件完成；绘制下一动画帧。
			engine.state.angle += .01f;
			if (engine.state.angle > 1) {
				engine.state.angle = 0;
			}

			//绘图被降低到屏幕更新速率，
			//因此，没有必要在此处计时。
			engine_draw_frame(&engine);
		}
	}
}
