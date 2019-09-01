// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include "Demo.hpp"

#include "App.hpp"

#define SOKOL_NO_DEPRECATED
#define SOKOL_IMPL
#define SOKOL_NO_ENTRY
#define SOKOL_GLCORE33
#if !defined(NDEBUG)
#   define SOKOL_DEBUG 1
#endif
#include <sokol_app.h>
#include <sokol_gfx.h>

namespace demo
{

namespace
{
App* g_app;

void sinit()
{
    g_app->init();
}

void smainLoop()
{
    g_app->mainLoop();
}

void scleanup()
{
    g_app->shutdown();
}

void sonEvent(const sapp_event* e)
{
    g_app->onEvent(*e);
}
}

int Demo::run(std::unique_ptr<App> app)
{
    g_app = app.get();

    sapp_desc desc = {};
    desc.window_title = "cef-3d-demo";
    desc.width = 1024;
    desc.height = 768;
    desc.init_cb = sinit;
    desc.frame_cb = smainLoop;
    desc.cleanup_cb = scleanup;
    desc.event_cb = sonEvent;

    int ret = sapp_run(&desc);
    g_app = nullptr;

    return ret;
}

}