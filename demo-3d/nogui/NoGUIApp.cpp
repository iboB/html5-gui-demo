// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include "NoGUIApp.hpp"

#include <demo/GUI.hpp>

#define SOKOL_NO_DEPRECATED
#include <sokol_app.h>

namespace
{
class NoGUI : public demo::GUI
{
    void update() override {}
    void draw() override {}
    void onEvent(const sapp_event& e) override
    {
        if (e.type == SAPP_EVENTTYPE_KEY_UP)
        {
            switch (e.key_code)
            {
                case SAPP_KEYCODE_Z: m_rotating = !m_rotating; break;
                case SAPP_KEYCODE_A: m_rotationSpeed += 0.3f; break;
                case SAPP_KEYCODE_S: m_rotationSpeed -= 0.3f; break;
                case SAPP_KEYCODE_Q: m_rotationAxis = R_X; break;
                case SAPP_KEYCODE_W: m_rotationAxis = R_Y; break;
                case SAPP_KEYCODE_E: m_rotationAxis = R_Z; break;
                default:;
            }
        }
    }
    void shutdown() override
    {
        delete this;
    }
    void updateRevolutions(const int* rev) override
    {
        printf("Revs: %d %d %d\n", rev[0], rev[1], rev[2]);
    }
};
}

demo::GUI* NoGUIApp::createGUI()
{
    return new NoGUI;
}