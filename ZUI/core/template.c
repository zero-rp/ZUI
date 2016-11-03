#include <ZUI.h>


ZEXPORT ZuiAny ZCALL ZuiTemplateProc(ZuiInt ProcId, ZuiControl cp, ZuiTemplate p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return 0;
	case Proc_OnCreate: {
		p = (ZuiTemplate)malloc(sizeof(ZTemplate));
		memset(p, 0, sizeof(ZTemplate));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		p->old_call = cp->call;
		return p;
		break;
	}
	case Proc_OnInit: {
		break;
	}
	default:
		break;
	}
	return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}





ZuiVoid ZuiAddTemplate(mxml_node_t *node)
{
	ZuiText classname=NULL;
	/*解析属性*/
	for (size_t i = 0; i < node->value.num_attrs; i++)
	{
		if (wcscmp(node->value.attrs[i].name,L"class")==0)
		{
			classname = node->value.attrs[i].value;
		}



	}
	ZuiControlRegisterAdd(classname, (ZCtlProc)&ZuiTemplateProc);



}


