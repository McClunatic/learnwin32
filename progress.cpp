#include <comdef.h>
#include <ShObjIdl.h>
#include <ShlObj.h>
#include <WinUser.h>
#include <chrono>
#include <thread>
#include <iostream>

_COM_SMARTPTR_TYPEDEF(IProgressDialog, __uuidof(IProgressDialog));

inline void throw_if_fail(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw _com_error(hr);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    HRESULT hr = CoInitializeEx(
        NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr)) {
        try {
            IProgressDialogPtr progress;
            throw_if_fail(CoCreateInstance(
                CLSID_ProgressDialog, NULL, CLSCTX_ALL,
                IID_PPV_ARGS(&progress)));
            throw_if_fail(progress->SetTitle(L"Mob's progress to 100%"));
            throw_if_fail(
                progress->SetCancelMsg(L"Mob's progress halted!", NULL));
            throw_if_fail(progress->Timer(PDTIMER_RESET, NULL));
            throw_if_fail(progress->StartProgressDialog(
                GetTopWindow(NULL),
                NULL,
                PROGDLG_NORMAL | PROGDLG_AUTOTIME,
                NULL));

            for (DWORD i = 1; i <= 100; ++i) {
                using namespace std::chrono_literals;
                if (progress->HasUserCancelled())
                    break;
                throw_if_fail(progress->SetProgress(i, 100));
                std::this_thread::sleep_for(200ms);
            }
            throw_if_fail(progress->StopProgressDialog());
        }
        catch (_com_error& err)
        {
            // Returning char not wchar_t despite TCHAR definition
            LPCSTR message = reinterpret_cast<const char*>(err.ErrorMessage());
            const char* title = "Error";
            MessageBoxA(
                NULL,
                message,
                title,
                MB_OK);
        }
        CoUninitialize();
    }

    return 0;
}