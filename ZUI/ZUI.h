#ifndef __ZUI_H__
#define __ZUI_H__

#include <stddef.h>

//config
#include "config.h"

#ifdef PLATFORM_OS_WIN
    # if defined(BUILDING_ZUI_SHARED)
        //编译成动态库
        #ifdef __cplusplus
        #define ZEXPORT extern "C" __declspec(dllexport)
        #else
        #define ZEXPORT __declspec(dllexport)
        #endif
        #define ZCALL __stdcall
    # elif defined(USING_ZUI_SHARED)
        //使用动态库
        #ifdef __cplusplus
        #define ZEXPORT extern "C" __declspec(dllimport)
        #else
        #define ZEXPORT __declspec(dllimport)
        #endif
        #define ZCALL __stdcall
    # else
        //编译成静态库
        #ifdef __cplusplus
        #define ZEXPORT extern "C"
        #else
        #define ZEXPORT 
        #endif
        #define ZCALL 
    # endif
#else
    #ifdef __cplusplus
        #define ZEXPORT extern "C" 
    #else
        #define ZEXPORT
    #endif
        #define ZCALL __attribute__((__stdcall__))
#endif

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

//--------------------------------------------------------------------基础数据类型
typedef wchar_t*        ZuiText, _ZuiText, ZText;   //内核默认Unicode存储字符
typedef float           ZuiReal;
#ifdef _WIN64
typedef int64_t         ZuiInt;
typedef uint64_t        ZuiUInt;
#else
typedef int             ZuiInt;
typedef unsigned int    ZuiUInt;
#endif
typedef int             ZuiBool;
typedef void            ZuiVoid;
typedef void*           ZuiAny;
typedef int             ZuiColor;
typedef unsigned        ZuiByte;
/**矩形*/
typedef struct _ZRect
{
    int left;   ///左边
    int top;    ///顶边
    int right;  ///右边
    int bottom; ///底边
} *ZuiRect, ZRect;
/**矩形(浮点)*/
typedef struct _ZRectR
{
    ZuiReal left;///左边
    ZuiReal top;///顶边
    ZuiReal right;///宽度
    ZuiReal bottom;///高度
} *ZuiRectR, ZRectR;
/**点*/
typedef struct _ZPoint
{
    int x;
    int y;
} *ZuiPoint, ZPoint;
/**点(浮点)*/
typedef struct _ZPointR
{
	ZuiReal x;
	ZuiReal y;
} *ZuiPointR, ZPointR;
/**大小*/
typedef struct _ZSize
{
    int cx;	///宽度
    int cy;	///高度
} *ZuiSize, ZSize;
/**大小(浮点)*/
typedef struct _ZSizeR
{
	ZuiReal cx;	///宽度
	ZuiReal cy;	///高度
} *ZuiSizeR, ZSizeR;

#define MAKEZRECT(r, L, T, R, B) \
    r.left = L; \
    r.top = T;\
    r.right = R;\
    r.bottom = B;
#define MAKEZRECTP(r, L, T, R, B) \
    r->left = L; \
    r->top = T;\
    r->right = R;\
    r->bottom = B;
//--------------------------------------------------------------------结构定义
//内部结构
typedef struct _ZControl *ZuiControl, ZControl;
//单个资源
typedef struct _ZRes *ZuiRes, ZRes;
//图形
typedef struct _ZGraphics *ZuiGraphics, ZGraphics;
//图像
typedef struct _ZImage *ZuiImage, ZImage;
//字体
typedef struct _ZFont *ZuiFont, ZFont;
//初始化结构
typedef struct _ZuiInitConfig
{
#if (defined PLATFORM_OS_WIN)
    ZuiAny m_hInstance;  //库所在的模块句柄,动态库默认自动为当前模块,静态链接须设置此参数
#endif
    ZuiBool debug;          //启动调试器
} *ZuiInitConfig, ZInitConfig;

