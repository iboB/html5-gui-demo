// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <helper/DirUtil.hpp>
#include <fsbrowser/FSBrowser.hpp>

#include <iostream>

#include <cef_cmake/disable_warnings.h>
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_resource_manager.h>
#include <cef_cmake/reenable_warnings.h>

#include <jsbind.hpp>

#define URI_ROOT "http://htmldemos"
const char* const URL = URI_ROOT "/fs-browser.html";

void setupResourceManagerDirectoryProvider(CefRefPtr<CefResourceManager> resource_manager, std::string uri, std::string dir)
{
    if (!CefCurrentlyOn(TID_IO)) {
        // Execute on the browser IO thread.
        CefPostTask(TID_IO, base::BindOnce(&setupResourceManagerDirectoryProvider, resource_manager, uri, dir));
        return;
    }

    resource_manager->AddDirectoryProvider(uri, dir, 1, dir);
}

// this is only needed so we have a way to break the message loop
class FSBrowserClient : public CefClient, public CefLifeSpanHandler, public CefRequestHandler, public CefResourceRequestHandler
{
public:
    FSBrowserClient()
        : m_resourceManager(new CefResourceManager)
    {
        auto exePath = DirUtil::getCurrentExecutablePath();
        auto assetPath = DirUtil::getAssetPath(exePath, "html");
        setupResourceManagerDirectoryProvider(m_resourceManager, URI_ROOT, assetPath);
    }

    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }

    void OnBeforeClose(CefRefPtr<CefBrowser> /*browser*/) override
    {
        CefQuitMessageLoop();
    }

    CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
        CefRefPtr<CefBrowser> /*browser*/,
        CefRefPtr<CefFrame> /*frame*/,
        CefRefPtr<CefRequest> /*request*/,
        bool /*is_navigation*/,
        bool /*is_download*/,
        const CefString& /*request_initiator*/,
        bool& /*disable_default_handling*/) override {
        return this;
    }

    cef_return_value_t OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback) override
    {
        return m_resourceManager->OnBeforeResourceLoad(browser, frame, request, callback);
    }

    CefRefPtr<CefResourceHandler> GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request) override
    {
        return m_resourceManager->GetResourceHandler(browser, frame, request);
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> /*browser*/,
        CefRefPtr<CefFrame> frame,
        CefProcessId /*source_process*/,
        CefRefPtr<CefProcessMessage> message) override
    {
        if (message->GetName() == "onRequest")
        {
            auto text = message->GetArgumentList()->GetString(0).ToString();
            std::cout << "request received: " << text << std::endl;

            auto resp = m_fsBrowser.onRequest(text);

            auto response = CefProcessMessage::Create("onResponse");
            response->GetArgumentList()->SetString(0, resp);
            frame->SendProcessMessage(PID_RENDERER, response);

            return true;
        }
        return false;
    }

private:
    CefRefPtr<CefResourceManager> m_resourceManager;

    fsbrowser::FSBrowser m_fsBrowser;

    IMPLEMENT_REFCOUNTING(FSBrowserClient);
    DISALLOW_COPY_AND_ASSIGN(FSBrowserClient);
};

jsbind::persistent jsOnResponseFunc;

void setOnResponse(jsbind::local func)
{
    jsOnResponseFunc.reset(func);
}

void request(std::string text)
{
    auto msg = CefProcessMessage::Create("onRequest");
    msg->GetArgumentList()->SetString(0, text);
    CefV8Context::GetCurrentContext()->GetFrame()->SendProcessMessage(PID_BROWSER, msg);
}

JSBIND_BINDINGS(App)
{
    jsbind::function("request", request);
    jsbind::function("setOnResponse", setOnResponse);
}

class RendererApp : public CefApp, public CefRenderProcessHandler
{
public:
    RendererApp() = default;

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override
    {
        return this;
    }

    void OnContextCreated(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/, CefRefPtr<CefV8Context> /*context*/) override
    {
        jsbind::initialize();
    }

    void OnContextReleased(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/, CefRefPtr<CefV8Context> /*context*/) override
    {
        jsbind::enter_context();
        jsOnResponseFunc.reset();
        jsbind::exit_context();
        jsbind::deinitialize();
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> /*browser*/,
        CefRefPtr<CefFrame> /*frame*/,
        CefProcessId /*source_process*/,
        CefRefPtr<CefProcessMessage> message) override
    {
        if (message->GetName() == "onResponse")
        {
            auto text = message->GetArgumentList()->GetString(0).ToString();
            jsbind::enter_context();
            jsOnResponseFunc.to_local()(text);
            jsbind::exit_context();
            return true;
        }
        return false;
    }
private:
    IMPLEMENT_REFCOUNTING(RendererApp);
    DISALLOW_COPY_AND_ASSIGN(RendererApp);
};

int main(int argc, char* argv[])
{
    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();
#if defined(_WIN32)
    CefEnableHighDPISupport();
    CefMainArgs args(GetModuleHandle(NULL));
    commandLine->InitFromString(GetCommandLineW());
#else
    CefMainArgs args(argc, argv);
    commandLine->InitFromArgv(argc, argv);
#endif

    void* windowsSandboxInfo = NULL;

#if defined(CEF_USE_SANDBOX) && defined(_WIN32)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo scopedSandbox;
    windowsSandboxInfo = scopedSandbox.sandbox_info();
#endif

    CefRefPtr<CefApp> app = nullptr;
    std::string appType = commandLine->GetSwitchValue("type");
    if (appType == "renderer" || appType == "zygote")
    {
        app = new RendererApp;
        // use nullptr for other process types
    }
    int result = CefExecuteProcess(args, app, windowsSandboxInfo);
    if (result >= 0)
    {
        // child process completed
        return result;
    }

    CefSettings settings;
    settings.remote_debugging_port = 1234;
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
    CefBrowserHost::CreateBrowser(windowInfo, new FSBrowserClient, URL, browserSettings, nullptr, nullptr);

    CefRunMessageLoop();

    CefShutdown();

    return 0;
}
