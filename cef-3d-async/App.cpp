// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include "App.hpp"
#include "URL.h"

#include <helper/DirUtil.hpp>

#include <cef_cmake/disable_warnings.h>
#include <include/cef_app.h>
#include <cef_cmake/reenable_warnings.h>

#define SOKOL_NO_DEPRECATED
#include <sokol_app.h>
#include <sokol_gfx.h>

namespace
{
App* g_app;

void setupResourceManagerDirectoryProvider(CefRefPtr<CefResourceManager> resource_manager, std::string uri, std::string dir)
{
    if (!CefCurrentlyOn(TID_IO)) {
        // Execute on the browser IO thread.
        CefPostTask(TID_IO, base::Bind(&setupResourceManagerDirectoryProvider,resource_manager, uri, dir));
        return;
    }

    resource_manager->AddDirectoryProvider(uri, dir, 1, dir);
}

const char* cef_vs_src =
"#version 330\n"
"in vec2 position;\n"
"in vec2 texcoord;\n"
"out vec2 uv;\n"
"out vec4 color;\n"
"void main() {\n"
"    gl_Position = vec4(position, 0, 1);\n"
"    uv = texcoord;\n"
"}\n";

const char* cef_fs_src =
"#version 330\n"
"uniform sampler2D tex;\n"
"in vec2 uv;\n"
"out vec4 frag_color;\n"
"void main() {\n"
"    frag_color = texture(tex, uv);\n"
"}\n";

}

struct float3
{
    float x, y, z;
};

struct float2
{
    float x, y;
};

class CefSokolRenderer
{
public:
    struct CefVertex
    {
        float2 pos;
        float2 uv;
    };

    CefSokolRenderer()
    {
        {
            CefVertex vertices[] = {
                {{-1, 1}, {0, 0}},
                {{1, 1}, {1, 0}},
                {{1, -1}, {1, 1}},
                {{-1, -1}, {0, 1}},
            };

            sg_buffer_desc vbuf = { };
            vbuf.usage = SG_USAGE_IMMUTABLE;
            vbuf.size = sizeof(vertices);
            vbuf.content = vertices;
            m_bindings.vertex_buffers[0] = sg_make_buffer(&vbuf);

            uint16_t indices[] = {
                1, 2, 0,
                0, 2, 3
            };

            sg_buffer_desc ibuf = { };
            ibuf.type = SG_BUFFERTYPE_INDEXBUFFER;
            ibuf.usage = SG_USAGE_IMMUTABLE;
            ibuf.size = sizeof(indices);
            ibuf.content = indices;
            m_bindings.index_buffer = sg_make_buffer(&ibuf);
        }

        {
            sg_shader_desc desc = { };
            desc.attrs[0].name = "position";
            desc.attrs[1].name = "texcoord";
            desc.fs.images[0].name = "tex";
            desc.fs.images[0].type = SG_IMAGETYPE_2D;
            desc.fs.source = cef_fs_src;
            desc.vs.source = cef_vs_src;
            m_shader = sg_make_shader(&desc);
        }

        {
            sg_pipeline_desc desc = { };
            desc.layout.buffers[0].stride = sizeof(CefVertex);
            auto& attrs = desc.layout.attrs;
            attrs[0].offset = offsetof(CefVertex, pos); attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
            attrs[1].offset = offsetof(CefVertex, uv); attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
            desc.shader = m_shader;
            desc.index_type = SG_INDEXTYPE_UINT16;
            desc.blend.enabled = true;
            desc.blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
            desc.blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            desc.blend.color_write_mask = SG_COLORMASK_RGB;
            m_pipeline = sg_make_pipeline(&desc);
        }
    }

    ~CefSokolRenderer()
    {
        sg_destroy_pipeline(m_pipeline);
        sg_destroy_shader(m_shader);
        sg_destroy_buffer(m_bindings.vertex_buffers[0]);
        sg_destroy_buffer(m_bindings.index_buffer);
        if (m_cefTexture.id)
        {
            sg_destroy_image(m_cefTexture);
        }
    }

