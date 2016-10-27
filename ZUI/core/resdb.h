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
typedef enum ZREST
{
	ZREST_IMG = 1,
	ZREST_TXT,
};
typedef enum ZRESDBT
{
	ZRESDBT_ZIP_FILE = 1,	///压缩文件,来自文件
	ZRESDBT_ZIP_STREAM,		///压缩文件,来着字节流
	ZRESDBT_FILE,			///文件
	ZRESDBT_STREAM,			///字节流
	ZRESDBT_URL,			///网络资源
};

#define ZRDB_TYPE_FILE		1
/**资源包结构*/
typedef struct _ZResDB
{
	unzFile uf;		//压缩包句柄
	ZuiText pass;	//压缩包密码
	ZuiInt type;	//资源包类型
} *ZuiResDB, ZResDB;
/**单个资源*/
typedef struct _ZRes
{
	ZuiAny p;		//资源指针

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
ZEXPORT ZuiResDB ZCALL ZuiResDBCreateFromBuf(ZuiAny data, ZuiInt len, ZuiText Pass);
ZEXPORT ZuiResDB ZCALL ZuiResDBCreateFromFile(ZuiText FileName, ZuiText Pass);
ZEXPORT ZuiVoid ZCALL ZuiResDBDestroy(ZuiResDB db);
ZEXPORT ZuiRes ZCALL ZuiResDBGetRes(ZuiText Path, ZuiInt type);//获取一个资源
ZEXPORT ZuiVoid ZCALL ZuiResDBDelRes(ZuiRes res);//释放一个资源


#endif //__ZUI_CORE_RESDB_H__

