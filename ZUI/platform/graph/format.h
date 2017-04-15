#ifndef __ZUI_PLATFORM_FORMAT_H__
#define __ZUI_PLATFORM_FORMAT_H__
#if defined(__cplusplus)
extern "C"
{
#endif

//字体格式控制

#define ZDT_TOP                      0x00000000     //顶对齐
#define ZDT_LEFT                     0x00000000     //左对齐
#define ZDT_CENTER                   0x00000001     //右对齐
#define ZDT_RIGHT                    0x00000002     //左对齐
#define ZDT_VCENTER                  0x00000004     //纵向居中
#define ZDT_BOTTOM                   0x00000008     //底对齐
#define ZDT_WORDBREAK                0x00000010     
#define ZDT_SINGLELINE               0x00000020     //单行
#define ZDT_EXPANDTABS               0x00000040
#define ZDT_TABSTOP                  0x00000080
#define ZDT_NOCLIP                   0x00000100
#define ZDT_EXTERNALLEADING          0x00000200
#define ZDT_CALCRECT                 0x00000400
#define ZDT_NOPREFIX                 0x00000800
#define ZDT_INTERNAL                 0x00001000
#define ZDT_EDITCONTROL              0x00002000
#define ZDT_PATH_ELLIPSIS            0x00004000
#define ZDT_END_ELLIPSIS             0x00008000
#define ZDT_MODIFYSTRING             0x00010000
#define ZDT_RTLREADING               0x00020000
#define ZDT_WORD_ELLIPSIS            0x00040000


#ifdef __cplusplus
}
#endif
#endif
