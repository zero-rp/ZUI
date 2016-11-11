#include "jsi.h"
#include "jsvalue.h"
#include "jsbuiltin.h"
#include "regexp.h"

void js_newregexp(js_State *J, const wchar_t *pattern, int flags)
{
	const wchar_t *error;
	js_Object *obj;
	Reprog *prog;
	int opts;

	obj = jsV_newobject(J, JS_CREGEXP, J->RegExp_prototype);

	opts = 0;
	if (flags & JS_REGEXP_I) opts |= REG_ICASE;
	if (flags & JS_REGEXP_M) opts |= REG_NEWLINE;

	prog = js_regcomp(pattern, opts, &error);
	if (!prog)
		js_syntaxerror(J, L"regular expression: %s", error);

	obj->u.r.prog = prog;
	obj->u.r.source = js_strdup(J, pattern);
	obj->u.r.flags = flags;
	obj->u.r.last = 0;
	js_pushobject(J, obj);
}

void js_RegExp_prototype_exec(js_State *J, js_Regexp *re, const wchar_t *text)
{
	int i;
	int opts;
	Resub m;

	opts = 0;
	if (re->flags & JS_REGEXP_G) {
		if (re->last > wcslen(text)) {
			re->last = 0;
			js_pushnull(J);
			return;
		}
		if (re->last > 0) {
			text += re->last;
			opts |= REG_NOTBOL;
		}
	}

	if (!js_regexec(re->prog, text, &m, opts)) {
		js_newarray(J);
		js_pushstring(J, text);
		js_setproperty(J, -2, L"input");
		js_pushnumber(J, js_utfptrtoidx(text, m.sub[0].sp));
		js_setproperty(J, -2, L"index");
		for (i = 0; i < m.nsub; ++i) {
			js_pushlstring(J, m.sub[i].sp, m.sub[i].ep - m.sub[i].sp);
			js_setindex(J, -2, i);
		}
		if (re->flags & JS_REGEXP_G)
			re->last = re->last + (m.sub[0].ep - text);
		return;
	}

	if (re->flags & JS_REGEXP_G)
		re->last = 0;

	js_pushnull(J);
}

static void Rp_test(js_State *J)
{
	js_Regexp *re;
	const wchar_t *text;
	int opts;
	Resub m;

	re = js_toregexp(J, 0);
	text = js_tostring(J, 1);

	opts = 0;
	if (re->flags & JS_REGEXP_G) {
		if (re->last > wcslen(text)) {
			re->last = 0;
			js_pushboolean(J, 0);
			return;
		}
		if (re->last > 0) {
			text += re->last;
			opts |= REG_NOTBOL;
		}
	}

	if (!js_regexec(re->prog, text, &m, opts)) {
		if (re->flags & JS_REGEXP_G)
			re->last = re->last + (m.sub[0].ep - text);
		js_pushboolean(J, 1);
		return;
	}

	if (re->flags & JS_REGEXP_G)
		re->last = 0;

	js_pushboolean(J, 0);
}

static void jsB_new_RegExp(js_State *J)
{
	js_Regexp *old;
	const wchar_t *pattern;
	int flags;

	if (js_isregexp(J, 1)) {
		if (js_isdefined(J, 2))
			js_typeerror(J, L"cannot supply flags when creating one RegExp from another");
		old = js_toregexp(J, 1);
		pattern = old->source;
		flags = old->flags;
	} else if (js_isundefined(J, 1)) {
		pattern = "";
		flags = 0;
	} else {
		pattern = js_tostring(J, 1);
		flags = 0;
	}

	if (js_isdefined(J, 2)) {
		const wchar_t *s = js_tostring(J, 2);
		int g = 0, i = 0, m = 0;
		while (*s) {
			if (*s == 'g') ++g;
			else if (*s == 'i') ++i;
			else if (*s == 'm') ++m;
			else js_syntaxerror(J, L"invalid regular expression flag: '%c'", *s);
			++s;
		}
		if (g > 1) js_syntaxerror(J, L"invalid regular expression flag: 'g'");
		if (i > 1) js_syntaxerror(J, L"invalid regular expression flag: 'i'");
		if (m > 1) js_syntaxerror(J, L"invalid regular expression flag: 'm'");
		if (g) flags |= JS_REGEXP_G;
		if (i) flags |= JS_REGEXP_I;
		if (m) flags |= JS_REGEXP_M;
	}

	js_newregexp(J, pattern, flags);
}

static void jsB_RegExp(js_State *J)
{
	if (js_isregexp(J, 1))
		return;
	jsB_new_RegExp(J);
}

static void Rp_toString(js_State *J)
{
	js_Regexp *re;
	wchar_t *out;

	re = js_toregexp(J, 0);

	out = js_malloc(J, (wcslen(re->source) + 6)*sizeof(wchar_t)); /* extra space for //gim */
	wcscpy(out, L"/");
	wcscat(out, re->source);
	wcscat(out, L"/");
	if (re->flags & JS_REGEXP_G) wcscat(out, L"g");
	if (re->flags & JS_REGEXP_I) wcscat(out, L"i");
	if (re->flags & JS_REGEXP_M) wcscat(out, L"m");

	if (js_try(J)) {
		js_free(J, out);
		js_throw(J);
	}
	js_pop(J, 0);
	js_pushstring(J, out);
	js_endtry(J);
	js_free(J, out);
}

static void Rp_exec(js_State *J)
{
	js_RegExp_prototype_exec(J, js_toregexp(J, 0), js_tostring(J, 1));
}

void jsB_initregexp(js_State *J)
{
	js_pushobject(J, J->RegExp_prototype);
	{
		jsB_propf(J, L"RegExp.prototype.toString", Rp_toString, 0);
		jsB_propf(J, L"RegExp.prototype.test", Rp_test, 0);
		jsB_propf(J, L"RegExp.prototype.exec", Rp_exec, 0);
	}
	js_newcconstructor(J, jsB_RegExp, jsB_new_RegExp, L"RegExp", 1);
	js_defglobal(J, L"RegExp", JS_DONTENUM);
}
