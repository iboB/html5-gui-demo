// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include "DemoCefApp.hpp"
#include "URL.h"

#include <demo/GUI.hpp>

#include <helper/DirUtil.hpp>

#define SOKOL_NO_DEPRECATED
#include <sokol_app.h>
#include <sokol_gfx.h>

#include <cef_cmake/disable_warnings.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_resource_manager.h>
#include <include/cef_app.h>
#include <cef_cmake/reenable_warnings.h>

namespace
{
void setupResourceManagerDirectoryProvider(CefRefPtr<CefResourceManager> resource_manager, std::string uri, std::string dir)
{
    if (!CefCurrentlyOn(TID_IO)) {
        // Execute on the browser IO thread.
        CefPostTask(TID_IO, base::Bind(&setupResourceManagerDirectoryProvider,resource_manager, uri, dir));
        return;
    }

    resource_manager->AddDirectoryProvider(uri, dir, 1, dir);
}

const char* cef_vs_src = R"glsl(
#version 330
in vec2 position;
in vec2 texcoord;
out vec2 uv;
void main() {
    gl_Position = vec4(position, 0, 1);
    uv = texcoord;
};
)glsl";

const char* cef_fs_src = R"glsl(
#version 330
uniform sampler2D tex;
in vec2 uv;
out vec4 frag_color;
void main() {
    frag_color = texture(tex, uv);
};
)glsl";

struct float3
{
    float x, y, z;
};

struct float2
{
    float x, y;
};

struct CefVertex
{
    float2 pos;
    float2 uv;
};

class CefGUI : public demo::GUI, public CefClient, public CefRequestHandler, public CefResourceRequestHandler, public CefRenderHandler
{
public:
    CefGUI()
        : m_resourceManager(new CefResourceManager)
    {
        {
            auto exePath = DirUtil::getCurrentExecutablePath();
            auto assetPath = DirUtil::getAssetPath(exePath, "html");
            setupResourceManagerDirectoryProvider(m_resourceManager, URI_ROOT, assetPath);
        }

        {
            CefWindowInfo windowInfo;
            windowInfo.SetAsWindowless(0);
            windowInfo.shared_texture_enabled = false;

            CefBrowserSettings browserSettings;
            m_browser = CefBrowserHost::CreateBrowserSync(windowInfo, this, URL, browserSettings, nullptr, nullptr);
        }

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

    ~CefGUI()
    {
    }

    void update() override
    {
        CefDoMessageLoopWork();
    }

    void draw() override
    {
        if (m_cefTexture.id == 0) return;

        if (m_imgDirty)
        {
            // bgra to rgba
            for (size_t i=0; i<m_imgBuffer.size(); i+=4)
            {
                auto p = m_imgBuffer.data() + i;
                std::swap(p[2], p[0]);
            }

            sg_image_content content = {};
            content.subimage[0][0].ptr = m_imgBuffer.data();
            content.subimage[0][0].size = 4 * m_imgWidth * m_imgHeight;
            sg_update_image(m_cefTexture, &content);

            m_imgDirty = false;
        }

        sg_apply_pipeline(m_pipeline);
        sg_apply_bindings(&m_bindings);
        sg_draw(0, 6, 1);
    }

    void onEvent(const sapp_event& e) override
    {
        auto mouseButtonToCef = [](sapp_mousebutton b) -> cef_mouse_button_type_t
        {
            switch (b) {
                case SAPP_MOUSEBUTTON_LEFT: return MBT_LEFT;
                case SAPP_MOUSEBUTTON_RIGHT: return MBT_RIGHT;
                case SAPP_MOUSEBUTTON_MIDDLE: return MBT_MIDDLE;
                default: return MBT_RIGHT;
            }
        };

        auto host = m_browser->GetHost();
        switch (e.type)
        {
        case SAPP_EVENTTYPE_MOUSE_MOVE:
        {
            CefMouseEvent cef;
            cef.x = int(e.mouse_x);
            cef.y = int(e.mouse_y);
            host->SendMouseMoveEvent(cef, false);
        }
        break;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
        {
            CefMouseEvent cef;
            cef.x = int(e.mouse_x);
            cef.y = int(e.mouse_y);
            host->SendMouseClickEvent(cef, mouseButtonToCef(e.mouse_button), false, 1);
        }
        break;
        case SAPP_EVENTTYPE_MOUSE_UP:
        {
            CefMouseEvent cef;
            cef.x = int(e.mouse_x);
            cef.y = int(e.mouse_y);
            host->SendMouseClickEvent(cef, mouseButtonToCef(e.mouse_button), true, 1);
        }
        break;
        default:;
        }
    }

    void shutdown() override
    {

        sg_destroy_pipeline(m_pipeline);
        sg_destroy_shader(m_shader);
        sg_destroy_buffer(m_bindings.vertex_buffers[0]);
        sg_destroy_buffer(m_bindings.index_buffer);
        if (m_cefTexture.id)
        {
            sg_destroy_image(m_cefTexture);
        }

        m_browser = nullptr;
    }

    void updateRevolutions(const int* rev) override
    {
        std::ostringstream sout;
        sout << "{";
        sout << R"json("x":)json" << rev[0] << ",";
        sout << R"json("y":)json" << rev[1] << ",";
        sout << R"json("z":)json" << rev[2];
        sout << "}";

        auto msg = CefProcessMessage::Create("msgFromApp");
        msg->GetArgumentList()->SetString(0, sout.str());
        m_browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, msg);
    }