//--------------------------------------------------------------------回调定义
typedef ZuiControl(ZCALL* FINDCONTROLPROC)(ZuiControl, ZuiAny);
typedef ZuiAny(ZCALL *ZCtlProc)(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
typedef ZuiAny(ZCALL *ZNotifyProc)(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

//查找控件参数
#define ZFIND_ALL           0x00000000  //查找全部控件
#define ZFIND_VISIBLE       0x00000001  //查找可视的控件
#define ZFIND_ENABLED       0x00000002  //查找可用的控件
#define ZFIND_HITTEST       0x00000004
#define ZFIND_UPDATETEST    0x00000008
#define ZFIND_TOP_FIRST     0x00000010  //自顶
#define ZFIND_ME_FIRST      0x80000000
//控件标志
#define ZFLAG_TABSTOP       0x00000001
#define ZFLAG_SETCURSOR     0x00000002
#define ZFLAG_WANTRETURN    0x00000004
//--------------------------------------------------------------------Debug类
#define ZLOG_TYPE_ERROR     0
#define ZLOG_TYPE_DEBUG     1
#define ZLOG_TYPE_WARNING     2
#define ZLOG_TYPE_INFO     3

#if LOG_DEBUG || RUN_DEBUG


#define LOG_ERROR(fmt, ...) do{\
    wchar_t buf[512];   \
    _snwprintf(buf, 512, fmt, __VA_ARGS__);\
    ZuiDebugLog(ZLOG_TYPE_ERROR, buf);  \
}while(0)
#define LOG_DEGUB(fmt, ...) do{\
    wchar_t buf[512];   \
    _snwprintf(buf, 512, fmt, __VA_ARGS__);\
    wprintf(buf);\
    ZuiDebugLog(ZLOG_TYPE_DEBUG, buf);  \
}while(0)
#define LOG_WARNING(fmt, ...) do{\
    wchar_t buf[512];   \
    _snwprintf(buf, 512, fmt, __VA_ARGS__);\
    ZuiDebugLog(ZLOG_TYPE_WARNING, buf);  \
}while(0)
#define LOG_INFO(fmt, ...) do{\
    wchar_t buf[512];   \
    _snwprintf(buf, 512, fmt, __VA_ARGS__);\
    ZuiDebugLog(ZLOG_TYPE_INFO, buf);  \
}while(0)


#define LOG_DUK(ctx) do{\
    wchar_t buf[1024];   \
    duk_get_prop_string((ctx), -1, "stack");\
    _snwprintf(buf, 1024, L"%s\r\n",duk_get_string_w((ctx), -1));\
    wprintf(buf);\
    ZuiDebugLog(ZLOG_TYPE_INFO, buf);  \
}while(0)
#else
#define LOG_ERROR(fmt, ...)
#define LOG_DEGUB(fmt, ...)
#define LOG_WARNING(fmt, ...)
#define LOG_INFO(fmt, ...)
#define LOG_DUK(ctx)
#endif 
//--------------------------------------------------------------------Base类
#if 1
//功能宏
#define Type_Null               0xc6268d0b

#define ZTYLE_BOX               1   //单线边框
#define ZTYLE_BKGColor          2   //具有背景色
//-----控件事件
#define Proc_CoreInit           1   //用于控件内核初始化
#define Proc_CoreUnInit         2   //用于控件内核反初始化
#define Proc_GetType            3   //取控件类型
#define Proc_GetObject          4  //获取控件被继承后某一类型的对象
#define Proc_OnCreate           5   //内核创建
#define Proc_OnInit             6   //用户初始化
#define Proc_OnDestroy          7   //销毁
#define Proc_OnSize             8   //
#define Proc_OnEvent            9   //
#define Proc_OnNotify           10   //
#define Proc_OnPaint            11  // 绘制循序：背景颜色->背景图->状态图->文本->边框
#define Proc_EndPaint           12  //子控件绘制完毕后调用
#define Proc_OnPaintBkColor     13  //背景色
#define Proc_OnPaintBkImage     14  //背景图片
#define Proc_OnPaintStatusImage 15  //状态图片
#define Proc_OnPaintText        16  //文本
#define Proc_OnPaintBorder      17  //边框
#define Proc_OnPostPaint        18  //

