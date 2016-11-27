#include <ZUI.h>
ZuiControl ZuiLayoutLoadMenuNode(mxml_node_t *tree, ZuiControl win) {
	mxml_node_t *node;
	ZuiText ClassName = NULL;
	ZuiStringFormat StringFormat = NULL;
	ZuiBool Visible = FALSE, Enabled = TRUE;
	ZuiControl Control;
	for (node = mxmlFindElement(tree, tree, NULL, NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlWalkNext(node, NULL, MXML_DESCEND)/*node = mxmlFindElement(node, tree, NULL,NULL,NULL,MXML_DESCEND)*/) {
		{
			ClassName = node->value.name;
#if !(defined NDEBUG)
			printf("layout创建控件: 类名:%ls\r\n", ClassName);
#endif
			if (wcscmp(ClassName, L"Template") == 0) {//模版类
				ZuiAddTemplate(node);
				node = node->next;
				if (node)
					ClassName = node->value.name;
				else
					continue;
			}
			if (wcscmp(ClassName, L"Menu") == 0) {//菜单类
				ZuiAddMenu(node, win);
				node = node->next;
				if (node)
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

				/*解析属性*/
				for (size_t i = 0; i < node->value.num_attrs; i++)
				{
					ZuiControlCall(Proc_SetAttribute, Control, node->value.attrs[i].name, node->value.attrs[i].value, NULL);
				}
			}
		}
	}
}


ZuiVoid ZuiAddMenu(mxml_node_t *node, ZuiControl win) {
	ZuiText name = NULL;
	/*解析属性*/
	for (size_t i = 0; i < node->value.num_attrs; i++)
	{
		if (wcscmp(node->value.attrs[i].name, L"name") == 0)
		{
			name = node->value.attrs[i].value;
		}
	}
	if (name)
	{
		ZuiMenu p = (ZuiMenu)malloc(sizeof(ZMenu));
		memset(p, 0, sizeof(ZMenu));
		p->win = NewZuiControl(L"window", TRUE, NULL, NULL);
		node->user_data = p->win;
		ZuiLayoutLoadMenuNode(node, p->win);

	}
}


