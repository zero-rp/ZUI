#include <stdio.h>
#include <ZUI.h>
#include "CodeEdit.h"
ZuiControl Designer;

int _stdcall _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	ZuiInit();
	ZuiControlRegisterAdd(L"codeedit", (ZCtlProc)&ZuiCodeEditProc);

	FILE*fp;
	long flen;
	void *p;

	fp = fopen("Designer.xml", "rb");
	fseek(fp, 0L, SEEK_END);
	flen = ftell(fp); /* 得到文件大小 */
	p = malloc(flen); /* 根据文件大小动态分配内存空间 */
	fseek(fp, 0L, SEEK_SET); /* 定位到文件开头 */
	fread(p, flen, 1, fp); /* 一次性读取全部文件内容 */
	fclose(fp);

	Designer = ZuiLayoutLoad(p, flen);









	ZuiMsgLoop();
}
int main() {
	_tWinMain(0, 0, 0, 0);

}