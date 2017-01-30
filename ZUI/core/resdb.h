/**
* @file		resdb.h
* @brief	资源包实现.
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
#ifndef __ZUI_CORE_RESDB_H__
#define __ZUI_CORE_RESDB_H__

/*
资源池设计方案


*/
/*资源类型*/
typedef enum ZREST
{
	ZREST_IMG = 1,
	ZREST_TXT,
	ZREST_STREAM,
	ZREST_ZIP
};
/*资源包类型*/
typedef enum ZRESDBT
{
	ZRESDBT_ZIP_FILE = 1,	///压缩文件,来自文件
	ZRESDBT_ZIP_STREAM,		///压缩文件,来着字节流
	ZRESDBT_FILE,			///文件
	ZRESDBT_STREAM,			///字节流
	ZRESDBT_URL,			///网络资源
	ZRESDBT_PE				///应用资源
};
/**资源包结构*/
typedef struct _ZResDB
{
	HINSTANCE Instance; //pe句柄
	unzFile uf;		//压缩包句柄
	ZuiText pass;	//压缩包密码
	ZuiInt type;	//资源包类型
} *ZuiResDB, ZResDB;
/**单个资源*/
typedef struct _ZRes
{
	ZuiAny p;		//资源指针
	ZuiInt plen;	//资源长度

	ZuiInt hash;
	ZuiInt ref;		//引用计数器
	ZuiInt type;	//资源类型
} *ZuiRes, ZRes;
/**资源池*/
typedef struct _ZResDBPool
{
	rb_root *resdb;	//所有已经加载过的资源包
	rb_root	*res;	//所有已经加载过的资源
} *ZuiResDBPool, ZResDBPool;
ZuiBool ZuiResDBInit();
/** 此函数用作从字节流中加载一个Zip型的资源包.
* @param data 流指针
* @param len 流长度
* @param Pass 包密码
* @return 资源包结构.
*/
ZEXPORT ZuiResDB ZCALL ZuiResDBCreateFromBuf(ZuiAny data, ZuiInt len, ZuiText Pass);
/** 此函数用作从文件加载一个Zip型的资源包.
* @param Path 文件路径
* @param Pass 包密码
* @return 资源包结构.
*/
ZEXPORT ZuiResDB ZCALL ZuiResDBCreateFromFile(ZuiText Path, ZuiText Pass);
/** 此函数用作绘制文本.
* @param Graphics ZuiGraphics对象
* @param StringFormat 文本格式
* @param String 文本
* @param Rect 矩形区域
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiResDBDestroy(ZuiResDB db);
/** 此函数用作绘制文本.
* @param Graphics ZuiGraphics对象
* @param StringFormat 文本格式
* @param String 文本
* @param Rect 矩形区域
* @return 此函数没有返回值.
*/
ZEXPORT ZuiRes ZCALL ZuiResDBGetRes(ZuiText Path, ZuiInt type);//获取一个资源
/** 此函数用作绘制文本.
* @param Graphics ZuiGraphics对象
* @param StringFormat 文本格式
* @param String 文本
* @param Rect 矩形区域
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiResDBDelRes(ZuiRes res);//释放一个资源
ZEXPORT ZuiRes ZCALL ZuiResDBNewTempRes(ZuiAny buf, ZuiInt buflen, ZuiInt type);

#endif //__ZUI_CORE_RESDB_H__

