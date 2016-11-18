/**
* @file		Label.h
* @brief	标签控件实现.
* @author	[Zero](22249030)
* @version	1.0
* @date		$date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __CodeEdit_H__
#define __CodeEdit_H__
#include "Scintilla.h" 
#include "SciLexer.h"
/**按钮控件结构*/
typedef struct _ZCodeEdit
{
	SciFnDirect fnDirect;
	sptr_t ptrDirect;

	ZCtlProc old_call;
	ZuiAny old_udata;
}*ZuiCodeEdit, ZCodeEdit;
ZEXPORT ZuiAny ZCALL ZuiCodeEditProc(ZuiInt ProcId, ZuiControl cp, ZuiLabel p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__CodeEdit_H__
