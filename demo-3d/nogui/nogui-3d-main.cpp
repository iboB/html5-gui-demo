// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <demo/Demo.hpp>
#include "NoGUIApp.hpp"

int main()
{
    return demo::Demo::run(std::make_unique<NoGUIApp>());
}