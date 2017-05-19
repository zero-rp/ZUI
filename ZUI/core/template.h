#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__
#include <ZUI.h>
#include "mxml.h"
ZuiBool ZuiTemplateInit();
ZuiVoid ZuiTemplateUnInit();
ZuiVoid ZuiAddTemplate(mxml_node_t *node);
ZuiVoid ZuiLoadTemplate(mxml_node_t *n, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
#endif //__TEMPLATE_H__
