#include <ZUI.h>
#if (defined PLATFORM_OS_WIN)
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Winmm.lib")

ZuiBool ZuiOsInitialize() {

	return TRUE;
}
ZuiBool ZuiOsUnInitialize() {

}






#endif


