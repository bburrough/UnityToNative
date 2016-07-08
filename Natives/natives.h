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

typedef void(__stdcall *ChooseFileSuccessCallback)(const wchar_t* str);
typedef void(__stdcall *ChooseFileCancelledCallback)();

DLLEXPORT void* __stdcall CreateChooseFileContext();
DLLEXPORT void __stdcall AddChooseFileType(void* context, const wchar_t* name, const wchar_t* extension);
DLLEXPORT void __stdcall ChooseFile(void* context, ChooseFileSuccessCallback successCallback, ChooseFileCancelledCallback cancelledCallback);
DLLEXPORT void __stdcall DestroyChooseFileContext(void* context);

//DLLEXPORT void __stdcall Teardown();

#ifdef __cplusplus
}
#endif

#endif //_BLIT_IT_NATIVES_H_
