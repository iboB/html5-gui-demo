// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

#include <cef_cmake/disable_warnings.h>
#include <include/cef_app.h>
#include <cef_cmake/reenable_warnings.h>

class DemoCefRendererApp : public CefApp, public CefRenderProcessHandler
{
public:
    DemoCefRendererApp();
    ~DemoCefRendererApp();

    void OnContextCreated(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
    void OnContextReleased(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
private:
    IMPLEMENT_REFCOUNTING(DemoCefRendererApp);
    DISALLOW_COPY_AND_ASSIGN(DemoCefRendererApp);
};