    void updateTextureFromCef(const uint8_t* buffer, int width, int height)
    {
        // there can be multiple updates per frame from cef, so we collect our
        // img data and mark the image as dirty
        if (m_imgWidth != width || m_imgHeight != height)
        {
            if (m_cefTexture.id)
            {
                sg_destroy_image(m_cefTexture);
            }

            m_imgBuffer.resize(width * height * 4);

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

        memcpy(m_imgBuffer.data(), buffer, m_imgBuffer.size());

        m_imgDirty = true;
    }

    /////////////////////////////////////
    // CEF
    CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
    CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/,
        CefProcessId /*source_process*/, CefRefPtr<CefProcessMessage> message) override
    {
        if (message->GetName() == "msgFromGUI")
        {
            auto msg = message->GetArgumentList()->GetString(0).ToString();
            switch(msg[0])
            {
                case 'a':
                    m_rotationAxis = RotationAxis(atoi(msg.c_str() + 2));
                    break;
                case 'r':
                    m_rotating = msg[2] == 't';
                    break;
                case 's':
                    m_rotationSpeed = float(atof(msg.c_str() + 2));
                    break;
            }
            return true;
        }
        return false;
    }

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
        CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback) override
    {
        return m_resourceManager->OnBeforeResourceLoad(browser, frame, request, callback);
    }

    CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) override
    {
        return m_resourceManager->GetResourceHandler(browser, frame, request);
    }

    /////////////////////////////////////
    // render handler
    void GetViewRect(CefRefPtr<CefBrowser> /*browser*/, CefRect &rect) override
    {
        rect = CefRect(0, 0, sapp_width(), sapp_height());
    }

    void OnPaint(CefRefPtr<CefBrowser> /*browser*/, PaintElementType /*type*/,
        const RectList& /*dirtyRects*/, const void* buffer, int width, int height) override
    {
        updateTextureFromCef(reinterpret_cast<const uint8_t*>(buffer), width, height);
    }

private:
    CefRefPtr<CefBrowser> m_browser;
    CefRefPtr<CefResourceManager> m_resourceManager;

    sg_pipeline m_pipeline = {};
    sg_bindings m_bindings = {};
    int m_imgWidth = 0, m_imgHeight = 0;
    std::vector<uint8_t> m_imgBuffer;
    bool m_imgDirty = true;
    sg_image m_cefTexture = {};
    sg_shader m_shader = {};

    IMPLEMENT_REFCOUNTING(CefGUI);
    DISALLOW_COPY_AND_ASSIGN(CefGUI);
};

}

demo::GUI* DemoCefApp::createGUI()
{
    return new CefGUI;
}