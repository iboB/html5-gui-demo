// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <include/cef_app.h>
#include <include/cef_client.h>

// this is only needed so we have a way to break the message loop
class MinimalClient : public CefClient, public CefLifeSpanHandler
{
public:
    MinimalClient() = default;

    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override
    {
        CefQuitMessageLoop();
    }

    IMPLEMENT_REFCOUNTING(MinimalClient);
    DISALLOW_COPY_AND_ASSIGN(MinimalClient);
};


int main(int argc, char* argv[])
{
#if defined(_WIN32)
    CefEnableHighDPISupport();
    CefMainArgs args(GetModuleHandle(NULL));
#else
    CefMainArgs args(argc, argv);
#endif

    void* windowsSandboxInfo = NULL;

#if defined(CEF_USE_SANDBOX) && defined(_WIN32)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo scopedSandbox;
    windowsSandboxInfo = scopedSandbox.sandbox_info();
#endif

    int result = CefExecuteProcess(args, nullptr, windowsSandboxInfo);
    if (result >= 0)
    {
        // child process completed
        return result;
    }

    CefSettings settings;
#if !defined(CEF_USE_SANDBOX)
    settings.no_sandbox = true;
#endif

    CefInitialize(args, settings, nullptr, windowsSandboxInfo);


    CefWindowInfo windowInfo;

#if defined(_WIN32)
    // On Windows we need to specify certain flags that will be passed to CreateWindowEx().
    windowInfo.SetAsPopup(NULL, "simple");
#endif
    CefBrowserSettings browserSettings;
    CefBrowserHost::CreateBrowser(windowInfo, new MinimalClient, "https://ibob.github.io/cef-demos/html/hello-demos.html", browserSettings, nullptr, nullptr);

    CefRunMessageLoop();

    CefShutdown();

    return 0;
}
