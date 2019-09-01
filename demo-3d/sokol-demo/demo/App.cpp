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

#include <cmath>
#include <chrono>

namespace demo
{

App::App() = default;
App::~App() = default;

namespace
{
struct Vertex
{
    vec3 pos;
    vec3 color;
};
struct matrix
{
    float m00, m10, m20, m30;
    float m01, m11, m21, m31;
    float m02, m12, m22, m32;
    float m03, m13, m23, m33;

    static constexpr matrix columns(
        float cr00, float cr01, float cr02, float cr03, //column 0
        float cr10, float cr11, float cr12, float cr13, //column 1
        float cr20, float cr21, float cr22, float cr23, //column 2
        float cr30, float cr31, float cr32, float cr33  //column 3
    )
    {
        return{
            cr00, cr01, cr02, cr03,
            cr10, cr11, cr12, cr13,
            cr20, cr21, cr22, cr23,
            cr30, cr31, cr32, cr33
        };
    }

    static constexpr matrix rows(
        float rc00, float rc01, float rc02, float rc03, //row 0
        float rc10, float rc11, float rc12, float rc13, //row 1
        float rc20, float rc21, float rc22, float rc23, //row 2
        float rc30, float rc31, float rc32, float rc33  //row 3
    )
    {
        return{
            rc00, rc10, rc20, rc30,
            rc01, rc11, rc21, rc31,
            rc02, rc12, rc22, rc32,
            rc03, rc13, rc23, rc33,
        };
    }

    static constexpr matrix identity()
    {
        return columns(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }

    static constexpr matrix translation(float x, float y, float z)
    {
        return rows(
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
        );
    }

    static matrix ortho_rh(float width, float height, float near_dist, float far_dist)
    {
        auto d = far_dist - near_dist;
        return rows(
            2/width,   0,        0,    0,
            0,         2/height, 0,    0,
            0,         0,        -1/d, -near_dist/d,
            0, 0, 0, 1
        );
    }

    static matrix perspective_rh(float width, float height, float near_dist, float far_dist)
    {
        auto d = far_dist - near_dist;
        return rows(
            (2*near_dist)/width, 0,             0,     0,
            0,            (2*near_dist)/height, 0,     0,
            0,            0,               -far_dist/d, -(far_dist*near_dist)/d,
            0,            0,               -1,          0
        );
    }

    static matrix rotation_x(float radians)
    {
        const auto c = std::cos(radians);
        const auto s = std::sin(radians);

        return rows(
            1, 0,  0, 0,
            0, c, -s, 0,
            0, s,  c, 0,
            0, 0,  0, 1
        );
    }

    static matrix rotation_y(float radians)
    {
        const auto c = std::cos(radians);
        const auto s = std::sin(radians);

        return rows(
            c, 0, s, 0,
            0, 1, 0, 0,
           -s, 0, c, 0,
            0, 0, 0, 1
        );
    }

    static matrix rotation_z(float radians)
    {
        const auto c = std::cos(radians);
        const auto s = std::sin(radians);

        return rows(
            c, -s, 0, 0,
            s,  c, 0, 0,
            0,  0, 1, 0,
            0,  0, 0, 1
        );
    }
};

matrix operator*(const matrix& a, const matrix& b)
{
    return matrix::columns(
        a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30,
        a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30,
        a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30,
        a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30,
        a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31,
        a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31,
        a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31,
        a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31,
        a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32,
        a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32,
        a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32,
        a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32,
        a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33,
        a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33,
        a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33,
        a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33
    );
}


const char* vs_src = R"glsl(
#version 330
uniform mat4 u_projView;
uniform mat4 u_model;
in vec4 a_position;
in vec3 a_color;
out vec3 v_color;
void main() {
    v_color = a_color;
    gl_Position = u_projView * u_model * a_position;
};
)glsl";

const char* fs_src = R"glsl(
#version 330
uniform sampler2D tex;
in vec3 v_color;
out vec4 frag_color;
void main() {
    frag_color = vec4(v_color, 1);
};
)glsl";

struct UniformBlock
{
    matrix projView;
    matrix model;
};

}

