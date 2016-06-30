
#define BLIT_IT_NATIVES_DLL_COMPILE

#include "natives.h"
#include "threadutils.h"

#include <Windows.h>

using namespace std;

ChooseFileSuccessCallback _chooseFileSuccessCallback = NULL;
ChooseFileCancelledCallback _chooseFileCancelledCallback = NULL;

pthread_mutex_t file_picker_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t file_picker_thread;
bool file_picker_running = false;

void* LoadGcodeAsync(void* arg)
{
    const int buf_len = 260;
    TCHAR cwd[buf_len] = { 0 };

    GetCurrentDirectory(buf_len, cwd);

    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szFile[buf_len];       // buffer for file name
                                 //HWND hwnd;              // owner window
                                 //HANDLE hf;              // file handle
    TCHAR filter[64] = TEXT("G-code\0*.gcode\0All\0*.*\0");

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    //ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = buf_len;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_READONLY;

    SetCurrentDirectory(cwd);

    if (GetOpenFileName(&ofn))
    {
        if (_chooseFileSuccessCallback)
            _chooseFileSuccessCallback();
    }
    else
    {
        if (_chooseFileCancelledCallback)
            _chooseFileCancelledCallback();
    }

    return NULL;
}


void __stdcall RegisterChooseFileSuccessCallback(ChooseFileSuccessCallback callback)
{
    _chooseFileSuccessCallback = callback;
}


void __stdcall RegisterChooseFileCancelledCallback(ChooseFileCancelledCallback callback)
{
    _chooseFileCancelledCallback = callback;
}


void __stdcall ChooseFile()
{
    pthread_mutex_lock(&shared_machine_mutex);
    if (shared_machine == NULL)
    {
        shared_machine = new Machine();
        loading_thread_started = true;
        pthread_create(&loading_thread, NULL, LoadGcodeAsync, NULL);
    }
    pthread_mutex_unlock(&shared_machine_mutex);
}