#define Proc_Invalidate         19  //刷新显示
#define Proc_FindControl        20  //查找控件
#define Proc_JsInit             21  //函数调用
#define Proc_JsCall             22  //函数调用
#define Proc_JsSet              23  //
#define Proc_JsGet              24  //处理js对象 返回0则是变量,返回1则是call

//----控件属性
#define Proc_SetAttribute       25  //解析属性
#define Proc_GetAttribute       26  //取属性
#define Proc_GetControlFlags    27  //
#define Proc_Activate           28  //活动
#define Proc_SetVisible         29  //设置是否可视
#define Proc_GetVisible         30  //是否可视
#define Proc_SetText            31  //设置文本
#define Proc_GetText            32  //设置文本
#define Proc_SetName            33  //设置名字
#define Proc_SetTooltip         34  //设置提示文本
#define Proc_GetPos             35  //得到控件位置
#define Proc_SetPos             36  //设置控件位置并重绘
#define Proc_SetOs              37  //设置控件的绘制管理者
#define Proc_SetAnimation       38  //设置动画类型
//设置大小的限制值
#define Proc_GetMinWidth        39  //
#define Proc_SetMinWidth        40  //
#define Proc_GetMaxWidth        41  //
#define Proc_SetMaxWidth        42  //
#define Proc_GetMinHeight       43  //
#define Proc_SetMinHeight       44  //
#define Proc_GetMaxHeight       45  //
#define Proc_SetMaxHeight       46  //
#define Proc_GetWidth           47  //
#define Proc_GetHeight          48  //
#define Proc_GetX               49  //
#define Proc_GetY               50
#define Proc_EstimateSize       51  //获取自适应大小
#define Proc_SetFloatPercent    52  //
#define Proc_GetPadding         53
#define Proc_SetPadding         54  // 设置外边距，由上层窗口绘制
#define Proc_GetFixedXY         55  // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define Proc_SetFixedXY         56  // 仅float为true时有效
#define Proc_GetFixedWidth      57  // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define Proc_SetFixedWidth      58  // 预设的参考值
#define Proc_GetFixedHeight     59  // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define Proc_SetFixedHeight     60  // 预设的参考值
#define Proc_GetRelativePos     61  // 相对(父控件)位置
#define Proc_GetImePoint        62  // 获取输入法位置
#define Proc_SetFloat           63  //设置为浮动控件
#define Proc_SetEnabled         64  //设置可用状态
#define Proc_SetFocus           65  //设置焦点
#define Proc_SetDrag            66  //设置拖拽控件

//-------绘图资源
#define Proc_SetBkColor         67  //设置背景色
#define Proc_SetBkImage         68  //设置背景图片
#define Proc_SetBorderColor     69  //设置边框颜色

#define Proc_SetAnimationType   70  //设置动画类型


#define ZuiControlSetDrag(p,b)  ZuiControlCall(Proc_SetDrag,(p),(b),NULL,NULL)
#define ZuiControlSetFixedHeight(p,h) ZuiControlCall(Proc_SetFixedHeight,(p),(h),NULL,NULL)
#endif // 1
//--------------------------------------------------------------------ScrollBar类
#if 1
#define Type_ScrollBar                  0xd2473c6e
#define Proc_ScrollBar_SetHorizontal    1000 //设置为横向滚动条
#define Proc_ScrollBar_SetScrollPos     1001 //设置位置
#define Proc_ScrollBar_GetScrollPos     1002 //获取位置
#define Proc_ScrollBar_GetScrollRange   1003
#define Proc_ScrollBar_SetOwner         1004
#define Proc_ScrollBar_SetScrollRange   1005
#endif //1
//--------------------------------------------------------------------Layout类
#if 1
#define Type_Layout                     0xbebc5b1a
#define Type_VerticalLayout             0xd72bfbd7
#define Type_HorizontalLayout           0xf20a2d07
#define Type_TileLayout                 0x15e48812
#define Type_TabLayout                  0x61b7311c

