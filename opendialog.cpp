// See: https://learn.microsoft.com/en-us/windows/win32/learnwin32/example--the-open-dialog-box
#include <sstream>
#include <typeinfo>
#include <windows.h>
#include <shobjidl.h>
#include <comdef.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        _COM_SMARTPTR_TYPEDEF(IFileOpenDialog, __uuidof(IFileOpenDialog));
        IFileOpenDialogPtr smartFileOpen(pFileOpen);

        std::stringstream os;
        os << "pFileOpen type: " << typeid(pFileOpen).name() << '\n';
        os << "smartFileOpen type: " << typeid(smartFileOpen).name() << '\n';
        os << "&(*smartFileOpen) type: " << typeid(&(*smartFileOpen)).name() << '\n';
        // OutputDebugStringA(os.str().c_str());
        MessageBoxA(NULL, os.str().c_str(), "Debug", MB_OK);

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = smartFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = smartFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return 0;
}