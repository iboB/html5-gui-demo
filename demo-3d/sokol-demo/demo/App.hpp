// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

struct sapp_event;
struct sapp_desc;

namespace demo
{

class GUI;
class Demo;

class App
{
public:
    App();
    virtual ~App();

    void init();
    void mainLoop();
    void shutdown();
    void onEvent(const sapp_event& e);

protected:
    virtual GUI* createGUI() = 0;

private:
    friend class Demo;

    GUI* m_gui = nullptr;
};

}
