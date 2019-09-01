// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include "App.hpp"

#include "GUI.hpp"

#define SOKOL_NO_DEPRECATED
#include <sokol_app.h>
#include <sokol_gfx.h>

namespace demo
{

App::App() = default;
App::~App() = default;

void App::init()
{
    sg_desc desc = {};
    desc.mtl_device = sapp_metal_get_device();
    desc.mtl_renderpass_descriptor_cb = sapp_metal_get_renderpass_descriptor;
    desc.mtl_drawable_cb = sapp_metal_get_drawable;
    desc.d3d11_device = sapp_d3d11_get_device();
    desc.d3d11_device_context = sapp_d3d11_get_device_context();
    desc.d3d11_render_target_view_cb = sapp_d3d11_get_render_target_view;
    desc.d3d11_depth_stencil_view_cb = sapp_d3d11_get_depth_stencil_view;
    desc.gl_force_gles2 = sapp_gles2();
    sg_setup(&desc);

    m_gui = createGUI();
}

void App::mainLoop()
{
    m_gui->update();

    sg_pass_action passAction = {};

    passAction.colors[0].action = SG_ACTION_CLEAR;
    passAction.colors[0].val[0] = 0.2f;
    passAction.colors[0].val[1] = 0.3f;
    passAction.colors[0].val[2] = 0.5f;
    passAction.colors[0].val[3] = 1.0f;

    const int width = sapp_width();
    const int height = sapp_height();
    sg_begin_default_pass(&passAction, width, height);


    m_gui->draw();

    sg_end_pass();
    sg_commit();
}

void App::shutdown()
{
    m_gui->shutdown();
    m_gui = nullptr;
    sg_shutdown();
}

void App::onEvent(const sapp_event& e)
{
    m_gui->onEvent(e);
}

}