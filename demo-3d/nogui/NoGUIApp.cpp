// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include "NoGUIApp.hpp"

#include <demo/GUI.hpp>

namespace
{
class NoGUI : public demo::GUI
{
    void update() override {}
    void draw() override {}
    void onEvent(const sapp_event&) override {}
    void shutdown() override
    {
        delete this;
    }
};
}

demo::GUI* NoGUIApp::createGUI()
{
    return new NoGUI;
}