// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

struct sapp_event;

namespace demo
{

class GUI
{
public:
    virtual ~GUI() = default;

    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void onEvent(const sapp_event& e) = 0;
    virtual void shutdown() = 0;
};

}