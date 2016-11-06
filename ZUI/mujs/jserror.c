#include "jsi.h"
#include "jsvalue.h"
#include "jsbuiltin.h"
#include <tchar.h>
#define QQ(X) #X
#define Q(X) _T(QQ(X))

static int jsB_stacktrace(js_State *J, int skip)
{
	wchar_t buf[256];
	int n = J->tracetop - skip;
	if (n <= 0)
		return 0;
	for (; n > 0; --n) {
		const wchar_t *name = J->trace[n].name;
		const wchar_t *file = J->trace[n].file;
		int line = J->trace[n].line;
		if (line > 0) {
			if (name[0])
				swprintf(buf, sizeof buf, L"\n\tat %ls (%ls:%d)", name, file, line);
			else
				swprintf(buf, sizeof buf, L"\n\tat %ls:%d", file, line);
		} else
			snprintf(buf, sizeof buf, L"\n\tat %ls (%ls)", name, file);
		js_pushstring(J, buf);
		if (n < J->tracetop - skip)
			js_concat(J);
	}
	return 1;
}

static void Ep_toString(js_State *J)
{
	wchar_t buf[256];
	const wchar_t *name = L"Error";
	const wchar_t *message = "";

	if (!js_isobject(J, -1))
		js_typeerror(J, L"not an object");

	if (js_hasproperty(J, 0, L"name"))
		name = js_tostring(J, -1);
	if (js_hasproperty(J, 0, L"message"))
		message = js_tostring(J, -1);

	swprintf(buf, sizeof buf, L"%ls: %ls", name, message);
	js_pushstring(J, buf);

	if (js_hasproperty(J, 0, L"stackTrace"))
		js_concat(J);
}

static int jsB_ErrorX(js_State *J, js_Object *prototype)
{
	int top = js_gettop(J);
	js_pushobject(J, jsV_newobject(J, JS_CERROR, prototype));
	if (top > 1) {
		js_pushstring(J, js_tostring(J, 1));
		js_setproperty(J, -2, L"message");
	}
	if (jsB_stacktrace(J, 1))
		js_setproperty(J, -2, L"stackTrace");
	return 1;
}

static void js_newerrorx(js_State *J, const wchar_t *message, js_Object *prototype)
{
	js_pushobject(J, jsV_newobject(J, JS_CERROR, prototype));
	js_pushstring(J, message);
	js_setproperty(J, -2, L"message");
	if (jsB_stacktrace(J, 0))
		js_setproperty(J, -2, L"stackTrace");
}

#define DERROR(name, Name) \
	static void jsB_##Name(js_State *J) { \
		jsB_ErrorX(J, J->Name##_prototype); \
	} \
	void js_new##name(js_State *J, const wchar_t *s) { \
		js_newerrorx(J, s, J->Name##_prototype); \
	} \
	void js_##name(js_State *J, const wchar_t *fmt, ...) { \
		va_list ap; \
		wchar_t buf[256]; \
		va_start(ap, fmt); \
		vswprintf(buf, sizeof buf, fmt, ap); \
		va_end(ap); \
		js_newerrorx(J, buf, J->Name##_prototype); \
		js_throw(J); \
	}

DERROR(error, Error)
DERROR(evalerror, EvalError)
DERROR(rangeerror, RangeError)
DERROR(referenceerror, ReferenceError)
DERROR(syntaxerror, SyntaxError)
DERROR(typeerror, TypeError)
DERROR(urierror, URIError)

#undef DERROR

void jsB_initerror(js_State *J)
{
	js_pushobject(J, J->Error_prototype);
	{
			jsB_props(J, L"name", L"Error");
			jsB_props(J, L"message", L"an error has occurred");
			jsB_propf(J, L"Error.prototype.toString", Ep_toString, 0);
	}
	js_newcconstructor(J, jsB_Error, jsB_Error, L"Error", 1);
	js_defglobal(J, L"Error", JS_DONTENUM);

	#define IERROR(NAME) \
		js_pushobject(J, J->NAME##_prototype); \
		jsB_props(J, L"name", Q(NAME)); \
		js_newcconstructor(J, jsB_##NAME, jsB_##NAME, Q(NAME), 1); \
		js_defglobal(J, Q(NAME), JS_DONTENUM);

	IERROR(EvalError);
	IERROR(RangeError);
	IERROR(ReferenceError);
	IERROR(SyntaxError);
	IERROR(TypeError);
	IERROR(URIError);

	#undef IERROR
}
