#include <ZUI.h>

rb_root *Global_TemplateClass;
ZuiBool ZuiTemplateInit()
{
	Global_TemplateClass = rb_new();
	return TRUE;
}
ZuiVoid ZuiAddTemplate(mxml_node_t *node)
{
	ZuiText classname=NULL;
	/*Ω‚Œˆ Ù–‘*/
	for (size_t i = 0; i < node->value.num_attrs; i++)
	{
		if (wcscmp(node->value.attrs[i].name,L"class")==0)
		{
			classname = node->value.attrs[i].value;
		}
	}
	if (classname)
	{
		mxml_node_t *new_node = mxmlClone(node, NULL);
		if (new_node)
		{
			rb_insert((key_t)Zui_Hash(classname), new_node, Global_TemplateClass);
		}
	}
}


