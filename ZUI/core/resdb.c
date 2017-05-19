#include <stdio.h>
#include <platform/platform.h>
#include "tree.h"
#include "resdb.h"
#include "function.h"
#include "builder.h"
#include <unzip.h>
#include "string.h"

#if (defined PLATFORM_OS_WIN)
#pragma comment(lib, "wininet.lib") 
#include <wininet.h>
#endif

ZuiResDBPool Global_ResDB;					//全局资源包
ZuiBool ZuiResDBInit() {
    Global_ResDB = malloc(sizeof(ZResDBPool));
    if (Global_ResDB) {
        memset(Global_ResDB, 0, sizeof(ZResDBPool));
        Global_ResDB->resdb = rb_new();
        Global_ResDB->res = rb_new();
        //创建流和文件的默认资源包
        ZuiResDB p = (ZuiResDB)malloc(sizeof(ZResDB));
        memset(p, 0, sizeof(ZResDB));
        p->type = ZRESDBT_FILE;
        rb_insert((key_t)Zui_Hash(L"file"), p, Global_ResDB->resdb);

        p = (ZuiResDB)malloc(sizeof(ZResDB));
        memset(p, 0, sizeof(ZResDB));
        p->type = ZRESDBT_STREAM;
        rb_insert((key_t)Zui_Hash(L"stream"), p, Global_ResDB->resdb);

        p = (ZuiResDB)malloc(sizeof(ZResDB));
        memset(p, 0, sizeof(ZResDB));
        p->type = ZRESDBT_URL;
        rb_insert((key_t)Zui_Hash(L"url"), p, Global_ResDB->resdb);
#if (defined PLATFORM_OS_WIN)
        p = (ZuiResDB)malloc(sizeof(ZResDB));
        memset(p, 0, sizeof(ZResDB));
        p->type = ZRESDBT_PE;
        p->Instance = m_hInstance;
        rb_insert((key_t)Zui_Hash(L"pe_zui"), p, Global_ResDB->resdb);
#endif
        p = (ZuiResDB)malloc(sizeof(ZResDB));
        memset(p, 0, sizeof(ZResDB));
        p->type = ZRESDBT_FONT;
        rb_insert((key_t)Zui_Hash(L"font"), p, Global_ResDB->resdb);

        //加载默认资源包
        ZuiResDBGetRes(L"pe_zui:zip:6666", ZREST_ZIP);
        return TRUE;
    }
    return FALSE;
}
ZuiVoid ZuiResDBUnInitCallBack(void *data) {
    ZuiResDBDestroy((ZuiResDB)data);
}
ZuiVoid ZuiResDBUnInit() {
    rb_node *node;
    rb_foreach(Global_ResDB->resdb, ZuiResDBUnInitCallBack);
    rb_free(Global_ResDB->resdb);
    do {
        node = rb_minkey(Global_ResDB->res);
        if (node)
            ZuiResDBDelRes((ZuiRes)node->data);
    } while (node);

    rb_free(Global_ResDB->res);
    free(Global_ResDB);
}
ZEXPORT ZuiResDB ZCALL ZuiResDBCreateFromBuf(ZuiAny data, ZuiInt len, ZuiText Pass)
{
    ZuiResDB p = (ZuiResDB)malloc(sizeof(ZResDB));
    if (p)
    {
        memset(p, 0, sizeof(ZResDB));
        if (Pass) {
            p->pass = (ZuiText)malloc((wcslen(Pass) + 1)*2);
            memcpy(p->pass, Pass, (wcslen(Pass) + 1) * 2);
        }
        p->uf = unzOpen(0, data, len);
        if (p->uf) {
            char name[265];
            if (unzGetGlobalComment(p->uf, (char *)&name, 255)>0) {
                p->type = ZRESDBT_ZIP_STREAM;
                int bufsize = ZuiAsciiToUnicode(&name, -1, 0, 0) * sizeof(wchar_t);
                wchar_t *txtbuf = malloc(bufsize + 30);
                bufsize = ZuiAsciiToUnicode(&name, bufsize / sizeof(wchar_t), txtbuf, bufsize);
                //添加到资源池
                rb_insert((key_t)Zui_Hash(txtbuf), p, Global_ResDB->resdb);
#if (defined HAVE_JS) && (HAVE_JS == 1)
                //加载引导文件
                wcscat(txtbuf, L":onload.js");
                ZuiBuilderJsLoad(Global_ctx, txtbuf);
#endif
                free(txtbuf);
            }else{
                p->type = 0;
            }
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
            p->pass = (ZuiText)malloc((wcslen(Pass) + 1) * 2);
            memcpy(p->pass, Pass, (wcslen(Pass) + 1) * 2);
        }
        p->uf = unzOpen(FileName, 0, 0);
        if (p->uf) {
            char name[255];
            p->type = ZRESDBT_ZIP_FILE;
            unzGetGlobalComment(p->uf, (char *)&name, 255);
            int bufsize = ZuiAsciiToUnicode(&name, -1, 0, 0) * sizeof(wchar_t);
            ZuiText txtbuf = (ZuiText)malloc(bufsize);
            bufsize = ZuiAsciiToUnicode(&name, bufsize / sizeof(wchar_t), txtbuf, bufsize);
            //添加到资源池
            rb_insert((key_t)Zui_Hash(txtbuf), p, Global_ResDB->resdb);
            free(txtbuf);
            return p;
        }
    }
    free(p);
    return 0;
}

