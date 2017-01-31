#include <ZUI.h>

rb_root *Global_TemplateClass;
ZuiBool ZuiTemplateInit()
{
    Global_TemplateClass = rb_new();
    return TRUE;
}
ZuiVoid ZuiAddTemplate(mxml_node_t *node)
{
    ZuiText classname = NULL;
    /*解析属性*/
    for (size_t i = 0; i < node->value.num_attrs; i++)
    {
        if (wcscmp(node->value.attrs[i].name, L"class") == 0)
        {
            classname = node->value.attrs[i].value;
        }
    }
    if (classname)
    {
        mxml_node_t *new_node = mxmlClone(node, NULL);
        if (new_node)
        {
            wcslwr(classname);
            rb_insert((key_t)Zui_Hash(classname), new_node, Global_TemplateClass);
        }
    }
}

ZuiVoid ZuiLoadTemplate(mxml_node_t *n, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    ZuiText Layout = "Layout";
    ZuiText ClassName = NULL;
    ZuiControl Control;
    mxml_node_t *node = mxmlClone(n, NULL);//先把节点克隆出来

    for (size_t i = 0; i < node->value.num_attrs; i++)
    {
        if (wcscmp(node->value.attrs[i].name, L"layout") == 0) {
            Layout = node->value.attrs[i].value;
        }
        else
        {
            ZuiControlCall(Proc_SetAttribute, p, node->value.attrs[i].name, node->value.attrs[i].value, NULL);
        }
    }
    if (wcscmp(Layout, L"layout") == 0)
    {
        p->m_sUserData = ZuiLayoutProc(Proc_OnCreate, p, 0, Param1, Param2, Param3);
        p->call = &ZuiLayoutProc;
    }
    else if (wcscmp(Layout, L"horizontal") == 0)
    {
        p->m_sUserData = ZuiHorizontalLayoutProc(Proc_OnCreate, p, 0, Param1, Param2, Param3);
        p->call = &ZuiHorizontalLayoutProc;
    }
    else if (wcscmp(Layout, L"vertical") == 0)
    {
        p->m_sUserData = ZuiVerticalLayoutProc(Proc_OnCreate, p, 0, Param1, Param2, Param3);
        p->call = &ZuiVerticalLayoutProc;
    }
    else if (wcscmp(Layout, L"tile") == 0)
    {
        p->m_sUserData = ZuiTileLayoutProc(Proc_OnCreate, p, 0, Param1, Param2, Param3);
        p->call = &ZuiTileLayoutProc;
    }
    node->user_data = p;
    for (node = mxmlFindElement(node, node, NULL, NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlWalkNext(node, NULL, MXML_DESCEND))
    {
        ClassName = node->value.name;
#if !(defined NDEBUG)
        printf("Template创建控件: 类名:%ls\r\n", ClassName);
#endif
        Control = NewZuiControl(ClassName, NULL, NULL, NULL);
        if (Control) {
            node->user_data = Control;//保存控件到节点
                                      /*添加到容器*/
            ZuiControlCall(Proc_Layout_Add, node->parent->user_data, Control, NULL, NULL);
            /*解析属性*/
            for (size_t i = 0; i < node->value.num_attrs; i++)
            {
                ZuiControlCall(Proc_SetAttribute, Control, node->value.attrs[i].name, node->value.attrs[i].value, NULL);
            }
        }
    }
    mxmlDelete(node);
}
