// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include "DemoCefRendererApp.hpp"

#include <jsbind.hpp>

jsbind::persistent jsReceiveFunc;

void jsSetReceiveFunc(jsbind::local func)
{
    jsReceiveFunc.reset(func);
}

void jsSend(std::string text)
{
    auto msg = CefProcessMessage::Create("msgFromGUI");
    msg->GetArgumentList()->SetString(0, text);
    CefV8Context::GetCurrentContext()->GetFrame()->SendProcessMessage(PID_BROWSER, msg);
}

JSBIND_BINDINGS(demo3d)
{
    jsbind::function("send", jsSend);
    jsbind::function("setReceiveFunc", jsSetReceiveFunc);
}

DemoCefRendererApp::DemoCefRendererApp() = default;
DemoCefRendererApp::~DemoCefRendererApp() = default;

void DemoCefRendererApp::OnContextCreated(CefRefPtr<CefBrowser> /*browser*/,
    CefRefPtr<CefFrame> /*frame*/, CefRefPtr<CefV8Context> /*context*/)
{
    jsbind::initialize();
}

void DemoCefRendererApp::OnContextReleased(CefRefPtr<CefBrowser> /*browser*/,
    CefRefPtr<CefFrame> /*frame*/, CefRefPtr<CefV8Context> /*context*/)
{
    jsbind::enter_context();
    jsReceiveFunc.reset();
    jsbind::exit_context();
    jsbind::deinitialize();
}

bool DemoCefRendererApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/,
    CefProcessId /*source_process*/, CefRefPtr<CefProcessMessage> message)
{
    if (message->GetName() == "msgFromApp")
    {
        auto text = message->GetArgumentList()->GetString(0).ToString();
        jsbind::enter_context();
        jsReceiveFunc.to_local()(text);
        jsbind::exit_context();
        return true;
    }
    return false;
}
