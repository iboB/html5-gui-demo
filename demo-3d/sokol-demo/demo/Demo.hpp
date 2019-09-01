// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

#include <memory>

namespace demo
{

class App;

class Demo
{
public:
    // init and run sokol
    static int run(std::unique_ptr<App> app);
};

}