    void draw()
    {
        if (m_cefTexture.id == 0) return;
        sg_apply_pipeline(m_pipeline);
        sg_apply_bindings(&m_bindings);
        sg_draw(0, 6, 1);
    }

    void updateTextureFromCef(const void* buffer, int width, int height)
    {
        // buffer is bgra
        if (m_imgWidth != width || m_imgHeight != height) {
            if (m_cefTexture.id)
            {
                sg_destroy_image(m_cefTexture);
            }

            sg_image_desc desc = {};
            desc.width = width;
            desc.height = height;
            desc.usage = SG_USAGE_DYNAMIC;
            desc.pixel_format = SG_PIXELFORMAT_RGBA8;
            desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
            desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;

            m_imgWidth = width;
            m_imgHeight = height;

            m_cefTexture = sg_make_image(&desc);
            m_bindings.fs_images[0] = m_cefTexture;
        }

        if (!m_cefTexture.id) return;

        sg_image_content content = {};
        content.subimage[0][0].ptr = buffer;
        content.subimage[0][0].size = 4 * width * height;
        sg_update_image(m_cefTexture, &content);
    }

    sg_pipeline m_pipeline = {};
    sg_bindings m_bindings = {};
    int m_imgWidth = 0, m_imgHeight = 0;
    sg_image m_cefTexture = {};
    sg_shader m_shader = {};
};

App::App()
    : m_resourceManager(new CefResourceManager)
{
    auto exePath = DirUtil::getCurrentExecutablePath();
    auto assetPath = DirUtil::getAssetPath(exePath, "html");
    setupResourceManagerDirectoryProvider(m_resourceManager, URI_ROOT, assetPath);

    g_app = this;
}

App::~App()
{
    g_app = nullptr;
}

void App::init()
{
    CefWindowInfo windowInfo;
    windowInfo.SetAsWindowless(0);
    windowInfo.shared_texture_enabled = false;

    CefBrowserSettings browserSettings;
    m_browser = CefBrowserHost::CreateBrowserSync(windowInfo, this, URL, browserSettings, nullptr, nullptr);

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

    m_cefSokolRenderer.reset(new CefSokolRenderer);
}

void App::mainLoop()
{
    CefDoMessageLoopWork();

    sg_pass_action passAction = {};

    passAction.colors[0].action = SG_ACTION_CLEAR;
    passAction.colors[0].val[0] = 0.2f;
    passAction.colors[0].val[1] = 0.3f;
    passAction.colors[0].val[2] = 0.5f;
    passAction.colors[0].val[3] = 1.0f;

    const int width = sapp_width();
    const int height = sapp_height();
    sg_begin_default_pass(&passAction, width, height);
    m_cefSokolRenderer->draw();
    sg_end_pass();
    sg_commit();
}

void App::shutdown()
{
    m_browser = nullptr;
    m_cefSokolRenderer.reset();
    sg_shutdown();
}

bool App::OnProcessMessageReceived(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/,
        CefProcessId /*source_process*/, CefRefPtr<CefProcessMessage> /*message*/)
{
    return false;
}

cef_return_value_t App::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
{
    return m_resourceManager->OnBeforeResourceLoad(browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler> App::GetResourceHandler(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
{
    return m_resourceManager->GetResourceHandler(browser, frame, request);
}

void App::GetViewRect(CefRefPtr<CefBrowser> /*browser*/, CefRect &rect)
{
    rect = CefRect(0, 0, sapp_width(), sapp_height());
}

void App::OnPaint(CefRefPtr<CefBrowser> /*browser*/, PaintElementType /*type*/,
    const RectList& /*dirtyRects*/, const void* buffer, int width, int height)
{
    if (!m_cefSokolRenderer) return;
    m_cefSokolRenderer->updateTextureFromCef(buffer, width, height);
}

namespace
{
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

void sonEvent(const sapp_event*)
{

}
}

sapp_desc App::getSokolDesc() const
{
    sapp_desc desc = {};
    desc.window_title = "cef-3d-demo";
    desc.width = 1024;
    desc.height = 768;
    desc.init_cb = sinit;
    desc.frame_cb = smainLoop;
    desc.cleanup_cb = scleanup;
    desc.event_cb = sonEvent;
    return desc;
}