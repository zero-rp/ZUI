#include <ZUI.h>



ZuiResDBPool Global_ResDB;					//全局资源包
ZuiBool ZuiResDBInit() {
	Global_ResDB = malloc(sizeof(ZResDBPool));
	memset(Global_ResDB, 0, sizeof(ZResDBPool));
	Global_ResDB->resdb = rb_new();
	Global_ResDB->res = rb_new();
	//创建流和文件的默认资源包
	ZuiResDB p = (ZuiResDB)malloc(sizeof(ZResDB));
	memset(p, 0, sizeof(ZResDB));
	p->type = ZRESDBT_FILE;
	rb_insert(Zui_Hash(L"file"), p, Global_ResDB->resdb);
	p = (ZuiResDB)malloc(sizeof(ZResDB));
	memset(p, 0, sizeof(ZResDB));
	p->type = ZRESDBT_STREAM;
	rb_insert(Zui_Hash(L"stream"), p, Global_ResDB->resdb);
	p = (ZuiResDB)malloc(sizeof(ZResDB));
	memset(p, 0, sizeof(ZResDB));
	p->type = ZRESDBT_URL;
	rb_insert(Zui_Hash(L"url"), p, Global_ResDB->resdb);
	p = (ZuiResDB)malloc(sizeof(ZResDB));
	memset(p, 0, sizeof(ZResDB));
	p->type = ZRESDBT_PE;
	rb_insert(Zui_Hash(L"pe"), p, Global_ResDB->resdb);
	return TRUE;
}
ZEXPORT ZuiResDB ZCALL ZuiResDBCreateFromBuf(ZuiAny data, ZuiInt len, ZuiText Pass)
{
	ZuiResDB p = (ZuiResDB)malloc(sizeof(ZResDB));
	if (p)
	{
		memset(p, 0, sizeof(ZResDB));
		if (Pass) {
			p->pass = (char *)malloc(strlen(Pass) + 1);
			memset(p->pass, 0, strlen(Pass) + 1);
			memcpy(p->pass, Pass, strlen(Pass));
		}
		p->uf = unzOpen(0, data, len);
		if (p->uf) {
			p->type = ZRDB_TYPE_FILE;
			return p;
		}
	}
	return 0;
}
ZEXPORT ZuiResDB ZCALL ZuiResDBCreateFromFile(ZuiText FileName, ZuiText Pass)
{
	ZuiResDB p = (ZuiResDB)malloc(sizeof(ZResDB));
	if (p)
	{
		memset(p, 0, sizeof(ZResDB));
		if (Pass) {
			p->pass = (char *)malloc(strlen(Pass) + 1);
			memset(p->pass, 0, strlen(Pass) + 1);
			memcpy(p->pass, Pass, strlen(Pass));
		}
		p->uf = unzOpen(FileName, 0, 0);
		if (p->uf) {
			char name[255];
			p->type = ZRDB_TYPE_FILE;
			unzGetGlobalComment(p->uf, &name, 255);
			int bufsize = ZuiAsciiToUnicode(&name, -1, 0, 0)*sizeof(wchar_t);
			wchar_t *txtbuf = malloc(bufsize);
			bufsize = ZuiAsciiToUnicode(&name, bufsize / sizeof(wchar_t), txtbuf, bufsize);
			//添加到资源池
			rb_insert(Zui_Hash(txtbuf), p, Global_ResDB->resdb);
			free(txtbuf);
			return p;
		}
	}
	free(p);
	return 0;
}
ZEXPORT ZuiVoid ZCALL ZuiResDBDestroy(ZuiResDB db)
{
	if (db)
	{
		unzClose(db->uf);
		free(db->pass);
		free(db);
	}
}
ZEXPORT ZuiRes ZCALL ZuiResDBGetRes(ZuiText Path, ZuiInt type) {
	if (Path) {
		_ZuiText name[255];
		ZuiText pp = Path;
		ZuiText p = &name;
		while (*(wchar_t *)pp != L':' && *(wchar_t *)pp != L'\0')
		{
			*p = *(wchar_t *)pp;
			p++;
			pp++;
		}
		*p = 0;
		pp++;
		//先查找已经加载过的资源里面是否存在
		rb_node *node = rb_search(Zui_Hash(Path), Global_ResDB->res);
		if (node) {
			((ZuiRes)node->data)->ref++;//增加引用计数
			return (ZuiRes)node->data;
		}
		//没找到就查找对应的资源包
		node = rb_search(Zui_Hash(name), Global_ResDB->resdb);
		if (!node) {
			return NULL;
		}
		//找到对应的资源包并提取资源
		ZuiResDB db = (ZuiResDB)node->data;
		ZuiAny buf=0;
		ZuiInt buflen=0;
		if (db->type == ZRESDBT_ZIP_FILE || db->type == ZRESDBT_ZIP_STREAM)
		{
			//转换路径编码
			ZuiInt len = ZuiUnicodeToAscii(pp, -1, 0, 0);
			ZuiAny n = malloc(len);
			ZuiUnicodeToAscii(pp, len, n, len);
			unz_file_info64 info;
			int ret = unzLocateFile(db->uf, n, 0);
			if (ret == 0)
			{
				unzGetCurrentFileInfo64(db->uf, &info, n, 256, 0, 0, 0, 0);
				unzOpenCurrentFilePassword(db->uf, db->pass);
				buf = malloc(info.uncompressed_size);
				buflen = (int)info.uncompressed_size;
				ret = unzReadCurrentFile(db->uf, buf, info.uncompressed_size);
				if (ret < 0) {
					free(buf);
					buf = buflen = 0;
				}
			}
			free(n);
		}
		else if (db->type == ZRESDBT_FILE) {
			FILE*f = _wfopen(pp, L"rb");
			fseek(f, 0L, SEEK_END);
			buflen = ftell(f); /* 得到文件大小 */
			buf = malloc(buflen); /* 根据文件大小动态分配内存空间 */
			fseek(f, 0L, SEEK_SET); /* 定位到文件开头 */
			fread(buf, buflen, 1, f); /* 一次性读取全部文件内容 */
			fclose(f);
		}
		else if (db->type == ZRESDBT_STREAM) {

		}
		if (buf == 0 || buflen == 0)
			return NULL;
		//创建对应的资源类型
		ZuiRes res = malloc(sizeof(ZRes));
		if (!res)
			return NULL;
		memset(res, 0, sizeof(ZRes));
		res->type = type;
		if (type == ZREST_IMG) {
			res->p = ZuiLoadImageFromBinary(buf, buflen);
			//释放缓冲
			free(buf);
		}
		else if (type == ZREST_TXT) {
			int bufsize;
			wchar_t *txtbuf;
			if (ZuiStingIsUtf8(buf, buflen))
			{
				bufsize = ZuiUtf8ToUnicode(buf, -1, 0, 0)*sizeof(wchar_t);
				txtbuf = malloc(bufsize + 2);
				bufsize = ZuiUtf8ToUnicode(buf, buflen, txtbuf, bufsize);
				txtbuf[bufsize] = 0;
			}
			else
			{
				bufsize = ZuiAsciiToUnicode(buf, -1, 0, 0)*sizeof(wchar_t);
				txtbuf = malloc(bufsize + 2);
				bufsize = ZuiAsciiToUnicode(buf, buflen, txtbuf, bufsize);
				txtbuf[bufsize] = 0;
			}
			free(buf);
			res->p = txtbuf;
		}
		if (!res->p) {
			free(res);
			return NULL;
		}
		//保存到资源map
		res->ref++;////增加引用计数
		res->hash = Zui_Hash(Path);
		rb_insert(res->hash, res, Global_ResDB->res);
		return res;
	}
	return NULL;
}
ZEXPORT ZuiVoid ZCALL ZuiResDBDelRes(ZuiRes res) {
	if (res && res->ref) {
		res->ref--;///减少引用计数
		if (res->ref == 0) {
			//计数为0,释放资源
			rb_delete(res->hash, Global_ResDB->res);//从map中移除
			if (res->type == ZREST_IMG) {
				ZuiDestroyImage(res->p);
			}
			else if (res->type == ZREST_TXT) {
				free(res->p);
			}
			free(res);
		}
	}
}
