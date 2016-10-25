/**
* @file		unzip.h
* @brief	Zip解压实现.
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
#ifndef _unz64_H
#define _unz64_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ZLIB_H
#include "../zlib/zlib.h"
#endif
#include <stdlib.h>

#define Z_BZIP2ED 12

#if defined(STRICTUNZIP) || defined(STRICTZIPUNZIP)
typedef struct TagunzFile__ { int unused; } unzFile__;
typedef unzFile__ *unzFile;
#else
typedef voidp unzFile;
#endif

#ifdef _MSC_VER
#define fopen64 _wfopen
#if (_MSC_VER >= 1400) && (!(defined(NO_MSCVER_FILE64_FUNC)))
#define ftello64 _ftelli64
#define fseeko64 _fseeki64
#else // old MSC
#define ftello64 ftell
#define fseeko64 fseek
#endif
#endif


#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 ZPOS64_T;
#else
typedef unsigned long long int ZPOS64_T;
#endif

#ifndef ZCALLBACK
#if (defined(WIN32) || defined(_WIN32) || defined (WINDOWS) || defined (_WINDOWS)) && defined(CALLBACK) && defined (USEWINDOWS_CALLBACK)
#define ZCALLBACK CALLBACK
#else
#define ZCALLBACK
#endif
#endif

#define UNZ_OK                          (0)
#define UNZ_END_OF_LIST_OF_FILE         (-100)
#define UNZ_ERRNO                       (Z_ERRNO)
#define UNZ_EOF                         (0)
#define UNZ_PARAMERROR                  (-102)
#define UNZ_BADZIPFILE                  (-103)
#define UNZ_INTERNALERROR               (-104)
#define UNZ_CRCERROR                    (-105)

#define ZLIB_FILEFUNC_SEEK_CUR (1)
#define ZLIB_FILEFUNC_SEEK_END (2)
#define ZLIB_FILEFUNC_SEEK_SET (0)

#define ZLIB_FILEFUNC_MODE_READ      (1)
#define ZLIB_FILEFUNC_MODE_WRITE     (2)
#define ZLIB_FILEFUNC_MODE_READWRITEFILTER (3)

#define ZLIB_FILEFUNC_MODE_EXISTING (4)
#define ZLIB_FILEFUNC_MODE_CREATE   (8)

typedef struct tm_unz_s
{
    uInt tm_sec;            /* seconds after the minute - [0,59] */
    uInt tm_min;            /* minutes after the hour - [0,59] */
    uInt tm_hour;           /* hours since midnight - [0,23] */
    uInt tm_mday;           /* day of the month - [1,31] */
    uInt tm_mon;            /* months since January - [0,11] */
    uInt tm_year;           /* years - [1980..2044] */
} tm_unz;

typedef struct unz_global_info64_s
{
    ZPOS64_T number_entry;         /* total number of entries in
                                     the central dir on this disk */
    uLong size_comment;         /* size of the global comment of the zipfile */
} unz_global_info64;

typedef struct unz_file_info64_s
{
    uLong version;              /* version made by                 2 bytes */
    uLong version_needed;       /* version needed to extract       2 bytes */
    uLong flag;                 /* general purpose bit flag        2 bytes */
    uLong compression_method;   /* compression method              2 bytes */
    uLong dosDate;              /* last mod file date in Dos fmt   4 bytes */
    uLong crc;                  /* crc-32                          4 bytes */
    ZPOS64_T compressed_size;   /* compressed size                 8 bytes */
    ZPOS64_T uncompressed_size; /* uncompressed size               8 bytes */
    uLong size_filename;        /* filename length                 2 bytes */
    uLong size_file_extra;      /* extra field length              2 bytes */
    uLong size_file_comment;    /* file comment length             2 bytes */

    uLong disk_num_start;       /* disk number start               2 bytes */
    uLong internal_fa;          /* internal file attributes        2 bytes */
    uLong external_fa;          /* external file attributes        4 bytes */

    tm_unz tmu_date;
} unz_file_info64;

typedef struct unz_file_info_s
{
    uLong version;              /* version made by                 2 bytes */
    uLong version_needed;       /* version needed to extract       2 bytes */
    uLong flag;                 /* general purpose bit flag        2 bytes */
    uLong compression_method;   /* compression method              2 bytes */
    uLong dosDate;              /* last mod file date in Dos fmt   4 bytes */
    uLong crc;                  /* crc-32                          4 bytes */
    uLong compressed_size;      /* compressed size                 4 bytes */
    uLong uncompressed_size;    /* uncompressed size               4 bytes */
    uLong size_filename;        /* filename length                 2 bytes */
    uLong size_file_extra;      /* extra field length              2 bytes */
    uLong size_file_comment;    /* file comment length             2 bytes */

    uLong disk_num_start;       /* disk number start               2 bytes */
    uLong internal_fa;          /* internal file attributes        2 bytes */
    uLong external_fa;          /* external file attributes        4 bytes */

    tm_unz tmu_date;
} unz_file_info;