class RotatingCube
{
public:
    RotatingCube()
    {
        {
            Vertex vertices[] = {
                //bottom
                {{-1,-1,-1},{1, 0, 0}}, {{ 1, 1,-1},{0, 1, 0}}, {{ 1,-1,-1},{0, 0, 1}}, {{-1, 1,-1},{1, 1, 1}},
                // top
                {{-1,-1, 1},{1, 0, 0}}, {{ 1,-1, 1},{0, 1, 0}}, {{ 1, 1, 1},{0, 0, 1}}, {{-1, 1, 1},{1, 1, 1}},
                //south
                {{-1,-1,-1},{1, 0, 0}}, {{ 1,-1,-1},{0, 1, 0}}, {{-1,-1, 1},{0, 0, 1}}, {{ 1,-1, 1},{1, 1, 1}},
                // north
                {{-1, 1,-1},{1, 0, 0}}, {{-1, 1, 1},{0, 1, 0}}, {{ 1, 1,-1},{0, 0, 1}}, {{ 1, 1, 1},{1, 1, 1}},
                //east
                {{ 1,-1,-1},{1, 0, 0}}, {{ 1, 1, 1},{0, 1, 0}}, {{ 1,-1, 1},{0, 0, 1}}, {{ 1, 1,-1},{1, 1, 1}},
                // west
                {{-1,-1,-1},{1, 0, 0}}, {{-1,-1, 1},{0, 1, 0}}, {{-1, 1, 1},{0, 0, 1}}, {{-1, 1,-1},{1, 1, 1}},
            };
            sg_buffer_desc vbuf = { };
            vbuf.usage = SG_USAGE_IMMUTABLE;
            vbuf.size = sizeof(vertices);
            vbuf.content = vertices;
            m_bindings.vertex_buffers[0] = sg_make_buffer(&vbuf);

            uint16_t indices[] = {
                // bottom
                0, 1, 2, 1, 0, 3,
                // top
                4, 5, 6, 6, 7, 4,
                // south
                8, 9, 10, 9, 11, 10,
                // north
                12, 13, 14, 14, 13, 15,
                //east
                16, 17, 18, 17, 16, 19,
                // west
                20, 21, 22, 22, 23, 20,
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
            desc.attrs[0].name = "a_position";
            desc.attrs[1].name = "a_color";

            desc.vs.uniform_blocks[0].size = sizeof(UniformBlock);
            desc.vs.uniform_blocks[0].uniforms[0].name = "u_projView";
            desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_MAT4;
            desc.vs.uniform_blocks[0].uniforms[1].name = "u_model";
            desc.vs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_MAT4;

            desc.fs.source = fs_src;
            desc.vs.source = vs_src;
            m_shader = sg_make_shader(&desc);
        }

        {
            sg_pipeline_desc desc = { };
            desc.layout.buffers[0].stride = sizeof(Vertex);
            auto& attrs = desc.layout.attrs;
            attrs[0].offset = offsetof(Vertex, pos); attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
            attrs[1].offset = offsetof(Vertex, color); attrs[1].format = SG_VERTEXFORMAT_FLOAT3;

            desc.shader = m_shader;
            desc.index_type = SG_INDEXTYPE_UINT16;
            desc.blend.enabled = false;

            desc.depth_stencil.depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL;
            desc.depth_stencil.depth_write_enabled = true;

            m_pipeline = sg_make_pipeline(&desc);
        }
    }

    ~RotatingCube()
    {
        sg_destroy_pipeline(m_pipeline);
        sg_destroy_shader(m_shader);
        sg_destroy_buffer(m_bindings.vertex_buffers[0]);
        sg_destroy_buffer(m_bindings.index_buffer);
    }

    void draw(vec3 rotation) // dt = delta from last update in seconds
    {
        sg_apply_pipeline(m_pipeline);
        sg_apply_bindings(&m_bindings);

        auto a = float(sapp_width()) / sapp_height();
        UniformBlock block = {
            matrix::perspective_rh(a, 1, 1, 1000) * matrix::translation(0, 0, -5),
            //matrix::ortho_rh(4 * a, 4, 5, -5),
            matrix::rotation_z(rotation.c[2]) * matrix::rotation_y(rotation.c[1]) * matrix::rotation_x(rotation.c[0])
        };
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &block, sizeof(UniformBlock));
        sg_draw(0, 36, 1);
    }

    sg_pipeline m_pipeline = {};
    sg_bindings m_bindings = {};
    sg_shader m_shader = {};
};

void App::init()
{
    m_currentFrameTime = App::getTicks();

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

    m_cube = std::make_unique<RotatingCube>();
    m_gui = createGUI();
}

void App::mainLoop()
{
    uint32_t now = App::getTicks();
    m_timeSinceLastFrame = now - m_currentFrameTime;
    m_currentFrameTime = now;

    if (m_gui->rotating())
    {
        auto a = m_gui->rotationAxis();
        auto& r = m_rotation.c[a];
        r += (float(m_timeSinceLastFrame) / 1000) * m_gui->rotationSpeed();
        if (r > M_PI * 2)
        {
            r -= M_PI * 2;
            ++m_revolutions.c[a];
            // TODO: update GUI
        }
    }

    m_gui->update();

    sg_pass_action passAction = {};

    passAction.colors[0].action = SG_ACTION_CLEAR;
    passAction.colors[0].val[0] = 0.2f;
    passAction.colors[0].val[1] = 0.3f;
    passAction.colors[0].val[2] = 0.5f;
    passAction.colors[0].val[3] = 1.0f;
    passAction.depth.action = SG_ACTION_CLEAR;
    passAction.depth.val = 1;
    passAction.stencil.action = SG_ACTION_DONTCARE;

    const int width = sapp_width();
    const int height = sapp_height();
    sg_begin_default_pass(&passAction, width, height);

    m_cube->draw(m_rotation);

    m_gui->draw();

    sg_end_pass();
    sg_commit();
}

void App::shutdown()
{
    m_gui->shutdown();
    m_gui = nullptr;
    m_cube.reset();
    sg_shutdown();
}

void App::onEvent(const sapp_event& e)
{
    m_gui->onEvent(e);
}

uint32_t App::getTicks()
{
    // actually the time returned is since the first time this function is called
    // but this is very early in the execution time, so it's fine
    static auto start = std::chrono::steady_clock::now();
    auto time = std::chrono::steady_clock::now() - start;
    return uint32_t(std::chrono::duration_cast<std::chrono::milliseconds>(time).count()) + 1;
}

}