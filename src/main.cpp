#include <pybind11/pybind11.h>
#include <comdef.h>
#include <shobjidl.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

_COM_SMARTPTR_TYPEDEF(IFileOpenDialog, __uuidof(IFileOpenDialog));

// https://learn.microsoft.com/en-us/windows/win32/cossdk/interpreting-error-codes
void ErrorDescription(HRESULT hr)
{
     if(FACILITY_WINDOWS == HRESULT_FACILITY(hr))
         hr = HRESULT_CODE(hr);
     TCHAR* szErrMsg;

     if(FormatMessage(
       FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
       NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       (LPTSTR)&szErrMsg, 0, NULL) != 0)
     {
         _tprintf(TEXT("%s"), szErrMsg);
         LocalFree(szErrMsg);
     } else
         _tprintf( TEXT("[Could not find a description for error # %#x.]\n"), hr);
}

class FileOpenDialog {
private:
    IFileOpenDialogPtr com;
public:
    static HRESULT CoInitializeEx() {
        return ::CoInitializeEx(
            NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    }

    HRESULT CoCreateInstance() {
        HRESULT res;
        res = ::CoCreateInstance(
            CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&com));
        if (FAILED(res))
            ErrorDescription(res);
        return res;
    }
};

namespace py = pybind11;

PYBIND11_MODULE(learnwin32, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: learnwin32

        .. autosummary::
           :toctree: _generate

           FileOpenDialog
           FileOpenDialog.CoInitializeEx
           FileOpenDialog.CoCreateInstance
    )pbdoc";

    py::class_<FileOpenDialog>(m, "FileOpenDialog", "FileOpenDialog class.")
        .def(py::init<>())
        .def_static("CoInitializeEx", &FileOpenDialog::CoInitializeEx,
                    "The CoInitializeEx static method.")
        .def("CoCreateInstance", &FileOpenDialog::CoCreateInstance,
             "The CoCreateInstance method.");

    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
}