typedef struct unz_file_pos_s
{
	uLong pos_in_zip_directory;   /* offset in zip file directory */
	uLong num_of_file;            /* # of file */
} unz_file_pos;

typedef struct unz64_file_pos_s
{
	ZPOS64_T pos_in_zip_directory;   /* offset in zip file directory */
	ZPOS64_T num_of_file;            /* # of file */
} unz64_file_pos;

typedef uLong(ZCALLBACK *read_file_func)      OF((voidpf opaque, voidpf stream, void* buf, uLong size));
typedef uLong(ZCALLBACK *write_file_func)     OF((voidpf opaque, voidpf stream, const void* buf, uLong size));
typedef int(ZCALLBACK *close_file_func)     OF((voidpf opaque, voidpf stream));
typedef int(ZCALLBACK *testerror_file_func) OF((voidpf opaque, voidpf stream));
typedef ZPOS64_T(ZCALLBACK *tell64_file_func)    OF((voidpf opaque, voidpf stream));
typedef long(ZCALLBACK *seek64_file_func)    OF((voidpf opaque, voidpf stream, ZPOS64_T offset, int origin));
typedef voidpf(ZCALLBACK *open64_file_func)    OF((voidpf opaque, const void* filename, int mode));

typedef struct zlib_filefunc64_def_s
{
	open64_file_func    zopen64_file;
	read_file_func      zread_file;
	write_file_func     zwrite_file;
	tell64_file_func    ztell64_file;
	seek64_file_func    zseek64_file;
	close_file_func     zclose_file;
	testerror_file_func zerror_file;
	voidpf              opaque;
} zlib_filefunc64_def;

#define ZREAD64(filefunc,filestream,buf,size)	((*((filefunc).zread_file))   ((filefunc).opaque,filestream,buf,size))
#define ZWRITE64(filefunc,filestream,buf,size)	((*((filefunc).zwrite_file))  ((filefunc).opaque,filestream,buf,size))
#define ZCLOSE64(filefunc,filestream)			((*((filefunc).zclose_file))  ((filefunc).opaque,filestream))
#define ZERROR64(filefunc,filestream)			((*((filefunc).zerror_file))  ((filefunc).opaque,filestream))
#define ZOPEN64(filefunc,filename,mode)			((*((filefunc).zopen64_file)) ((filefunc).opaque,(filename),(mode)))
#define ZTELL64(filefunc,filestream)            ((*((filefunc).ztell64_file)) ((filefunc).opaque,(filestream)))
#define ZSEEK64(filefunc,filestream,pos,mode)   ((*((filefunc).zseek64_file)) ((filefunc).opaque,(filestream),(pos),(mode)))

int unzStringFileNameCompare OF ((const char* fileName1, const char* fileName2, int iCaseSensitivity));
/*打开压缩文件*/
unzFile unzOpen (const wchar_t *path,void * buf, int len);
/*关闭压缩文件*/
int unzClose(unzFile file);
/*取全局信息*/
int unzGetGlobalInfo64(unzFile file, unz_global_info64 *pglobal_info);
/*取注释*/
int unzGetGlobalComment(unzFile file, char * szComment, uLong uSizeBuf);
/*到第一个文件*/
int unzGoToFirstFile(unzFile file);
/*到下一个文件*/
int unzGoToNextFile(unzFile file);
/*定位文件*/
int unzLocateFile(unzFile file, const char *szFileName, int iCaseSensitivity);
/*取当前文件信息*/
int unzGetCurrentFileInfo64(unzFile file, unz_file_info64 *pfile_info, char *szFileName, uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize, char *szComment, uLong commentBufferSize);
/*打开当前文件 带密码*/
int unzOpenCurrentFilePassword(unzFile file, const char* password);
/*打开当前文件*/
int unzOpenCurrentFile3(unzFile file, int* method, int* level, int raw, const char* password);
/*关闭当前文件*/
int unzCloseCurrentFile(unzFile file);
/*读当前文件*/
int unzReadCurrentFile(unzFile file, voidp buf, unsigned len);

#ifdef __cplusplus
}
#endif

#endif /* _unz64_H */
