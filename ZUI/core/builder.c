#include <ZUI.h>


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
				ClassName = node->value.name;
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
ZEXPORT ZuiControl ZCALL ZuiBuilderJs(js_State *J) {
	js_newcfunction(J, ZuiJsBind_Call_exit,L"exit", 0);
	js_setglobal(J, L"exit");

	js_newcfunction(J, ZuiJsBind_Call_print, L"a", 0);
	js_setglobal(J, L"a");
}


ZEXPORT ZuiBool ZCALL ZuiBuilderJsLoad(js_State *J, ZuiText str, ZuiInt len) {
	js_dostring(J, str);
}
