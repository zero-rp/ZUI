#include <ZUI.h>

js_State *Global_Js;
ZEXPORT ZuiControl ZCALL ZuiLayoutLoad(ZuiAny xml, ZuiInt len) {
	mxml_node_t *tree;
	mxml_node_t *node;
	ZuiText ClassName = NULL;
	ZuiStringFormat StringFormat = NULL;
	ZuiBool Visible = FALSE, Enabled = TRUE;
	ZuiControl Control;
	ZuiControl win = NULL;//本xml对应的窗口
	tree = mxmlLoadString(NULL, xml, len);

	for (node = mxmlFindElement(tree, tree, NULL, NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlWalkNext(node, NULL, MXML_DESCEND)/*node = mxmlFindElement(node, tree, NULL,NULL,NULL,MXML_DESCEND)*/) {
		{
			ClassName = node->value.name;
#if !(defined NDEBUG)
			printf("layout创建控件: 类名:%ls\r\n", ClassName);
#endif
			if (wcscmp(ClassName, L"Template")==0) {//模版类
				ZuiAddTemplate(node);
				node = node->next;
				if(node)
					ClassName = node->value.name;
				else
					continue;
			}
			if (wcscmp(ClassName, L"LoadScript") == 0) {
				ZuiText src = NULL;
				for (size_t i = 0; i < node->value.num_attrs; i++)
				{
					if (wcscmp(node->value.attrs[i].name, L"src") == 0) {
						src = node->value.attrs[i].value;
					}
				}
				ZuiRes res = ZuiResDBGetRes(src, ZREST_TXT);
				ZuiBuilderJsLoad(win->m_pManager->m_js, res->p, res->plen);
				ZuiResDBDelRes(res);
			}
			else if (!node->user_data) {//当前节点还未创建
				Control = NewZuiControl(ClassName, NULL, NULL, NULL);
				if (node->parent->user_data && wcsicmp(ClassName, L"window") != 0) {
					//上级控件已存在且当前欲创建的子窗口不为窗口对象
					if (Control) {
						node->user_data = Control;//保存控件到节点
						/*添加到容器*/
						ZuiControlCall(Proc_Layout_Add, node->parent->user_data, Control, NULL, NULL);
					}
					else {
						/*当前控件创建失败 子控件肯定创建不了 删除节点*/
						mxmlDelete(node);
						/*再次从头处理*/
						node = tree;
						continue;//窗口创建失败就没必要继续下去了
					}
				}
				else if (!node->parent->user_data && wcsicmp(ClassName, L"window") == 0) {
					//上级控件已存在且当前欲创建的子窗口不为窗口对象
					if (Control) {
						node->user_data = Control;//保存控件到节点
						win = Control;
					}
					else
						break;//窗口创建失败就没必要继续下去了
				}
				/*解析属性*/
				for (size_t i = 0; i < node->value.num_attrs; i++)
				{
					ZuiControlCall(Proc_SetAttribute, Control, node->value.attrs[i].name, node->value.attrs[i].value, NULL);
				}
			}
		}
	}
	/*解析完成后释放xml树*/
	mxmlDelete(tree);
	return win;
}
//----------------------------------------------------------------------------

static void ZuiJsBind_Call_exit(js_State *J) {
	exit(0);
}
static void ZuiJsBind_Call_print(js_State *J) {
	for (size_t i = 0; i < js_gettop(J) - 1; i++)
	{
		printf("%ls", js_tostring(J, i+1));
	}
	printf("\r\n");
	js_pushundefined(J);
}


//--------------------------------------------------Control_Begin
#define TAG L"Control"

//调用函数
static void Control_prototype_call(js_State *J)
{
	ZuiControl p = js_touserdata(J, 0, TAG);
	ZuiText name = js_getcfunctionname(J, -2);
	ZuiControlCall(Proc_JsCall, p, name, J, NULL);
}
static int Control_has(js_State *J, void *p, const wchar_t *name) {
	if (ZuiControlCall(Proc_JsHas, p, name, J, NULL)) {
		js_newcfunction(J, Control_prototype_call, name, 0);
	}
	return 1;
}
//设置参数
static int Control_put(js_State *J, void *p, const wchar_t *name)
{
	ZuiControlCall(Proc_JsPut, p, name, J, NULL);
	return 1;
}
//创建控件
static void new_Control(js_State *J)
{
	ZuiText *classname = js_tostring(J, 1);
	ZuiControl p = NewZuiControl(classname, NULL, NULL, NULL);

	js_currentfunction(J);
	js_getproperty(J, -1, L"prototype");
	js_newuserdatax(J, TAG, p, Control_has, Control_put, NULL, NULL);
}

void ZuiBuilderJs_Control(js_State *J)
{
	js_getglobal(J, L"Object");
	{
		//js_newcfunction(J, Control_prototype_call, L"Control.prototype",0);
		//js_defproperty(J, -2, L"call", JS_DONTENUM);
	}
	js_newcconstructor(J, new_Control, new_Control, L"Control",0);
	js_defglobal(J, L"Control", JS_DONTENUM);
}
//查找控件
static void ZuiJsBind_Call_GetByName(js_State *J) {
	if (js_isstring(J, 1)) {
		ZuiPaintManager p = js_getcontext(J);
		if (p && p->m_pRoot)
		{
			ZuiControl cp = ZuiControlFindName(p->m_pRoot, js_tostring(J, 1));
			if (cp)
			{
				js_newobject(J);
				js_getproperty(J, -1, L"prototype");
				js_newuserdatax(J, TAG, cp, Control_has, Control_put, NULL, NULL);
				return;
			}
		}
	}
	js_pushundefined(J);
}
ZuiVoid ZuiBuilderJs_pushControl(js_State *J, ZuiControl cp) {
	if (!cp)
		return;
	js_newobject(J);
	js_getproperty(J, -1, L"prototype");
	js_newuserdatax(J, TAG, cp, Control_has, Control_put, NULL, NULL);
	return ;
}
//--------------------------------------------------Control_End
static void ZuiJsBind_Call_LayoutLoad(js_State *J) {
	if (js_isstring(J, 1)) {
		ZuiRes res = ZuiResDBGetRes(js_tostring(J, 1), ZREST_STREAM);
		if (res)
		{
			ZuiControl p = ZuiLayoutLoad(res->p, res->plen);
			ZuiResDBDelRes(res);
			if (p) {
				js_newobject(J);
				js_getproperty(J, -1, L"prototype");
				js_newuserdatax(J, TAG, p, Control_has, Control_put, NULL, NULL);
				return;
			}
		}
	}
	js_pushundefined(J);
}
//---------------------------------------------------------
ZEXPORT ZuiBool ZCALL ZuiBuilderJs(js_State *J) {
	js_newcfunction(J, ZuiJsBind_Call_exit,L"exit", 0);
	js_setglobal(J, L"exit");

	js_newcfunction(J, ZuiJsBind_Call_print, L"print", 0);
	js_setglobal(J, L"print");

	js_newcfunction(J, ZuiJsBind_Call_GetByName, L"GetByName", 0);
	js_setglobal(J, L"GetByName");

	js_newcfunction(J, ZuiJsBind_Call_LayoutLoad, L"LayoutLoad", 0);
	js_setglobal(J, L"LayoutLoad");

	ZuiBuilderJs_Control(J);
	return TRUE;
}

ZEXPORT ZuiBool ZCALL ZuiBuilderJsLoad(js_State *J, ZuiText str, ZuiInt len) {
	js_dostring(J, str);
}
ZuiBool ZuiBuilderInit() {
	Global_Js = js_newstate(NULL, NULL, JS_STRICT);
	ZuiBuilderJs(Global_Js);
	return TRUE;
}