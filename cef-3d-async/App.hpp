// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

#include <cef_cmake/disable_warnings.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_resource_manager.h>
#include <cef_cmake/reenable_warnings.h>

#include <memory>

struct sapp_event;
struct sapp_desc;

class CefSokolRenderer;

class App : public CefClient, public CefRequestHandler, public CefResourceRequestHandler, public CefRenderHandler
{
public:
    App();
    ~App();
    sapp_desc getSokolDesc() const;

    void init();
    void mainLoop();
    void shutdown();
    void onEvent(const sapp_event& e);

    /////////////////////////////////////
    // CEF
    CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
    CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

    /////////////////////////////////////
    // request handler

    CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(CefRefPtr<CefBrowser> /*browser*/,
        CefRefPtr<CefFrame> /*frame*/, CefRefPtr<CefRequest> /*request*/, bool /*is_navigation*/,
        bool /*is_download*/, const CefString& /*request_initiator*/, bool& /*disable_default_handling*/) override {
        return this;
    }

    /////////////////////////////////////
    // resource request handler

    cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback) override;

    CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) override;

    /////////////////////////////////////
    // render handler
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
        const RectList& dirtyRects, const void* buffer, int width, int height) override;

private:
    CefRefPtr<CefBrowser> m_browser;
    CefRefPtr<CefResourceManager> m_resourceManager;

    std::unique_ptr<CefSokolRenderer> m_cefSokolRenderer;

    IMPLEMENT_REFCOUNTING(App);
    DISALLOW_COPY_AND_ASSIGN(App);
};