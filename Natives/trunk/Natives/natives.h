#ifndef _BLIT_IT_NATIVES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef BLIT_IT_NATIVES_DLL_COMPILE
#define DLLEXPORT extern __declspec(dllexport)
#else
#define DLLEXPORT extern __declspec(dllimport)
#endif

    typedef void(__stdcall *ChooseFileSuccessCallback)();
    DLLEXPORT void __stdcall RegisterChooseFileSuccessCallback(ChooseFileSuccessCallback callback);

    typedef void(__stdcall *ChooseFileCancelledCallback)();
    DLLEXPORT void __stdcall RegisterChooseFileCancelledCallback(ChooseFileCancelledCallback callback);

    DLLEXPORT void __stdcall ChooseFile();

#ifdef __cplusplus
}
#endif

#endif //_BLIT_IT_NATIVES_H_
