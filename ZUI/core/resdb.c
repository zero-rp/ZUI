#include <ZUI.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib") 

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
			char name[255];
			p->type = ZRESDBT_ZIP_STREAM;
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
			p->type = ZRESDBT_ZIP_FILE;
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
		ZuiAny buf = 0;
		ZuiInt buflen = 0;
		/*压缩*/if (db->type == ZRESDBT_ZIP_FILE || db->type == ZRESDBT_ZIP_STREAM)
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
		/*文件*/else if (db->type == ZRESDBT_FILE) {
			FILE*f = _wfopen(pp, L"rb");
			fseek(f, 0L, SEEK_END);
			buflen = ftell(f); /* 得到文件大小 */
			buf = malloc(buflen); /* 根据文件大小动态分配内存空间 */
			fseek(f, 0L, SEEK_SET); /* 定位到文件开头 */
			fread(buf, buflen, 1, f); /* 一次性读取全部文件内容 */
			fclose(f);
		}
		/*字节*/else if (db->type == ZRESDBT_STREAM) {

		}
		/*网络*/else if (db->type == ZRESDBT_URL) {
			const wchar_t *parseptr1;
			const wchar_t *parseptr2;
			wchar_t host[256];
			wchar_t prot[20];
			WORD wport = INTERNET_DEFAULT_HTTP_PORT;
			ZuiBool https = FALSE;
			int len;
			int i;
			parseptr2 = pp;
			parseptr1 = wcschr(parseptr2, ':');
			if (NULL != parseptr1) {
				len = parseptr1 - parseptr2;
				for (i = 0; i < len; i++) {
					if (!iswalpha(parseptr2[i])) {
						goto url_erro;
					}
				}
				if (len == 5) {
					wport = INTERNET_DEFAULT_HTTPS_PORT;
					https = TRUE;
				}
				parseptr1++;
				parseptr2 = parseptr1;
				for (i = 0; i < 2; i++) {
					if (L'/' != *parseptr2) {
						goto url_erro;
					}
					parseptr2++;
				}
				parseptr1 = wcschr(parseptr2, L':');
				if (NULL == parseptr1)//判断有无端口号
				{
					parseptr1 = wcschr(parseptr2, L'/');
					if (NULL == parseptr1) {
						goto url_erro;
					}
					len = parseptr1 - parseptr2;
					//解析主机
					memcpy(&host, parseptr2, len*sizeof(wchar_t));
					host[len] = 0;
				}
				else {
					len = parseptr1 - parseptr2;
					//解析主机
					memcpy(&host, parseptr2, len*sizeof(wchar_t));
					host[len] = 0;
					parseptr1++;
					parseptr2 = parseptr1;
					parseptr1 = wcschr(parseptr2, L'/');
					if (NULL == parseptr1) {
						goto url_erro;
					}
					len = parseptr1 - parseptr2;
					memcpy(&prot, parseptr2, len*sizeof(wchar_t));
					prot[len] = 0;
					wport = _wtoi(prot);
					//解析端口
				}
				HINTERNET hInet = InternetOpen(L"ZuiHttp", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
				if (hInet)
				{
					HINTERNET  hConnect = InternetConnect(hInet, host, wport, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
					if (hConnect)
					{
						HINTERNET hOpenRequest = 0;
						if(https)
							hOpenRequest = HttpOpenRequest(hConnect, L"GET", parseptr1, HTTP_VERSION, NULL, 0, INTERNET_FLAG_SECURE, 0); //创建https请求
						else
							hOpenRequest = HttpOpenRequest(hConnect, L"GET", parseptr1, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 1); //创建http请求
						if (hOpenRequest)
						{
							if (HttpSendRequestA(hOpenRequest, NULL, 0, NULL, 0))
							{
								len = 20;
								HttpQueryInfo(hOpenRequest, HTTP_QUERY_CONTENT_LENGTH, &prot, &len, 0);
								prot[len] = 0;
								buflen = _wtoi(prot);
								buf = malloc(buflen);
								if (!InternetReadFile(hOpenRequest, buf, buflen, &buflen)) {
									buflen = 0;
									free(buf);
								}
							}
							InternetCloseHandle(hOpenRequest);
						}
						InternetCloseHandle(hConnect);
					}
					InternetCloseHandle(hInet);
				}
			url_erro:;
			}
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
			if (!res->p) {
				free(res);
				return NULL;
			}
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
