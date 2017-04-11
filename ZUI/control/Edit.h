/**
* @file     Edit.h
* @brief    编辑框控件实现.
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
#ifndef __EDIT_H__
#define __EDIT_H__
#define Type_Edit           0xa4fb51ff

#define Proc_Edit_AddLine           201     //添加一行
#define Proc_Edit_AddChar           202     //添加字符
#define Proc_Edit_AddTxt            203     //添加文本串
#define Proc_Edit_MovePos           204     //移动读写位置 以当前输入位置为原点
#define Proc_Edit_MoveLine          205     //移动读写行 以当前输入位置为原点
#define Proc_Edit_CalcCurPos        206     //计算光标位置
#define Proc_Edit_OnPaintCur        207     //绘制光标
#define Proc_Edit_OnPaintObject     208     //绘制对象
#define Proc_Edit_Delete            209     //退格
#define Proc_Edit_MovePosLineEnd    210     //移动读写位置到行尾

/*编辑框对象类型*/
enum ZEditObjectType
{
    ZEOT_TXT = 1,
    ZEOT_IMG,
    ZEOT_CONTROL
};
/**编辑框单个对象*/
typedef struct _ZEditObject
{
    enum ZEditObjectType    type;   //对象类型
    ZSizeR	size;                   //对象大小
}*ZuiEditObject, ZEditObject;

//文本对象结构
#define Proc_Edit_ObjectTextBufLen	512	//缓冲区增长步长
typedef struct _ZEditObjectText
{
    ZEditObject obj;
    ZuiStringFormat sf;     //文本格式
    ZuiText	buf;            //文本数据
    ZuiInt	buflen;         //缓冲区长度
    ZuiInt	write_len;      //写入数据的长度
    ZuiInt	write_pos;      //写入位置
    ZuiPointR out_pt;       //文本输出坐标数组
}*ZuiEditObjectText, ZEditObjectText;
//图片对象结构
typedef struct _ZEditObjectImage
{
    ZEditObject obj;
    ZuiImage img;   //图片对象
}*ZuiEditObjectImage, ZEditObjectImage;


/**编辑框列结构*/
typedef struct _ZEditLine
{
    DArray *m_array;    //行数据
    ZRectR rc;         //行位置
}*ZuiEditLine, ZEditLine;

/**编辑框控件结构*/
typedef struct _ZEdit
{
    ZuiStringFormat sf;     //编辑框默认文本格式

    ZuiUInt m_uState;		//控件状态
    ZuiColor FrameColor;    //边框颜色
    ZuiColor FireColor;     //点燃颜色
    ZuiColor FocusColor;    //焦点颜色
    
    
    ZuiInt line_num;		//列数量
    DArray *line_data;		//列数据
    ZPoint write_pos;       //当前行写入位置 第几列第几个对象 列索引为光标前的的对象 由不同的对象记录具体的读写位置
    

	ZPointR cur_pos;		//光标位置 记录具体坐标位置 此位置通过写入位置计算得来
	ZuiBool cur_type;		//光标状态
	ZuiInt	cur_height;		//光标高度


	ZuiControl m_pVerticalScrollBar;    //纵向滚动条
	ZuiControl m_pHorizontalScrollBar;  //横向滚动条
    
    ZCtlProc old_call;
	ZuiAny old_udata;
}*ZuiEdit, ZEdit;
ZEXPORT ZuiAny ZCALL ZuiEditProc(ZuiInt ProcId, ZuiControl cp, ZuiEdit p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif //__EDIT_H__
