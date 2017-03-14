/**
* @file		List.h
* @brief	列表控件实现.
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
#ifndef __LIST_H__
#define __LIST_H__


/**列表头结构*/
typedef struct _ZListHeader
{

    ZuiAny old_udata;
    ZCtlProc old_call;
}*ZuiListHeader, ZListHeader;
/**列表体结构*/
typedef struct _ZListBody
{

    ZuiAny old_udata;
    ZCtlProc old_call;
}*ZuiListBody, ZListBody;
/**列表控件结构*/
typedef struct _ZList
{
    ZuiBool m_bScrollSelect;
    ZuiInt m_iCurSel;
    ZuiInt m_iExpandedItem;

    ZuiControl m_pList;//列表体容器
    ZuiControl m_pHeader;//表头容器

    ZuiAny old_udata;
	ZCtlProc old_call;
}*ZuiList, ZList;
ZEXPORT ZuiAny ZCALL ZuiListProc(ZuiInt ProcId, ZuiControl cp, ZuiList p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZEXPORT ZuiAny ZCALL ZuiListBodyProc(ZuiInt ProcId, ZuiControl cp, ZuiListBody p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZEXPORT ZuiAny ZCALL ZuiListHeaderProc(ZuiInt ProcId, ZuiControl cp, ZuiListHeader p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__LIST_H__
