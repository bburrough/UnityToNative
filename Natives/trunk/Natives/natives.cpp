
#define BLIT_IT_NATIVES_DLL_COMPILE

#include "natives.h"

#include "threadutils.h"

#include <list>
#include <string>

#include <Windows.h>

using namespace std;

struct ChooseFileContext
{
    ChooseFileSuccessCallback _chooseFileSuccessCallback;
    ChooseFileCancelledCallback _chooseFileCancelledCallback;
    pthread_t file_picker_thread;
    list<wstring> filetype_names;
    list<wstring> filetype_extensions;
};


void* FilePickerThread(void* arg)
{
    ChooseFileContext* cfc = (ChooseFileContext*)arg;

    const int buf_len = 260;
    //TCHAR cwd[buf_len] = { 0 };

    //GetCurrentDirectory(buf_len, cwd);

    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szFile[buf_len];       // buffer for file name
                                 //HWND hwnd;              // owner window
                                 //HANDLE hf;              // file handle


    
    //string test("G-code\0*.gcode\0All\0*.*\0");
    wstring filetype_filters;
    list<wstring>::const_iterator ext_itr = cfc->filetype_extensions.begin();
    for (list<wstring>::const_iterator name_itr = cfc->filetype_names.begin(); name_itr != cfc->filetype_names.end(); ++name_itr)
    {
        filetype_filters += *name_itr;
        filetype_filters.push_back('\0');
        filetype_filters += L"*.";
        filetype_filters += *ext_itr;
        filetype_filters.push_back('\0');
        ++ext_itr;
    }
    filetype_filters += L"All";
    filetype_filters.push_back('\0');
    filetype_filters += L"*.*";
    filetype_filters.push_back('\0');
    

    //TCHAR filter[64] = TEXT(((const char*)test.c_str()));

    TCHAR* filter = (TCHAR*)filetype_filters.c_str();

    
    

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

    //SetCurrentDirectory(cwd);

    if (GetOpenFileName(&ofn))
    {
        pthread_testcancel();
        if (cfc->_chooseFileSuccessCallback)
            cfc->_chooseFileSuccessCallback(ofn.lpstrFile);
    }
    else
    {
        pthread_testcancel();
        if (cfc->_chooseFileCancelledCallback)
            cfc->_chooseFileCancelledCallback();
    }

    return NULL;
}


void* __stdcall CreateChooseFileContext()
{
    ChooseFileContext* cfc = new ChooseFileContext();
    cfc->_chooseFileCancelledCallback = NULL;
    cfc->_chooseFileSuccessCallback = NULL;
    return cfc;
}


void __stdcall AddChooseFileType(void* context, const wchar_t* name, const wchar_t* extension)
{
    ChooseFileContext* cfc = (ChooseFileContext*)context;
    cfc->filetype_names.push_back(wstring(name));
    cfc->filetype_extensions.push_back(wstring(extension));
}


void __stdcall ChooseFile(void* context, ChooseFileSuccessCallback successCallback, ChooseFileCancelledCallback cancelledCallback)
{
    ChooseFileContext* cfc = (ChooseFileContext*)context;
    cfc->_chooseFileSuccessCallback = successCallback;
    cfc->_chooseFileCancelledCallback = cancelledCallback;
    pthread_create(&cfc->file_picker_thread, NULL, FilePickerThread, cfc);
}


void __stdcall DestroyChooseFileContext(void* context)
{
    ChooseFileContext* cfc = (ChooseFileContext*)context;
    pthread_cancel(cfc->file_picker_thread);
    delete cfc;
}