//资源操作
ZEXPORT ZuiVoid ZCALL ZuiResDBDestroy(ZuiResDB db)
{
    if (db)
    {
        switch (db->type)
        {
        case ZRESDBT_ZIP_FILE:
        case ZRESDBT_ZIP_STREAM:
            free(db->pass);
            unzClose(db->uf);
            break;
        case ZRESDBT_FILE:
        default:
            break;
        }
        
        
        free(db);
    }
}
ZEXPORT ZuiRes ZCALL ZuiResDBGetRes(ZuiText Path, ZuiInt type) {
    if (Path) {
        rb_node *node;
        _ZuiText pathbuf[1024];
        ZuiText arr[20];
        ZuiInt arrnum = 20;
        wcscpy(pathbuf, Path);
        ZuiStingSplit(pathbuf, L":", arr, &arrnum);
        if (arrnum < 2)
            return NULL;
        //先查找已经加载过的资源里面是否存在
        node = rb_search((key_t)Zui_Hash(Path), Global_ResDB->res);
        if (node) {
            ((ZuiRes)node->data)->ref++;//增加引用计数
            return (ZuiRes)node->data;
        }
        //没找到就查找对应的资源包
        node = rb_search((key_t)Zui_Hash(arr[0]), Global_ResDB->resdb);
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
            ZuiInt len = ZuiUnicodeToAscii(arr[1], -1, 0, 0);
            ZuiAny n = malloc(len);
            ZuiUnicodeToAscii(arr[1], len, n, len);
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
            FILE*f = _wfopen(arr[1], L"rb");
            if (f) {
                fseek(f, 0L, SEEK_END);
                buflen = ftell(f); /* 得到文件大小 */
                buf = malloc(buflen); /* 根据文件大小动态分配内存空间 */
                fseek(f, 0L, SEEK_SET); /* 定位到文件开头 */
                fread(buf, buflen, 1, f); /* 一次性读取全部文件内容 */
                fclose(f);
            }
        }
        /*字节*/else if (db->type == ZRESDBT_STREAM) {
            buflen = _wtoi(arr[2]);
            buf = malloc(buflen);
            memcpy(buf, _wtoi(arr[1]), buflen);
        }
#if (defined PLATFORM_OS_WIN)
        /*网络*/else if (db->type == ZRESDBT_URL) {
            const wchar_t *parseptr1;
            const wchar_t *parseptr2;
            wchar_t host[256];
            wchar_t prot[20];
            WORD wport = INTERNET_DEFAULT_HTTP_PORT;
            ZuiBool https = FALSE;
            int len;
            int i;
            parseptr2 = Path + 4;
            parseptr1 = wcschr(parseptr2, L':');
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
                parseptr1 = NULL;
                if (NULL == parseptr1)//判断有无端口号
                {
                    parseptr1 = wcschr(parseptr2, L'/');
                    if (NULL == parseptr1) {
                        goto url_erro;
                    }
                    len = parseptr1 - parseptr2;
                    //解析主机
                    memcpy(&host, parseptr2, len * sizeof(wchar_t));
                    host[len] = 0;
                }
                else {
                    len = parseptr1 - parseptr2;
                    //解析主机
                    memcpy(&host, parseptr2, len * sizeof(wchar_t));
                    host[len] = 0;
                    parseptr1++;
                    parseptr2 = parseptr1;
                    parseptr1 = wcschr(parseptr2, L'/');
                    if (NULL == parseptr1) {
                        goto url_erro;
                    }
                    len = parseptr1 - parseptr2;
                    memcpy(&prot, parseptr2, len * sizeof(wchar_t));
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
                        if (https)
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
        /*PE*/else if (db->type == ZRESDBT_PE)
        {
            //定位我们的自定义资源，这里因为我们是从本模块定位资源，所以将句柄简单地置为NULL即可
            HRSRC hRsrc = FindResource(db->Instance, _wtoi(arr[2]), arr[1]);
            if (hRsrc) {
                //获取资源的大小
                buflen = SizeofResource(db->Instance, hRsrc);
                if (buflen) {
                    //加载资源
                    HGLOBAL hGlobal = LoadResource(db->Instance, hRsrc);
                    if (hGlobal) {
                        //锁定资源
                        buf = malloc(buflen);
                        if (buf)
                        {
                            void * pbuf = LockResource(hGlobal);
                            if (pbuf)
                            {
                                memcpy(buf, pbuf, buflen);
                            }
                            else
                            {
                                free(buf);
                                buf = 0;
                            }
                        }
                    }
                }
            }
        }
#endif
        /*字体*/else if (db->type == ZRESDBT_FONT)
        {
            buf = buflen = -1;
        }
        if (buf == 0 || buflen == 0)
            return NULL;
        {
            //创建对应的资源类型
            ZuiRes res = malloc(sizeof(ZRes));
            if (!res)
                return NULL;
            memset(res, 0, sizeof(ZRes));
            res->type = type;
            if (type == ZREST_IMG) {
                ZuiImage img = ZuiLoadImageFromBinary(buf, buflen);
                res->p = img;
                //释放缓冲
                free(buf);
                if (!res->p) {
                    free(res);
                    return NULL;
                }
                for (size_t i = 2; i < arrnum; i++)
                {
                    if (wcsncmp(arr[i], L"src='", 5) == 0) {
                        ZuiText pstr = NULL;
                        img->src.left = _tcstol(arr[i] + 5, &pstr, 10);  ASSERT(pstr);
                        img->src.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                        img->src.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
                        img->src.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
                    }
                }

            }
            else if (type == ZREST_TXT) {
                int bufsize;
                wchar_t *txtbuf;
                if (ZuiStingIsUtf8(buf, buflen))
                {
                    bufsize = ZuiUtf8ToUnicode(buf, -1, 0, 0) * sizeof(wchar_t);
                    txtbuf = malloc(bufsize + 2);
                    bufsize = ZuiUtf8ToUnicode(buf, buflen, txtbuf, bufsize);
                    txtbuf[bufsize] = 0;
                }
                else
                {
                    bufsize = ZuiAsciiToUnicode(buf, -1, 0, 0) * sizeof(wchar_t);
                    txtbuf = malloc(bufsize + 2);
                    bufsize = ZuiAsciiToUnicode(buf, buflen, txtbuf, bufsize);
                    txtbuf[bufsize] = 0;
                }
                free(buf);
                res->p = txtbuf;
            }
            else if (type == ZREST_ZIP)
            {
                res->p = ZuiResDBCreateFromBuf(buf, buflen, 0);
                free(buf);
                if (!res->p) {
                    free(res);
                    return NULL;
                }
                if (res->type > 0) {
                    //是资源包 不由Res管理
                    free(res);
                    return NULL;
                }
            }
            else if (type == ZREST_STREAM) {
                res->p = buf;
                res->plen = buflen;
            }
            else if (type == ZREST_FONT)
            {
                ZuiText name = Global_DefaultFontName;//字体名字
                ZuiColor color = ARGB(254, 0, 0, 0);
                ZuiUInt size = 12;
                ZuiBool bold = FALSE;
                ZuiBool italic = FALSE;
                for (size_t i = 1; i < arrnum; i++)
                {
                    if (wcsncmp(arr[i], L"name='", 6) == 0) {
                        if (arr[i][wcslen(arr[i])-1] == '\'')
                        {
                            name = arr[i] + 6;
                            name[wcslen(name)-1] = 0;
                        }
                    }
                    else if (wcsncmp(arr[i], L"size=", 5) == 0)
                    {
                        size = _wtoi(arr[i] + 5);
                    }
                    else if (wcsncmp(arr[i], L"bold=", 5) == 0)
                    {
                        //粗体
                        if (wcsncmp(arr[i] + 5, L"true", 4) == 0)
                        {
                            bold = TRUE;
                        }
                    }
                    else if (wcsncmp(arr[i], L"italic=", 7) == 0)
                    {
                        //斜体
                        if (wcsncmp(arr[i] + 7, L"true", 4) == 0)
                        {
                            italic = TRUE;
                        }
                    }
                }
                res->p = ZuiCreateFont(name, size, bold, italic);
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
            else if (res->type == ZREST_TXT || res->type == ZREST_STREAM) {
                free(res->p);
            }
            else if (res->type == ZREST_ZIP) {
                ZuiResDBDestroy(res->p);
            }
            free(res);
        }
    }
}
//ZEXPORT ZuiRes ZCALL ZuiResDBNewTempRes(ZuiAny b, ZuiInt buflen, ZuiInt type) {
//    //创建对应的资源类型
//    ZuiRes res = ZuiMalloc(sizeof(ZRes));
//    if (!res)
//        return NULL;
//    memset(res, 0, sizeof(ZRes));
//    res->type = type;
//    ZuiAny buf = ZuiMalloc(buflen);
//    memcpy(buf, b, buflen);
//    if (type == ZREST_IMG) {
//        ZuiImage img = ZuiLoadImageFromBinary(buf, buflen);
//        res->p = img;
//        //释放缓冲
//        ZuiFree(buf);
//        if (!res->p) {
//            ZuiFree(res);
//            return NULL;
//        }
//    }
//    else if (type == ZREST_TXT) {
//        int bufsize;
//        wchar_t *txtbuf;
//        if (ZuiStingIsUtf8(buf, buflen))
//        {
//            bufsize = ZuiUtf8ToUnicode(buf, -1, 0, 0) * sizeof(wchar_t);
//            txtbuf = ZuiMalloc(bufsize + 2);
//            bufsize = ZuiUtf8ToUnicode(buf, buflen, txtbuf, bufsize);
//            txtbuf[bufsize] = 0;
//        }
//        else
//        {
//            bufsize = ZuiAsciiToUnicode(buf, -1, 0, 0) * sizeof(wchar_t);
//            txtbuf = ZuiMalloc(bufsize + 2);
//            bufsize = ZuiAsciiToUnicode(buf, buflen, txtbuf, bufsize);
//            txtbuf[bufsize] = 0;
//        }
//        ZuiFree(buf);
//        res->p = txtbuf;
//    }
//    else if (type == ZREST_ZIP)
//    {
//        res->p = ZuiResDBCreateFromBuf(buf, buflen, 0);
//        ZuiFree(buf);
//        if (!res->p) {
//            ZuiFree(res);
//            return NULL;
//        }
//    }
//    else if (type == ZREST_STREAM) {
//        res->p = buf;
//        res->plen = buflen;
//    }
//    if (!res->p) {
//        ZuiFree(res);
//        return NULL;
//    }
//    //保存到资源map
//    res->ref++;////增加引用计数
//    return res;
//}
#if (defined PLATFORM_OS_WIN)
ZEXPORT ZuiBool ZCALL ZuiResDBAddPE(ZuiText name,HINSTANCE hInstance) {
    ZuiResDB p = (ZuiResDB)malloc(sizeof(ZResDB));
    memset(p, 0, sizeof(ZResDB));
    p->type = ZRESDBT_PE;
    p->Instance = hInstance;
    return rb_insert((key_t)Zui_Hash(name), p, Global_ResDB->resdb);
}
#endif
