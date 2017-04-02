/**
* @file     List.h
* @brief    列表控件实现.
* @author   [Zero](22249030)
* @version  1.0
* @date     $date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __LIST_H__
#define __LIST_H__

#define Type_List                           0x86c6c0f9
#define Type_ListBody                       0x66e9ee9e
#define Type_ListElement                    0x0
#define Type_ListHeader                     0x39323603
#define Type_ListHeaderItem                 0x25996255


#define Proc_List_GetHeader                 400    //取列表头控件
#define Proc_List_GetListInfo               401    //取列表属性
#define Proc_List_Add                       Proc_Layout_Add
#define Proc_List_GetCount                  Proc_Layout_GetCount
#define Proc_List_AddAt                     Proc_Layout_AddAt
#define Proc_List_Remove                    Proc_Layout_Remove
#define Proc_List_RemoveAt                  Proc_Layout_RemoveAt
#define Proc_List_RemoveAll                 Proc_Layout_RemoveAll
#define Proc_List_SetScrollPos              Proc_Layout_SetScrollPos
#define Proc_List_GetItemAt                 Proc_Layout_GetItemAt
#define Proc_List_SelectItem                402     //选择表项
#define Proc_List_SelectMultiItem           403     //多项选择
#define Proc_List_SetMultiSelect            404     //置允许多项选择
#define Proc_List_IsMultiSelect             405     //是否允许多项选择
#define Proc_List_UnSelectItem              406
#define Proc_List_SelectAllItems            407     //选择全部行
#define Proc_List_UnSelectAllItems          408     //取消所有选择项



#define Proc_List_GetVerticalScrollBar      Proc_Layout_GetVerticalScrollBar
#define Proc_List_GetHorizontalScrollBar    Proc_Layout_GetHorizontalScrollBar
#define Proc_List_EnableScrollBar           Proc_Layout_EnableScrollBar

#define Proc_ListHeaderItem_SetSepWidth     400
#define Proc_ListHeaderItem_SetNormalImage  401
#define Proc_ListHeaderItem_SetHotImage     402
#define Proc_ListHeaderItem_SetPushedImage  403
#define Proc_ListHeaderItem_SetFocusedImage 404
#define Proc_ListHeaderItem_SetSepImage     405
#define Proc_ListHeaderItem_GetThumbRect    406
#define Proc_ListHeaderItem_SetDragable     407
#define Proc_ListHeaderItem_EstimateSize    Proc_EstimateSize

#define Proc_ListBody_SetOwner              400    //设置宿主
#define Proc_ListBody_SetScrollPos          Proc_Layout_SetScrollPos

#define Proc_ListElement_SetOwner           400    //设置宿主
#define Proc_ListElement_SetIndex           401    //
#define Proc_ListElement_GetIndex           402    //

/**列表头元素结构*/
typedef struct _ZListHeaderItem
{
    ZPoint ptLastMouse;
    ZuiBool m_bDragable;
    UINT m_uButtonState;    //按钮状态
    int m_iSepWidth;        //分隔符区域


    ZuiRes m_diNormal;  //通常图片
    ZuiRes m_diHot;        //点燃图片
    ZuiRes m_diPushed;  //按下图片
    ZuiRes m_diFocused; //焦点图片
    ZuiRes m_diSep;

    ZRect Thumb;
    ZuiAny old_udata;
    ZCtlProc old_call;
}*ZuiListHeaderItem, ZListHeaderItem;

/**列表头结构*/
typedef struct _ZListHeader
{
    ZSize cXY;  //表头大小
    ZuiAny old_udata;
    ZCtlProc old_call;
}*ZuiListHeader, ZListHeader;

/**列表体结构*/
typedef struct _ZListBody
{
    ZuiControl m_pOwner;//宿主列表
    ZuiAny old_udata;
    ZCtlProc old_call;
}*ZuiListBody, ZListBody;

/**列表元素结构*/
typedef struct _ZListElement
{
    ZuiInt m_iIndex;        //列索引
    ZuiBool m_bSelected;    //选中
    UINT m_uButtonState;    //列状态
    ZuiControl m_pOwner;    //宿主

    ZuiAny old_udata;
    ZCtlProc old_call;
}*ZuiListElement, ZListElement;

/**列表属性结构*/
#define ZLIST_MAX_COLUMNS 32
typedef struct _ZListInfo
{
    int nColumns;//列数量
    ZRect rcColumn[ZLIST_MAX_COLUMNS];

    ZuiBool bAlternateBk;       //使用交替背景色
    DWORD dwLineColor;          //行间线颜色
    DWORD dwBkColor;            //背景色
    DWORD dwHotBkColor;         //点燃背景色
    DWORD dwSelectedBkColor;    //选中背景色
    DWORD dwDisabledBkColor;    //禁用背景色

    ZuiBool bShowRowLine;       //显示行线
    ZuiBool bShowColumnLine;    //显示列线
} *ZuiListInfo, ZListInfo;

/**列表控件结构*/
typedef struct _ZList
{
    ZuiBool m_bScrollSelect;
    ZuiInt m_iCurSel;
    ZuiInt m_iExpandedItem;

    ZuiControl m_pList;     //列表体容器
    ZuiControl m_pHeader;   //表头容器

    ZListInfo m_ListInfo;   //列表属性

    ZuiAny old_udata;
    ZCtlProc old_call;
}*ZuiList, ZList;
ZEXPORT ZuiAny ZCALL ZuiListProc(ZuiInt ProcId, ZuiControl cp, ZuiList p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZEXPORT ZuiAny ZCALL ZuiListBodyProc(ZuiInt ProcId, ZuiControl cp, ZuiListBody p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZEXPORT ZuiAny ZCALL ZuiListElementProc(ZuiInt ProcId, ZuiControl cp, ZuiListElement p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZEXPORT ZuiAny ZCALL ZuiListHeaderProc(ZuiInt ProcId, ZuiControl cp, ZuiListHeader p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZEXPORT ZuiAny ZCALL ZuiListHeaderItemProc(ZuiInt ProcId, ZuiControl cp, ZuiListHeaderItem p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif  //__LIST_H__
