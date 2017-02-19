#include <ZUI.h>
void *zui_malloc(unsigned int _Size, const char *_Func, const char *_File, unsigned int _Line) {

    return malloc(_Size);
}
void zui_free(void *_Ptr, const char *_Func, const char *_File, unsigned int _Line) {

    free(_Ptr);
}

void* zui_realloc(void*  _Block, size_t _Size, const char *_Func, const char *_File, unsigned int _Line) {
    return realloc(_Block, _Size);
}
char* __cdecl zui_strdup(char const* _Source, const char *_Func, const char *_File, unsigned int _Line) {
    return _strdup(_Source);
}
wchar_t* __cdecl zui_wcsdup(wchar_t const* _String, const char *_Func, const char *_File, unsigned int _Line) {
    return _wcsdup(_String);
}