//--------base
#define Proc_Layout_Add                 101 ///添加控件
#define Proc_Layout_AddAt               102 ///插入控件
#define Proc_Layout_Remove              103 ///移除指定控件, 参数1为真由调用方销毁成员
#define Proc_Layout_RemoveAt            104 ///移除指定位置控件
#define Proc_Layout_RemoveAll           105 ///移除全部控件
#define Proc_Layout_GetCount            106 ///取容器控件数量
#define Proc_Layout_GetItemIndex        107 ///取指定控件索引
#define Proc_Layout_SetItemIndex        108 ///设置控件所在位置
#define Proc_Layout_GetItemAt           109 ///索引取控件
#define Proc_Layout_SetFloatPos         110 ///设置浮动控件位置
#define Proc_Layout_SetInset            112 ///设置内边距
//--------Tile
#define Proc_TileLayout_SetColumns      150
#define Proc_TileLayout_SetItemSize     151
//--------Tab
#define Proc_TabLayout_SelectItem       151    //当前Tab

//--------滚动条
#define Proc_Layout_GetScrollPos        113
#define Proc_Layout_GetScrollRange      114
#define Proc_Layout_SetScrollPos        115
#define Proc_Layout_SetScrollStepSize   116
#define Proc_Layout_GetScrollStepSize   117
#define Proc_Layout_LineUp              118
#define Proc_Layout_LineDown            119
#define Proc_Layout_PageUp              120
#define Proc_Layout_PageDown            121
#define Proc_Layout_HomeUp              122
#define Proc_Layout_EndDown             123
#define Proc_Layout_LineLeft            124
#define Proc_Layout_LineRight           125
#define Proc_Layout_PageLeft            126
#define Proc_Layout_PageRight           127
#define Proc_Layout_HomeLeft            128
#define Proc_Layout_EndRight            129
#define Proc_Layout_EnableScrollBar     130
#define Proc_Layout_GetVerticalScrollBar    131
#define Proc_Layout_GetHorizontalScrollBar  132
#define Proc_Layout_ProcessScrollBar    133 //调整滚动条位置
#define Proc_Layout_GetChildAlign       134
#define Proc_Layout_GetChildVAlign      135

#define ZuiLayoutAdd(p,cp) ZuiControlCall(Proc_Layout_Add,(p),(cp),NULL,NULL)

#endif // 1
//--------------------------------------------------------------------Window类
#if 1
#define Type_Window                 0x5e298cdf

#define Proc_Window_SetNoBox        1001    //设置为无边框窗体
#define Proc_Window_SetWindowMin    1002    //
#define Proc_Window_SetWindowMax    1003
#define Proc_Window_SetWindowRestor 1004
#define Proc_Window_SetMinInfo      1005
#define Proc_Window_SetMaxInfo      1006
#define Proc_Window_SetSize         1007
#define Proc_Window_SetComBo        1008
#define Proc_Window_Popup           1009
#define Proc_Window_SetToolWindow   1010
#define Proc_Window_Center          1011

#endif // 1
//--------------------------------------------------------------------Button类
#if 1
#define Type_Button                 0x5e7331d7

#define Proc_Button_SetResNormal    201    //
#define Proc_Button_SetResHot       202    //高亮状态
#define Proc_Button_SetResPushed    203    //按下状态
#define Proc_Button_SetResFocused   204    //焦点图片
#define Proc_Button_SetResDisabled  205    //非激活状态

#define Proc_Button_SetColorNormal    206    //
#define Proc_Button_SetColorHot       207    //高亮状态
#define Proc_Button_SetColorPushed    208    //按下状态
#define Proc_Button_SetColorFocused   209    //焦点图片
#define Proc_Button_SetColorDisabled  210    //非激活状态
#endif // 1
//--------------------------------------------------------------------Label类
#if 1
#define Type_Label                  0xe538ac26
#define Proc_Label_SetFont          151     //设置字体
#define Proc_Label_SetTextColor     152     //设置文本颜色
#define Proc_Label_SetTextPadding   153     //字体边距
#endif // 1
//--------------------------------------------------------------------类
#if 1

#endif // 1
//--------------------------------------------------------------------类
#if 1

