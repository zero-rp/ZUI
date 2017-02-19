/**
* @file		memory.h
* @brief	内存管理器.
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
#ifndef __ZUI_CORE_MEMORY_H__
#define __ZUI_CORE_MEMORY_H__

#ifdef _DEBUG
//开启内存跟踪和检查
void *zui_malloc(unsigned int _Size, const char *_Func, const char *_File, unsigned int _Line);
void zui_free(void *_Ptr, const char *_Func, const char *_File, unsigned int _Line);
void* zui_realloc(void*  _Block, size_t _Size, const char *_Func, const char *_File, unsigned int _Line);
char* __cdecl zui_strdup(char const* _Source, const char *_Func, const char *_File, unsigned int _Line);
wchar_t* __cdecl zui_wcsdup(wchar_t const* _String, const char *_Func, const char *_File, unsigned int _Line);
#define ZuiMalloc(size)   zui_malloc(size, __FUNCTION__, __FILE__, __LINE__)
#define ZuiFree(ptr)    zui_free(ptr, __FUNCTION__, __FILE__, __LINE__);
#define ZuiRealloc(ptr,size)  zui_realloc(ptr, size, __FUNCTION__, __FILE__, __LINE__);
#define ZuiStrdup(ptr)   zui_strdup(ptr, __FUNCTION__, __FILE__, __LINE__);
#define ZuiWcsdup(ptr)   zui_wcsdup(ptr, __FUNCTION__, __FILE__, __LINE__);
#else
#define ZuiMalloc   malloc
#define ZuiFree free
#define ZuiRealloc  realloc
#define ZuiStrdup   _strdup
#define ZuiWcsdup   _wcsdup
#endif 


#endif //__ZUI_CORE_MEMORY_H__
