/**
* @file     TileLayout.h
* @brief    瓦片布局容器.
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
#ifndef __TILELAYOUT_H__
#define __TILELAYOUT_H__
#define Type_TileLayout                 0x15e48812
#define Proc_TileLayout_SetColumns      150
#define Proc_TileLayout_SetItemSize     151
/**瓦片布局结构*/
typedef struct _ZuiTileLayout
{
    SIZE m_szItem;
    int m_nColumns;
    ZCtlProc old_call;
    void *old_udata;
}*ZuiTileLayout, ZTileLayout;

void* ZCALL ZuiTileLayoutProc(int ProcId, ZuiControl cp, ZuiTileLayout p, void* Param1, void* Param2, void* Param3);


#endif // __UITILELAYOUT_H__