#endif // 1
//--------------------------------------------------------------------SplitterBar类
#if 1
#define Type_SplitterBar    0xd77cbbba
#endif // 1
//--------------------------------------------------------------------ProgressBar类
#if 1
#define Type_ProgressBar            0x251a8a44
#endif // 1
//--------------------------------------------------------------------Option类
#if 1
#define Type_Option                         0x8f0a0522
#define Proc_Option_SetSelected             1031    //
#define Proc_Option_GetSelected             1032    //
#define Proc_Option_SetResSelected          1033    //选中的普通状态
#define Proc_Option_SetResSelectedHot       1034    //选中的点燃状态
#define Proc_Option_SetResSelectedPushed    1035    //选中的按下状态
#define Proc_Option_SetGroup                1036
#endif // 1
//--------------------------------------------------------------------CheckBox类
#if 1
#define Type_CheckBox            0x251a8a44
#endif // 1
//--------------------------------------------------------------------List类
#if 1
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
#define Proc_List_EnsureVisible             409     //定位滚动条


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
#define Proc_ListElement_Select             403     //选中
#define Proc_ListElement_SelectMulti        404     //多选
#endif // 1
//--------------------------------------------------------------------TreeView类
#if 1
#define Type_TreeView                   0xa1526942
#define Type_TreeNode                   0xa78958f4

#define Proc_TreeView_Add                   Proc_Layout_Add  
#define Proc_TreeView_AddAt                 Proc_Layout_AddAt
#define Proc_TreeView_Remove                Proc_Layout_Remove
#define Proc_TreeView_RemoveAt              Proc_Layout_RemoveAt
#define Proc_TreeView_RemoveAll             Proc_Layout_RemoveAll
#define Proc_TreeView_SetItemExpand         1000    //展开关闭
#define Proc_TreeView_SetItemCheckBox       1001    //选中反选

#define Proc_TreeNode_GetCountChild         1000
#define Proc_TreeNode_GetChildNode          1001
#define Proc_TreeNode_SetTreeView           1002
#define Proc_TreeNode_Add                   Proc_Layout_Add  
#define Proc_TreeNode_AddAt                 Proc_Layout_AddAt
#define Proc_TreeNode_Remove                Proc_Layout_Remove
#define Proc_TreeNode_AddChildNode          1003
#define Proc_TreeNode_CalLocation           1004    //计算缩进
#define Proc_TreeNode_GetLastNode           1005
#define Proc_TreeNode_GetTreeIndex          1006    //取得全局树视图的索引
#define Proc_TreeNode_GetDottedLine         1007
#define Proc_TreeNode_SetParentNode         1008    //设置父节点
#define Proc_TreeNode_GetItemButton         1009
#define Proc_TreeNode_IsHasChild            1010    //是否有子节点
#define Proc_TreeNode_SetVisibleFolderBtn   1011
#define Proc_TreeNode_GetVisibleFolderBtn   1012
#define Proc_TreeNode_SetVisibleCheckBtn    1013
#define Proc_TreeNode_GetVisibleCheckBtn    1014
#define Proc_TreeNode_GetFolderButton       1015
#define Proc_TreeNode_GetCheckBox           1016
#endif // 1
//--------------------------------------------------------------------导出函数
#if defined(__cplusplus)
extern "C"
{
#endif
    //初始化
    ZEXPORT ZuiBool ZCALL ZuiInit(ZuiInitConfig config);
    //反初始化
    ZEXPORT ZuiBool ZCALL ZuiUnInit();
    //Zui消息循环.
    ZEXPORT ZuiInt ZCALL ZuiMsgLoop();
    //退出Zui消息循环.
    ZEXPORT ZuiVoid ZCALL ZuiMsgLoop_exit();

    //创建控件
    ZEXPORT ZuiControl ZCALL NewZuiControl(ZuiText classname, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
    //销毁控件
    ZEXPORT ZuiVoid ZCALL FreeZuiControl(ZuiControl p, ZuiBool Delayed);
    //调用控件处理函数
    ZEXPORT ZuiAny ZCALL ZuiControlCall(ZuiInt ProcId, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

    //载入布局窗口
    ZEXPORT ZuiControl ZCALL ZuiLayoutLoad(ZuiAny xml, ZuiInt len);




#if defined(__cplusplus)
}
#endif

#endif
